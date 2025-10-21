#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <SPIFFS.h>

#define CHUNK_SIZE 240

typedef struct {
  uint16_t chunkId;
  uint16_t totalChunks;
  uint8_t data[CHUNK_SIZE];
} Packet;

File outputFile;
uint16_t receivedChunks = 0;
uint16_t totalChunks = 0;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  Packet packet;
  memcpy(&packet, incomingData, sizeof(Packet));

  if (receivedChunks == 0) {
    totalChunks = packet.totalChunks;
    outputFile = SPIFFS.open("/received.json", FILE_WRITE);
    if (!outputFile) {
      Serial.println("Gagal membuka file di SPIFFS!");
      return;
    }
  }

  outputFile.write(packet.data, CHUNK_SIZE);
  receivedChunks++;
  Serial.printf("Diterima chunk %d/%d\n", packet.chunkId + 1, totalChunks);

  if (receivedChunks == totalChunks) {
    outputFile.close();
    Serial.println("File lengkap diterima dan disimpan sebagai /received.json");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal mount SPIFFS!");
    return;
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW gagal diinisialisasi!");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP Receiver siap menerima data...");
}

void loop() {}
