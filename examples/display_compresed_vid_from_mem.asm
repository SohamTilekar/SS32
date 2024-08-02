; R0: Px color Data
; R1: Codinated Data
; R2: Address
; R3: Repitation
; R4: 1
; R5: 24

LDI R2 - offset
LDI R4 - 1
LDI R5 - 24
OPW-En - 1
loop:
    LD R0 - R2
    ADD R1 - R1, R4
    ADD R2 - R2, R4
    LD R3 - R2
    RS R3 - R3, R5
    OPD1W - R1
    OPD2W - R0
    innerloop:
        LD R0 - R2
        ADD R1 - R1, R4
        ADD R2 - R2, R4
        OPD1W - R1
        OPD2W - R0
        SUB R3 - R3, R4
        JP-Zr innerloop
    JMP loop
offset:
#d 0x00000000

00000000
62000013
64000001
65000018
a0000280
50200000
11410000
12420000
50230000
1353b000
a1000000
a0000100
50200000
11410000
12420000
a1000000
a0000100
13431000
3d00000b
38000004
00000000