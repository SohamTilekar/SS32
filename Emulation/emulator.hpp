#include "logger.cpp"
#include <bitset>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>

using Word = std::bitset<32>;

struct Port {
    uint8_t decode_data : 3;
    Word data1;
    Word data2;
    std::bitset<24> **VidArray;
    Logger *logger;
    Port(Logger *log);
    ~Port();
    void portCycle();
};

struct RAM {
    Word *data;

    RAM() {
        data = new Word[16777216];
        // Initialize RAM with random data
        for (int i = 0; i < 16777216; i++) {
            data[i] = Word(rand());
        }
    }

    ~RAM() { delete[] data; }
};
struct Register {
    Word data;
};

struct RegisterBank {
    Register registers[16];
    Register PC;
};

struct CPU {
    Logger *logger;
    RegisterBank registers;
    RAM memory;
    Port ports;
    std::bitset<3> flags; // 0: Barrow, 1: Carry, 2: Compare False
    void ExecInstr();
    CPU(Logger *log);
    void Execute();
};

void StartVideoDisplay(std::bitset<24> **videoData);