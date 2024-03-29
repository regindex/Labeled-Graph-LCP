// Copyright (c) 2024, REGINDEX.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *  Extension of Manber-Myers suffix doubling algorithm for computing the LCP of a (pruned) Wheeler graph.
 *  input: ds_t ds: data structure containing 1) a mutable LCP vector supporting Range Minimum Queries,
 *                  2) a bitvector storing the bucket ids, 3) a vector storing the node predecessors.
 *  functionalities: update_LCP(i,j): perform LCP[i] = j.
 *                   begin_LCP(): interator pointing to first LCP position.
 *                   end_LCP(): interator pointing to last LCP position.
 *                   get_pred(i): get predecessor for node i.
 *                   get_bucket(i): get bucket id of node i.
 *                   get_h(): get prefix length of current doubling iteration.
 *                   rmq(i,j): get minimum value in LCP[i+1..j].
 */

//============================================================================

#ifndef INTERNAL_DOUBLING_ALGO_HPP_
#define INTERNAL_DOUBLING_ALGO_HPP_

#include "internal/common.hpp"
#include "internal/static_rmq.hpp"

template<class ds_t>
void prefix_doubling_LCP(ds_t& ds,bool check_output,bool printLCP,bool verb)
{
    // do first iteration for h = 1, then iternate until
    // we have finished all doubling steps.
    uint not_filled_yet = ds.get_no_nodes() - ds.get_sigma() - 1;
    do{
        if(verb) std::cout << "======= doubling step: " << ds.get_h() << 
            " - LCP entries not yet filled: " << not_filled_yet << "\n";
        // initialize iterator
        auto it = ds.begin_LCP();
        // iterate over LCP positions
        for(++it; it != ds.end_LCP(); ++it)
        {
            // if the LCP entry is empty, then try to fill in it
            if(*it == EMPTY)
            {
                // compute the predecessors of the ith and (i-1)th nodes
                // std::cout << it.pos() << " " << it.pos()-1 << "\n";
                uint pred = ds.get_pred(it.pos()), predm1 = ds.get_pred(it.pos()-1);
                // skip this instance of the cycle if pred is not valid
                if(pred == INF)
                {
                    ds.update_LCP(it.pos(), INF );
                    not_filled_yet--;
                }
                else
                {
                    // we fill in the ith LCP entry only if the predecessors
                    // induces i and i-1 from two different buckets.
                    if(ds.get_bucket(pred) != ds.get_bucket(predm1))
                    {
                        // update LCP value according to the current 
                        // prefix length h and the RMQ in LCP[pred(i-1)+1,pred(i)]
                        ds.update_LCP(it.pos(),ds.get_h() + 
                                               ds.rmq(predm1,pred) );
                        not_filled_yet--;
                    } // end-if
                } // end-else
            } // end-if
        } // end-for
    } // end-do 
    while ( (not_filled_yet > 0) && ds.doubling_step() ); // update ds data structure according to
    // fix first LCP entry        // the next prefix length: h = h*2
    ds.update_LCP(0,0);
    //ds.update_LCP(1,INF);
    // print the LCP vector
    if( printLCP )
        ds.print_LCP();
    // check output
    if( check_output )
        ds.check_output();

    ds.to_file_LCP(ds.get_path()+".LCP");
}

#endif /* INTERNAL_DOUBLING_ALGO_HPP_ */