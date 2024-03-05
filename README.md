# Labeled-Graph-LCP

### Description

This tool implements software for computing the Longest Common Prefix (LCP) of an arbitrary labeled graph. It comprises three modules: 1) the pre-processing step turning arbitrary labeled graphs $G$ into a deterministic Wheeler pseudoforest $G_{is}$, 2) the LCP computation step constructing the LCP vector of $G_{is}$, 3) the post-processing step mapping the LCP of $G_{is}$ to the LCP of the original graph.

Module 2) contains three different LCP construction algorithm implementations: (i) a generalization of the algorithm of Beller et al., (ii) a generalization of the Manber-Myers doubling algorithm, and (iii) a re-design of the Beller et al. algorithm using a dynamic interval stabbing data structure.
As for (iii), we implement an interval stabbing data structure optimized for small alphabets (ASCII alphabet), which relies on scanning a bit-packed array interleaving BWT blocks of length $\sigma$ with bit vectors storing information about the intervals crossing the BWT blocks.

### Requirements

* A modern C++14 compiler such as `g++` version 8.4 or higher.
* A Linux or macOS 64-bit operating system.
* The sdsl-lite library, which will be automatically downloaded.

### Download

To clone the repository, run the following commands:

```console
git clone https://github.com/regindex/Labeled-Graph-LCP.git
cd Labeled-Graph-LCP
git submodule update --init --recursive
```

### Compile

You can compile the code as follows:

```console
python3 compile.py install
```

The current installation can be cleaned as follows:

```console
python3 make.py clean
```

### Input

This tool can take in input a labeled graph in .dot format; the input graph must contain a single source labeled by the smallest id (see below for an example),
```
strict digraph {
  S1 -> S2 [ label = 84 ];
  S2 -> S3 [ label = 65 ];
  S3 -> S4 [ label = 84 ];
  S4 -> S5 [ label = 65 ];
  S5 -> S2 [ label = 84 ];
  S5 -> S6 [ label = 67 ];
  S6 -> S6 [ label = 67 ];
  S6 -> S7 [ label = 65 ];
  S7 -> S8 [ label = 67 ];
  S8 -> S9 [ label = 84 ];
  S9 -> S7 [ label = 65 ];
  S9 -> S4 [ label = 84 ];
}
```

Alternatively, this tool can take in input a deterministic Wheeler pseudoforest encoded with three files: a .L file containing the outgoing labels,
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
Module 1: Pre-process the input graph 

```console
Usage: build/preprocessing [options] [input]
Step 1: Compute the deterministic Wheeler pseudoforest of an arbitrary labeled graph.

	-o, --outpath
		Specify the output file paths.
 ```

Module 2: Compute the LCP of a Wheeler pseudoforest
```console
Usage: build/compute-LCP [options] [input]
Step 2: Compute the longest common prefix (LCP) vector of a Wheeler pseudoforest.

	-b, --Beller-gen
		Computes the LCP array with a generalization of the Beller et al. algorithm.
	-s, --interval-stabbing
		Computes the LCP array with a re-design of the Beller et al. algorithm relying on a dynamic interval stabbing data structure.
	-d, --doubling-algo
		Computes the LCP array with an extension of the Manber-Myers doubling algorithm.
	-v, --verbose
		Activate the verbose mode.
	-l, --print-LCP
		Print the resulting LCP vector to stdout (debug-only).
 ```
 If the input is already a Wheeler pseudoforest, Module 2 should be run directly, skipping the pre-processing step.

### Run on example data

```console
# compute the pre-processing of a .dot graph and the LCP of the resulting Wheeler pseudoforest
build/preprocessing data/randWheelerDFA.dot
build/compute-LCP --interval-stabbing data/randWheelerDFA.infsup.dot
# if the input is a Wheeler pseudoforest we can directly run the LCP construction algorithm
build/computeLCP --doubling ../data/BLexample.infsup
```

### External resources

* [sdsl-lite](https://github.com/simongog/sdsl-lite.git)
* [finite-automata-partition-refinement](https://github.com/regindex/finite-automata-partition-refinement.git)

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation program, project REGINDEX, grant agreement No. 101039208.