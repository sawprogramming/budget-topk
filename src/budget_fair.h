#ifndef _BUDGET_FAIR_
#define _BUDGET_FAIR_

#include "budget_util.h"
#include "topk_scoreset.h"
#include "topk_score_range.h"

namespace budget {

class Fair {
public:
	static topk::scoreset TopK(const size_t& k, std::initializer_list<topk::scoreset*> sets, const size_t& budget);
};

} // budget namespace

#endif