; R1: offset / address of the image pixel
; R2: 1
; R3: VM address
; R0: pix POS
; R4: pix data

LDI R1 - offset
LDI R2 - 1
LDI R3 - 0x00fb4fff
OPW-En - 1
loop:
    ADD R0 - R0, R2
    ADD R5 - R0, R3
    ADD R1 - R1, R2
    LD R4 - R1
    ST R4 - R5
    JMP loop
HLT
offset:
; Writing an Image data in the format of 0x00RRGGBB

00000000
6100000b
62000001
63fb4fff
a0000280
10200000
10350000
11210000
50140000
84500000
38000004
ffffffff