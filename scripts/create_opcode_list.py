import table as t

for i in range(len(t.instructions)):
    hex_str = ""
    if i < 16:
        hex_str = "0x0" + hex(i)[2:]
    else:
        hex_str = hex(i)

    (name,mode) = t.instructions[i]
    if name == None:
        instStr = "InstructionId::NONE"
        modeStr = "AddressMode::NONE"
    else:
        instStr = "InstructionId::" + name
        modeStr = "AddressMode::" + mode
    print("{", "{:<21}".format(instStr + ","), "{:<32}".format(modeStr + " },"), "/*", hex_str, "*/")
