=================================
===       SPECIFICATION       ===
=================================

==== Overview ====
* Minicomputer designed to be implemented with TTL
* 64 bit data width
* 64 bit register width
* 32 bit address width
* 32 bit instruction width
* Multi Bus architecture

{10}{4}{4}

=== Instructions ===
ID: 
    n1: 0000
    n2: 0001
    R0: 0010
    R1: 0011
    R2: 0100
    A: 0101

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
