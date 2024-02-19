#include <iostream>
#include <string>

#include "internal/WG_string.hpp"
#include "internal/doubling_ds.hpp"
#include "algos/Beller_gen_algorithm.hpp"
#include "algos/doubling_algorithm.hpp"

// struct storing parameters
struct Args
{
	std::string input;
	int algo = -1;
	bool prep = false, check = false, print = false, verb = false;
};

// function that prints the instructions for using the tool
void print_help(char** argv) { 
	std::cout << std::endl <<
		"Usage: " << argv[0] << " [options] [input]" << std::endl 

		<< "Tool to compute the longest common suffix vector (LCS) of a Wheeler graph." << std::endl << std::endl

	 	<< "	-b, --Beller" << std::endl 
		<< "		Computes the LCS array with an extension of Beller et al. algorithm." << std::endl 

	 	<< "	-d, --doubling" << std::endl 
		<< "		Computes the LCS array with an extension of Manber-Myers doubling algorithm." << std::endl 

	 	<< "	-p, --preprocessing" << std::endl 
		<< "		Perform preprocessing step computing the infsup automaton." << std::endl 

	 	<< "	-v, --verbose" << std::endl 
		<< "		Activate the verbose mode." << std::endl 

	 	<< "	-c, --check-output" << std::endl 
		<< "		Check final LCS vector correctness (debug-only)." << std::endl 

	 	<< "	-l, --print-lcs" << std::endl 
		<< "		Print the final LCS vector (debug-only)." << std::endl 
		<< std::endl;
}

// function for parsing the input arguments
void parseArgs(int argc, char** argv, Args& arg) {

	if(argc < 3){ print_help(argv); exit(1); }

	// read and parse input parameters
	for(size_t i=1;i<argc;++i)
	{
		std::string param = argv[i];

		if( param == "-b" or param == "--Beller" )
		{
			arg.algo = 0;
		}
		else if( param == "-d" or param == "--doubling" )
		{
			arg.algo = 1;
		}
		else if( param == "-p" or param == "--preprocessing" )
		{
			arg.prep = true;
		}
		else if( param == "-v" or param == "--verbose" )
		{
			arg.verb = true;
		}
		else if( param == "-c" or param == "--check-output" )
		{
			arg.check = true;
		}
		else if( param == "-l" or param == "--print-lcs" )
		{
			arg.print = true;
		}
		else if( param == "-h" or param == "--help" )
		{
			print_help(argv);
			exit(1);
		}
		else
		{
			if(i == argc-1){ /* read input */ arg.input = argv[i]; }
			else{ std::cerr << "Unknown option. Use -h for help." << std::endl; exit(-1); }
		}
	}
	// check algo parameter
	if( arg.algo == -1 )
	{
		std::cerr << "Select an algorithm! exiting..." << std::endl;
		exit(1);
	}
}

int main(int argc, char** argv)
{
	// read input parameters
	Args arg;
	parseArgs(argc,argv,arg);

	if(arg.prep)
	{
		// compute infsup automaton
		std::string command = "python3 external/finite-automata-partition-refinement/partition_refinement.py --prune --compact " + arg.input;
		std::system(command.c_str());
	}
	
	if(arg.algo == 0)
	{
		if(arg.verb) std::cout << "Running Beller et al. algorithm" << std::endl;
		// construct fm-index for pruned Wheeler graph.
		wg::wg_fm_index<wg::huff_wt, wg::bit_vector> wg(arg.input);
		// compute LCS
		compute_LCS(wg,arg.check,arg.print,arg.verb);
	}
	else if(arg.algo == 1)
	{
		if(arg.verb) std::cout << "Running prefix doubling algorithm" << std::endl;
		// compute LCS with doubling algorithm
		ds::doubling_ds<ds::static_rmq, wg::bit_vector> ds(arg.input);
		// compute LCS
		prefix_doubling_lcs(ds,arg.check,arg.print,arg.verb);
	}

	return 0;
}