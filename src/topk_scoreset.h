#ifndef _SCORESET_H_
#define _SCORESET_H_

#include <set>
#include <utility>

namespace topk {

typedef std::pair<size_t, double> score;

struct score_comp {
	// sort scoresets by score descending, movie id ascending
	bool operator() (const score& left, const score& right) const {
		if      (left.second  > right.second) return  true;
		else if (right.second <  left.second) return false;
		else if (left.first   <  right.first) return  true;
		else                                  return false;
	}
};

typedef std::multiset<score, score_comp> scoreset; 

} // topk namespace

#endif