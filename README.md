# Top-k Under Budgetary Constraints
This project aims to implement the algorithms described in the paper, ["Best Effort Top-k Query Processing Under Budgetary Constraints"] .

> Existing
algorithms for top-k processing are budget-oblivious, i.e., they do
not take budget constraints into account when making scheduling
decisions, but focus on the performance to compute the final topk
results. Under budget constraints, these algorithms therefore
often return results that are a lot worse than the results that can
be achieved with a clever, budget-aware scheduling algorithm.
This paper introduces novel algorithms for budget-aware top-k
processing that produce results that have a significantly higher
quality than those of state-of-the-art budget-oblivious solutions.

### Data Collections
This program uses a subset of the data extracted from [IMDB] containing the actors, genres, keywords, and movies lists. These subsets  contain data for 1,250,000 movies and 2,000,000 actors. This program parses each file and adds the relevant information to the data structures. Any issues with lines unable to be parsed or unable to be inserted into the data structures can be found in the log file, located at logs/imdb_error.log.

Information on obtaining the data is located [here][imdb-data].

*TV shows were removed from the genres, keywords, and movies lists for this program*

### Getting Started
To run this program, simply load up the Visual Studio solution file, build, and run! The Release configuration is best unless you need the features of the debugger; it runs 519% faster and uses 63% less memory.

### Notes on System Requirements
* Windows (64 bit) (tested on 8.1)
    * This program uses Windows exclusive libraries for memory mapping files and for parallel execution of functions.
    * The program uses over 2GB of RAM, thus 64-bit is required for it to work correctly.
* Visual Studio (created using Visual Studio Premium 2013)
* Multi-core processor
* 4GB RAM

### Version
0.7.0

### License
GPL2

["Best Effort Top-k Query Processing Under Budgetary Constraints"]: <http://www.ics.uci.edu/~chenli/pub/2009-icde-topk.pdf>
[IMDB]: <http://www.imdb.com/>
[imdb-data]: <http://www.imdb.com/interfaces>