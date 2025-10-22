maxsat_approx
===============

Objective
---------
This is a single-file C++ educational tool that implements approximation algorithms for weighted Max-SAT and a brute-force exact solver for comparison.

What it provides
- A derandomized 1/2-approximation: derandomize a uniform p=1/2 random assignment by conditional expectation.
- An LP-relaxation + derandomized randomized rounding: solve a linear relaxation to obtain per-variable probabilities, then derandomize the rounding by conditional expectation.
- A brute-force exact solver that enumerates all 2^n assignments (note: exponential-time). The program measures run-time of each method for side-by-side comparison.

Source
------
`maxsat_approx.cpp` — single-file C++17 implementation.

Input format
------------
The program reads a weighted CNF instance from a file named `input1.txt` (hardcoded in the source). Place this file in the same folder as the executable.

File format (plain text):
```
n m
w1 k1 lit11 lit12 ... lit1k1
w2 k2 lit21 lit22 ... lit2k2
...
wm km litm1 litm2 ... litmkm
```
- n: number of Boolean variables (variables are indexed 1..n)
- m: number of clauses
- For each clause i:
  - wi: clause weight (double or integer, non-negative)
  - ki: number of literals in the clause (integer >= 0)
  - litij: integer literal; positive `t` means variable `x_t`, negative `-t` means ¬x_t

Example
-------
A tiny example (n=2, m=3):
```
2 3
1 2 1 2
2 1 -1
3 1 2
```
Meaning:
- Clause1 (weight 1): x1 ∨ x2
- Clause2 (weight 2): ¬x1
- Clause3 (weight 3): x2

Build and Run
-----
Compile commands (run in the folder containing `maxsat_approx.cpp`):


g++ -O2 -std=c++17 maxsat_approx.cpp -o maxsat_approx
./max_sat_approx


1) Use the hardcoded file (default behaviour)

Place your instance in the same folder and name it `input1.txt`, then run:


Note: the README examples above assume the current working directory contains the executable and input file. Use absolute paths if running from elsewhere.

Output
------
The program prints:
- The derandomized 1/2 assignment and its satisfied weight
- The derandomized LP-rounding assignment and its satisfied weight
- LP variable probabilities (x_i)
- The brute-force optimal value and assignment (the brute-force always runs by default; for large n it will take an impractical amount of time)
- Timings for each phase (microsecond precision), plus an averaged micro-benchmark if measured times are below timer resolution

Important notes
---------------
- Brute-force runs exhaustively over all 2^n assignments. This is exponential; for n &gt; ~28 you should expect very long runtimes. The program will refuse to enumerate if it would overflow a 64-bit counter (n &gt;= 64).
- The LP solver is a compact tableau-based Simplex implementation (dense). It's fine for small-to-medium instances but not optimized for very large or sparse LPs. For heavy use consider integrating GLPK/COIN-OR or another LP solver.
- Timings: the program prints durations (microseconds). If a measured interval is below the timer resolution it will run a micro-benchmark (1000 repeats) and print averaged per-repeat times.

Customizing input filename
--------------------------
The code currently opens `input1.txt` by default. To change this behavior:
- Edit `maxsat_approx.cpp` and modify the `FNAME` constant near `main()` to the filename you want; or restore command-line argument handling if you prefer supplying a filename at runtime.
