// Copyright (c) 2024, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 * static_rmq: a wrapper on the sdsl rmq_succinct_sct RMQ implementation
 */

//============================================================================

#ifndef INTERNAL_STATIC_RMQ_HPP_
#define INTERNAL_STATIC_RMQ_HPP_

#include "common.hpp"

namespace ds{

class static_rmq{

private:
	
	// forward iterator declaration
	class iterator;

public:

	/*
	 * empty constructor.
	 */
	static_rmq(){}

	/*
	 *
	 * constructor that initializes a LCS vector of size n
	 * and the pointer to RMQ data structure.
	 */
	static_rmq(uint_t n_): n(n_){
		// initialize the LCS array
		lcs = new sdsl::int_vector<>(n,INF);
		// init rmq pointer to null
		rmq = nullptr;
	}

	uint_t lcs_size(){ return n; }

	iterator begin() { /* return pointer to first entry in the lcs array */ return iterator(lcs,0); }
	iterator end(){ /* return pointer to last entry in the lcs array */ return iterator(lcs,n); }

	static_rmq & operator= (const static_rmq & other) {

		n = other.n;
		lcs = other.lcs;
		rmq = other.rmq;

	    return *this;
	}
	
	/* update RMQ ds according to the current LCS vector */
	void update_rmq_support()
	{
		// delete old ds if pointer is not null
		if(rmq != nullptr)
			delete rmq;
		// compute rmq data structures working
		// on the current LCS state
		rmq = new sdsl::rmq_succinct_sct<>(lcs);
	}

	/* update RMQ ds according to the current LCS vector */
	uint_t rm_query(uint_t i, uint_t j)
	{
		assert(j >= i);
		assert(j <= n);

		if(i == j)
			return (*lcs)[i];

		// return result of rmq query
		return (*lcs)[(*rmq)(i, j)];
	}

	/* return ith LCS entry */
	uint_t get(uint_t i)
	{
		assert(i<n);

		// return LCS entry
		return (*lcs)[i];
	}

	/* update the ith LCS entry */
	void update(uint_t i, uint_t val)
	{
		assert(i<n);
		// update LCS entry
		(*lcs)[i] = val;
	}

private:

	// vector length
	uint_t n;
	// LCP array
	sdsl::int_vector<>* lcs;
	// RMQ data structure
	sdsl::rmq_succinct_sct<>* rmq;
	
	// iterator class for the LCS vector
    class iterator
    {
    	public:

	        iterator():                    v(nullptr), i(0) {}
	        iterator(sdsl::int_vector<>* v, uint_t i): v(v),       i(i) {}

	        uint_t       operator*()             {return (*v)[i];}
	        const uint_t operator*()       const {return (*v)[i];}

	        iterator& operator++()          {++i;return *this;}
	        iterator  operator++(int)       {iterator r(*this);++i;return r;}

	        bool operator!=(const iterator &r) const {return i != r.i;}
	        bool operator==(const iterator &r) const {return i == r.i;}

	        uint_t pos() {return i;}

    	private:
    		// pointer to lcs vector
    		sdsl::int_vector<>* v;
    		// position in lcs vector
    		uint_t        i;
    };
    
};

}

#endif /* INTERNAL_STATIC_RMQ_HPP_ */