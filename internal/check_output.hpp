// Copyright (c) 2024, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  check_output: Function to check the correctness of the resulting LCP vector
 */

#ifndef CHECK_OUTPUT_HPP_
#define CHECK_OUTPUT_HPP_

#include "common.hpp"

//namespace wg{

//template<class wt_t, class bit_vec_t>
template<typename LCP_type>
class check_LCP_correctness{

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

	/* check if the input stream is pointing to an empty file */
	bool is_empty_stream(std::ifstream& file)
	{
	    return file.peek() == std::ifstream::traits_type::eof();
	}

	/* parse a dot file */
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
	check_LCP_correctness(){}
	/*
	*  Constructor that takes in input an input graph and its LCP, and checks
	*  if the LCP values are correct by explicitly visiting the graph.
	*/
	check_LCP_correctness(std::string filepath, uint_t n_, LCP_type* LCP_)
														: n(n_), LCP(LCP_)
	{
		// initialize transitions list
		transitions = std::vector<uint_t>(n,INF);
		labels = std::vector<char_t>(n,0);
		// read dot graph
		read_dot(filepath,transitions,labels);

		for(uint_t i=1;i<n;++i)
		{
			// initialize needed variables
			std::string prefix = "";
			uint_t curr = i, prev = i-1;
			bool correct = true;
			std::vector<bool> visited(n,0);
			// iterate over LCP array and check the incoming strings
			if((*LCP)[i] == INF)
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
						// stop if we reach the source or fall in a cycle
						if( transitions[curr] == INF ){ break; }
						if( visited[curr] && visited[prev] ){ break; }
						//	set nodes to visited
						visited[curr] = visited[prev] = true;
						curr = transitions[curr]; prev = transitions[prev]; 
					}
				}
			}
			else
			{
				for(uint_t j=0;j<(*LCP)[i];++j)
				{
					if( labels[curr] != labels[prev] )
					{
						correct = false;
						break;
					}
					prefix.push_back(labels[curr]);
					curr = transitions[curr]; prev = transitions[prev]; 
				}
				// if the preceding labels is the same of current one
				// the LCP values is not correct.
				if( labels[curr] == labels[prev] )
					correct = false;
			}
			//
			if(correct == false)
			{
				std::cout << "LCP[" << i << "] not correct!\n";
				exit(1);
			}
		}
		std::cout << "The LCP vector is correct!\n";
	}

private:
	// no. nodes
	uint_t n;
	// edges list
	std::vector<uint_t> transitions;
	// labels list
	std::vector<char_t> labels;
	// LCP vector pointer
	LCP_type* LCP;
};

//}

#endif