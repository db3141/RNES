# Constants
CXX=clang++

LINKFLAGS=
INCLUDEFLAGS=

OTHER_FLAGS=

DEBUGFLAGS=-Wall -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined
GDBFLAGS=-Wall -ggdb
RELEASEFLAGS=-O3
CPPFLAGS=-std=c++20

OUTNAME=6502_Emulator
BUILD_DIR=build

DEBUG_OUT=$(BUILD_DIR)/$(OUTNAME)_debug
GDB_OUT=$(BUILD_DIR)/$(OUTNAME)_gdb
RELEASE_OUT=$(BUILD_DIR)/$(OUTNAME)_release

# Obj output
objdir=objdir
debugObjDir=$(objdir)/debug
gdbObjDir=$(objdir)/gdb
releaseObjDir=$(objdir)/release

objs=main.o ram.o cpu.o cpu_instructions.o cpu_debugger.o
headers=memory.hpp ram.hpp cpu.hpp

debugObjs=$(addprefix $(debugObjDir)/,$(objs))
gdbObjs=$(addprefix $(gdbObjDir)/,$(objs))
releaseObjs=$(addprefix $(releaseObjDir)/,$(objs))

# Debug Build
$(DEBUG_OUT): $(debugObjs)
	$(CXX) -o $@ $(debugObjs) $(CPPFLAGS) $(LINKFLAGS) $(DEBUGFLAGS) $(OTHER_FLAGS)

$(debugObjDir)/%.o: %.cpp $(headers) | objdirs
	$(CXX) -c -o $@ $(patsubst $(debugObjDir)/%,%,$(@:.o=.cpp)) $(INCLUDEFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(OTHER_FLAGS)

# GDB Build
$(GDB_OUT): $(gdbObjs)
	$(CXX) -o $@ $(gdbObjs) $(CPPFLAGS) $(LINKFLAGS) $(GDBFLAGS) $(OTHER_FLAGS)

$(gdbObjDir)/%.o: %.cpp $(headers) | objdirs
	$(CXX) -c -o $@ $(patsubst $(gdbObjDir)/%,%,$(@:.o=.cpp)) $(INCLUDEFLAGS) $(CPPFLAGS) $(GDBFLAGS) $(OTHER_FLAGS)

# Release Build
$(RELEASE_OUT): $(releaseObjs)
	$(CXX) -o $@ $(releaseObjs) $(CPPFLAGS) $(LINKFLAGS) $(RELEASEFLAGS) $(OTHER_FLAGS)

$(releaseObjDir)/%.o: %.cpp $(headers) | objdirs
	$(CXX) -c -o $@ $(patsubst $(releaseObjDir)/%,%,$(@:.o=.cpp)) $(INCLUDEFLAGS) $(CPPFLAGS) $(RELEASEFLAGS) $(OTHER_FLAGS)


# Helpers
.PHONY: objdirs
objdirs:
	-mkdir $(objdir)
	-mkdir $(debugObjDir) $(gdbObjDir) $(releaseObjDir)
	-mkdir $(BUILD_DIR)

.PHONY: clean
clean:
	-rm $(DEBUG_OUT) $(RELEASE_OUT) $(GDB_OUT) $(debugObjs) $(gdbObjs) $(releaseObjs)
	-rmdir $(debugObjDir) $(gdbObjDir) $(releaseObjDir)
	-rmdir $(objdir)
	-rmdir $(BUILD_DIR)

