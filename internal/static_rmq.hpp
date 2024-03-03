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
	 * constructor that initializes a LCP vector of size n
	 * and the pointer to RMQ data structure.
	 */
	static_rmq(uint_t n_): n(n_){
		// initialize the LCP array
		LCP = new int_vector(n,EMPTY);
		// init rmq pointer to null
		rmq = nullptr;
	}

	uint_t LCP_size(){ return n; }

	iterator begin() { /* return pointer to first entry in the LCP array */ return iterator(LCP,0); }
	iterator end(){ /* return pointer to last entry in the LCP array */ return iterator(LCP,n); }

	static_rmq & operator= (const static_rmq & other) {

		n = other.n;
		LCP = other.LCP;
		rmq = other.rmq;

	    return *this;
	}
	
	/* update RMQ ds according to the current LCP vector */
	void update_rmq_support()
	{
		// delete old ds if pointer is not null
		if(rmq != nullptr)
			delete rmq;
		// compute rmq data structures working
		// on the current LCP state
		rmq = new sdsl::rmq_succinct_sct<>(LCP);
	}

	/* update RMQ ds according to the current LCP vector */
	uint_t rm_query(uint_t i, uint_t j)
	{
		assert(j >= i);
		assert(j <= n);

		if(i == j)
			return (*LCP)[i];

		// return result of rmq query
		return (*LCP)[(*rmq)(i, j)];
	}

	/* return ith LCP entry */
	uint_t get(uint_t i)
	{
		assert(i<n);

		// return LCP entry
		return (*LCP)[i];
	}

	/* update the ith LCP entry */
	void update(uint_t i, uint_t val)
	{
		assert(i<n);
		// update LCP entry
		(*LCP)[i] = val;
	}

	/* return pointer to the LCP vector */
	int_vector* get_LCP_vector()
	{
		return LCP;
	}

private:

	// vector length
	uint_t n;
	// LCP array
	int_vector* LCP;
	// RMQ data structure
	sdsl::rmq_succinct_sct<>* rmq;
	
	// iterator class for the LCP vector
    class iterator
    {
    	public:

	        iterator():                    v(nullptr), i(0) {}
	        iterator(int_vector* v, uint_t i): v(v),       i(i) {}

	        uint_t       operator*()             {return (*v)[i];}
	        const uint_t operator*()       const {return (*v)[i];}

	        iterator& operator++()          {++i;return *this;}
	        iterator  operator++(int)       {iterator r(*this);++i;return r;}

	        bool operator!=(const iterator &r) const {return i != r.i;}
	        bool operator==(const iterator &r) const {return i == r.i;}

	        uint_t pos() {return i;}

    	private:
    		// pointer to LCP vector
    		int_vector* v;
    		// position in LCP vector
    		uint_t        i;
    };
    
};

}

#endif /* INTERNAL_STATIC_RMQ_HPP_ */