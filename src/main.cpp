#include <iostream>
#include "imdb_importer.h"
#include "imdb_scorer.h"

using namespace std;

int main() {
	IMDB::dataset  data;
	IMDB::Importer importer;
	IMDB::Scorer   scorer(data);
	topk::scoreset by_tags, by_genres, by_titles, by_actors;

	// import the data
	importer.Import(data);

	// add queries manually here

	system("pause");
	return 0;
}