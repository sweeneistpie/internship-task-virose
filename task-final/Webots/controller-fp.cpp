#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include "serial/serial.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace webots;
using namespace rapidjson;
using namespace std;

void playPoseFromJSON(const string &filename, map<string, Motor*> &motors, int timeStep, Robot *robot) {
    ifstream file("../../poses/" + filename);
    if (!file.is_open()) {
        cerr << " Gagal buka file: " << filename << endl;
        return;
    }

    string jsonStr((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    Document doc;
    doc.Parse(jsonStr.c_str());
    if (!doc.IsObject() || !doc.HasMember("pose_group")) return;

    const Value &groups = doc["pose_group"];
    for (SizeType g = 0; g < groups.Size(); g++) {
        const Value &poseArray = groups[g]["pose"];
        for (SizeType p = 0; p < poseArray.Size(); p++) {
            const Value &positions = poseArray[p]["posisi"];
            int i = 0;
            for (auto &motorPair : motors) {
                if (i >= positions.Size()) break;
                motorPair.second->setPosition(positions[i].GetDouble());
                i++;
            }
            robot->step(timeStep);
        }
    }
}

int main() {
    Robot *robot = new Robot();
    int timeStep = robot->getBasicTimeStep();

    std::map<std::string, Motor*> motors = {
        {"Head", robot->getMotor("Head")},
        {"Neck", robot->getMotor("Neck")},
        {"ShoulderL", robot->getMotor("ShoulderL")},
        {"ShoulderR", robot->getMotor("ShoulderR")},
        {"ArmUpperL", robot->getMotor("ArmUpperL")},
        {"ArmUpperR", robot->getMotor("ArmUpperR")},
        {"ArmLowerL", robot->getMotor("ArmLowerL")},
        {"ArmLowerR", robot->getMotor("ArmLowerR")},
        {"PelvYL", robot->getMotor("PelvYL")},
        {"PelvYR", robot->getMotor("PelvYR")},
        {"PelvL", robot->getMotor("PelvL")},
        {"PelvR", robot->getMotor("PelvR")},
        {"LegUpperL", robot->getMotor("LegUpperL")},
        {"LegUpperR", robot->getMotor("LegUpperR")},
        {"LegLowerL", robot->getMotor("LegLowerL")},
        {"LegLowerR", robot->getMotor("LegLowerR")},
        {"AnkleL", robot->getMotor("AnkleL")},
        {"AnkleR", robot->getMotor("AnkleR")},
        {"FootL", robot->getMotor("FootL")},
        {"FootR", robot->getMotor("FootR")}
    };

    serial::Serial espSerial("COM3", 115200, serial::Timeout::simpleTimeout(1000));

    if (espSerial.isOpen())
        std::cout << " Serial connected to ESP32 Receiver!" << std::endl;
    else {
        std::cerr << " Gagal membuka port serial!" << std::endl;
        return 1;
    }

    while (robot->step(timeStep) != -1) {
        if (espSerial.available()) {
            std::string data = espSerial.readline();
            data.erase(remove(data.begin(), data.end(), '\r'), data.end());
            data.erase(remove(data.begin(), data.end(), '\n'), data.end());

            if (data.empty()) continue;

            std::cout << " Data diterima: " << data << std::endl;

            std::string filename;

            if (data == "W") filename = "pose_maju.json";
            else if (data == "S") filename = "pose_mundur.json";
            else if (data == "A") filename = "pose_geser_kiri.json";
            else if (data == "D") filename = "pose_geser_kanan.json";
            else if (data == "Q") filename = "pose_belok-kiri.json";
            else if (data == "E") filename = "pose_belok-kanan.json";
            else {
                std::cout << "Perintah tidak dikenal: " << data << std::endl;
                continue;
            }
        }
    }
    delete robot;
    return 0;
}