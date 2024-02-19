// Copyright (c) 2023, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  check_output: // TO COMPLETE
 */

#ifndef CHECK_OUTPUT_HPP_
#define CHECK_OUTPUT_HPP_

// #include "bit_vector.hpp"
// #include "huff_wavelet_tree.hpp"
#include "common.hpp"

//namespace wg{

//template<class wt_t, class bit_vec_t>
template<typename lcs_type>
class check_LCS_correctness{

private:
	/* parse an edge line */
	void tokenize(std::string const &str, const char delim, 
	            std::vector<std::string> &out) 
	{ 
	    // construct a stream from the string 
	    std::stringstream ss(str); 
	 
	    std::string s; 
	    while (std::getline(ss, s, delim)) { 
	        out.push_back(s); 
	    } 
	}

	bool is_empty_stream(std::ifstream& file)
	{
	    return file.peek() == std::ifstream::traits_type::eof();
	}

	/* */
	void read_dot(std::string graph, std::vector<uint_t>& M,
		                             std::vector<char_t>& L)
	{
		// open stream to input
	    std::ifstream input(graph);
	    // check if stream was opened correctly
	    if(is_empty_stream(input))
	    {
	    	std::cerr << "Error! the .dot file is empty." << std::endl;
	    	exit(1);
	    }
	    std::string line;
	    // delimiter
	    const char delim = ' ';
	    // edge variables
	    uint_t origin, dest; int label;

	    while(std::getline(input, line))
	    {
	    	// read edge line
	   		std::vector<std::string> out; 
      		tokenize(line, delim, out); 

	        // skip lines not encoding an edge in the dot file
	        if(out.size() < 8){ continue; }
	        // read an edge in the dot file
	        origin = stoull(out[0].substr(2, out[0].size())) - 1;
	        dest = stoull(out[2].substr(1, out[2].size())) - 1;
	        label = stoi(out[6]);

	        // add edge
	        M[dest] = origin;
	        // add label
	        L[dest] = label;
	    }

	    // close output
	    input.close();
	}

public:
	// empty constructor
	check_LCS_correctness(){}
	/*
	* TO COMPLETE!
	*/
	check_LCS_correctness(std::string filepath, uint_t n_, lcs_type* LCS_)
														: n(n_), LCS(LCS_)
	{
		//
		transitions = std::vector<uint_t>(n,INF);
		labels = std::vector<char_t>(n,0);
		// read dot
		read_dot(filepath,transitions,labels);

		for(uint_t i=1;i<n;++i)
		{
			//std::cout << "i: " << i << std::endl;
			//
			std::string prefix = "";
			//
			uint_t curr = i, prev = i-1;
			//
			bool correct = true;
			std::vector<bool> visited(n,0);
			//
			if((*LCS)[i] == INF)
			{
				while(true)
				{
					if( labels[curr] != labels[prev] )
					{
						correct = false;
						break;
					}
					else
					{
						prefix.push_back(labels[curr]);
						//
						if( transitions[curr] == INF ){ break; }
						if( visited[curr] && visited[prev] ){ break; }
						//
						visited[curr] = visited[prev] = true;
						curr = transitions[curr]; prev = transitions[prev]; 
					}
				}
			}
			else
			{
				for(uint_t j=0;j<(*LCS)[i];++j)
				{
					//std::cout << "j: " << j << std::endl;
					if( labels[curr] != labels[prev] )
					{
						correct = false;
						break;
					}
					//
					prefix.push_back(labels[curr]);
					//
					curr = transitions[curr]; prev = transitions[prev]; 
					//exit(1);
				}
				//
				if( labels[curr] == labels[prev] )
					correct = false;
			}
			//
			if(correct == false)
			{
				std::cout << "LCS[" << i << "] uncorrect!\n";
				exit(1);
			}
			//else
			//	std::cout << prefix << std::endl;
		}
		std::cout << "The output is correct!\n";
	}

private:
	// no. nodes
	uint_t n;
	// edges list
	std::vector<uint_t> transitions;
	// labels list
	std::vector<char_t> labels;
	// lcs vector pointer
	lcs_type* LCS;
	//sdsl::int_vector<>* LCS;
};

//}

#endif