#include <Arduino.h>
#include <Wire.h>

#ifndef BRINGUP_STAGE
#define BRINGUP_STAGE 1
#endif

#ifndef OLED_ADDR
#define OLED_ADDR 0x3C
#endif

#ifndef TEST_FREQ_MHZ
#define TEST_FREQ_MHZ 101.7
#endif

#ifndef RDA_VOLUME
#define RDA_VOLUME 2
#endif

#ifndef SWEEP_START_MHZ
#define SWEEP_START_MHZ 87.5
#endif

#ifndef SWEEP_END_MHZ
#define SWEEP_END_MHZ 108.0
#endif

#ifndef SWEEP_STEP_MHZ
#define SWEEP_STEP_MHZ 0.2
#endif

#ifndef SWEEP_DWELL_MS
#define SWEEP_DWELL_MS 1600
#endif

#ifndef TUNE_STEP_MHZ
#define TUNE_STEP_MHZ 0.1
#endif

#ifndef ENCODER_REVERSE
#define ENCODER_REVERSE 0
#endif

#if BRINGUP_STAGE >= 3
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

constexpr uint8_t I2C_SDA_PIN = 6; // XIAO ESP32C3 D4
constexpr uint8_t I2C_SCL_PIN = 7; // XIAO ESP32C3 D5

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 32;
constexpr int8_t OLED_RESET_PIN = -1;

constexpr uint8_t RDA5807M_WRITE_ADDR = 0x10;
constexpr uint8_t RDA5807M_READ_ADDR = 0x11;

constexpr uint8_t ENCODER_A_PIN = 2;  // XIAO ESP32C3 D0
constexpr uint8_t ENCODER_B_PIN = 3;  // XIAO ESP32C3 D1
constexpr uint8_t ENCODER_SW_PIN = 4; // XIAO ESP32C3 D2

#if BRINGUP_STAGE >= 3
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);
#endif

bool i2cBusReady = false;

void printBootHeader() {
  Serial.println();
  Serial.println("Tin-Can Radio V0 bring-up");
  Serial.print("Stage: ");
  Serial.println(BRINGUP_STAGE);
  Serial.print("I2C SDA GPIO");
  Serial.print(I2C_SDA_PIN);
  Serial.print(", SCL GPIO");
  Serial.println(I2C_SCL_PIN);
}

bool startI2c() {
  pinMode(I2C_SDA_PIN, INPUT_PULLUP);
  pinMode(I2C_SCL_PIN, INPUT_PULLUP);
  delay(20);

  const bool sdaHigh = digitalRead(I2C_SDA_PIN) == HIGH;
  const bool sclHigh = digitalRead(I2C_SCL_PIN) == HIGH;

  Serial.print("I2C preflight SDA=");
  Serial.print(sdaHigh ? "HIGH" : "LOW");
  Serial.print(" SCL=");
  Serial.println(sclHigh ? "HIGH" : "LOW");

  if (!sdaHigh || !sclHigh) {
    Serial.println("I2C bus held low; check wiring, power, and swapped SDA/SCL.");
    return false;
  }

  Wire.setTimeOut(50);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000);
  return true;
}

void scanI2cBus() {
  if (!i2cBusReady) {
    Serial.println("I2C scan skipped because bus preflight failed");
    delay(3000);
    return;
  }

  int count = 0;
  const uint8_t addresses[] = {
    0x10, // RDA5807M common write/control address
    0x11, // RDA5807M common read/status address
    0x3C, // SSD1306 common address
    0x3D, // SSD1306 alternate address
  };

  Serial.println("Probing expected I2C addresses...");
  for (uint8_t address : addresses) {
    Serial.print("Probe 0x");
    if (address < 16) {
      Serial.print("0");
    }
    Serial.println(address, HEX);

    Wire.beginTransmission(address);
    const uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      count++;
    } else if (error == 4) {
      Serial.print("Unknown I2C error at 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }

  if (count == 0) {
    Serial.println("No expected I2C devices found");
  } else {
    Serial.print("Devices found: ");
    Serial.println(count);
  }
}

#if BRINGUP_STAGE >= 3
void showStatus(const char* line1, const char* line2, const char* line3 = nullptr) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  display.println(line2);
  if (line3 != nullptr) {
    display.println(line3);
  }
  display.display();
}

void setupOled() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 init failed. Try OLED_ADDR=0x3D if scanner shows 0x3D.");
    while (true) {
      delay(1000);
    }
  }

  showStatus("Tin-Can Radio V0", "OLED OK");
  Serial.println("OLED OK");
}
#endif

#if BRINGUP_STAGE >= 4
struct RdaStatus {
  bool available;
  bool tuned;
  bool stereo;
  uint8_t rssi;
};

uint16_t clampRdaVolume() {
  if (RDA_VOLUME < 0) {
    return 0;
  }
  if (RDA_VOLUME > 15) {
    return 15;
  }
  return RDA_VOLUME;
}

uint16_t frequencyToChannel(float frequencyMhz) {
  if (frequencyMhz < 87.0f) {
    frequencyMhz = 87.0f;
  }
  if (frequencyMhz > 108.0f) {
    frequencyMhz = 108.0f;
  }

  // RDA5807M band 0 starts at 87.0 MHz with 100 kHz channel spacing.
  return static_cast<uint16_t>((frequencyMhz - 87.0f) * 10.0f + 0.5f);
}

bool writeRdaRegisters(const uint16_t* registers, size_t registerCount) {
  Wire.beginTransmission(RDA5807M_WRITE_ADDR);
  for (size_t i = 0; i < registerCount; i++) {
    Wire.write(static_cast<uint8_t>(registers[i] >> 8));
    Wire.write(static_cast<uint8_t>(registers[i] & 0xFF));
  }
  return Wire.endTransmission() == 0;
}

bool resetRda5807m() {
  const uint16_t resetRegisters[] = {
    0x0002, // reg 0x02: soft reset
  };

  const bool ok = writeRdaRegisters(resetRegisters, 1);
  delay(100);
  return ok;
}

bool tuneRda5807m(float frequencyMhz) {
  const uint16_t channel = frequencyToChannel(frequencyMhz);
  const uint16_t volume = clampRdaVolume();

  const uint16_t registers[] = {
    0xC001,                                // reg 0x02: enable, unmute, audio output high-Z disabled
    static_cast<uint16_t>((channel << 6) | 0x0010), // reg 0x03: channel + tune, 87-108 MHz, 100 kHz
    0x0400,                                // reg 0x04: common library default
    static_cast<uint16_t>(0x88A0 | volume), // reg 0x05: low starting volume
    0x0000,                                // reg 0x06
    0x4202,                                // reg 0x07: common library default
  };

  return writeRdaRegisters(registers, sizeof(registers) / sizeof(registers[0]));
}

RdaStatus readRdaStatus() {
  Wire.requestFrom(RDA5807M_READ_ADDR, static_cast<uint8_t>(4));
  if (Wire.available() < 4) {
    return {false, false, false, 0};
  }

  const uint16_t statusA = (Wire.read() << 8) | Wire.read();
  const uint16_t statusB = (Wire.read() << 8) | Wire.read();
  return {
    true,
    (statusA & 0x4000) != 0,
    (statusA & 0x0400) != 0,
    static_cast<uint8_t>(statusB >> 9),
  };
}

void printRdaStatus() {
  const RdaStatus status = readRdaStatus();
  if (!status.available) {
    Serial.println("RDA5807M status read unavailable");
    return;
  }

  Serial.print("RDA status tuned=");
  Serial.print(status.tuned ? "yes" : "no");
  Serial.print(" stereo=");
  Serial.print(status.stereo ? "yes" : "no");
  Serial.print(" rssi=");
  Serial.println(status.rssi);
}

void updateStationDisplay(float frequencyMhz) {
  const RdaStatus status = readRdaStatus();

  char line1[22];
  char line2[22];
  char line3[22];
  snprintf(line1, sizeof(line1), "FM %.1f MHz", static_cast<double>(frequencyMhz));

  if (status.available) {
    snprintf(line2, sizeof(line2), "RSSI %u %s", status.rssi, status.tuned ? "TUNE" : "SEARCH");
    snprintf(line3, sizeof(line3), "%s Vol %u", status.stereo ? "Stereo" : "Mono", clampRdaVolume());
  } else {
    snprintf(line2, sizeof(line2), "RDA status lost");
    snprintf(line3, sizeof(line3), "Check I2C");
  }

  showStatus("Tin-Can Radio", line1, line2);
  display.setCursor(0, 24);
  display.println(line3);
  display.display();

  if (status.available) {
    Serial.print("FM ");
    Serial.print(frequencyMhz, 1);
    Serial.print(" MHz tuned=");
    Serial.print(status.tuned ? "yes" : "no");
    Serial.print(" stereo=");
    Serial.print(status.stereo ? "yes" : "no");
    Serial.print(" rssi=");
    Serial.println(status.rssi);
  } else {
    Serial.println("RDA5807M status read unavailable");
  }
}

void updateFixedStationDisplay() {
  updateStationDisplay(TEST_FREQ_MHZ);
}

void setupRadio() {
  Serial.println("Initializing RDA5807M...");
  showStatus("Tin-Can Radio", "Init RDA5807M");

  const bool resetOk = resetRda5807m();
  const bool tuneOk = tuneRda5807m(TEST_FREQ_MHZ);

  Serial.print("RDA reset: ");
  Serial.println(resetOk ? "OK" : "FAILED");
  Serial.print("RDA tune: ");
  Serial.println(tuneOk ? "OK" : "FAILED");
  Serial.print("Frequency MHz: ");
  Serial.println(TEST_FREQ_MHZ, 1);
  Serial.print("Volume: ");
  Serial.println(clampRdaVolume());

  char frequencyLine[22];
  snprintf(frequencyLine, sizeof(frequencyLine), "FM %.1f MHz", static_cast<double>(TEST_FREQ_MHZ));

  if (resetOk && tuneOk) {
    showStatus("RDA5807M OK", frequencyLine, "Listen for audio");
  } else {
    showStatus("RDA5807M FAIL", "Check wiring/power", "See Serial");
  }
}

#if BRINGUP_STAGE >= 5
float sweepFrequencyMhz = SWEEP_START_MHZ;

void sweepNextFrequency() {
  const bool tuneOk = tuneRda5807m(sweepFrequencyMhz);
  delay(250);
  const RdaStatus status = readRdaStatus();

  Serial.print("FM ");
  Serial.print(sweepFrequencyMhz, 1);
  Serial.print(" MHz tune=");
  Serial.print(tuneOk ? "OK" : "FAILED");

  if (status.available) {
    Serial.print(" tuned=");
    Serial.print(status.tuned ? "yes" : "no");
    Serial.print(" stereo=");
    Serial.print(status.stereo ? "yes" : "no");
    Serial.print(" rssi=");
    Serial.println(status.rssi);
  } else {
    Serial.println(" status=unavailable");
  }

  char line1[22];
  char line2[22];
  snprintf(line1, sizeof(line1), "FM %.1f MHz", static_cast<double>(sweepFrequencyMhz));
  if (status.available) {
    snprintf(line2, sizeof(line2), "RSSI %u %s", status.rssi, status.tuned ? "TUNE" : "");
  } else {
    snprintf(line2, sizeof(line2), "No RDA status");
  }
  showStatus("FM sweep", line1, line2);

  sweepFrequencyMhz += SWEEP_STEP_MHZ;
  if (sweepFrequencyMhz > SWEEP_END_MHZ + 0.01f) {
    sweepFrequencyMhz = SWEEP_START_MHZ;
    Serial.println("FM sweep restart");
  }

  delay(SWEEP_DWELL_MS);
}
#endif

#if BRINGUP_STAGE >= 6
float currentFrequencyMhz = TEST_FREQ_MHZ;
uint8_t encoderLastState = 0;
int8_t encoderAccumulator = 0;
bool buttonStableState = HIGH;
bool buttonLastReading = HIGH;
uint32_t buttonLastChangeMs = 0;
uint32_t lastStatusRefreshMs = 0;

uint8_t readEncoderState() {
  const uint8_t a = digitalRead(ENCODER_A_PIN) == HIGH ? 1 : 0;
  const uint8_t b = digitalRead(ENCODER_B_PIN) == HIGH ? 1 : 0;
  return static_cast<uint8_t>((a << 1) | b);
}

void setupControls() {
  pinMode(ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  delay(5);

  encoderLastState = readEncoderState();
  buttonStableState = digitalRead(ENCODER_SW_PIN);
  buttonLastReading = buttonStableState;

  Serial.println("Encoder controls ready");
  Serial.println("Rotate: tune 0.1 MHz, press: return to startup station");
}

void clampCurrentFrequency() {
  if (currentFrequencyMhz < 87.0f) {
    currentFrequencyMhz = 87.0f;
  }
  if (currentFrequencyMhz > 108.0f) {
    currentFrequencyMhz = 108.0f;
  }
}

void tuneCurrentFrequency() {
  clampCurrentFrequency();
  const bool ok = tuneRda5807m(currentFrequencyMhz);
  delay(120);

  Serial.print("Encoder tune ");
  Serial.print(currentFrequencyMhz, 1);
  Serial.print(" MHz ");
  Serial.println(ok ? "OK" : "FAILED");

  updateStationDisplay(currentFrequencyMhz);
  lastStatusRefreshMs = millis();
}

void handleEncoderRotation() {
  static const int8_t transitionTable[16] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0,
  };

  const uint8_t state = readEncoderState();
  if (state == encoderLastState) {
    return;
  }

  const uint8_t transition = static_cast<uint8_t>((encoderLastState << 2) | state);
  encoderLastState = state;
  encoderAccumulator += transitionTable[transition & 0x0F];

  if (encoderAccumulator >= 4 || encoderAccumulator <= -4) {
    int8_t direction = encoderAccumulator > 0 ? 1 : -1;
#if ENCODER_REVERSE
    direction = -direction;
#endif
    encoderAccumulator = 0;

    currentFrequencyMhz += direction * TUNE_STEP_MHZ;
    tuneCurrentFrequency();
  }
}

void handleEncoderButton() {
  const bool reading = digitalRead(ENCODER_SW_PIN);
  const uint32_t now = millis();

  if (reading != buttonLastReading) {
    buttonLastChangeMs = now;
    buttonLastReading = reading;
  }

  if (now - buttonLastChangeMs < 35) {
    return;
  }

  if (reading != buttonStableState) {
    buttonStableState = reading;
    if (buttonStableState == LOW) {
      currentFrequencyMhz = TEST_FREQ_MHZ;
      tuneCurrentFrequency();
    }
  }
}

void updateInteractiveRadio() {
  handleEncoderRotation();
  handleEncoderButton();

  if (millis() - lastStatusRefreshMs >= 1000) {
    updateStationDisplay(currentFrequencyMhz);
    lastStatusRefreshMs = millis();
  }
}
#endif
#endif

void setup() {
  Serial.begin(115200);
  delay(1000);
  printBootHeader();

#if BRINGUP_STAGE >= 2
  i2cBusReady = startI2c();
  Serial.println(i2cBusReady ? "I2C started" : "I2C not started");
#endif

#if BRINGUP_STAGE >= 3
  setupOled();
#endif

#if BRINGUP_STAGE >= 4
  setupRadio();
#endif

#if BRINGUP_STAGE >= 6
  setupControls();
  currentFrequencyMhz = TEST_FREQ_MHZ;
  updateStationDisplay(currentFrequencyMhz);
#endif
}

void loop() {
#if BRINGUP_STAGE == 1
  Serial.println("alive");
  delay(1000);
#elif BRINGUP_STAGE == 2
  scanI2cBus();
  delay(3000);
#elif BRINGUP_STAGE == 3
  Serial.println("OLED test alive");
  delay(3000);
#elif BRINGUP_STAGE == 4
  updateFixedStationDisplay();
  delay(3000);
#elif BRINGUP_STAGE == 5
  sweepNextFrequency();
#elif BRINGUP_STAGE >= 6
  updateInteractiveRadio();
  delay(2);
#endif
}
