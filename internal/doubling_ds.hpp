// Copyright (c) 2024, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  doubling_ds: Implementation of the data structure containing all objects 
 *  		     needed to run the LCS prefix-doubling algorithm.
 *      modules: H: mutable LCS vector supporting RM queries.
 *               I: bitvector encoding bucket ids.
 *               M: vector storing node predecessors.
 */

#ifndef INTERNAL_DOUBLING_DS_HPP_
#define INTERNAL_DOUBLING_DS_HPP_

// include rmq data structure
#include "static_rmq.hpp"
#include "check_output.hpp"

namespace ds{

template<class rmq_t, class bit_vec_t>
class doubling_ds{

private:
	// character frequency
	std::vector<uint_t> freq;
	/* compute character frequencies */
	void compute_character_frequencies(std::string filepath, uint_t& size
	                                   , std::vector<uint_t>& freq)
	{
		// resize frequency vector
		freq = std::vector<uint_t>(128,0);
		// open L vector stream
		std::ifstream Lstring(filepath); char_t c;
		// read vector len
		Lstring.seekg(0, std::ios::end);
		size = Lstring.tellg();
    	Lstring.seekg(0, std::ios::beg);
    	// scan file
		for(uint_t i=0;i<size;++i){
			// read bit
			Lstring.read(reinterpret_cast<char*>(&c), sizeof(char_t));
			// increase character frequency
			freq[c]++;
		}
		// close stream
		Lstring.close();
	}

	/* check for the number of source edges */
	uint_t check_source(std::string filepath)
	{
		uint_t no_sources = 0; char_t bit = '1';
		// open stream
		std::ifstream bitstring(filepath);
		// check first bites
		while( bit == '1' ){
			// read bit
			bitstring.read(reinterpret_cast<char*>(&bit), sizeof(char_t));
			// increase no sources
			no_sources++;
		}
		// close stream
		bitstring.close();
		
		return no_sources - 1;
	}

	/* init bucket ids */
	void init_bucket_ids()
	{
		// set first bit
		I.set_bit(0); sigma = 0;
		// set sources bit
		uint_t sum = sources; I.set_bit(sum);
		// iterate over char freq
		for(uint_t i=0;i<freq.size();++i)
		{
			if(freq[i] != 0)
			{
				uint_t old = sum;
				// set a new bit
				I.set_bit(sum);
				// update bit in the LCP
				H.update(sum,0); sigma++;
				// position of the next bit
				sum += freq[i];
				// save sum in freq vector
				freq[i] = old;
			}
		}
		// initialize rank 1 support
		I.init_rank_1_support();
		// initialize rmq support
		H.update_rmq_support();
	}

	/* function to initialize the predecessor vector by reading .L and .out files */
	void init_predecessor_vector(std::string filepath,
								 std::string filepath2)
	{
		// resize predecessor vector
		M.resize(n);
		// open streams
		std::ifstream bitstring(filepath);
		std::ifstream Lstring(filepath2);
		// init variables
		char_t bit = '0', c; uint_t run = 0, id = 0;
		// set first entries
		for(uint_t i=0;i<sources;++i)
			M[i] = INF;
		// iterate over .out file
		for(uint_t i=0;i<(2*n)-sources;++i)
		{
			// read bit
			bitstring.read(reinterpret_cast<char*>(&bit), sizeof(char_t));
			//std::cout << bit << "";
			// extend a run of zeroes otherwise insert a new predecessor
			if(bit == '0')
				run++;
			else
			{
				for(uint_t j=0;j<run;++j)
				{
					Lstring.read(reinterpret_cast<char*>(&c), sizeof(char_t));
					//std::cout << c << "\n";
					// set a new entry in M vector
					M[freq[c]++] = id;
				}
				// start with a new zero run
				run = 0; id++;
			}
		}
		//std::cout << "\n";
		//for(uint_t j=0;j<n;++j)
			//std::cout << M[j] << " ";
		// close streams
		bitstring.close(); Lstring.close();
	}

    /* update predecessor vector for the next h*2 iteration */
	void update_predecessor_vector()
	{
		// initialize a temp vector
		//std::vector<uint_t> temp(n,INF);
		std::vector<uint_t> temp(n,INF);
		// iterate over all M positions
		for(uint_t i=sources;i<n;++i)
		{
			// only consider non-null predecessors
			if( M[i] != INF )
			{
				// fill in temporary vector
				temp[i] = M[i];
				// compute the new pred
				if( M[i] > i )
					M[i] = M[M[i]];
				else
					M[i] = temp[M[i]];
				//
				// if( M[i] == i ){ std::cout << "capita\n"; M[i] = INF; }
			}
		}
		//for(uint_t j=0;j<n;++j)
		//	std::cout << M[j] << " ";
		//exit(1);
		// clear temp vector
		temp.clear();
	}

	/* update bucket ids vector for the next h*2 iteration */
	void update_bucket_vector()
	{
		// old prefix length
		uint_t h_ = h/2;
		// iterate over the LCS vector
		for(uint_t i=sources;i<n;++i)
		{
			// set ith new bit in bucket bitvector if both
			// LCS[i] is nonempty and LCS[i] was filled in 
			// the previous round
			//if( (H.get(i) != INF) and (H.get(i) >= h_) )
			if( (H.get(i) < EMPTY) and (H.get(i) >= h_) )
				I.set_bit(i);
		}
		// init rank 1 support for the mutated bitvector
		I.init_rank_1_support();
	}

public:
	// empty constructor
	doubling_ds(){}
	/*
	* REWRITE THIS DESCRIPTION!!!
	* Constructor that takes in input the basepath of the input files and construct
	* the doubling data structure for running prefix doubling algorithm.
	*/
	doubling_ds(std::string basepath): path(basepath){
		// compute character frequencies
		compute_character_frequencies(basepath+".L",n,freq);
		// compute number of sources
		sources = check_source(basepath+".in"); n += sources;
		// initialize range minimum query data structure
		H = rmq_t(n);
		// construct bucket index data structure
		I = bit_vec_t(n);
		// write bucket ids and init rank support
		init_bucket_ids();
		// initialize predecessor vector
		init_predecessor_vector(basepath+".out", basepath+".L");
		// initialize h to 1
		h = 1;
	}

	/* double prefix length and update data structures accordignly */
	bool_t doubling_step()
	{
		// double prefix length
		h *= 2;
		if( h > (n-sources) ) 
			return false;
		// update predecessor vector
		update_predecessor_vector();
		// update bucket id vector
		update_bucket_vector();
		// update RMQ support
		H.update_rmq_support();

		return true;
	}

	/* return iterator pointing first LCS vector position */
	auto begin_lcs()
	{
		// return iterator for the first LCS position
		return H.begin();
	}

	/* return iterator pointing last LCS vector position */
	auto end_lcs()
	{
		// return iterator for the first LCS position
		return H.end();
	}

	/* get current prefix length */
	uint_t get_h()
	{
		// return iterator for the first LCS position
		return h;
	}

	/* get no. nodes */
	uint_t get_no_nodes()
	{
		// rewrite
		return n;
	}

	/* get no. nodes */
	uint_t get_sigma()
	{
		// rewrite
		return sigma;
	}

	/* get predecessor of a certain node */
	uint_t get_pred(uint_t i)
	{
		// return the pref
		return M[i];
	}

	/* get bucket id of a certain node */
	uint_t get_bucket(uint_t i)
	{
		// return rank query on I for index i 
		return I.rank_1(i+1);
	}

	/* get rm query of LCS[i+1..j] */
	uint_t rmq(uint_t i, uint_t j)
	{
		// return rmq query between i and j
		return H.rm_query(i+1,j);
	}

	/* update an LCS entry */
	void update_lcs(uint_t i, uint_t val)
	{
		//
		H.update(i,val);
	}

	/* print the LCS vector */
	void print_lcs()
	{
		std::cout << "LCS ====" << std::endl;
		for(auto it = H.begin();it != H.end();++it)
		{
			std::cout << it.pos() << " : ";
			if(*it == INF)
				std::cout << "inf" << std::endl;
			else
				std::cout << *it << std::endl;
		}
	}

	/* */
	void check_output()
	{
		// std::cout << path;
		// 
		check_LCS_correctness<sdsl::int_vector<>>(path,n,H.get_lcs_vector());
	}

private:
	// number of nodes
	uint_t n;
	// number of sources
	uint_t sources;
	// alphabet size
	uint_t sigma;
	// mutable LCS and RMQ data structure
	rmq_t H;
	// bitvector encoding bucket indexes
	bit_vec_t I;
	// predecessor vector
	std::vector<uint_t> M;
	// prefix length
	uint_t h;
	// input file path
	std::string path;
};

}

#endif