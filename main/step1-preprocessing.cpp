#include <iostream>
#include <string>
//#include <ctime>
//#include <chrono>

// struct storing parameters
struct Args
{
	std::string input;
	std::string output;
	//bool verbose = false;
};

// function that prints the instructions for using the tool
void print_help(char** argv) { 
	std::cout << std::endl <<
		"Usage: " << argv[0] << " [options] [input]" << std::endl 

		<< "Step 1: Compute the deterministic Wheeler pseudoforest of an arbitrary labeled graph." << std::endl << std::endl

	 	<< "	-o, --outpath" << std::endl 
		<< "		Specify the output file paths." << std::endl 

	 	//<< "	-v, --verbose" << std::endl 
		//<< "		Activate the verbose mode." << std::endl 

		<< std::endl;
}

// function for parsing the input arguments
void parseArgs(int argc, char** argv, Args& arg) {

	if(argc < 2){ print_help(argv); exit(1); }

	// read and parse input parameters
	for(size_t i=1;i<argc;++i)
	{
		std::string param = argv[i];

		//if( param == "-v" or param == "--verbose" )
		//{
		//	arg.verbose = true;
		//}
		if( param == "-o" or param == "--outpath" )
		{
			arg.output = argv[++i];
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
}

int main(int argc, char** argv)
{
	// read input parameters
	Args arg;
	parseArgs(argc,argv,arg);

	std::cout << "### Running the preprocessing step on: " << arg.input << std::endl;
	// compute Wheeler pseudoforest
	std::string command;
	if(arg.output.empty())
		command = "python3 external/finite-automata-partition-refinement/partition_refinement.py --prune --compact " + arg.input;
	else
		command = "python3 external/finite-automata-partition-refinement/partition_refinement.py -outpath " + arg.output + " --prune --compact " + arg.input;
	// execute command
	// if(arg.verbose) std::cout << "Executing: " << command << std::endl;
	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::system(command.c_str());
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	//if(arg.verbose) std::cout << "Elapsed time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;

	return 0;
}