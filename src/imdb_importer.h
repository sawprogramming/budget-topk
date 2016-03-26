#ifndef _IMDB_IMPORTER_
#define _IMDB_IMPORTER_

#include <set>
#include <ppl.h>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <windows.h>
#include "movie.h"
#include "pcre.h"

class IMDB_Importer {
public:
	bool Import(const std::string directory = "");

	const std::set<Movie>& GetMovies() const;

private:
	bool ImportActors     (const std::string directory);
	bool ImportGenres     (const std::string directory);
	bool ImportMovies     (const std::string directory);
	bool ImportTags       (const std::string directory);

	std::set<Movie> movies_;
	std::ofstream   log_file_;
};

#endif