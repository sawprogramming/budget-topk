#ifndef _BUDGET_UTIL_
#define _BUDGET_UTIL_

#include "topk_scoreset.h"
#include "topk_score_range.h"

namespace budget {

static double AverageScore(const topk::scoreset& list, const size_t& num_accesses) {
	double sum = 0;

	for (auto score : list) {
		sum += score.second;
	}

	return sum / num_accesses;
}

static double ScoreReduction(const topk::scoreset& list, const size_t& num_accesses) {
	return list.at(0) - list.at(num_accesses - 1);
}

static double ListUtil(const topk::scoreset& list, const size_t& num_accesses, const double& alpha) {
	return alpha * AverageScore(list, num_accesses) + (1 - alpha) * ScoreReduction(list, num_accesses);
}

} // budget namespace

#endif