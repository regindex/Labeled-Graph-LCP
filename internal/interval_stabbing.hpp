// Copyright (c) 2024, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#ifndef INTERNAL_INTERVAL_STABBING_HPP_
#define INTERNAL_INTERVAL_STABBING_HPP_

#include "common.hpp"
#include "WG_string.hpp"

// define birdges data type
typedef std::pair<char_t,uint_t> bridge;

namespace wg{

template<class wfmi_t>
class interval_stabbing{

private:
	// mask vector containing 128,64,32 ... in hex.
	static constexpr unsigned char mask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	// check if s[i] == 1
	inline bool bget(const char* s, uint i) { return ( (s[i/8]&mask[i%8]) ? 1 : 0 ); }
	// set ith bit to 1
	inline void bset(char* s, uint i) { s[(i)/8]=1 ? mask[(i)%8]|s[(i)/8] : (~mask[(i)%8])&s[(i)/8]; } 
	// set ith bit to 0
	inline void bunset(char* s, uint i) { s[(i)/8]=0 ? mask[(i)%8]|s[(i)/8] : (~mask[(i)%8])&s[(i)/8]; } 
	// compute ith BWT block offset in the packed BWT vector
	uint_t compute_BWT_offset(uint_t i) { return (i*128 + i*32) + 16; }
	// compute 8 bytes unsigned int from sequence of bytes
	inline uint64_t get_8_uint(char* buf)
	{
		return ((uint64_t)(((uint8_t *)(buf))[0])<<56) |
		       ((uint64_t)(((uint8_t *)(buf))[1])<<48) |
		       ((uint64_t)(((uint8_t *)(buf))[2])<<40) |
		       ((uint64_t)(((uint8_t *)(buf))[3])<<32) |
		       ((uint64_t)(((uint8_t *)(buf))[4])<<24) |
		       ((uint64_t)(((uint8_t *)(buf))[5])<<16) |
		       ((uint64_t)(((uint8_t *)(buf))[6])<<8) |
		       ((uint64_t)(((uint8_t *)(buf))[7])<<0);
	}
	// void print packet bwt()
	void print_bwt()
	{
		uint_t offset = 0;
    	for(uint_t i=0;i<no_blocks;++i)
    	{	
    		for(uint_t j=0;j<128;++j)
    			std::cout << bget(&packed_BWT[offset],j);
    		offset += 16;
    		std::cout << "|";
    		for(uint_t j=0;j<128;++j)
    			std::cout << packed_BWT[offset+j];
    		std::cout << "|";
    		offset += 128;
    		for(uint_t j=0;j<128;++j)
    			std::cout << bget(&packed_BWT[offset],j);
    		offset += 16;
    		std::cout << "|";
    	}
		for(uint_t j=0;j<128;++j){
			std::cout << bget(&packed_BWT[offset],j);
		}
    	std::cout << std::endl;
	}
	
public:
	//
	wfmi_t* wfm_index;

	// empty constructor
	interval_stabbing(){}
	/*
	* Constructor that takes in input the BWT and the FM-index of the 
	* Wheeler graph to compute a bit-packed data structure simulating
	* dynamic interval stabbing queries
	*/
	interval_stabbing(std::string filepath, wfmi_t* i): wfm_index(i)
	{
		// open stream
		std::ifstream bitstring(filepath);
		// compute bitvector length
		bitstring.seekg(0, std::ios::end);
		this->size = bitstring.tellg();
    	bitstring.seekg(0, std::ios::beg);
    	// compute number of blocks
    	no_blocks = std::ceil(double(this->size)/sigma);
    	no_bytes = (no_blocks * sigma) + (no_blocks * 2 * (sigma/8)) + (sigma/8);
    	// initialize packed BWT array with the following structure: |16 bits|128 bits|16 bits|16 bits|
    	// 16 bits for the bitvector storing the characters crossing the left border
    	// 128 bits for storing a BWT block of 128 characters
    	// 16 bits for the bitvector storing the stabbed intervals
    	// 16 bits for the bitvector storing the characters crossing the right border
    	packed_BWT = new char[no_bytes+1];
    	X = new char[128]; 
    	memset(packed_BWT,0,no_bytes+1); memset(X,0,128);
    	
    	uint_t n=0, cnt=0;
    	std::vector<int> last_occ(sigma,-1);
    	std::vector<bool> curr_block(sigma,0);
    	// construct n/sigma blocks
    	while(n < size){
    		// increment block count
    		cnt++;
    		// reset current block
    		std::fill(curr_block.begin(), curr_block.end(), 0);
    		// init needed variables
    		uint_t block_idx = compute_BWT_offset(n/128);
    		// iterate over all blocks of size sigma
    		for(uint_t i=0;i<std::min(sigma,size-n);++i)
	    	{
	    		char curr;
	    		bitstring.read(reinterpret_cast<char*>(&curr), sizeof(char));
	    		packed_BWT[block_idx++] = curr;
	    		curr_block[curr] = 1;
	    		// fill bitvectors storing crossing intervals
	    		if(last_occ[curr] != -1)
	    		{
	    			if(last_occ[curr] != cnt)
	    			{
	    				// set bits for crossing intervals
	    				for(uint_t j=last_occ[curr];j<cnt;++j)
	    					bset(&packed_BWT[compute_BWT_offset(j-1)+128+16],curr);
	    			}
	    		}
	    		last_occ[curr] = cnt;
	    	}
	    	// go to the next block 
	    	n += sigma;
    	}
    	// close input stream
    	bitstring.close();
	}

	int64_t check_interval_crossing_border(uint_t i, char_t c, sdsl::wt_huff<>& wt) // b|blo (b) cco|bordo
	{
		////// std::cout << "i: " << i << " c: " << c << std::endl;
		uint_t rank, left = 0, right = 0;
		int64_t result = -1;
		// compute left and right end of the interval
		rank = wt.rank(i,c);
		if(rank > 0)
		{
			left = wt.select(rank,c); right = wt.select(rank+1,c);
			////// std::cout << "(" << left << ".." << right << ")\n";
			// check if the interval intersects i and is not stabbed already
			if((i > left and i <= right) and (!bget(&packed_BWT[compute_BWT_offset(left/128)+128],left)))
			{
				// set interval as stabbed
				bset(&packed_BWT[compute_BWT_offset(left/128)+128],left);
				// remove character from borders
				for(uint_t j=left/128;j<right/128;++j)
					bunset(&packed_BWT[compute_BWT_offset(j)+128+16],c);
				// store result
				result = right;
			}
		}

		return result;
	}

	/* check if we are stabbing some intervals crossing a border */
	int64_t get_crossing_interval(uint_t i, char_t c, bool is_left_border)
	{
		uint_t rank, left = 0, right = 0;
		int64_t result = -1;
		// compute left and right end of the interval
		rank = wfm_index->rank_wt(i,c);
		//std::cout << "rank= " << rank << std::endl;
		if(rank > 0 and rank < wfm_index->char_occs_bwt(c))
		{
			// use the BWT wavelet tree to get the closest left and right occurrences
			left = wfm_index->select_wt(rank-1,c); right = wfm_index->select_wt(rank,c);
			// std::cout << "(" << left << ".." << right << "]" << std::endl;
			// std::cout << "(" << left - ((left/128)*128) << ".." << right - ((right/128)*128) << "]" << std::endl;
			// compute the offset in the block
			uint_t block_offset = left - ((left/128)*128);
			// get bool marking if (left,right] has been already stabbed
			bool_t stabbed = bget(&packed_BWT[compute_BWT_offset(left/128)+128],block_offset);
			// return the interval (left,right] if it is the first time it gets stabbed.
			if((i > left and i <= right) and (not stabbed))
			{
				// set interval as stabbed
				bset(&packed_BWT[compute_BWT_offset(left/128)+128],block_offset);
				// remove character from borders
				for(uint_t j=left/128;j<right/128;++j)
					bunset(&packed_BWT[compute_BWT_offset(j)+128+16],c);
				// store result
				result = right;
			}
			//}
		}
		return result;
	}

	/* compute an interval stabbing query */
	std::vector<bridge> interval_stab_query(uint_t i)
	{
		// prefetch needed memory locations
		__builtin_prefetch(X);
		// clear X vector
		std::memset(X,-1,128);
		// identify block
		uint_t b = i/128, offset, block_off;
		offset =  i - (b*128);
		block_off = compute_BWT_offset(b);
		// prefetch needed memory locations
		__builtin_prefetch(&packed_BWT[block_off-16]);

		std::vector<bridge> res;
		// first scan in an internal block
		for(uint_t j=0;j<offset;++j)
			X[packed_BWT[block_off + j]] = j;
		// second scan in an internal block
		for(uint_t j=offset;j<128;++j)
		{
			// if > -1 we have stabbed an interval
			if(X[packed_BWT[block_off + j]] > -1)
			{
				// check if interval has been already stabbed
				if(!bget(&packed_BWT[block_off+128],X[packed_BWT[block_off + j]]))
				{
					// push an interval in the results and the set corresponding bit to 1
					res.push_back(std::make_pair(packed_BWT[block_off + j],b*128 + j));
					bset(&packed_BWT[block_off+128],X[packed_BWT[block_off + j]]);
				}
				// set that we have already stabbed an interval for character X[packed_BWT[block_off + j]]
				X[packed_BWT[block_off + j]] = -2;
			} 
		}
		// scan first left-word
		uint64_t word = get_8_uint(&packed_BWT[block_off-16]);
		if(word != 0)
			for(uint_t j=0;j<64;++j)
			{
				uint_t pos;
				if(bget(&packed_BWT[block_off-16],j))
				{
					pos = j;
					// check character in the border only if we do not have already
					// stabbed an interval of the same character 
					if(X[pos] != -2)
					{
						int64_t b =  get_crossing_interval(i,pos,true);
						if(b > -1){ res.push_back(std::make_pair(pos,(uint64_t)b)); }
					}
				}
			}
		// scan second left-word
		word = get_8_uint(&packed_BWT[block_off-8]);
		if(word != 0)
			for(uint_t j=0;j<64;++j)
			{
				uint_t pos;
				if(bget(&packed_BWT[block_off-8],j))
				{
					pos = 64 + j;
					// check character in the border only if we do not have already
					// stabbed an interval of the same character 
					if(X[pos] != -2)
					{
						int64_t b =  get_crossing_interval(i,pos,true);
						if(b > -1){ res.push_back(std::make_pair(pos,(uint64_t)b)); }
					}
				}
			}
		// scan right border
		word = get_8_uint(&packed_BWT[block_off+144]);
		if(word != 0)
			for(uint_t j=0;j<64;++j)
			{
				uint_t pos;
				if(bget(&packed_BWT[block_off+144],j))
				{
					pos = j;
					// check character in the border only if we do not have already
					// stabbed an interval of the same character 
					if(X[pos] != -2)
					{
						int64_t b =  get_crossing_interval(i,pos,false);
						if(b > -1){ res.push_back(std::make_pair(pos,(uint64_t)b)); }
					}
				}
			}
		//
		word = get_8_uint(&packed_BWT[block_off+152]);
		if(word != 0)
			for(uint_t j=0;j<64;++j)
			{
				uint_t pos;
				if(bget(&packed_BWT[block_off+152],j))
				{
					pos = 64 + j;
					// check character in the border only if we do not have already
					// stabbed an interval of the same character 
					if(X[pos] != -2)
					{
						int64_t b =  get_crossing_interval(i,pos,false);
						if(b > -1){ res.push_back(std::make_pair(pos,(uint64_t)b)); }
					}
				}
			}

		return res;
	}

	/* get LCP entries containing a zero value */
	std::vector<uint_t> get_0_entries()
	{
		std::vector<uint_t> res;
		std::vector<bridge> occs = wfm_index->first_char_occs();
		// forward each position
		for(uint_t i=0;i<occs.size();++i)
			res.push_back(wfm_index->forward(occs[i].second,occs[i].first));

		return res;
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

	/* get input graph number of nodes */
	uint_t no_nodes()
	{
		return wfm_index->get_no_nodes();
	} 

private:
	// array packing BWT blocks + bitvectors
	char* packed_BWT;
	// ausiliary array
	char* X;
	// size 
	uint_t size;
	// no blocks
	uint_t no_blocks, no_bytes;
	// alphabet size
	const uint_t sigma = 128;
};

}

#endif