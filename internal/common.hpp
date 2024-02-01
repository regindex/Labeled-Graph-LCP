// Copyright (c) 2023, REGINDEX.  All rights reserved.
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

#include <math.h>

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
    #define INF UINT64_MAX
#else
    typedef uint32_t uint_t;
    typedef uint8_t char_t;
    typedef bool bool_t;
    #define INF UINT32_MAX
#endif

typedef sdsl::wt_huff<>::value_type char_wt;
typedef sdsl::wt_huff<>::size_type uint_wt;
typedef std::pair<std::vector<uint_wt>,std::vector<uint_wt>> rank_pair;
typedef std::pair<std::vector<char_wt>,rank_pair> rank_pair_char;

#endif /* INTERNAL_COMMON_HPP_ */