#ifndef _MOVIE_H_
#define _MOVIE_H_

#include <set>
#include <string>
#include <iostream>

class Movie {
public:
	Movie(const std::string& title = "", const unsigned __int16& year = 0);

	friend bool          operator<  (const Movie& left, const Movie& right);
	friend std::istream& operator>> (std::istream& is, Movie& m);
	friend std::ostream& operator<< (std::ostream& os, const Movie& m);

	void AddActor (const std::string& actor) const;
	void AddGenre (const std::string& genre) const;
	void AddTag   (const std::string& tag  ) const;
	
	const __int16&               GetYear   () const;
	const std::string&           GetTitle  () const;
	const std::set<std::string>& GetActors () const;
	const std::set<std::string>& GetGenres () const;
	const std::set<std::string>& GetTags   () const;

private:
	__int16                       year_;
	std::string                   title_;
	mutable std::set<std::string> tags_;
	mutable std::set<std::string> genres_;
	mutable std::set<std::string> actors_;
};

#endif