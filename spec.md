=================================
===       SPECIFICATION       ===
=================================

==== Overview ====
* Minicomputer designed to be implemented with TTL
* 32 bit data width
* 32 bit register width
* 23 bit address width
* 9 bit instruction width
* Multi Bus architecture

=== Memory Mapping ===
Program Memory: 0x000000 - 0x7fff98
IO Memory: 0x7fffa0 - 0x7fffb8


=== Instructions ===
ID: 
    n1: 000
    n2: 001
    R0: 010
    R1: 011
    R2: 100
    A : 101

NOP : No Operation

LD{id} | {address} : Load
LDI{id} | {data} : Load Immediate
ST{id} | {address} : Store
MOV{id1}{id2} : Move

ADI : Add Integer

SBI : Subtract Integer

MULIU : Multiply Integer Unsigned
MULIS : Multiply Integer Signed

DIVIU : Divide Integer Unsigned
DIVIS : Divide Integer Signed

NI : Negate Integer

ADF : Add Floating Point

SBF : Subtract Floating Point

MULF : Multiply Floating Point

DIVF : Divide Floating Point

NF : Negate Floating Point

UITF : Unsigned Integer to Floating Point
SITF : Signed Integer to Floating Point
FTI : Floating Point to Integer

CGTUI : Compare Greater Than Unsigned Integer
CEQUI : Compare Equal Unsigned Integer
CLTUI : Compare Less Than Unsigned Integer
CGTSI : Compare Greater Than Signed Integer
CEQSI : Compare Equal Signed Integer
CLTSI : Compare Less Than Signed Integer

CEQF : Compare Equal Floating Point
CGTF : Compare Greater Than Floating Point
CLTF : Compare Less Than Floating Point

JMP : Jump
JMPIFC : Jump If Carry
JMPIFNC : Jump If Not Carry
// JMPIFZ : Jump If Zero
// JMPIFNZ : Jump If Not Zero
JMPIFCOM : Jump If Compare
JMPIFNCOM : Jump If Not Compare

=== Registers ====
* 9 registers
* 16 in planing

==== Special Purpose Registers ====
* PC : Program Counter # sudo Register
* IR : Instruction Register 16 bit instruction & 16 bit address, data, ...
* N1 : Number 1 Register for ALU
* N2 : Number 2 Register for ALU
* AOR : ALU Output Register
* MAR : Memory Address Register
* CFR : CPU Flags Register 3 bit flags
<!-- * HEX : Hexadecimal Display Register -->

==== General Purpose Registers ====
* R0 - R2 : 3 General Purpose Registers
