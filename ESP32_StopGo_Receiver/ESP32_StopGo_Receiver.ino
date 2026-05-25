#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>

/*
  STOPP / GÅ - MOTTAKER
  Kort nr. 2
  MAC: 0C:DC:7E:62:9E:40

  WS2812B:
  DIN til GPIO 4

  LED-oppsett:
  13 stk WS2812B totalt

  LED 0-5   = grønn side
  LED 6     = midt-LED, bytter mellom grønn og rød
  LED 7-12  = rød side

  Innebygd LED på ESP32:
  GPIO 2 forsøkes holdt AV. På noen kort er power-LED ikke styrbar.
*/

const int LED_PIN = 4;
const int LED_COUNT = 13;

const int BUILTIN_LED_PIN = 2;
const int BUILTIN_LED_OFF_STATE = LOW;

const int GREEN_START = 0;
const int GREEN_END = 6;

const int RED_START = 6;
const int RED_END = 12;

const int BRIGHTNESS = 220;

const unsigned long RAINBOW_SPEED = 20;
const uint8_t RAINBOW_STEP = 8;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint8_t mode = 0;

unsigned long lastRainbowUpdate = 0;
uint8_t rainbowOffset = 0;

typedef struct Message {
  uint8_t mode;
} Message;

Message incomingMessage;

void keepBuiltinLedOff() {
  digitalWrite(BUILTIN_LED_PIN, BUILTIN_LED_OFF_STATE);
}

void printMode(uint8_t selectedMode) {
  if (selectedMode == 0) Serial.print("REGNBUE");
  if (selectedMode == 1) Serial.print("GRØNN");
  if (selectedMode == 2) Serial.print("RØD");
  if (selectedMode == 3) Serial.print("AV");
}

uint32_t wheel(byte pos) {
  pos = 255 - pos;

  if (pos < 85) {
    return strip.Color(255 - pos * 3, 0, pos * 3);
  }

  if (pos < 170) {
    pos -= 85;
    return strip.Color(0, pos * 3, 255 - pos * 3);
  }

  pos -= 170;
  return strip.Color(pos * 3, 255 - pos * 3, 0);
}

void showRainbow() {
  if (millis() - lastRainbowUpdate < RAINBOW_SPEED) {
    return;
  }

  lastRainbowUpdate = millis();
  rainbowOffset += RAINBOW_STEP;

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, wheel((i * 18 + rainbowOffset) & 255));
  }

  strip.show();
}

void showGreen() {
  strip.clear();

  for (int i = GREEN_START; i <= GREEN_END; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));
  }

  strip.show();
}

void showRed() {
  strip.clear();

  for (int i = RED_START; i <= RED_END; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }

  strip.show();
}

void showOff() {
  strip.clear();
  strip.show();
}

void updateLights() {
  keepBuiltinLedOff();

  if (mode == 0) {
    return;
  }

  if (mode == 1) {
    showGreen();
  }

  if (mode == 2) {
    showRed();
  }

  if (mode == 3) {
    showOff();
  }

  keepBuiltinLedOff();
}

void onDataReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  keepBuiltinLedOff();

  if (len != sizeof(incomingMessage)) {
    Serial.println("FEIL: Mottatt feil meldingslengde");
    return;
  }

  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));

  mode = incomingMessage.mode;

  Serial.print("Mottatt modus ");
  Serial.print(mode);
  Serial.print(" = ");
  printMode(mode);
  Serial.println();

  updateLights();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(BUILTIN_LED_PIN, OUTPUT);
  keepBuiltinLedOff();

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();

  keepBuiltinLedOff();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  keepBuiltinLedOff();

  Serial.println();
  Serial.println("=================================");
  Serial.println("STOPP / GÅ - MOTTAKER STARTET");
  Serial.println("Kort nr. 2");
  Serial.print("Mottaker MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println("LED-data på GPIO 4");
  Serial.println("13 stk WS2812B");
  Serial.println("Innebygd LED forsøkes holdt AV");
  Serial.println("=================================");

  if (esp_now_init() != ESP_OK) {
    Serial.println("FEIL: Kunne ikke starte ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onDataReceive);

  mode = 0;
  keepBuiltinLedOff();
}

void loop() {
  keepBuiltinLedOff();

  if (mode == 0) {
    showRainbow();
  }

  keepBuiltinLedOff();
}
