#include "emulator.hpp"

Port::Port(Logger *logger, bool log) {
    decode_data = 0;
    data1 = 0;
    data2 = 0;
    this->log = log;
    buffer = new uint8_t[WIDTH * HEIGHT * 4];
    this->logger = logger;
}

Port::~Port() { delete[WIDTH * HEIGHT * 4] buffer; }

void Port::portCycle() {
    switch (decode_data) {
    case 0: {
        if (log)
            logger->log("Port Disable" + std::to_string(data1.to_ulong()) +
                        "\n");
        break;
    }
    case 1: {
        if (log)
            logger->log("Port 1: " + data1.to_string() + ": " +
                        data2.to_string() + "\n");
        int index = ((((data1.to_ulong() & 0xFF00) >> 8) - 1) * WIDTH +
                     (data1.to_ulong() & 0xFF)) *
                    4;
        buffer[index] = data2.to_ulong() & 0xFF;             // Blue
        buffer[index + 1] = (data2.to_ulong() >> 8) & 0xFF;  // Green
        buffer[index + 2] = (data2.to_ulong() >> 16) & 0xFF; // Red
        buffer[index + 3] = 0xFF;                            // Alpha
        break;
    }
    case 2: {
        if (log)
            logger->log("Port 2: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 3: {
        if (log)
            logger->log("Port 3: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 4: {
        if (log)
            logger->log("Port 4: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 5: {
        if (log)
            logger->log("Port 5: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 6: {
        if (log)
            logger->log("Port 6: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    case 7: {
        if (log)
            logger->log("Port 7: " + std::to_string(data1.to_ulong()) + "\n");
        break;
    }
    default:
        if (log)
            logger->log("Unknown Port\n");
        break;
    }
};
