#include "budget_fair.h"

namespace budget {

topk::scoreset Fair::TopK(const size_t& K, std::initializer_list<topk::scoreset*> sets, const size_t& budget) {
	size_t                i = 0, k = 0;
	topk::scoreset        results;
	topk::score_range_set topk;
	std::vector<double>   estimated_utilities;
	std::vector<size_t>   list_accesses;
	double                sum_utilities = 0;

	// get the iterators to the sets passed in so we can do sequential access and calculate the utility of each list
	std::vector<topk::score>::const_iterator* itrs = new std::vector<topk::score>::const_iterator[sets.size()];
	for (auto set : sets) {
		itrs[i] = set->begin();
		estimated_utilities.push_back(ListUtil(*set, budget, 0.50));
		sum_utilities += estimated_utilities[i++];
	}

	// set how many accesses each list gets
	for (i = 0; i < estimated_utilities.size(); ++i) {
		list_accesses.push_back(budget * estimated_utilities[i] / sum_utilities);
	}

	// calculate the top-k using these numbers
	for (i = 0; i < list_accesses.size(); ++i) {
		for (size_t j = 0; j < list_accesses[i]; ++j) {
			topk::score rank = *(itrs[i]);

			// add the key to the top-k if it isn't in there
			if (!topk.HasKey(rank.first)) {
				topk.push_back(topk::score_range_pair(rank.first, sets.size()));
			}

			// update the worst and best scores for each value in the topk list
			for (auto temp = topk.begin(false); temp != topk.end(); ++temp) {
				if (temp->first == rank.first)  temp->second.Discover(i, rank.second);
				else                            temp->second.UpdateRange(i, rank.second);
			}

			(itrs[i])++;
		}
	}
	delete[] itrs;

	// sort the results!
	topk.at(0);

	// return the top-k
	for (auto scorerange : topk) {
		results.push_back({ scorerange.first, scorerange.second.worst_score() });
		if (++k == K) break;
	}
	return results;
}

} // budget namespace