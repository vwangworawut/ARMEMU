# ARMEMU

Using C, created an ARM emulator that mimics ARM machine code by reading in the ARM functions line by line and executing it through C based on what instructions are read. Wrote each ARM function to perform sum of array, finding max in array, fibonacci sequence both iteratively and recurisvely and finding substring.

- Emulated how an ARM CPU processes and executes ARM machine code
- Implemented register state, memory (stack) and functions with ARM instructions
- Emulator supports data memory, memory and branch instructions

Single Cycle Processor.circ is a single cycle processor created using Logisim and reads in the machine language of these ARM functions and shows a CPU processes these instructions per clock cycle.

To compile:
gcc -o armemu armemu.c sum_a_s.s find_max_s.s fib_iter_s.s fib_rec_s.s find_str_s.s 

To run:
./armemu
