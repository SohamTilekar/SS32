#include <cstdint>
#include <float.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using int32 = std::int32_t;

struct Memory {
    uint32 data[2 ^ 23];
};

struct Instruction {
    unsigned int opcode : 9;
    unsigned int data : 23;
};

struct CPU {
    uint32 PC;

    uint32 N1, N2, A, IR, R0, R1, R2; // Registers

    // CPU Flags
    bool carry;
    bool comp;

    bool hlt;

    Memory *mem;

    CPU(Memory *mem) {
        reset();
        mem = mem;
    }

    void exec_inst() {
        if (PC >= 65536) {
            std::cout << "PC out of bounds" << std::endl;
            hlt = true;
            return;
        }
        IR = mem->data[PC];
        Instruction inst = *(Instruction *)&IR;
        PC++;
        switch (inst.opcode) {
        case 000000000: { // NOP
            break;
        }
        case 000000001: { // ADI
            A = N1 + N2;
            break;
        }
        case 000000010: { // SBI
            A = N1 - N2;
            break;
        }
        case 000000011: { // MULIU
            A = N1 * N2;
            break;
        }
        case 000000100: { // MULIS
            A = static_cast<uint32>(static_cast<int32>(N1) *
                                    static_cast<int32>(N2));
            break;
        }
        case 000000101: { // DIVIU
            A = N1 / N2;
            break;
        }
        case 000000110: { // DIVIS
            A = static_cast<uint32>(static_cast<int32>(N1) /
                                    static_cast<int32>(N2));
            break;
        }
        case 000000111: { // NIGATE
            A = ~N1;
            break;
        }
        case 000001000: { // LD, N1
            N1 = mem->data[inst.data];
            break;
        }
        case 000001001: { // LD, N2
            N2 = mem->data[inst.data];
            break;
        }
        case 000001010: { // LD, R0
            R0 = mem->data[inst.data];
            break;
        }
        case 000001011: { // LD, R1
            R1 = mem->data[inst.data];
            break;
        }
        case 000001100: { // LD, R2
            R2 = mem->data[inst.data];
            break;
        }
        case 000001101: {
            break;
        }
        case 000001110: {
            break;
        }
        case 000001111: {
            break;
        }
        case 000010000: { // ST, N1
            mem->data[inst.data] = N1;
            break;
        }
        case 000010001: { // ST, N2
            mem->data[inst.data] = N2;
            break;
        }
        case 000010010: { // ST, R0
            mem->data[inst.data] = R0;
            break;
        }
        case 000010011: { // ST, R1
            mem->data[inst.data] = R1;
            break;
        }
        case 000010100: { // ST, R2
            mem->data[inst.data] = R2;
            break;
        }
        case 000010101: { // ST, A
            mem->data[inst.data] = A;
            break;
        }
        case 000010110: {
            break;
        }
        case 000010111: {
            break;
        }
        case 000011000: { // LDI, N1
            N1 = inst.data;
            break;
        }
        case 000011001: { // LDI, N2
            N2 = inst.data;
            break;
        }
        case 000011010: { // LDI, R0
            R0 = inst.data;
            break;
        }
        case 000011011: { // LDI, R1
            R1 = inst.data;
            break;
        }
        case 000011100: { // LDI, R2
            R2 = inst.data;
            break;
        }
        case 000011101: {
            break;
        }
        case 000011110: {
            break;
        }
        case 000011111: {
            break;
        }
        case 000100000: { // ADF
            A = static_cast<uint32>(static_cast<float>(N1) +
                                    static_cast<float>(N2));
            break;
        }
        case 000100001: { // SBF
            A = static_cast<uint32>(static_cast<float>(N1) -
                                    static_cast<float>(N2));
            break;
        }
        case 000100010: { // MULF
            A = static_cast<uint32>(static_cast<float>(N1) *
                                    static_cast<float>(N2));
            break;
        }
        case 000100011: { // DIVF
            A = static_cast<uint32>(static_cast<float>(N1) /
                                    static_cast<float>(N2));
            break;
        }
        case 000100100: { // NF
            A = static_cast<uint32>(-static_cast<float>(N1));
            break;
        }
        case 000100101: { // UI2F
            A = static_cast<uint32>(float(static_cast<uint32>(N1)));
            break;
        }
        case 000100110: { // SI2F
            A = static_cast<uint32>(float(static_cast<int32>(N1)));
            break;
        }
        case 000100111: { // F2UI
            A = uint32(static_cast<float>(N1));
            break;
        }
        case 000101000: { // CGTUI
            comp = N1 > N2;
            break;
        }
        case 000101001: { // CEQUI
            comp = N1 == N2;
            break;
        }
        case 000101010: { // CLTUI
            comp = N1 < N2;
            break;
        }
        case 000101011: { // CGTSI
            comp = static_cast<int32>(N1) > static_cast<int32>(N2);
            break;
        }
        case 000101100: { // CEQSI
            comp = static_cast<int32>(N1) == static_cast<int32>(N2);
            break;
        }
        case 000101101: { // CLTSI
            comp = static_cast<int32>(N1) < static_cast<int32>(N2);
            break;
        }
        case 000101110: { // CGTF
            comp = static_cast<float>(N1) > static_cast<float>(N2);
            break;
        }
        case 000101111: { // CEQF
            comp = static_cast<float>(N1) == static_cast<float>(N2);
            break;
        }
        case 000110000: { // CLTF
            comp = static_cast<float>(N1) < static_cast<float>(N2);
            break;
        }
        case 000110001: { // JMP
            PC = inst.data;
            break;
        }
        case 000110010: { // JMPIFC
            if (carry) {
                PC = inst.data;
            }
            break;
        }
        }
    };

    void exec() {
        while (true) {
            exec_inst();
            if (hlt) {
                break;
            }
        }
    }

    void reset() {
        PC = 0;
        N1 = N2 = A = R0 = R1 = R2 = 0;
        carry = comp = false;
    }
};

int main() {
    Memory *mem = new Memory();
    CPU cpu(mem);
    delete mem;
    return 0;
}