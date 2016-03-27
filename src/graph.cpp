#include "graph.h"

template <class F, class S>
bool graph<F, S>::Insert(const F& value, const vertex& pos) {
	if (!HasVertex(value)) {
		f_vertices_[value] = pos;
		return true;
	}

	return false;
}

template <class F, class S>
bool graph<F, S>::Insert(const S& value, const vertex& pos) {
	if (!HasVertex(value)) {
		s_vertices_[value] = pos;
		return true;
	}

	return false;
}

template <class F, class S>
bool graph<F, S>::HasVertex(const F& value) const {
	return f_vertices_.find(value) != f_vertices_.end(); 
}

template <class F, class S>
bool graph<F, S>::HasVertex(const S& value) const {
	return s_vertices_.find(value) != s_vertices_.end(); 
}

template <class F, class S>
bool graph<F, S>::AddEdge(const F& from, const S& to) {
	vertex f, t;

	if (HasVertex(from) && HasVertex(to)) {
		f = f_vertices_.at(from);
		t = s_vertices_.at(to);
		adjacency_list_[f].push_back(t);
		return true;
	}

	return false;
}

template <class F, class S>
void graph<F, S>::SetSize(const size_t& size) {
	adjacency_list_.resize(size);
}

template <class F, class S>
const std::vector<std::vector<vertex>>& graph<F, S>::GetAdjacencyList() const {
	return adjacency_list_;
}