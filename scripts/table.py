IMPL = "IMPLICIT"

ACCU = "ACCUMULATOR"

IMMD = "IMMEDIATE"

ZRPG = "ZERO_PAGE"
ZRPX = "ZERO_PAGE_X"
ZRPY = "ZERO_PAGE_Y"

RELA = "RELATIVE"

ABSO = "ABSOLUTE"
ABSX = "ABSOLUTE_X"
ABSY = "ABSOLUTE_Y"

INDR = "INDIRECT"

INDX = "INDEXED_INDIRECT"
INDY = "INDIRECT_INDEXED"

instructions = [
    # 0x0x
    ("BRK", IMPL),
    ("ORA", INDX),
    (None, None),
    (None, None),
    (None, None),
    ("ORA", ZRPG),
    ("ASL", ZRPG),
    (None, None),
    ("PHP", IMPL),
    ("ORA", IMMD),
    ("ASL", ACCU),
    (None, None),
    (None, None),
    ("ORA", ABSO),
    ("ASL", ABSO),
    (None, None),

    # 0x1x
    ("BPL", RELA),
    ("ORA", INDY),
    (None, None),
    (None, None),
    (None, None),
    ("ORA", ZRPX),
    ("ASL", ZRPX),
    (None, None),
    ("CLC", IMPL),
    ("ORA", ABSY),
    (None, None),
    (None, None),
    (None, None),
    ("ORA", ABSX),
    ("ASL", ABSX),
    (None, None),

    # 0x2x
    ("JSR", ABSO),
    ("AND", INDX),
    (None, None),
    (None, None),
    ("BIT", ZRPG),
    ("AND", ZRPG),
    ("ROL", ZRPG),
    (None, None),
    ("PLP", IMPL),
    ("AND", IMMD),
    ("ROL", ACCU),
    (None, None),
    ("BIT", ABSO),
    ("AND", ABSO),
    ("ROL", ABSO),
    (None, None),

    # 0x3x
    ("BMI", RELA),
    ("AND", INDY),
    (None, None),
    (None, None),
    (None, None),
    ("AND", ZRPX),
    ("ROL", ZRPX),
    (None, None),
    ("SEC", IMPL),
    ("AND", ABSY),
    (None, None),
    (None, None),
    (None, None),
    ("AND", ABSX),
    ("ROL", ABSX),
    (None, None),

    # 0x4x
    ("RTI", IMPL),
    ("EOR", INDX),
    (None, None),
    (None, None),
    (None, None),
    ("EOR", ZRPG),
    ("LSR", ZRPG),
    (None, None),
    ("PHA", IMPL),
    ("EOR", IMMD),
    ("LSR", ACCU),
    (None, None),
    ("JMP", ABSO),
    ("EOR", ABSO),
    ("LSR", ABSO),
    (None, None),

    # 0x5x
    ("BVC", RELA),
    ("EOR", INDY),
    (None, None),
    (None, None),
    (None, None),
    ("EOR", ZRPX),
    ("LSR", ZRPX),
    (None, None),
    ("CLI", IMPL),
    ("EOR", ABSY),
    (None, None),
    (None, None),
    (None, None),
    ("EOR", ABSX),
    ("LSR", ABSX),
    (None, None),

    # 0x6x
    ("RTS", IMPL),
    ("ADC", INDX),
    (None, None),
    (None, None),
    (None, None),
    ("ADC", ZRPG),
    ("ROR", ZRPG),
    (None, None),
    ("PLA", IMPL),
    ("ADC", IMMD),
    ("ROR", ACCU),
    (None, None),
    ("JMP", INDR),
    ("ADC", ABSO),
    ("ROR", ABSO),
    (None, None),

    # 0x7x
    ("BVS", RELA),
    ("ADC", INDY),
    (None, None),
    (None, None),
    (None, None),
    ("ADC", ZRPX),
    ("ROR", ZRPX),
    (None, None),
    ("SEI", IMPL),
    ("ADC", ABSY),
    (None, None),
    (None, None),
    (None, None),
    ("ADC", ABSX),
    ("ROR", ABSX),
    (None, None),

    # 0x8x
    (None, None),
    ("STA", INDX),
    (None, None),
    (None, None),
    ("STY", ZRPG),
    ("STA", ZRPG),
    ("STX", ZRPG),
    (None, None),
    ("DEY", IMPL),
    (None, None),
    ("TXA", IMPL),
    (None, None),
    ("STY", ABSO),
    ("STA", ABSO),
    ("STX", ABSO),
    (None, None),

    # 0x9x
    ("BCC", RELA),
    ("STA", INDY),
    (None, None),
    (None, None),
    ("STY", ZRPX),
    ("STA", ZRPX),
    ("STX", ZRPY),
    (None, None),
    ("TYA", IMPL),
    ("STA", ABSY),
    ("TXS", IMPL),
    (None, None),
    (None, None),
    ("STA", ABSX),
    (None, None),
    (None, None),

    # 0xAx
    ("LDY", IMMD),
    ("LDA", INDX),
    ("LDX", IMMD),
    (None, None),
    ("LDY", ZRPG),
    ("LDA", ZRPG),
    ("LDX", ZRPG),
    (None, None),
    ("TAY", IMPL),
    ("LDA", IMMD),
    ("TAX", IMPL),
    (None, None),
    ("LDY", ABSO),
    ("LDA", ABSO),
    ("LDX", ABSO),
    (None, None),

    # 0xBx
    ("BCS", RELA),
    ("LDA", INDY),
    (None, None),
    (None, None),
    ("LDY", ZRPX),
    ("LDA", ZRPX),
    ("LDX", ZRPY),
    (None, None),
    ("CLV", IMPL),
    ("LDA", ABSY),
    ("TSX", IMPL),
    (None, None),
    ("LDY", ABSX),
    ("LDA", ABSX),
    ("LDX", ABSY),
    (None, None),

    # 0xCx
    ("CPY", IMMD),
    ("CMP", INDX),
    (None, None),
    (None, None),
    ("CPY", ZRPG),
    ("CMP", ZRPG),
    ("DEC", ZRPG),
    (None, None),
    ("INY", IMPL),
    ("CMP", IMMD),
    ("DEX", IMPL),
    (None, None),
    ("CPY", ABSO),
    ("CMP", ABSO),
    ("DEC", ABSO),
    (None, None),

    # 0xDx
    ("BNE", RELA),
    ("CMP", INDY),
    (None, None),
    (None, None),
    (None, None),
    ("CMP", ZRPX),
    ("DEC", ZRPX),
    (None, None),
    ("CLD", IMPL),
    ("CMP", ABSY),
    (None, None),
    (None, None),
    (None, None),
    ("CMP", ABSX),
    ("DEC", ABSX),
    (None, None),

    # 0xEx
    ("CPX", IMMD),
    ("SBC", INDX),
    (None, None),
    (None, None),
    ("CPX", ZRPG),
    ("SBC", ZRPG),
    ("INC", ZRPG),
    (None, None),
    ("INX", IMPL),
    ("SBC", IMMD),
    ("NOP", IMPL),
    (None, None),
    ("CPX", ABSO),
    ("SBC", ABSO),
    ("INC", ABSO),
    (None, None),

    # 0xFx
    ("BEQ", RELA),
    ("SBC", INDY),
    (None, None),
    (None, None),
    (None, None),
    ("SBC", ZRPX),
    ("INC", ZRPX),
    (None, None),
    ("SED", IMPL),
    ("SBC", ABSY),
    (None, None),
    (None, None),
    (None, None),
    ("SBC", ABSX),
    ("INC", ABSX),
    (None, None)
]

def check_duplicates():
    seen = set()
    for i in instructions:
        if i != (None, None) and i in seen:
            print(i)
            return False
        seen.add(i)
    return True


if __name__=="__main__":
    assert(len(instructions) == 256)
    assert(check_duplicates())
    print("Test passed")
