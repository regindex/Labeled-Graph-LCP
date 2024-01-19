// Copyright (c) 2023, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  Extension of Beller et al. algorithm for computing the LCS of a (pruned) Wheeler graph.
 */

//============================================================================

#ifndef INTERNAL_LCS_COMPUTATION_HPP_
#define INTERNAL_LCS_COMPUTATION_HPP_

#include "common.hpp"
#include "WG_string.hpp"

typedef std::pair<uint_t,uint_t> interval;
typedef std::queue<interval> queue;

template<class wg_t>
std::vector<uint_t> compute_LCS(wg_t& wg)
{
    // initialize LCS vector
    std::vector<uint_t> LCS(wg.get_no_nodes(),U_MAX);
    // initalize current and next queue
    queue* curr_queue = new queue();
    queue* next_queue = new queue();
    uint_t d = 0;
    // fill next queue with first interval
    next_queue->push(std::make_pair(0,wg.get_no_nodes()));

    while(not next_queue->empty())
    {
        // curr_queue is set to next queue
        std::swap(curr_queue,next_queue);
        // empty next_queue
        delete next_queue;
        next_queue = new queue();

        while(not curr_queue->empty())
        {
            interval curr_int = curr_queue->front(); 
            curr_queue->pop(); 

            if(LCS[curr_int.first] == U_MAX)
            { 
                // set LCP entry
                LCS[curr_int.first] = d-1;
                // perform forward step
                rank_pair intervals = 
                wg.forward_all(curr_int.first,curr_int.second);
                // fill d+1 queue
                for(uint_t i=0;i<intervals.first.size();++i)
                {
                    if(intervals.first[i] == intervals.second[i])
                        continue;
                    // push new interval in the queue
                    next_queue->push(std::make_pair(intervals.first[i],intervals.second[i]));
                }
            }
        }
        // increase distance
        ++d;
    }
    // fix first LCS entry
    LCS[0] = 0;
    // delete last pointers
    delete next_queue;
    delete curr_queue;

    // print the LCS
    std::cout << "LCS =====\n";
    for(uint_t i=0;i<LCS.size();++i)
    {
        std::cout << i << " : ";
        if( LCS[i] == U_MAX )
            std::cout << "inf\n";
        else
         std::cout << LCS[i] << "\n";
    }

    return LCS;
}

#endif /* INTERNAL_LCS_COMPUTATION_HPP_ */