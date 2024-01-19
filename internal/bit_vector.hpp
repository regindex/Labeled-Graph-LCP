// Copyright (c) 2023, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 * bit_vector: a wrapper on bit_vector of the sdsl library, with support for rank/select
 */

//============================================================================

#ifndef INTERNAL_BIT_VECTOR_HPP_
#define INTERNAL_BIT_VECTOR_HPP_

#include "common.hpp"

namespace wg{

class bit_vector{

public:

	/*
	 * empty constructor. Initialize bitvector with length 0.
	 */
	bit_vector(){}

	/*
	 * constructor that builds a bitvector given an input file
	 */
	bit_vector(std::string filepath){
		// open stream
		std::ifstream bitstring(filepath);
		// compute bitvector length
		bitstring.seekg(0, std::ios::end);
		uint_t size = bitstring.tellg();
    	bitstring.seekg(0, std::ios::beg);

		if(size==0)
		{
			std::cerr << "Error! empty bitvector, exiting...\n";
			exit(1);
		}

		// initialize and fill bitvector
		u = size; char_t curr_bit;
		bv = sdsl::bit_vector(u,0);
		// iterate over all bits in the input
		for(uint_t i=0;i<size;++i){
			// get new bit
			bitstring.read(reinterpret_cast<char*>(&curr_bit), sizeof(char));
			if(curr_bit == '1')
				bv[i] = 1;
		}

		// close stream
		bitstring.close();
		// rank and select support
		rank0 = sdsl::bit_vector::rank_0_type(&bv);
		select1 = sdsl::bit_vector::select_1_type(&bv);
	}

	/*
	*  get the size of the bitvector
	*/
	uint_t size(){return u;}
	
	bool operator[](uint_t i){
		// check index size
		assert(i<u);
		
		return bv[i];
	}

	bit_vector & operator= (const bit_vector & other) {

		u = other.bv.size();
		bv = sdsl::bit_vector(other.bv);
		rank0 = sdsl::bit_vector::rank_0_type(&bv);
		select1 = sdsl::bit_vector::select_1_type(&bv);

	    return *this;
	}

	uint_t rank_0(uint_t i)
	{
		// check index size
		assert(i<u);
		
		return rank0(i);
	}

	uint_t select_1(uint_t i)
	{
		// check index size
		assert(i<=u);

		if( i==0 )
			return 0;
	
		return select1(i);
	}

private:

	//bitvector length
	uint_t u = 0;
	// bitvector + rank/select support
	sdsl::bit_vector bv;
	sdsl::bit_vector::rank_0_type rank0;
	sdsl::bit_vector::select_1_type select1;
};

}

#endif /* INTERNAL_BIT_VECTOR_HPP_ */