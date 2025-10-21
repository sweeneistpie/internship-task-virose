#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <serial/serial.h>

#define CHUNK_SIZE 240 

int main() {
    std::string port;
    std::cout << "Masukkan port serial: ";
    std::cin >> port;

    try {
        serial::Serial esp(port, 115200, serial::Timeout::simpleTimeout(2000));
        std::ifstream file("data.json", std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Gagal membuka file data.json\n";
            return 1;
        }

        std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(file)), {});
        file.close();

        size_t totalChunks = (fileData.size() + CHUNK_SIZE - 1) / CHUNK_SIZE;
        std::cout << "Mengirim " << totalChunks << " potongan data..." << std::endl;

        for (size_t i = 0; i < totalChunks; i++) {
            uint16_t chunkId = i;
            uint16_t total = totalChunks;

            size_t start = i * CHUNK_SIZE;
            size_t end = std::min(start + CHUNK_SIZE, fileData.size());
            std::vector<uint8_t> packet(fileData.begin() + start, fileData.begin() + end);

            // Header 4 byte (2 byte id, 2 byte total)
            esp.write(reinterpret_cast<uint8_t*>(&chunkId), 2);
            esp.write(reinterpret_cast<uint8_t*>(&total), 2);
            esp.write(packet);

            std::cout << "Kirim chunk " << i + 1 << "/" << totalChunks << std::endl;
            serial::Timeout::simpleTimeout(50);
        }

        std::cout << "Selesai mengirim file!" << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
