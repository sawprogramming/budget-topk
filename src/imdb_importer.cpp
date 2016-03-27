#include "imdb_importer.h"

bool IMDB_Importer::Import(const std::string directory) {
	clock_t begin, end;
	double  time_spent;
	bool    m = false, g = false, t = false, a = false;

	log_file_.open("../logs/imdb_error.log");

	// import
	std::cout << "Importing IMDB database..." << std::endl;
	begin = clock();

	// prepare the graphs
	m = ImportMovies(directory);
	movie_to_actor_.SetSize(movies_.size());
	movie_to_genre_.SetSize(movies_.size());
	movie_to_tag_.SetSize(movies_.size());

	// load the graphs
	concurrency::parallel_invoke(
		[&] () { a = ImportActors(directory); },
		[&] () { g = ImportGenres(directory); },
		[&] () { t = ImportTags(directory);   }
	);

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	std::cout << "... finished after " << time_spent << "s!" << std::endl;
	log_file_.clear();
	log_file_.close();

	return m && g && t && a;
}

bool IMDB_Importer::ImportActors(const std::string directory) {
	__int16      year;
	size_t      lineEnd, line_length;
	const char* error;
	int         rc, erroffset, progress = 0, ovector[15];
	std::string file_name = directory + "actors.list";

	// memory map the file
	HANDLE  hFile         = CreateFile(file_name.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE  hMMFile       = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, "imdb_actors");
	LPCTSTR file_contents = (LPTSTR)MapViewOfFile(hMMFile, FILE_MAP_READ, 0, 0, 0);
	size_t  total_length  = strlen(file_contents);

	// compile the regexes
	const char* name_pattern   = "(.*?)\\t+(?:.*)";
	const char* year_pattern   = "\\t+(.*?)\\s\\((\\d{4})(?:\\/\\w+)*\\)(?:.*)";
	const char* noyear_pattern = "\\t+(.*?)\\s\\((?:\\?{4})(?:\\/\\w+)*\\)(?:.*)";
	pcre*       name_regex     = pcre_compile(name_pattern,   0, &error, &erroffset, 0);
	pcre*       year_regex     = pcre_compile(year_pattern,   0, &error, &erroffset, 0);
	pcre*       noyear_regex   = pcre_compile(noyear_pattern, 0, &error, &erroffset, 0);

	// extract information from file
	for (size_t lineStart = 0; lineStart < total_length; lineStart = lineEnd + 1) {
		for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
			if (file_contents[lineEnd] == '\n') break;
		}
		if (file_contents[lineStart] == '\n') continue;
		// allocate enough memory then copy the line
		line_length       = lineEnd - lineStart;
		char* line        = new char[line_length + 1];
		line[line_length] = '\0';
		memcpy(line, file_contents + lineStart, line_length);

		// get offsets of actor name
		if   (pcre_exec(name_regex, NULL, line, (int)line_length, 0, 0, ovector, 15) == 2);
		else {
			log_file_ << "Actors: failed to match:" << std::endl
				      << "\t" <<               line << std::endl;
			continue;
		}

		// extract actor
		size_t actor_length = ovector[3] - ovector[2];
		char*  actor        = new char[actor_length + 1];
		actor[actor_length] = '\0';
		memcpy(actor, line + ovector[2], actor_length);
		delete[] line;

		// find movies they starred in
		for (lineStart = lineStart + ovector[3]; lineStart < total_length; lineStart = lineEnd + 1) {
			if (file_contents[lineStart] == '\n') break;
			for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
				if (file_contents[lineEnd] == '\n') break;
			}

			// allocate enough memory then copy the line
			line_length       = lineEnd - lineStart;
			line              = new char[line_length + 1];
			line[line_length] = '\0';
			memcpy(line, file_contents + lineStart, line_length);

			if      ((rc = pcre_exec(year_regex, NULL, line, (int)line_length, 0, 0, ovector, 15))   == 3);
			else if ((rc = pcre_exec(noyear_regex, NULL, line, (int)line_length, 0, 0, ovector, 15)) == 2);
			else 	{
				log_file_ << "Actors: failed to match:" << std::endl
						  << "\t" <<               line << std::endl;
				continue;
			}

			// skip any TV shows
			if (line[ovector[2]] == '"') {
				delete [] line;
				continue;
			}

			// extract title
			size_t title_length = ovector[3] - ovector[2];
			char*  title        = new char[title_length + 1];
			title[title_length] = '\0';
			memcpy(title, line + ovector[2], title_length);
			
			// extract year
			if (rc == 3) {
				char* syear = new char[5];
				syear[4]    = '\0';
				memcpy(syear, line + ovector[4], 4);
				year        = (__int16)atoi(syear);
				delete [] syear;
			} else year = 0;


			// add the actor and relation if it hasn't been already
			if (movie_to_actor_.Insert(actor, actors_.size())) {
				actors_.push_back(actor);
			}
			if (!movie_to_actor_.AddEdge(Movie(title, year), actor)) {
				log_file_ << "Actors: missing movie '" << Movie(title, year).GetTitle() << "'" << std::endl;
			}

			delete [] title;
			delete [] line;
		}
	}

	UnmapViewOfFile(file_contents);
	CloseHandle(hMMFile);
	return true;
}

bool IMDB_Importer::ImportGenres(const std::string directory) {
	__int16      year;
	size_t      lineEnd, line_length;
	const char* error;
	int         rc, erroffset, progress = 0, ovector[15];
	std::string file_name = directory + "genres.list";

	// memory map the file
	HANDLE  hFile         = CreateFile(file_name.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE  hMMFile       = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, "imdb_genres");
	LPCTSTR file_contents = (LPTSTR)MapViewOfFile(hMMFile, FILE_MAP_READ, 0, 0, 0);
	size_t  total_length  = strlen(file_contents);

	// compile the regexes
	const char* year_pattern   = "(.*?)\\s\\((\\d{4})(?:\\/\\w+)*\\)(?:.*?)(?:\\t+)(\\w+)";
	const char* noyear_pattern = "(.*?)\\s\\((?:\\?{4})(?:\\/\\w+)*\\)(?:.*?)(?:\\t+)(\\w+)";
	pcre*       year_regex     = pcre_compile(year_pattern,   0, &error, &erroffset, 0);
	pcre*       noyear_regex   = pcre_compile(noyear_pattern, 0, &error, &erroffset, 0);

	// extract information from file
	for (size_t lineStart = 0; lineStart < total_length; lineStart = lineEnd + 1) {
		for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
			if (file_contents[lineEnd] == '\n') break;
		}

		// just skip any TV shows
		if (file_contents[lineStart] == '"') continue;

		// allocate enough memory then copy the line
		line_length       = lineEnd - lineStart;
		char* line        = new char[line_length + 1];
		line[line_length] = '\0';
		memcpy(line, file_contents + lineStart, line_length);

		// get offsets of matching data
		if      ((rc = pcre_exec(year_regex, NULL, line, (int)line_length, 0, 0, ovector, 15))   == 4);
		else if ((rc = pcre_exec(noyear_regex, NULL, line, (int)line_length, 0, 0, ovector, 15)) == 3);
		else 	{
			log_file_ << "Genres: failed to match:" << std::endl
				      << "\t" <<             line << std::endl;
			continue;
		}

		// extract title
		size_t title_length = ovector[3] - ovector[2];
		char*  title        = new char[title_length + 1];
		title[title_length] = '\0';
		memcpy(title, line + ovector[2], title_length);
			
		// extract year
		if (rc == 4) {
			char* syear = new char[5];
			syear[4]    = '\0';
			memcpy(syear, line + ovector[4], 4);
			year        = (__int16)atoi(syear);
			delete [] syear;
		} else year = 0;

		// extract genre
		size_t genre_length = rc == 4 ? ovector[7] - ovector[6] : ovector[5] - ovector[4];
		char*  genre        = new char[genre_length + 1];
		genre[genre_length] = '\0';
		memcpy(genre, line + (rc == 4 ? ovector[6] : ovector[4]), genre_length);

		// add the genre and relation if it hasn't been already
		if (movie_to_genre_.Insert(genre, genres_.size())) {
			genres_.push_back(genre);
		}
		if (!movie_to_genre_.AddEdge(Movie(title, year), genre)) {
			log_file_ << "Genres: missing movie '" << Movie(title, year).GetTitle() << "'" << std::endl;
		}

		delete [] title;
		delete [] line;
	}

	UnmapViewOfFile(file_contents);
	CloseHandle(hMMFile);
	return true;
}

bool IMDB_Importer::ImportMovies(const std::string directory) {
	__int16     year;
	size_t      lineEnd, line_length;
	const char* error;
	int         rc, erroffset, progress = 0, ovector[15];
	std::string file_name = directory + "movies.list";
	
	// memory map the file
	HANDLE  hFile         = CreateFile(file_name.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE  hMMFile       = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, "imdb_movies");
	LPCTSTR file_contents = (LPTSTR)MapViewOfFile(hMMFile, FILE_MAP_READ, 0, 0, 0);
	size_t  total_length  = strlen(file_contents);

	// compile the regexes
	const char* year_pattern   = "(.*?)\\s\\((\\d{4})(?:\\/\\w+)*\\)(?:.*)";
	const char* noyear_pattern = "(.*?)\\s\\((?:\\?{4})(?:\\/\\w+)*\\)(?:.*?)(?:.*)";
	pcre*       year_regex     = pcre_compile(year_pattern,   0, &error, &erroffset, 0);
	pcre*       noyear_regex   = pcre_compile(noyear_pattern, 0, &error, &erroffset, 0);

	// extract information from file
	for (size_t lineStart = 0; lineStart < total_length; lineStart = lineEnd + 1) {
		for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
			if (file_contents[lineEnd] == '\n') break;
		}

		// just skip any TV shows
		if (file_contents[lineStart] == '"') continue;

		// allocate enough memory then copy the line
		line_length       = lineEnd - lineStart;
		char* line        = new char[line_length + 1];
		line[line_length] = '\0';
		memcpy(line, file_contents + lineStart, line_length);

		// get offsets of matching data
		if      ((rc = pcre_exec(year_regex, NULL, line, (int)line_length, 0, 0, ovector, 15))   == 3);
		else if ((rc = pcre_exec(noyear_regex, NULL, line, (int)line_length, 0, 0, ovector, 15)) == 2);
		else 	{
			log_file_ << "Movies: failed to match:" << std::endl
				      << "\t" <<               line << std::endl;
			continue;
		}

		// extract title
		size_t title_length = ovector[3] - ovector[2];
		char*  title        = new char[title_length + 1];
		title[title_length] = '\0';
		memcpy(title, line + ovector[2], title_length);
			
		// extract year
		if (rc == 3) {
			char* syear = new char[5];
			syear[4]    = '\0';
			memcpy(syear, line + ovector[4], 4);
			year        = (__int16)atoi(syear);
			delete [] syear;
		} else year = 0;

		// add the movie to our set
		Movie m(title, year);
		size_t pos = movies_.size();
		if (movie_to_actor_.Insert(m, pos)) {
			movie_to_genre_.Insert(m, pos);
			movie_to_tag_.Insert(m, pos);
			movies_.push_back(m);
		}

		delete [] title;
		delete [] line;
	}

	UnmapViewOfFile(file_contents);
	CloseHandle(hMMFile);
	return true;
}

bool IMDB_Importer::ImportTags(const std::string directory) {
	__int16      year;
	size_t      lineEnd, line_length;
	const char* error;
	int         rc, erroffset, progress = 0, ovector[15];
	std::string file_name = directory + "keywords.list";

	// memory map the file
	HANDLE  hFile         = CreateFile(file_name.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE  hMMFile       = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, "imdb_keywords");
	LPCTSTR file_contents = (LPTSTR)MapViewOfFile(hMMFile, FILE_MAP_READ, 0, 0, 0);
	size_t  total_length  = strlen(file_contents);

	// compile the regexes
	const char* year_pattern   = "(.*?)\\s\\((\\d{4})(?:\\/\\w+)*\\)(?:.*?)(?:\\t+)(.*)";
	const char* noyear_pattern = "(.*?)\\s\\((?:\\?{4})(?:\\/\\w+)*\\)(?:.*?)(?:\\t+)(.*)";
	pcre*       year_regex     = pcre_compile(year_pattern,   0, &error, &erroffset, 0);
	pcre*       noyear_regex   = pcre_compile(noyear_pattern, 0, &error, &erroffset, 0);

	// extract information from file
	for (size_t lineStart = 0; lineStart < total_length; lineStart = lineEnd + 1) {
		for (lineEnd = lineStart; lineEnd < total_length; ++lineEnd) {
			if (file_contents[lineEnd] == '\n') break;
		}

		// just skip any TV shows
		if (file_contents[lineStart] == '"') continue;

		// allocate enough memory then copy the line
		line_length       = lineEnd - lineStart;
		char* line        = new char[line_length + 1];
		line[line_length] = '\0';
		memcpy(line, file_contents + lineStart, line_length);

		// get offsets of matching data
		if      ((rc = pcre_exec(year_regex, NULL, line, (int)line_length, 0, 0, ovector, 15))   == 4);
		else if ((rc = pcre_exec(noyear_regex, NULL, line, (int)line_length, 0, 0, ovector, 15)) == 3);
		else 	{
			log_file_ << "Tags: failed to match:" << std::endl
				      << "\t" <<             line << std::endl;
			continue;
		}

		// extract title
		size_t title_length = ovector[3] - ovector[2];
		char*  title        = new char[title_length + 1];
		title[title_length] = '\0';
		memcpy(title, line + ovector[2], title_length);
			
		// extract year
		if (rc == 4) {
			char* syear = new char[5];
			syear[4]    = '\0';
			memcpy(syear, line + ovector[4], 4);
			year        = (__int16)atoi(syear);
			delete [] syear;
		} else year = 0;

		// extract tag
		size_t tag_length = rc == 4 ? ovector[7] - ovector[6] : ovector[5] - ovector[4];
		char*  tag        = new char[tag_length + 1];
		tag[tag_length]   = '\0';
		memcpy(tag, line + (rc == 4 ? ovector[6] : ovector[4]), tag_length);

		// add the tag and relation if it hasn't been already
		if (movie_to_tag_.Insert(tag, tags_.size())) {
			tags_.push_back(tag);
		}
		if (!movie_to_tag_.AddEdge(Movie(title, year), tag)) {
			log_file_ << "Tags: missing movie '" << Movie(title, year).GetTitle() << "'" << std::endl;
		}

		delete [] title;
		delete [] line;
	}

	UnmapViewOfFile(file_contents);
	CloseHandle(hMMFile);
	return true;
}

const std::vector<Movie>& IMDB_Importer::GetMovies() const { return movies_; };