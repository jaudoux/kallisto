#ifndef KALLISTO_WEIGHTS_H
#define KALLISTO_WEIGHTS_H

#include "KmerIndex.h"

#include <unordered_map>
#include <utility>
#include <vector>

using WeightMap = std::unordered_map<int, std::vector<double>>;

std::vector<double> calc_eff_lens(const std::vector<int>& lengths, double mean) {
  // for now do the total naive thing and subtract mean frag length
  std::vector<double> eff_lens;
  eff_lens.reserve(lengths.size());

  auto n_too_short = 0;

  for (auto& cur_len: lengths) {
      double cur_len_dbl = static_cast<double>(cur_len);
      double cur_eff_len = cur_len_dbl - mean + 1.0;
      if (cur_eff_len < 1.0) {
          cur_eff_len = 1.0;
          ++n_too_short;
      }
      eff_lens.push_back( cur_eff_len );
  }

  if (n_too_short > 0) {
      std::cerr << "Total transcripts with effective length less than " << mean <<
          ":\t" << n_too_short  << std::endl;
  }

  return eff_lens;
}


WeightMap calc_weights(
  const std::vector<int>& counts,
  const EcMap& ecmap,
  const std::vector<double>& eff_lens) {

  // TODO: throw some assertions in here to make sure the length of counts
  // and ec map are correct... as well as eff_lens size is reasonable

  // weights are stored _exactly_ in the same orientation as the ec map
  WeightMap weights;

  for (auto& kv : ecmap) {

    //std::cout << kv.first;
    std::vector<double> trans_weights;
    trans_weights.reserve(kv.second.size());

    for (auto& trans_id : kv.second) {
      trans_weights.push_back( static_cast<double>(counts[kv.first]) /
                               eff_lens[trans_id] );
    }

    weights.insert( {kv.first, trans_weights} );
  }


  return weights;
}

#endif // KALLISTO_WEIGHTS_H