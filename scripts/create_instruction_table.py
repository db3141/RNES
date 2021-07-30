import table as t

uniqueInstructions = set([x for (x, _) in t.instructions])
uniqueInstructions.remove(None)

instructionNames = list(uniqueInstructions)
instructionNames.sort()

for i in range(len(instructionNames)):
    print("&CPU::instruction" + str(instructionNames[i]), end=",\n")
print("nullptr")
