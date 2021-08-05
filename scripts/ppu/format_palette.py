f = open("ntscpalette.pal", "rb")

data = f.read()

f.close()

for i in range(0, 64):
    print("{", str(hex(data[3*i + 0])) + ", " + str(hex(data[3*i + 1])) + ", " + str(hex(data[3*i + 2])) + ", 0xff },")
