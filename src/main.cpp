#include <iostream>
#include <fstream>
#include <vector>
#include "imdb_importer.h"
#include "pcre.h"

using namespace std;

int main() {
	IMDB_Importer importer;
	importer.Import("../data/");
	const std::set<Movie>& movies = importer.GetMovies();
	std::set<std::string> genres;

	for (auto movie : movies) {
		for (auto genre : movie.GetTags()) {
			genres.insert(genre);
		}
	}

	cout << to_string(genres.size()) << endl;
	system("pause");
	return 0;
}