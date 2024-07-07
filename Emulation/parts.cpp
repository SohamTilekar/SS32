#include "emulator.hpp"

Port::Port(Logger *log) {
    decode_data = 0;
    data1 = 0;
    data2 = 0;
    VidArray = new std::bitset<24> *[256];
    for (int i = 0; i < 256; i++) {
        VidArray[i] = new std::bitset<24>[256];
    }
    logger = log;
}

Port::~Port() { delete[] VidArray; }

void Port::portCycle() {
    switch (decode_data) {
    case 0: {
        logger->log("Port Disable" + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 1: {
        logger->log("Port 1: " + data1.to_string() + ": " + data2.to_string() +
                    "\n");
        unsigned short x = (data1.to_ulong() & 0xFF);
        unsigned short y = (data1.to_ulong() & 0xFF00) >> 8;
        unsigned int color = (data2.to_ulong() & 0xFFFFFF);
        VidArray[y][x] = color;
        std::this_thread::sleep_for(std::chrono::seconds(0));
        break;
    }
    case 2: {
        logger->log("Port 2: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 3: {
        logger->log("Port 3: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 4: {
        logger->log("Port 4: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 5: {
        logger->log("Port 5: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 6: {
        logger->log("Port 6: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 7: {
        logger->log("Port 7: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    default:
        logger->log("Unknown Port\n");
        break;
    }
};
