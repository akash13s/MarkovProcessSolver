### How to run the program

```
Compile command: g++ --std=c++11 MarkovProcessSolver.cpp
Run commands: (different examples)

1. Run - without any flags
./a.out <path to input file>
eg: 
run: ./a.out /home/as18464/MarkovProcessSolver/input.txt

2. Run with a discount factor:
./a.out -df <discount-factor> <path to input file>
eg:
run: ./a.out -df 0.9 /home/as18464/MarkovProcessSolver/input.txt

3. Run with minimising values as costs
./a.out -min <path to input file>
eg:
run: ./a.out -min /home/as18464/MarkovProcessSolver/input.txt

4. Run with a tolerance
./a.out -tol <tolerance> <path to input file>
run: ./a.out -tol 0.01 /home/as18464/MarkovProcessSolver/input.txt

5. Run with a cutoff for value iteration
./a.out -iter <iterations> <path to input file>
run: ./a.out -iter 200 /home/as18464/MarkovProcessSolver/input.txt

6. Run with all the flags above
eg: /a.out -min -df 0.9 -tol 0.001 -iter 200 /home/as18464/MarkovProcessSolver/input.txt

```

The code was run successfully on the following department Linux machines:
- snappy1.cims.nyu.edu
