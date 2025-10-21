# Dokumentasi Task 4
## Deskripsi
Proyek ini bertujuan mengirim data dari laptop menuju ESP Receiver melalui ESP Bridge. ESP Receiver akan menampilkan output data tersebut melalui serial monitor.
## Diagram Alur Kode
Laptop → ESP-Bridge → ESP-Receiver → Serial Monitor
## Struktur Folder dan File
TASK 4/                                                                                           
├── lib/                                                                                          
│   ├── serial/                                                                                  
│       ├── include/                                                                              
│           ├── serial.h                                                                          
├── src/                                                                                          
│   ├── CMakeLists.txt                                                                            
│   ├── data.json                                                                                 
│   ├── main.cpp                                                                                  
│   ├── espbridge.cpp                                                                             
    └── espreceiver.cpp
## Cara Kerja Kode
1. Program membaca file **`data.json`** dalam mode binary
   ```cpp
   std::ifstream file("data.json", std::ios::binary);
   if (!file.is_open()) {
    std::cerr << "Gagal membuka file data.json\n";
    return 1;
   }
   std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(file)), {});
   file.close();
   ```
   file tersebut berisi sebagai berikut
   ```json
   {
    "nama": "Bandung Bondowoso",
    "jurusan": "Teknik Mesin",
    "umur": 20,
    "deskripsi": "Saya adalah pribadi pemberani dan suka tantangan. Saya aktif dalam proyek mobil hemat energi, menjadi ketua divisi mekanik, dan senang belajar tentang sistem mesin modern."
   }
   ```
2. File yang berukuran 240 byte dipecah menjadi beberapa bagian
   ```cpp
   size_t totalChunks = (fileData.size() + CHUNK_SIZE - 1) / CHUNK_SIZE;
   std::cout << "Mengirim " << totalChunks << " potongan data..." << std::endl;

3. Pecahan dari file dikirim secara berurutan ke ESP-Bridge
   ```cpp
   for (size_t i = 0; i < totalChunks; i++) {
    uint16_t chunkId = i;
    uint16_t total = totalChunks;

    size_t start = i * CHUNK_SIZE;
    size_t end = std::min(start + CHUNK_SIZE, fileData.size());
    std::vector<uint8_t> packet(fileData.begin() + start, fileData.begin() + end);

    esp.write(reinterpret_cast<uint8_t*>(&chunkId), 2);
    esp.write(reinterpret_cast<uint8_t*>(&total), 2);
    esp.write(packet);

    std::cout << "Kirim chunk " << i + 1 << "/" << totalChunks << std::endl;
    serial::Timeout::simpleTimeout(50);
   }
   ```
4. Dari ESP-Bridge dikirim ke ESP-Receiver melalui ESP NOW
   ```cpp
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
5. Receiver menerima chunk, menyusunnya kembali, dan menyimpan hasilnya
   ```cpp
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
6. Output Kode
   ```
   [KONTEN FILE YANG DITERIMA]
   NAMA: Emir Chaesa
   JURUSAN: Teknik Material Metalurgi
   UMUR: 30
   DESKRIPSI DIRI: Saya adalah orang yang sangat disiplin dan rendah hati. Saya juga senang bermain bola di lapangan. Saya Juga General Manager dari tim VI-ROSE.
```
