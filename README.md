# Wheeler-Automata-LCS

### Description

This tool implements a software to compute the LCS of a (pruned) Wheeler automaton.

### Requirements

* A modern C++14 compiler such as `g++` version 8.4 or higher.
* A Linux or macOS 64-bit operating system.
* The sdsl-lite library which will be automatically downloaded.

### Download

To clone the repository, run the following commands:

```console
git clone https://github.com/regindex/Wheeler-Automata-LCS.git
cd Wheeler-Automata-LCS
git submodule update --init --recursive
```

### Compile

You can compile the code as follows:

```console
python3 compile.py install
```

you can clean the current install as follows:

```console
python3 compile.py clean
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

### Usage

```console
Usage: ./lcs_exe [options] [input]
Tool to compute the longest common suffix vector (LCS) of a Wheeler graph.

	-b, --Beller
		Computes the LCS array with an extension of Beller et al. algorithm.
	-d, --doubling
		Computes the LCS array with an extension of Manber-Myers doubling algorithm.
 ```

### Run on example data

```console
# insert the base path of the input files (data/SHexample.L,data/SHexample.out,data/SHexample.in)
./lcs_exe --doubling ../data/BLexample.infsup
```

### External resources

* [sdsl-lite](https://github.com/simongog/sdsl-lite.git)

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation program, project REGINDEX, grant agreement No 101039208.