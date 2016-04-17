#include "imdb_importer.h"
#include "imdb_scorer.h"
#include "topk_nra.h"
#include "budget_fair.h"
#include "budget_ranking.h"
#include "histogram.h"
using namespace std;

void GetUserInput(const IMDB::dataset& data, const IMDB::Scorer& scorer);

int main() {
	size_t K = 10;
	IMDB::dataset  data;
	IMDB::Importer importer;
	IMDB::Scorer   scorer(data);
	topk::scoreset by_tags, by_genres, by_titles, by_actors;

	// import the data
	importer.Import(data);

	// get query from user
	GetUserInput(data, scorer);

	//// test case query
	//scorer.ScoreTitles("300",      by_titles);
	//scorer.ScoreActors("Butler, Gerard", by_actors);
	//scorer.ScoreGenres("Action,Fantasy,War", by_genres); 
	//scorer.ScoreTags("spartan,greek,historical-fiction", by_tags);  
	
	//cout << "\nNRA:\n";
	//topk::scoreset top = topk::NRA::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors });
	//for (auto movie : top) {
	//	cout << "\t" << data.movies[movie.first] << endl;
	//}
	//cout << endl;

	//cout << "\nFair(500):\n";
	//topk::scoreset topb = budget::Fair::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 500);
	//for (auto movie : topb) {
	//	cout << "\t" << data.movies[movie.first] << endl;
	//}
	//cout << endl;

	//cout << "\nRanking(500):\n";
	//topk::scoreset topr = budget::Ranking::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 500);
	//for (auto movie : topr) {
	//	cout << "\t" << data.movies[movie.first] << endl;
	//}
	//cout << endl;

	system("pause");
	return 0;
}

void GetUserInput(const IMDB::dataset& data, const IMDB::Scorer& scorer) {
	size_t K = 10;

	cout << "\nEnter value for K: ";
	cin >> K;
	while (K != 0) {
		size_t  k = 0;
		clock_t begin, end;
		double  time_spent;
		string actor, genres, tags, title;
		topk::scoreset by_tags, by_genres, by_titles, by_actors;

		// get user input
		cin.ignore();
		cout << "Enter title: ";
		getline(cin, title);

		cout << "Enter actor name: ";
		getline(cin, actor);

		cout << "Enter genres (separated by commas): ";
		getline(cin, genres);

		cout << "Enter tags (separated by commas): ";
		getline(cin, tags);

		// calculate score lists
		cout << " calculating scores... " << flush;
		begin = clock();
		scorer.ScoreTitles(title, by_titles);  // "300"
		scorer.ScoreActors(actor, by_actors);  // "Butler, Gerard"
		scorer.ScoreGenres(genres, by_genres); // "Action,Fantasy,War"
		scorer.ScoreTags(tags, by_tags);       // "spartan,greek,historical-fiction"
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;

		// calculate top-k using NRA
		cout << " NRA: calculating top-" << K << "... " << flush;
		begin = clock();
		topk::scoreset top = topk::NRA::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors });
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : top) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using NRA(500);
		cout << " NRA(500): calculating top-" << K << "... " << flush;
		begin = clock();
		top = topk::NRA::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 500);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : top) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using NRA(1000);
		cout << " NRA(1000): calculating top-" << K << "... " << flush;
		begin = clock();
		top = topk::NRA::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 1000);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : top) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using NRA(2000);
		cout << " NRA(2000): calculating top-" << K << "... " << flush;
		begin = clock();
		top = topk::NRA::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 2000);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : top) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using Fair(500);
		cout << " Fair(500): calculating top-" << K << "... " << flush;
		begin = clock();
		topk::scoreset topf = budget::Fair::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 500);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : topf) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using Fair(1000);
		cout << " Fair(1000): calculating top-" << K << "... " << flush;
		begin = clock();
		topf  = budget::Fair::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 1000);
		end   = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : topf) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using Fair(2000);
		cout << " Fair(2000): calculating top-" << K << "... " << flush;
		begin = clock();
		topf  = budget::Fair::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 2000);
		end   = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : topf) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using Ranking(500);
		cout << " Ranking(500): calculating top-" << K << "... " << flush;
		begin = clock();
		topk::scoreset topr = budget::Ranking::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 500);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : topr) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using Ranking(1000);
		cout << " Ranking(1000): calculating top-" << K << "... " << flush;
		begin = clock();
		topr = budget::Ranking::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 1000);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : topr) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		// calculate top-k using Ranking(2000);
		cout << " Ranking(2000): calculating top-" << K << "... " << flush;
		begin = clock();
		topr = budget::Ranking::TopK(K, { &by_tags, &by_genres, &by_titles, &by_actors }, 2000);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		cout << "finished after " << time_spent << "s!" << endl;
		for (auto movie : topr) {
			cout << "\t" << data.movies[movie.first] << endl;
		}
		cout << endl;

		cout << "Enter value for K: ";
		cin >> K;
	}
}