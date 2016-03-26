#include "movie.h"

Movie::Movie(const std::string& title, const unsigned __int16& year) : title_(title), year_(year) {}

bool operator<(const Movie& left, const Movie& right) {
	if      (_strcmpi(left.title_.c_str(), right.title_.c_str()) < 0) return  true;
	else if (_strcmpi(left.title_.c_str(), right.title_.c_str()) > 0) return false;
	else if (left.year_ < right.year_)                                return  true;
	else                                                              return false;
}

std::ostream& operator<<(std::ostream& os, const Movie& m) {
	os << m.title_ << " (" << m.year_ << ")" << std::endl;

	os << "\tGenre(s): ";
	for (auto genre : m.genres_) {
		os << genre << ", ";
	}
	os << std::endl;

	os << "\tTags(s): ";
	for (auto tag : m.tags_) {
		os << tag << ", ";
	}
	os << std::endl;

	os << "\tActors(s):" << std::endl;
	for (auto actor : m.actors_) {
		os << "\t\t" << actor << std::endl;
	}

	return os;
}


void Movie::AddActor (const std::string& actor) const { actors_.insert(actor); }
void Movie::AddGenre (const std::string& genre) const { genres_.insert(genre); }
void Movie::AddTag   (const std::string&   tag) const {     tags_.insert(tag); }

const __int16&               Movie::GetYear   () const { return   year_; }
const std::string&           Movie::GetTitle  () const { return  title_; }
const std::set<std::string>& Movie::GetActors () const { return actors_; }
const std::set<std::string>& Movie::GetGenres () const { return genres_; }
const std::set<std::string>& Movie::GetTags   () const { return   tags_; }