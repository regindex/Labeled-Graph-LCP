// Copyright (c) 2024, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  common definitions.
 */

//============================================================================


#ifndef INTERNAL_COMMON_HPP_
#define INTERNAL_COMMON_HPP_

#include <vector>
#include <cassert>
#include <queue>
#include <algorithm>

#include <math.h>
#include <stdio.h>

#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/util.hpp>

#ifndef M64
	#define M64 0
#endif

#if M64
    typedef uint64_t uint_t;
    typedef uint8_t char_t;
    typedef bool bool_t;
    typedef sdsl::int_vector<64> int_vector;
    #define INF UINT64_MAX
    #define EMPTY UINT64_MAX - 1
#else
    typedef uint32_t uint_t;
    typedef uint8_t char_t;
    typedef bool bool_t;
    typedef sdsl::int_vector<32> int_vector;
    #define INF UINT32_MAX
    #define EMPTY UINT32_MAX - 1
#endif

typedef sdsl::wt_huff<>::value_type char_wt;
typedef sdsl::wt_huff<>::size_type uint_wt;
typedef std::pair<std::vector<uint_wt>,std::vector<uint_wt>> rank_pair;
typedef std::pair<std::vector<char_wt>,rank_pair> rank_pair_char;

/* function printing the LCP vector */
void print_LCP(const std::vector<uint_t>& LCP)
{
    std::cout << "LCP =====\n";
    for(uint_t i=0;i<LCP.size();++i)
    {
        std::cout << i << " : ";
        if( LCP[i] == INF )
            std::cout << "inf\n";
        else
         std::cout << LCP[i] << "\n";
    }
}

/* print vector to file */
void write_LCP(const std::string ofile_path, const std::vector<uint_t>& LCP)
{
    FILE * ofile = std::fopen(ofile_path.c_str(), "w");
    fwrite(&LCP[0], sizeof(uint_t), LCP.size(), ofile);
    fclose(ofile);
}

#endif /* INTERNAL_COMMON_HPP_ */