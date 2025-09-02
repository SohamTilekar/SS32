# SS32 CPU
## Overview
The SS32 CPU is a custom-designed minicomputer implemented with TTL. It features a 32-bit data width, 32-bit register width, and a 23-bit address width. The CPU supports a multi-bus architecture and a 9-bit instruction width.
![CPU Internaly](./examples/CPU.png)
![Mini Computer With SS32 CPU, Rendering the SnailByte Logo](./examples/Computer.png)

## Components
The SS32 CPU is in the Shit Development, With Many Bugs in the Logisim, Non Latest Assembler, No Documentation, etc.

# Assembler
Assembler Made Using the CustomASM in `assembler/astCPU.asm`.
Just Use CustomASM with the asm file & `astCPU.asm` file

# Emulator
The emulator simulates the SS32 CPU, allowing you to run and test programs on your computer.
![Emulator Rendering Patern 1](./examples/Emulator-Rendering-Patern-1.png)
![Emulator Rendering Patern 2](./examples/Emulator-Rendering-Patern-2.png)

# Examples
The examples directory contains example programs that can be run on the SS32 CPU.

# Building the Emulator
To build the emulator, navigate to the `emulator` directory and run:
```bash
cargo build --release
```
