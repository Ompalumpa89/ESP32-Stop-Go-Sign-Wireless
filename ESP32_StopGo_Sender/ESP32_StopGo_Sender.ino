#include <WiFi.h>
#include <esp_now.h>

/*
  STOPP / GÅ - SENDER
  Kort nr. 1
  MAC: E8:31:CD:D6:EF:C8

  Bruker BOOT-knappen på ESP32-kortet.
  BOOT = GPIO 0

  Sender til kort nr. 2:
  MAC: 0C:DC:7E:62:9E:40

  Modus:
  0 = regnbue
  1 = grønn
  2 = rød
  3 = av
*/

uint8_t receiverMac[] = {0x0C, 0xDC, 0x7E, 0x62, 0x9E, 0x40};

const int BUTTON_PIN = 0;
const unsigned long DEBOUNCE_DELAY = 70;

uint8_t mode = 0;

bool lastReading = HIGH;
bool buttonHandled = false;
unsigned long lastDebounceTime = 0;

typedef struct Message {
  uint8_t mode;
} Message;

Message outgoingMessage;

void printMode(uint8_t selectedMode) {
  if (selectedMode == 0) Serial.print("REGNBUE");
  if (selectedMode == 1) Serial.print("GRØNN");
  if (selectedMode == 2) Serial.print("RØD");
  if (selectedMode == 3) Serial.print("AV");
}

void onDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("Sendestatus: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FEIL");
}

void sendMode() {
  outgoingMessage.mode = mode;

  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage));

  Serial.print("Sender modus ");
  Serial.print(mode);
  Serial.print(" = ");
  printMode(mode);
  Serial.print(" | Resultat: ");
  Serial.println(result == ESP_OK ? "SENDT" : "FEIL");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println();
  Serial.println("=================================");
  Serial.println("STOPP / GÅ - SENDER STARTET");
  Serial.println("Kort nr. 1");
  Serial.print("Sender MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println("Knapp: BOOT-knapp på GPIO 0");
  Serial.println("Sender til kort nr. 2");
  Serial.println("=================================");

  if (esp_now_init() != ESP_OK) {
    Serial.println("FEIL: Kunne ikke starte ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("FEIL: Kunne ikke legge til mottaker");
    return;
  }

  sendMode();
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading == LOW && !buttonHandled) {
      mode++;

      if (mode > 3) {
        mode = 0;
      }

      sendMode();
      buttonHandled = true;
    }

    if (reading == HIGH) {
      buttonHandled = false;
    }
  }

  lastReading = reading;
}
