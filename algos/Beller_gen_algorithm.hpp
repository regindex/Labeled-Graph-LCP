// Copyright (c) 2023, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  Extension of Beller et al. algorithm for computing the LCS of a (pruned) Wheeler graph.
 */

//============================================================================

#ifndef INTERNAL_LCS_COMPUTATION_HPP_
#define INTERNAL_LCS_COMPUTATION_HPP_

#include "internal/common.hpp"
#include "internal/WG_string.hpp"
#include "internal/hybrid_queue.hpp"

void print_LCS(std::vector<uint_t>& LCS)
{
    // print the LCS
    std::cout << "LCS =====\n";
    for(uint_t i=0;i<LCS.size();++i)
    {
        std::cout << i << " : ";
        if( LCS[i] == INF )
            std::cout << "inf\n";
        else
         std::cout << LCS[i] << "\n";
    }
}

template<class wg_t>
std::vector<uint_t> compute_LCS(wg_t& wg)
{
    // initialize LCS vector
    std::vector<uint_t> LCS(wg.get_no_nodes(),INF);
    // initalize interval queue
    wg::hybrid_queue queue(wg.get_no_nodes());
    // fill next queue with first interval
    queue.push(0,wg.get_no_nodes());

    // scan intervals for next l value
    while(queue.set_queue_for_next_l())
    {
        //// std::cout << "=== l=" << queue.get_l() << "\n";
        // scan all intervals associated to the
        // current LCP value
        while(not queue.empty())
        {
            // pop interval on the top of the queue
            interval curr_int = queue.pop_front(); 
            //// std::cout << curr_int.first << " " << curr_int.second << std::endl;

            // we propagate an [l,r] interval only if we 
            // set a new LCS value in LCS[l] 
            if(LCS[curr_int.first] == INF)
            { 
                // set LCP entry
                LCS[curr_int.first] = queue.get_l()-1;
                // perform forward step
                rank_pair intervals = 
                wg.forward_all(curr_int.first,curr_int.second);
                // fill d+1 queue
                for(uint_t i=0;i<intervals.first.size();++i)
                {
                    if(LCS[intervals.first[i]] != INF)
                        continue;

                    // push new interval in the queue
                    queue.push(intervals.first[i],intervals.second[i]);
                }
            } // end if
        } // end while
    } // end while
    // fix first LCP entry
    LCS[0] = 0;

    //print LCS
    print_LCS(LCS);

    return LCS;
}

#endif /* INTERNAL_LCS_COMPUTATION_HPP_ */