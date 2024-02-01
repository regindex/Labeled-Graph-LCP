// Copyright (c) 2023, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  wg_fm_index: Implementation of the FM-index for a (pruned) Wheeler graph.
 */

#ifndef WG_STRING_HPP_
#define WG_STRING_HPP_

#include "bit_vector.hpp"
#include "huff_wavelet_tree.hpp"

namespace wg{

template<class wt_t, class bit_vec_t>
class wg_fm_index{

private:
	/* compute prefix sum */
	void prefix_sum(std::vector<uint_t>& C)
	{
		for (uint_t i = 1; i < C.size(); i++)
			C[i] = C[i - 1] + C[i];
	}
	/* initialize and compute C vector */
	std::vector<uint_t> construct_C()
	{
		// initialize C vector
		std::vector<uint_t> C(128,0);
		// fill character frequencies
		for(uint_t i=0;i<L.alph_size();++i)
			C[L.at_char(i)] = L.at_freq(i);
		// run prefix sum on C vector
		prefix_sum(C);

		return C;
	}
	/* check for the number of source edges */
	uint_t check_source(std::string filepath)
	{
		uint_t no_sources = 0; uint_t bit = 49;
		// open stream
		std::ifstream bitstring(filepath);
		// check first bites
		while( bit == 49 ){
			// read bit
			bitstring.read(reinterpret_cast<char*>(&bit), sizeof(bool_t));
			// increase no sources
			no_sources++;
		}
		// close stream
		bitstring.close();
		
		return no_sources - 1;
	}

public:
	// empty constructor
	wg_fm_index(){}
	/*
	* Constructor that takes in input the basepath of the input files and construct
	* an FM-index for the (pruned) Wheeler automaton with support to forward search.
	*/
	wg_fm_index(std::string basepath){
		// construct the wavalet tree for the outgoinf labels
		L = wt_t(basepath+".L");
		// construct the bitvector encoding the out degrees
		out = bit_vec_t(basepath+".out");
		// compute the number of source nodes
		has_source_incoming_edge = check_source(basepath+".in");
		// initialize C vector
		C = construct_C();
	}

	/* get number of edges */
	uint_t get_L_length()
	{
		return L.size();
	}

	/* get number of states */
	uint_t get_no_nodes()
	{
		return L.size() + has_source_incoming_edge;
	}

	/* get alphabet sigma */
	uint_t get_sigma()
	{
		return L.alphabet_size();
	}

	/* compute forward search step in the interval [i,j] */
	rank_pair forward_all(uint_t i, uint_t j)
	{
		// Find the interval in L
		i = out.rank_0(out.select_1(i)); j = out.rank_0(out.select_1(j));

		// search intervals for all characters
		rank_pair_char res =  L.search_intervals_in_range(i, j);
		for(uint_t i=0;i<res.first.size();++i)
		{
			// use C vector to map the intervals on F
			res.second.first[i] += C[res.first[i]-1] + has_source_incoming_edge;
			res.second.second[i] += C[res.first[i]-1] + has_source_incoming_edge;
		}

		return res.second;
	}

private:
	// wavelet tree
	wt_t L;
	// BWT C vector (F column)
	std::vector<uint_t> C;
	// bitvector encoding out-degrees
	bit_vec_t out;
	// number of source states
	uint_t has_source_incoming_edge;
};

}

#endif