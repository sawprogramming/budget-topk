#include "budget_ranking.h"

namespace budget {

topk::scoreset Ranking::TopK(const size_t& K, std::initializer_list<topk::scoreset*> sets, const size_t& budget) {
	size_t                k = 0, i = 0;
	std::vector<size_t>   list_accesses = GenerateAllocationList(GenerateRankLists(sets, budget), sets.size(), budget, 0.50);
	topk::scoreset        results;
	topk::score_range_set topk;

	// get the iterators to the sets passed in so we can do sequential accesses
	std::vector<topk::score>::const_iterator* itrs = new std::vector<topk::score>::const_iterator[sets.size()];
	for (auto set : sets) {
		itrs[i++] = set->begin();
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

Ranking::rank_lists Ranking::GenerateRankLists(std::initializer_list<topk::scoreset*> sets, const size_t& budget) {
	size_t     i = 0;
	rank_lists output;

	// get the iterators to the sets passed in so we can do sequential accesses
	std::vector<topk::score>::const_iterator* itrs = new std::vector<topk::score>::const_iterator[sets.size()];
	std::vector<topk::score>::const_iterator* ends = new std::vector<topk::score>::const_iterator[sets.size()];
	for (auto set : sets) {
		itrs[i] = set->begin();
		ends[i++] = set->end();
	}

	// generate the rank lists
	for (i = 0; i < sets.size(); ++i) {
		double prev = 1;
		for (size_t j = 0; j < budget && (itrs[i] != ends[i]); ++j) {
			output.first.push_back(std::make_tuple(itrs[i]->first, i, itrs[i]->second));
			output.second.push_back(std::make_tuple(itrs[i]->first, i, prev - itrs[i]->second));
			prev = itrs[i]->second;
			(itrs[i])++;
		}
	}
	delete[] itrs;
	delete[] ends;

	// force the lists to sort
	std::sort(output.first.begin(), output.first.end(), [](const rank& left, const rank& right) { return std::get<2>(left) > std::get<2>(right); });
	std::sort(output.second.begin(), output.second.end(), [](const rank& left, const rank& right) { return std::get<2>(left) > std::get<2>(right); });

	return output;
}

std::vector<size_t> Ranking::GenerateAllocationList(const rank_lists& ranks, const size_t& num_lists, const size_t& budget, const double& alpha) {
	std::vector<size_t>                    list_accesses(num_lists, 0);
	std::vector<std::pair<size_t, size_t>> best_objs(num_lists, { -1, -1 });

	// initialize best objects
	for (size_t list = 0; list < num_lists; ++list) {
		SetNextBestObj(ranks, best_objs, list);
	}

	// schedule accesses based on sum of ranks
	for (size_t spent_budget = 0; spent_budget < budget; ++spent_budget) {
		size_t best_list = 0;

		// choose the best list
		for (size_t list = 1; list < num_lists; ++list) {
			if (CombinedScore(best_objs[list], alpha) < CombinedScore(best_objs[best_list], alpha)) {
				best_list = list;
			}
		}

		// increase the accesses the best list gets and find the next best score for it
		++list_accesses[best_list];
		SetNextBestObj(ranks, best_objs, best_list);
	}

	return list_accesses;
}


void Ranking::SetNextBestObj(const rank_lists& ranks, std::vector<std::pair<size_t, size_t>>& best_objs, const size_t& list) {
	const rank_list &score_list = ranks.first, &delta_list = ranks.second;

	// only find one if we aren't at the end of the list
	if ((best_objs[list].first + 1) < score_list.size()) {
		for (size_t score_index = best_objs[list].first + 1; score_index < score_list.size(); ++score_index) {
			if (std::get<1>(score_list[score_index]) == list) {
				for (size_t delta_index = 0; delta_index < delta_list.size(); ++delta_index) {
					if (std::get<1>(score_list[score_index]) == std::get<1>(delta_list[delta_index])) {
						if (std::get<0>(score_list[score_index]) == std::get<0>(delta_list[delta_index])) {
							best_objs[list] = { score_index, delta_index };
							return;
						}
					}
				}
			}
		}
	}

	// if nothing could be found, return an impossible object
	best_objs[list] = { score_list.size() + 1, delta_list.size() + 1 };
}

double Ranking::CombinedScore(const std::pair<size_t, size_t>& indexes, const double& alpha) {
	return alpha * indexes.first + (1 - alpha) * indexes.second;
}

} // budget namespace