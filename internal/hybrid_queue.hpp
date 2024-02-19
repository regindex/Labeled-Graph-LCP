// Copyright (c) 2023, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  hybrid_queue: Implementation of the hybrid queue from Beller et al. paper
 */

#ifndef INTERNAL_HYBRID_QUEUE_HPP_
#define INTERNAL_HYBRID_QUEUE_HPP_

#include "common.hpp"

typedef std::pair<uint_t,uint_t> interval;
typedef sdsl::bit_vector sdsl_bvec;

namespace wg{

class hybrid_queue{

private:
	/* init bitvectors for the LCS+1 intervals */
	void init_next_bitvec_queue()
	{
		// (re-)init bitvectors for the queue intervals associated to the next LCS value 
		if( Bn == nullptr ){ Bn = new sdsl_bvec(bit_size_pad,0); En = new sdsl_bvec(bit_size_pad,0); }
		else{ for(uint_t i=0;i<bit_size;++i){ (*Bn)[i] = 0; (*En)[i] = 0; } /*Bn->bit_resize(0); Bn->bit_resize(bit_size);*/ }
	}

	/* swap pointers for the bitvector queue */
	void swap_bitvec_queue()
	{
		// swap queues
		std::swap(B,Bn); std::swap(E,En);
	}

	/* delete current LCS value bitvector based queue */
	void delete_curr_bitvec_queue()
	{
		// free pointers
		if( B != nullptr ){ delete B; delete E; B = E = nullptr; }
	}

	/* delete next LCS value bitvector based queue */
	void delete_next_bitvec_queue()
	{
		// free pointers
		if( Bn != nullptr ){ delete Bn; delete En; Bn = En = nullptr; }
	}
	
public:
	// empty constructor
	hybrid_queue(){}
	/*
	* Constructor that takes in input the maximum size of the bitvector and initialize
	* the hybrid index constisting of the classic STL queue + two bit vectors encoding
	* the beginnings and ends of the intervals.
	*/
	hybrid_queue(uint_t n): bit_size(n+1)
	{
		// initialize queue size
		size = 0;
		// initialize l value
		l = INF;
		// we start with the classic queue type
		Switch = false; Switch2 = false;
		// no ones in bit vector
		bit_set = 0;
		// compute queue change trigger
		trigger = n/(2*log(n));
		// set pointers to null
		B = E = Bn = En = nullptr; 
		// set 64 bits padded length
		bit_size_pad = ((bit_size/64)+1)*64 + 1;
	}

	/* push new interval in the queue */
	void push(uint_t i, uint_t j)
	{
		// push to the bitvector-based queue
		if( Switch2 )
		{
			//std::cout << "pusha: " << i << " - " << j << std::endl;
			assert(i < bit_size and j < bit_size);
			// set the two interval bits
			(*Bn)[i] = 1; (*En)[j] = 1;
			// increase bit counter
			bit_set++;

		}
		// push in the the STL queue
		else
			queue_pair.push(std::make_pair(i,j));
	}

	/* set up queue for the next LCS value */
	bool_t set_queue_for_next_l()
	{
		// increase LCS value
		l++;
		// update switch
		Switch = Switch2;
		// setup queue type
		if( Switch ){
			//std::cout << "SET BITVEC QUEUE\n";
			// set bitvector queue
			size = bit_set;
			swap_bitvec_queue();
			bit_set = B_i = E_i = 0;
		}
		else{
			// set STL queue
			size = queue_pair.size();
			// delete old bitvector queue
			delete_curr_bitvec_queue();
		}

		// if queue is empty return false
		if( size == 0 ){ /*clear();*/ return false; }

		// set next switch
		if( size > trigger ){
			//std::cout << "SET NEXT BITVEC QUEUE\n";
			// set switch on and init bitvectors	
			Switch2 = true;
			init_next_bitvec_queue();
		}
		else{
			//std::cout << "DELETE NEXT BITVEC QUEUE\n";
			// set switch on and delete bitvectors
			Switch2 = false;
			// delete old bitvector queue
			delete_next_bitvec_queue();
		}
		
		// return true is size > 0
		return true;
	}

	/* get queue front and pop it */
	interval pop_front()
	{
		// pop from a bitvector based queue
		if( Switch )
		{
			// scan the bitvector and check if the substrings B[B_i..B_i+64]
			// and E[E_i..E_i+64] contain some ones
			uint64_t b_i = 0, e_i = 0;
			while( (b_i = B->get_int(B_i)) == 0 ){ B_i += 64; }
			while( (e_i = E->get_int(E_i)) == 0 ){ E_i += 64; }

			// get leftmost bits for both bitvectors
			b_i = B_i + __builtin_ctzl(b_i);
			e_i = E_i + __builtin_ctzl(e_i);
			size--;

			// set the two leftmost bits to zero
			(*B)[b_i] = 0; (*E)[e_i] = 0;

			return std::make_pair(b_i,e_i);
		}
		// pop from STL queue
		else
		{
			interval res = queue_pair.front();
			queue_pair.pop();
			size--;
			return res;
		}
	}

	/* get queue front and pop it */
	interval pop_front_old()
	{
		// pop from a bitvector based queue
		if( Switch )
		{
			// scan bitvectors to find intervals
			while(not (*B)[B_i++]){ }
			while(not (*E)[E_i++]){ }
			size--;

			return std::make_pair(B_i-1,E_i-1);
		}
		// pop from STL queue
		else
		{
			interval res = queue_pair.front();
			queue_pair.pop();
			size--;
			return res;
		}
	}

	/* get current LCP value */
	uint_t get_l()
	{
		return l;
	}

	/* tell if the queue is empty */
	bool_t empty()
	{
		return (size==0);
	} 

	/* get size of the queue */
	uint_t get_size()
	{
		return size;
	} 

private:
	// classic queue containing pair intervals
	std::queue<interval> queue_pair;
	// size of the queue
	uint_t size;
	// current l value
	uint_t l;
	// bit vector queue
	sdsl_bvec* B; sdsl_bvec* E;
	sdsl_bvec* Bn; sdsl_bvec* En;
	// bit vectors size
	uint_t bit_size;
	uint_t bit_size_pad;
	// not bits set
	uint_t bit_set;
	// bitvector front indexes
	uint_t B_i, E_i;
	// switch between two queues
	bool_t Switch, Switch2;
	// limit to trigger switch
	uint_t trigger;
};

}

#endif