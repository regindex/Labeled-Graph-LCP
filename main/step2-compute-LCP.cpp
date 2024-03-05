#include <iostream>
#include <string>
#include <chrono>

#include "internal/WG_string.hpp"
#include "internal/doubling_ds.hpp"
#include "internal/interval_stabbing.hpp"
#include "algos/interval_stabbing_algorithm.hpp"
#include "algos/doubling_algorithm.hpp"

// struct storing parameters
struct Args
{
	std::string input;
	int algo = -1;
	bool check = false, print = false, verb = false;
};

// function that prints the instructions for using the tool
void print_help(char** argv) { 
	std::cout << std::endl <<
		"Usage: " << argv[0] << " [options] [input]" << std::endl 

		<< "Step 2: Compute the longest common prefix (LCP) vector of a Wheeler pseudoforest." << std::endl << std::endl

	 	<< "	-b, --Beller-gen" << std::endl 
		<< "		Computes the LCP array with a generalization of the Beller et al. algorithm." << std::endl 

	 	<< "	-s, --interval-stabbing" << std::endl 
		<< "		Computes the LCP array with a re-design of the Beller et al. algorithm relying on a dynamic interval stabbing data structure." << std::endl 

	 	<< "	-d, --doubling-algo" << std::endl 
		<< "		Computes the LCP array with an extension of the Manber-Myers doubling algorithm." << std::endl 

	 	//<< "	-p, --preprocessing" << std::endl 
		//<< "		Perform preprocessing step computing the Wheeler pseudoforest of an arbitrary labeled graph." << std::endl 

	 	<< "	-v, --verbose" << std::endl 
		<< "		Activate the verbose mode." << std::endl 

	 	//<< "	-c, --check-output" << std::endl 
		//<< "		Check final LCP vector correctness (debug-only)." << std::endl 

	 	<< "	-l, --print-LCP" << std::endl 
		<< "		Print the resulting LCP vector to stdout (debug-only)." << std::endl 
		<< std::endl;
}

// function for parsing the input arguments
void parseArgs(int argc, char** argv, Args& arg) {

	if(argc < 3){ print_help(argv); exit(1); }

	// read and parse input parameters
	for(size_t i=1;i<argc;++i)
	{
		std::string param = argv[i];

		if( param == "-b" or param == "--Beller-gen" )
		{
			arg.algo = 0;
		}
		else if( param == "-d" or param == "--doubling" )
		{
			arg.algo = 1;
		}
		else if( param == "-s" or param == "--interval-stabbing" )
		{
			arg.algo = 2;
		}
		//else if( param == "-p" or param == "--preprocessing" )
		//{
		//	arg.prep = true;
		//}
		else if( param == "-v" or param == "--verbose" )
		{
			arg.verb = true;
		}
		else if( param == "-c" or param == "--check-output" )
		{
			arg.check = true;
		}
		else if( param == "-l" or param == "--print-LCP" )
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
		std::cerr << "Select a LCP construction algorithm! exiting..." << std::endl;
		exit(1);
	}
}

int main(int argc, char** argv)
{
	// read input parameters
	Args arg;
	parseArgs(argc,argv,arg);

	/*if(arg.prep)
	{
		std::cout << "Running the preprocessing step on: " << arg.input << std::endl;
		// compute infsup automaton
		std::string command = "python3 external/finite-automata-partition-refinement/partition_refinement.py --prune --compact " + arg.input;
		std::cout << "Executing: " << command << std::endl;
		std::system(command.c_str());
	}*/

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
	if(arg.algo == 0)
	{
		if(arg.verb) std::cout << "Running generalization of Beller et al. algorithm" << std::endl;
		// construct fm-index for pruned Wheeler graph
		wg::wg_fm_index<wg::huff_wt, wg::bit_vector> wg(arg.input);
		// compute LCP using the generalization of the algorithm of Beller et al.
		compute_LCP_Beller_gen(wg,arg.check,arg.print,arg.verb);
	}
	else if(arg.algo == 1)
	{
		if(arg.verb) std::cout << "Running generalization of Manber-Myers doubling algorithm" << std::endl;
		// compute prefix doubling data structure
		ds::doubling_ds<ds::static_rmq, wg::bit_vector> ds(arg.input);
		// compute LCP using the prefix doubling algorithm
		prefix_doubling_LCP(ds,arg.check,arg.print,arg.verb);
	}
	else if(arg.algo == 2)
	{
		if(arg.verb) std::cout << "Running interval stabbing algorithm" << std::endl;
		// construct fm-index for pruned Wheeler graph.
		wg::wg_fm_index<wg::huff_wt, wg::bit_vector> wg(arg.input);
		// compute interval stabbing data structure
		wg::interval_stabbing<wg::wg_fm_index<wg::huff_wt, wg::bit_vector>> sds(arg.input+".L",&wg);
		// compute LCP using interval stabbing data structure
		compute_LCP_interval_stabbing(sds,arg.check,arg.print,arg.verb);
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	if(arg.verb) std::cout << "Elapsed time = " << float(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count())/1000 << "[s]" << std::endl;

	return 0;
}