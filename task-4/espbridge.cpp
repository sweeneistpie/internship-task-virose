#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define CHUNK_SIZE 240

uint8_t receiverMac[] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC};

typedef struct {
  uint16_t chunkId;
  uint16_t totalChunks;
  uint8_t data[CHUNK_SIZE];
} Packet;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Status pengiriman: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Berhasil" : "Gagal");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW gagal diinisialisasi");
    return;
  }
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  esp_now_register_send_cb(OnDataSent);
}

void loop() {
  if (Serial.available() >= 4) {
    Packet packet;
    Serial.readBytes((char*)&packet.chunkId, 2);
    Serial.readBytes((char*)&packet.totalChunks, 2);
    int readLen = Serial.readBytes((char*)packet.data, CHUNK_SIZE);
    if (readLen > 0) {
      esp_now_send(receiverMac, (uint8_t*)&packet, sizeof(Packet));
      Serial.printf("Kirim chunk %d/%d\n", packet.chunkId + 1, packet.totalChunks);
    }
  }
}
