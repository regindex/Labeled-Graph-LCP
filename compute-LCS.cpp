#include <iostream>
#include <string>

#include "internal/WG_string.hpp"
#include "internal/LCS_computation.hpp"

int main(int argc, char** argv)
{
	// read input file
	std::string input_file = argv[1];
	// construct fm-index for pruned Wheeler graph.
	wg::wg_fm_index<wg::huff_wt, wg::bit_vector> wg(input_file);
	// compute LCS
	compute_LCS(wg);

	return 0;
}