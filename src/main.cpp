#include <iostream>
#include <fstream>
#include <vector>
#include "imdb_importer.h"
#include "pcre.h"

using namespace std;

int main() {
	IMDB_Importer importer;
	importer.Import("../data/");
	//importer.GetMovies();

	system("pause");
	return 0;
}