#ifndef _IMDB_IMPORTER_
#define _IMDB_IMPORTER_

#include <set>
#include <ppl.h>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <vector>
#include "graph.h"
#include "movie.h"
#include "pcre.h"

class IMDB_Importer {
public:
	bool Import(const std::string directory = "");

	const std::vector<Movie>& GetMovies() const;

private:
	bool ImportActors     (const std::string directory);
	bool ImportGenres     (const std::string directory);
	bool ImportMovies     (const std::string directory);
	bool ImportTags       (const std::string directory);

	//std::set<Movie> movies_;
	std::ofstream   log_file_;

	std::vector<Movie>       movies_;
	std::vector<std::string> actors_;
	std::vector<std::string> genres_;
	std::vector<std::string> tags_;

	graph<Movie, std::string> movie_to_actor_;
	graph<Movie, std::string> movie_to_genre_;
	graph<Movie, std::string>   movie_to_tag_;
};

#endif