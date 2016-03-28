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
#include "imdb_dataset.h"

namespace IMDB {

class Importer {
public:
	bool Import(dataset& s);

private:
	bool ImportActors ();
	bool ImportGenres ();
	bool ImportMovies ();
	bool ImportTags   ();

	//std::set<Movie> movies_;
	std::ofstream   log_file_;
	dataset*        data_;
};

} // IMDB namespace

#endif