# RNES
A work in progress NES emulator written in C++20 using SDL2.

## Dependencies
- A C++20 compatible compiler
- CMake 3.16+
- SDL2

## Building (Linux)
Clone the repository and change directory into it:
```
git clone https://github.com/db3141/RNES.git && cd RNES/
```

Then build the project (replacing <TARGET> with the target you want to build):
```
mkdir build && cd build
cmake .. && cmake --build . --target <TARGET>
```
The current available build targets are:
- ```cpu_test```
- ```ppu_test```
- ```sdl_test```
  
## Running Tests
First make sure to cd into the root directory of the project. In the case of being in the build directory run:
```
cd ..
```
  
### CPU Test
The CPU test makes sure that the 6502 CPU emulator is working correctly. This uses the tests provided in https://github.com/Klaus2m5/6502_65C02_functional_tests.
  
To run the tests do:
```
build/tests/cpu_test tests/cpu_test/6502_functional_test.bin
```
This will drop you into a debugger (similar to gdb). To run the tests enter the command ```c``` or ```continue```. If the tests were passed then the debugger should stop after detecting an infinite loop at address 0x3469 otherwise the tests failed. The debugger can be exitted by entering ```q```, ```quit``` or by pressing CTRL+C.
  
### PPU Test
The PPU test makes sure the NES PPU emulator renders frames correctly. To test on a particular memory dump run:
```
build/tests/ppu_test <ppu_dump> <oam_dump>
```
  
To run using the included memory dumps do:
  
```
build/tests/ppu_test tests/ppu_test/ppu_dump.bin tests/ppu_test/oam_dump.bin
```
  
This should output a frame from the title screen of Super Mario Bros.
  
![alt text](https://raw.githubusercontent.com/db3141/RNES/master/screenshots/ppu_test.png)
  
### SDL Test
The SDL test makes sure that SDL2 is installed properly. To run the test do:
```
build/tests/sdl_test
```

This should open a window with a gray background.
