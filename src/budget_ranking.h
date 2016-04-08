#ifndef _BUDGET_RANKING_
#define _BUDGET_RANKING_

#include <tuple>
#include "budget_util.h"
#include "topk_scoreset.h"
#include "topk_score_range.h"

namespace budget {

class Ranking {
public:
	static topk::scoreset TopK(const size_t& k, std::initializer_list<topk::scoreset*> sets, const size_t& budget);

private:
	typedef std::tuple<size_t, size_t, double> rank;
	typedef std::vector<rank>                  rank_list;
	typedef std::pair<rank_list, rank_list>    rank_lists;

	static rank_lists          GenerateRankLists      (std::initializer_list<topk::scoreset*> sets, const size_t& budget);
	static std::vector<size_t> GenerateAllocationList (const rank_lists& ranks, const size_t& num_lists, const size_t& budget, const double& alpha);
	static void                SetNextBestObj         (const rank_lists& ranks, std::vector<std::pair<size_t, size_t>>& best_objs, const size_t& list);
	static double              CombinedScore          (const std::pair<size_t, size_t>& indexes, const double& alpha);
};

} // budget namespace

#endif