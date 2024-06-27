// Displaying a colour gradient to the screen
ST R0, 0x7fffA0
line_loop:
    LDI N2, 1
    MOV R1, N1
    ADI
    MOV A, R1
    Mov R0, N2
    MULUI
    MOV A, R2
    ST R1, 0x7fffA1
    ST R2, 0x7fffA2
    IOCS
    LDI N2, 256
    CLIUI
    JPFCMP line_loop
MOV R0, N1
LDI N2, 1
ADI
MOV A, R0
ST R0, 0x7fffA0
LDI, N2, 256
CLIUI
JPFCMP line_loop
HLT