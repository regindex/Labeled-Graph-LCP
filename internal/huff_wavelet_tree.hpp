// Copyright (c) 2023, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 * huff_wt: a wrapper on wt_huff of the sdsl library, with support for rank/select
 */

//============================================================================

#ifndef INTERNAL_WAVELET_TREE_HPP_
#define INTERNAL_WAVELET_TREE_HPP_

#include "common.hpp"

namespace wg{

class huff_wt{

public:

	/* empty constructor */
	huff_wt(){}

	/* constructor that builds a wt given an input file */
	huff_wt(std::string filepath){
		// compute huffman encoded wavelet tree
		sdsl::construct(wt, filepath.c_str(), 1);
		// compute distinct characters vector
		cs.resize(128);
		distinct_element_in_range(0,size(),this->cs,this->freq);
		// check alphabet size
		if( cs.size() < 1 ){
			std::cerr << "Error! The alphabet is empty, exiting...\n";
			exit(1);
		}
	}

	char_t operator[](uint_t i){

		assert(i<wt.size());
		return wt[i];
	}

	huff_wt & operator= (const huff_wt & other) {

		wt = other.wt;
		cs = other.cs;
		freq = other.freq;
	    return *this;
	}

	uint_t size(){
		return wt.size();
	}

	uint_t alphabet_size(){
		return cs.size();
	}

	uint_t rank(uint_t i, char_t c){

		assert(i<=wt.size());
		return wt.rank(i,c);
	}

	uint_t select(uint_t i, char_t c){

		return wt.select(i+1,c);
	}

	/* find all distinct element in the range [i,j] */
	void distinct_element_in_range(uint_wt i, uint_wt j,
		                           std::vector<char_wt>& cs, std::vector<uint_wt>& freq)
	{
		// alphabet size - ASCII alphabet
		uint_wt k = 128;
		// initialize alphabet vector
		cs.resize(k); std::iota(std::begin(cs), std::end(cs), 0);
		// resize vector storing character frequencies
		freq.resize(k);
		// search character frequencies in the wavelet tree
		sdsl::interval_symbols(wt,i,j,k,cs,freq,freq);
		// keeps character with a positive frequency
		cs.resize(k); freq.resize(k);
	}

	/* wrapper to interval_symbols function */
	rank_pair_char search_intervals_in_range(uint_wt i, uint_wt j)
	{
		assert(cs.size() > 0);
		// alphabet size
		uint_wt k = cs.size();
		// initialize alphabet vector
		std::vector<char_wt> cs_(k);
		// initialize rank vectors
		std::vector<uint_wt> rank_i, rank_j;
		rank_i.resize(k); rank_j.resize(k);
		// search character frequencies in the wavelet tree
		if(i > j){
			std::cout << "i > j " << i << " " << j << std::endl;
		}
		else if( j > wt.size() ){
			std::cout << "j > wt.size()" << std::endl; exit(1);
		}
		sdsl::interval_symbols(wt,i,j,k,cs_,rank_i,rank_j);

		if( k != cs_.size() )
			{ rank_i.resize(k); rank_j.resize(k); cs_.resize(k); }

		return std::make_pair(cs_,std::make_pair(rank_i,rank_j));
	}

	/* return alphabet size */
	uint_t alph_size()
	{
		assert(cs.size() == freq.size());
		return cs.size();
	}

	/* return i-th char in the alphabet */
	char_wt at_char(uint_t i)
	{
		assert(i<cs.size());
		return cs[i];
	}

	/* return frequency of the i-th char in the alphabet */
	uint_wt at_freq(uint_t i)
	{
		assert(i<freq.size());
		return freq[i];
	}

private:

	// wavelet tree + rank/select support
	sdsl::wt_huff<> wt;
	// character frequencies
	std::vector<uint_wt> freq;
	// distinct characters
	std::vector<char_wt> cs;
};

}

#endif /* INTERNAL_WAVELET_TREE_HPP_ */