// Copyright (c) 2024, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  Extension of Beller et al. algorithm for computing the LCP of a (pruned) Wheeler graph.
 *  Implementation of the interval stabbing algorithm computing the LCP of a (pruned) Wheeler graph.
 */

//============================================================================

#ifndef INTERNAL_STABBING_HPP_
#define INTERNAL_STABBING_HPP_

#include "internal/common.hpp"
#include "internal/WG_string.hpp"
#include "internal/hybrid_queue.hpp"
#include "internal/check_output.hpp"

/* Code implementing the generalization of the Beller et al. algorithm 
 * input: 
        wg: (wg_t) Wheeler graph FM-index data structure
        check_output: (bool) flag saying if we need to check the result correctness    
        printLCP: (bool) flag saying if we need to print the LCP vector
        verb: (bool) flag saying if we need to activate the verbose mode
    output:
        LCP: (std::vector<uint_t>) a vector containing the LCP values
*/
template<class wg_t>
std::vector<uint_t> compute_LCP_Beller_gen(wg_t& wg,bool check_output,bool printLCP, bool verb)
{
    // initialize needed variables
    interval curr_int; 
    // initialize LCP vector
    std::vector<uint_t> LCP(wg.get_no_nodes(),INF);
    // initalize interval queue
    wg::hybrid_queue<interval> queue(wg.get_no_nodes());
    // fill next queue with first interval
    queue.push(0,wg.get_no_nodes());

    // scan intervals for next l value
    while(queue.set_queue_for_next_l())
    {
        if(verb) std::cout << "======== filling LCP values=" << queue.get_l() << 
            " - size of the queue= " << queue.get_size() << std::endl;
        // scan all intervals associated to the
        // current LCP value
        while(not queue.empty())
        {
            // pop interval on the top of the queue
            queue.pop_front(curr_int); 
            //std::cout << curr_int.first << " " << curr_int.second << std::endl;

            // we propagate an [l,r] interval only if we 
            // set a new LCP value in LCP[l] 
            if(LCP[curr_int.first] == INF)
            { 
                // set LCP entry
                LCP[curr_int.first] = queue.get_l()-1;
                // perform forward step
                rank_pair intervals = 
                wg.forward_all(curr_int.first,curr_int.second);
                // fill d+1 queue
                for(uint_t i=0;i<intervals.first.size();++i)
                {
                    //std::cout << "--> " << intervals.first[i] << " " << intervals.second[i] << "\n";
                    if(LCP[intervals.first[i]] != INF)
                        continue;

                    // push new interval in the queue
                    queue.push(intervals.first[i],intervals.second[i]);
                }
            } // end if
        } // end while
    } // end while
    // fix first LCP entry
    LCP[0] = 0;
    // check output
    if( check_output )
        check_LCP_correctness<std::vector<uint_t>>(wg.get_path(),wg.get_no_nodes(),&LCP);
    //print LCP
    if( printLCP )
        print_LCP(LCP);

    write_LCP(wg.get_path()+".LCP", LCP);

    return LCP;
}

/* Code implementing the interval stabbing algorithm
 * input: 
        wg: (is_t) Interval stabbing data structure containing the Wheeler graph FM-index
        check_output: (bool) flag saying if we need to check the result correctness    
        printLCP: (bool) flag saying if we need to print the LCP vector
        verb: (bool) flag saying if we need to activate the verbose mode
    output:
        LCP: (std::vector<uint_t>) a vector containing the LCP values
*/
template<class is_t>
std::vector<uint_t> compute_LCP_interval_stabbing(is_t& stab_ds,bool check_output,bool printLCP, bool verb)
{
    // initialize needed variables
    uint_t curr_i; 
    // initialize LCP vector
    std::vector<uint_t> LCP(stab_ds.no_nodes(),INF);
    // initalize interval queue
    wg::hybrid_queue<uint_t> queue(stab_ds.no_nodes(),true);
    // fill next queue with first interval
    std::vector<uint_t> first_entries = stab_ds.get_0_entries();
    for(uint_t i=0;i<first_entries.size();++i)
    {
        //std::cout << "inserisco: " << first_entries[i] << std::endl;
        queue.push(first_entries[i]);
        LCP[first_entries[i]] = 0;
    }

    // scan intervals for next l value
    while(queue.update_interval_stabbing_queue())
    {
        if(verb) std::cout << "======== filling LCP values=" << queue.get_l()+1 << 
                        " - size of the queue= " << queue.get_size() << std::endl;
        //if(queue.get_l()+1 == 5){ exit(1); }
        // scan all intervals associated to the current LCP value
        while(not queue.empty())
        {
            // pop interval on the top of the queue
            queue.pop_front(curr_i); 
            //std::cout << "Processing: " << curr_i << " " << curr_i + 1 << std::endl;
            // compute stab query in the interval list
            std::vector<bridge> bridges = 
                 stab_ds.interval_stab_query(stab_ds.wfm_index->pos_to_node(curr_i));
            // Process all stabbed bridges
            for(uint_t i=0;i<bridges.size();++i)
            {
                //std::cout << char(bridges[i].first) << " " << bridges[i].second << std::endl;
                uint_t i_p = stab_ds.wfm_index->forward(bridges[i].second,bridges[i].first);
                // set new LCP entry
                LCP[i_p] = queue.get_l()+1;
                // std::cout << "insert--> " << i_p << "\n";
                // push new value in the queue
                queue.push(i_p);
            }
        }
    }
    // fix first LCP entry
    LCP[0] = 0;
    // print LCP
    if( printLCP )
        print_LCP(LCP);
    // check output
    if( check_output )
        check_LCP_correctness<std::vector<uint_t>>(stab_ds.wfm_index->get_path(),
                                                   stab_ds.wfm_index->get_no_nodes(),&LCP);


    write_LCP(stab_ds.wfm_index->get_path()+".LCP", LCP);

    return LCP;
}

#endif /* INTERNAL_LCP_COMPUTATION_HPP_ */