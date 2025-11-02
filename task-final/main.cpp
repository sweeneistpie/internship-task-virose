#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <esp_err.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define MAC_ADDRESS_TOTAL 12
#define MAC_ADDRESS_LENGTH 6
#define MAC_INDEX_KU 4

uint8_t mac_addresses[MAC_ADDRESS_TOTAL][MAC_ADDRESS_LENGTH] = {
    {0x24, 0x0A, 0xC4, 0x0A, 0x10, 0x10},  // Fauzan Firdaus
    {0x24, 0x0A, 0xC4, 0x0A, 0x10, 0x11},  // Africha Sekar wangi
    {0x24, 0x0A, 0xC4, 0x0A, 0x11, 0x10},  // Rafaina Erin Sadia
    {0x24, 0x0A, 0xC4, 0x0A, 0x11, 0x11},  // Antonius Michael Yordanis Hartono
    {0x24, 0x0A, 0xC4, 0x0A, 0x12, 0x10},  // Dinda Sofi Azzahro
    {0x24, 0x0A, 0xC4, 0x0A, 0x12, 0x11},  // Muhammad Fahmi Ilmi
    {0x24, 0x0A, 0xC4, 0x0A, 0x13, 0x10},  // Dhanishara Zaschya Putri Syamsudin
    {0x24, 0x0A, 0xC4, 0x0A, 0x13, 0x11},  // Irsa Fairuza
    {0x24, 0x0A, 0xC4, 0x0A, 0x14, 0x10},  // Revalinda Bunga Nayla Laksono
    {0x24, 0x0A, 0xC4, 0x0A, 0x21, 0x11},  // BISMA
    {0x24, 0x0A, 0xC4, 0x0A, 0x21, 0x10},  // JSON
    {0x24, 0x0A, 0xC4, 0x0A, 0x20, 0x11},  // FARUG
};

const char* mac_names[MAC_ADDRESS_TOTAL] = {
    "Fauzan Firdaus",                      // 0
    "Africha Sekar Wangi",                 // 1
    "Rafaina Erin Sadia",                  // 2
    "Antonius Michael Yordanis Hartono",   // 3
    "Dinda Sofi Azzahro",                  // 4
    "Muhammad Fahmi Ilmi",                 // 5
    "Dhanishara Zaschya Putri Syamsudin",  // 6
    "Irsa Fairuza",                        // 7
    "Revalinda Bunga Nayla Laksono",       // 8
    "BISMA",                               // 9
    "JSON",                                // 10
    "FARUG",                               // 11
};

typedef enum {
    FAUZAN_FIRDAUS = 0,
    AFRICHA_SEKAR_WANGI,
    RAFAINA_ERIN_SADIA,
    ANTONIUS_MICHAEL_YORDANIS_HARTONO,
    DINDA_SOFI_AZZAHRO,
    MUHAMMAD_FAHMI_ILMI,
    DHANISHARA_ZASCHYA_PUTRI_SYAMSUDIN,
    IRSA_FAIRUZA,
    REVALINDA_BUNGA_NAYLA_LAKSONO,
    BISMA,
    JSON,
    FARUG,
} ADDRESS_ASSIGNMENT;

void onDataRecv(const uint8_t* mac, const uint8_t* data, int len) {
    if (len < 1) return;

        char c = (char)data[0];
        Serial.write(c);
        Serial.write('\n');     
    }


void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    esp_wifi_set_mac(WIFI_IF_STA, mac_addresses[MAC_INDEX_KU]);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Gagal inisialisasi ESP-NOW!");
        while (1) delay(1000);
    }

    esp_now_register_recv_cb(onDataRecv);

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    for (int i = 0; i < MAC_ADDRESS_TOTAL; i++) {
        if (i == MAC_INDEX_KU) continue;
        memcpy(peerInfo.peer_addr, mac_addresses[i], MAC_ADDRESS_LENGTH);
        esp_now_add_peer(&peerInfo);
    }

    Serial.println("Siap menerima data ASCII");
}

void loop() {
    delay(10);
}
