#include "emulator.hpp"

CPU::CPU(Logger *logger) : logger(logger), ports(logger) {
    registers.PC.data = 0;
    flags = 0;
}

void CPU::ExecInstr() {
    // Fetch
    Word instr = memory.data[registers.PC.data.to_ulong()];
    if ((instr >> 28).to_ulong() == 0) {
        // NOP
        logger->log("NOP\n");
        exit(0);
    } else if ((instr >> 28).to_ulong() == 1) {
        // ALUCalc
        logger->log("ALUCalc: ");
        unsigned short alu_op_code =
            (instr >> 12).to_ulong() & 0xF; // Extract bits 17-20
        unsigned short SR1 = (instr >> 24).to_ulong() & 0xF; // Extract bits 5-8
        unsigned short SR2 =
            (instr >> 20).to_ulong() & 0xF; // Extract bits 9-12
        unsigned short DR =
            (instr >> 16).to_ulong() & 0xF; // Extract bits 13-16
        switch (alu_op_code) {
        case 0: { // Add
            logger->log("Add : SR1 = " + std::to_string(SR1) + ", SR2 = " +
                        std::to_string(SR2) + ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                registers.registers[SR1].data.to_ulong() +
                registers.registers[SR2].data.to_ulong();
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            flags[1] = (registers.registers[DR].data.to_ulong() > UINT32_MAX);
            break;
        }
        case 1: { // Subtract
            logger->log("Subtract : SR1 = " + std::to_string(SR1) + ", SR2 = " +
                        std::to_string(SR2) + ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                registers.registers[SR1].data.to_ulong() -
                registers.registers[SR2].data.to_ulong();
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            flags[0] = (registers.registers[DR].data.to_ulong() < 0);
            break;
        }
        case 2: { // Multiply
            logger->log("Multiply : SR1 = " + std::to_string(SR1) + ", SR2 = " +
                        std::to_string(SR2) + ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                registers.registers[SR1].data.to_ulong() *
                registers.registers[SR2].data.to_ulong();
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 3: { // Nigate
            logger->log("Negate : SR1 = " + std::to_string(SR1) +
                        ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                registers.registers[SR1].data.to_ulong() * -1;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 4: { // AND
            logger->log("And : SR1 = " + std::to_string(SR1) + ", SR2 = " +
                        std::to_string(SR2) + ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                registers.registers[SR1].data & registers.registers[SR2].data;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 5: { // OR
            logger->log("Or : SR1 = " + std::to_string(SR1) + ", SR2 = " +
                        std::to_string(SR2) + ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                registers.registers[SR1].data | registers.registers[SR2].data;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 6: { // NOT
            logger->log("Not : SR1 = " + std::to_string(SR1) +
                        ", DR = " + std::to_string(DR));
            registers.registers[DR].data = ~registers.registers[SR1].data;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 7: { // NAND
            logger->log("Nand : SR1 = " + std::to_string(SR1) + ", SR2 = " +
                        std::to_string(SR2) + ", DR = " + std::to_string(DR));
            registers.registers[DR].data = ~(registers.registers[SR1].data &
                                             registers.registers[SR2].data);
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 8: { // XOR
            logger->log("Xor : SR1 = " + std::to_string(SR1) + ", SR2 = " +
                        std::to_string(SR2) + ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                registers.registers[SR1].data ^ registers.registers[SR2].data;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 9: { // XNOR
            logger->log("Xnor : SR1 = " + std::to_string(SR1) + ", SR2 = " +
                        std::to_string(SR2) + ", DR = " + std::to_string(DR));
            registers.registers[DR].data = ~(registers.registers[SR1].data ^
                                             registers.registers[SR2].data);
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 10: { // Shift Left
            logger->log("Shift Left : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2) +
                        ", DR = " + std::to_string(DR));
            registers.registers[DR].data = registers.registers[SR1].data << SR2;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 11: { // Shift Right
            logger->log("Shift Right : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2) +
                        ", DR = " + std::to_string(DR));
            registers.registers[DR].data = registers.registers[SR1].data >> SR2;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 12: { // Arithmetic Shift Right
            logger->log("Arithmetic Shift Right : SR1 = " +
                        std::to_string(SR1) + ", SR2 = " + std::to_string(SR2) +
                        ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                (registers.registers[SR1].data.to_ulong() >> SR2) & 0xFFFFFFFF;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 13: { // Rotate Left
            logger->log("Rotate Left : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2) +
                        ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                (registers.registers[SR1].data << SR2) |
                (registers.registers[SR1].data >> (32 - SR2));
            logger->log("Rotate Right : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2) +
                        ", DR = " + std::to_string(DR));
            registers.registers[DR].data =
                (registers.registers[SR1].data >> SR2) |
                (registers.registers[SR1].data << (32 - SR2));
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        case 15: { // 0
            logger->log("0 : DR = " + std::to_string(DR));
            registers.registers[DR].data = 0;
            logger->log(
                ", Result = " +
                std::to_string(registers.registers[DR].data.to_ulong()) + "\n");
            break;
        }
        default:
            logger->log("Unknown ALU operation\n");
            registers.registers[DR].data = 0;
            break;
        }
    } else if ((instr >> 28).to_ulong() == 2) {
        // Compare
        logger->log("Compare: ");
        unsigned short alu_op_code =
            (instr >> 12).to_ulong() & 0xF; // Extract bits 17-20
        unsigned short SR1 = (instr >> 24).to_ulong() & 0xF; // Extract bits 5-8
        unsigned short SR2 =
            (instr >> 20).to_ulong() & 0xF; // Extract bits 9-12
        switch (alu_op_code) {
        case 0: { // Greater Than
            logger->log("Greater Than : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2));
            flags[2] = (registers.registers[SR1].data.to_ulong() >
                        registers.registers[SR2].data.to_ulong());
            logger->log(", Result = " + std::to_string(flags[2]));
            break;
        }
        case 1: { // Equal
            logger->log("Equal : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2));
            flags[2] = (registers.registers[SR1].data ==
                        registers.registers[SR2].data);
            logger->log(", Result = " + std::to_string(flags[2]));
            break;
        }
        case 2: { // Less Than
            logger->log("Less Than : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2));
            flags[2] = (registers.registers[SR1].data.to_ulong() <
                        registers.registers[SR2].data.to_ulong());
            logger->log(", Result = " + std::to_string(flags[2]));
            break;
        }
        case 3: { // Greater Than or Equal
            logger->log("Greater Than or Equal : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2));
            flags[2] = (registers.registers[SR1].data.to_ulong() >=
                        registers.registers[SR2].data.to_ulong());
            logger->log(", Result = " + std::to_string(flags[2]));
            break;
        }
        case 4: { // Less Than or Equal
            logger->log("Less Than or Equal : SR1 = " + std::to_string(SR1) +
                        ", SR2 = " + std::to_string(SR2));
            flags[2] = (registers.registers[SR1].data.to_ulong() <=
                        registers.registers[SR2].data.to_ulong());
            logger->log(", Result = " + std::to_string(flags[2]));
            break;
        }
        default:
            logger->log("Unknown Compare operation");
            flags[2] = 0;
            break;
        }
    } else if ((instr >> 28).to_ulong() == 3) {
        // Jump
        logger->log("Jump: ");
        unsigned short jump_if =
            (instr >> 24).to_ulong() & 0xF; // Extract bits 5-8
        logger->log("Jump if " + std::to_string(jump_if));
        unsigned short jump_to =
            (instr >> 20).to_ulong() & 0xF; // Extract bits 9-12
        if (jump_if == 0) {
            // Jump
            logger->log(", to " + std::to_string(jump_to));
            registers.PC.data =
                (8 << registers.registers[jump_to].data.to_ulong()) >> 8;
        } else if (jump_if == 1 && flags[1] == 1) {
            // Jump if Carry
            logger->log("Jump if Carry to " + std::to_string(jump_to));
            registers.PC.data =
                (8 << registers.registers[jump_to].data.to_ulong()) >> 8;
        } else if (jump_if == 2 && flags[2] == 1) {
            // Jump if Compare True
            logger->log("Jump if Compare True to " + std::to_string(jump_to));
            registers.PC.data =
                (8 << registers.registers[jump_to].data.to_ulong()) >> 8;
        } else if (jump_if == 3 && flags[2] == 0) {
            // Jump if Compare False
            logger->log("Jump if Compare False to " + std::to_string(jump_to));
            registers.PC.data =
                (8 << registers.registers[jump_to].data.to_ulong()) >> 8;
        } else {
            logger->log("Unknown jump condition");
            logger->log("Halt");
            exit(0);
        }
    } else if ((instr >> 28).to_ulong() == 4) {
        // Load
        unsigned short DR = (instr >> 28).to_ulong() & 0xF; // Extract bits 5-8
        unsigned long address = ((instr << 8) >> 8).to_ulong();
        logger->log("Load: DR = " + std::to_string(DR) +
                    ", Address = " + std::to_string(address));
        registers.registers[DR].data = memory.data[address];
    } else if ((instr >> 28).to_ulong() == 5) {
        // Load From Register
        unsigned short SR = (instr >> 20).to_ulong() & 0xF; // Extract bits 9-12
        unsigned short DR =
            (instr >> 16).to_ulong() & 0xF; // Extract bits 12-15
        logger->log("Load From Register: SR = " + std::to_string(SR) +
                    ", DR = " + std::to_string(DR));
        registers.registers[DR].data =
            memory.data[((registers.registers[SR].data << 8) >> 8).to_ulong()];
    } else if ((instr >> 28).to_ulong() == 6) {
        // Load Immediate
        unsigned short DR = (instr >> 24).to_ulong() & 0xF; // Extract bits 5-8
        unsigned long immediate = ((instr << 8) >> 8).to_ulong();
        registers.registers[DR].data = immediate;
        logger->log("Load Immediate: DR = " + std::to_string(DR) +
                    ", Immediate = " + std::to_string(immediate));
    } else if ((instr >> 28).to_ulong() == 7) {
        // Store
        unsigned short SR = (instr >> 24).to_ulong() & 0xF; // Extract bits 5-8
        unsigned long address = ((instr << 8) >> 8).to_ulong();
        logger->log("Store: SR = " + std::to_string(SR) +
                    ", Address = " + std::to_string(address));
        memory.data[address] = registers.registers[SR].data;
    } else if ((instr >> 28).to_ulong() == 8) {
        // Store From Register
        unsigned short SR = (instr >> 24).to_ulong() & 0xF; // Extract bits 5-8
        unsigned short AdrR =
            (instr >> 20).to_ulong() & 0xF; // Extract bits 9-12
        logger->log("Store From Register: SR = " + std::to_string(SR) +
                    ", AdrR = " + std::to_string(AdrR));
        memory.data[((registers.registers[AdrR].data << 8) >> 8).to_ulong()] =
            registers.registers[SR].data;
    } else if ((instr >> 28).to_ulong() == 9) {
        // Move
        unsigned short SR = (instr >> 24).to_ulong() & 0xF; // Extract bits 5-8
        unsigned short DR =
            (instr >> 16).to_ulong() & 0xF; // Extract bits 12-15
        logger->log("Move: SR = " + std::to_string(SR) +
                    ", DR = " + std::to_string(DR));
        registers.registers[DR].data = registers.registers[SR].data;
    } else if ((instr >> 28).to_ulong() == 10) {
        // OutputPortWrite
        unsigned short SR = (instr >> 24).to_ulong() & 0xF; // Extract bits 5-8
        unsigned short DecodeData = (instr >> 9).to_ulong() & 0x7;
        unsigned short WD = (instr >> 8).to_ulong() & 0x1;
        unsigned short WDD = (instr >> 7).to_ulong() & 0x1;
        logger->log("OutputPortWrite: SR = " + std::to_string(SR) +
                    ", DecodeData = " + std::to_string(DecodeData) +
                    ", WD = " + std::to_string(WD) +
                    ", WDD = " + std::to_string(WDD) + '\n');
        if (WDD == 0) {
            if (WD == 0) {
                ports.data1 = registers.registers[SR].data;
            } else {
                ports.data2 = registers.registers[SR].data;
            }
        } else {
            ports.decode_data = DecodeData;
        }
        ports.portCycle();
    } else if ((instr >> 28).to_ulong() == 11) {
        logger->log("Halt\n");
        exit(0);
    } else if ((instr >> 28).to_ulong() == 12) {
        logger->log("Halt\n");
        exit(0);
    } else if ((instr >> 28).to_ulong() == 13) {
        logger->log("Halt\n");
        exit(0);
    } else if ((instr >> 28).to_ulong() == 14) {
        logger->log("Halt\n");
        exit(0);
    } else if ((instr >> 28).to_ulong() == 15) {
        logger->log("Halt\n");
        exit(0);
    } else {
        logger->log("Unknown instruction\nHalt\n");
        exit(0);
    }
    registers.PC.data = registers.PC.data.to_ulong() + 1;
    logger->log("PC: " + std::to_string(registers.PC.data.to_ulong()) + "\n\n");
};

void CPU::Execute() {
    while (true) {
        ExecInstr();
    }

    throw std::runtime_error("Reached end of memory");
};

// Function to swap byte order
uint32_t swap_endian(uint32_t value) {
    return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) |
           ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
}

int main(int argc, char *argv[]) {
    std::cout << "Emulation" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " <RAM_file_path> [-L <LogFile_path>]" << std::endl;
        return 1;
    }

    const char *ram_file_path = argv[1];
    const char *log_file_path = "cpu.log"; // Default log file path
    bool log_flag_provided = false;

    if (argc > 2 && std::strcmp(argv[2], "-L") == 0) {
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0]
                      << " <RAM_file_path> [-L <LogFile_path>]" << std::endl;
            return 1;
        }
        log_file_path = argv[3];
        log_flag_provided = true;
    }

    Logger logger(log_file_path, log_flag_provided);
    CPU cpu(&logger);

    std::cout << "Loading RAM file: " << ram_file_path << std::endl;
    std::ifstream file(ram_file_path, std::ios::binary | std::ios::in);
    if (!file) {
        std::cerr << "Unable to open RAM file" << std::endl;
        return 1;
    }

    size_t index = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (index >= 16777216) {
            std::cerr << "Exceeded maximum RAM size" << std::endl;
            break;
        }

        // Convert the hex string to a 32-bit unsigned integer
        uint32_t value = std::stoul(line, nullptr, 16);

        // Store the value in RAM at the corresponding index
        cpu.memory.data[index++] = Word(value);
    }

    file.close();
    std::thread videoDisplayThread(StartVideoDisplay, cpu.ports.VidArray);
    videoDisplayThread.detach();

    cpu.Execute();
    std::cout << "End of program" << std::endl;
    return 0;
}
