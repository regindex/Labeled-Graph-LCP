# Wheeler-Pseudoforest-LCP

### Description

This tool implements a software computing the Longest common prefix (LCP) of a deterministic Wheeler Pseudoforest. 

### Requirements

* A modern C++14 compiler such as `g++` version 8.4 or higher.
* A Linux or macOS 64-bit operating system.
* The sdsl-lite library which will be automatically downloaded.

### Download

To clone the repository, run the following commands:

```console
git clone https://github.com/regindex/Wheeler-Pseudoforest-LCP.git
cd Wheeler-Pseudoforest-LCP
git submodule update --init --recursive
```

### Compile

You can compile the code as follows:

```console
python3 compile.py install
```

you can clean the current install as follows:

```console
python3 make.py clean
```

### Input

This tool takes in input a deterministic Wheeler pseudoforest encoded with three files: a .L file containing the outgoing labels,
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

Note that all nodes except the sources must be reached by exactly one incoming edge.

### Usage

```console
Usage: build/computeLCP [options] [input]
Tool to compute the longest common prefix (LCP) vector of a Wheeler pseudoforest.

	-b, --Beller-gen
		Computes the LCP array with a generalization of Beller et al. algorithm.
	-d, --doubling
		Computes the LCP array with an extension of Manber-Myers doubling algorithm.
	-s, --interval-stabbing
		Computes the LCP array with an dynamic interval stabbing algorithm.
	-p, --preprocessing
		Perform preprocessing step computing the Wheeler pseudoforest of an arbitrary labeled graph.
	-v, --verbose
		Activate the verbose mode.
	-l, --print-LCP
		Print the resulting LCP vector to stdout (debug-only).
 ```

### Run on example data

```console
# insert the base path of the input files (data/SHexample.L,data/SHexample.out,data/SHexample.in)
build/computeLCP --doubling ../data/BLexample.infsup
# compute the preprocessing of a .dot graph and the LCP of the resulting Wheeler pseudoforest
build/computeLCP --preprocessing data/randWheelerDFA.dot 
build/computeLCP --interval-stabbing data/randWheelerDFA.infsup.dot
```

### External resources

* [sdsl-lite](https://github.com/simongog/sdsl-lite.git)
* [finite-automata-partition-refinement](https://github.com/regindex/finite-automata-partition-refinement.git)

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation program, project REGINDEX, grant agreement No 101039208.