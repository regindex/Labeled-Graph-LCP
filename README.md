# Wheeler-Automata-LCS

### Description

This tool implements a software to compute the LCS of a (pruned) Wheeler automaton.

### Requirements

* A modern C++17 compiler such as `g++` version 8.4 or higher.
* A Linux or macOS 64-bit operating system.
* The sdsl-lite library.

### Download

To clone the repository, run the following commands:

```console
git clone https://github.com/regindex/Wheeler-Automata-LCS.git
```

### Compile

You can compile the code as follows:

```console
g++ -o exe -std=c++11 compute-LCS.cpp -lsdsl
```

### Input

This tool takes in input a Wheeler automaton encoded with three files: a .L file containing the outgoing labels,
```
abbbbcdef
```
a .out file encoding the out-degrees in unary,
```
001010101000011111
```
a .in file encoding the in-degrees in unary,
```
010101010101010101
```

### Run on example data

```console
# insert the base path of the input files (data/SHexample.L,data/SHexample.out,data/SHexample.in)
./exe data/SHexample
```

### External resources

* [sdsl-lite](https://github.com/simongog/sdsl-lite.git)

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation program, project REGINDEX, grant agreement No 101039208.