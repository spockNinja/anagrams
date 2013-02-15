// This program will take the words found in /460/words
// and sort them into appropriate word lists

#include <stdio.h>
#include <stlib.h>
#include <fcnt1.h>

SRC_LIST = "/460/words";

// Usually runs with no args, but can take an optional word list location
int main (int argc, char* argv[]) {

	if (argc == 2) {
		SRC_LIST = argv[1];
	}

	// Open the file for reading
	int word_file = open(SRC_LIST, O_RDONLY);

	if (word_file < 0) {
		fprintf(stderr, "Unable to open %s.", SRC_LIST);
		exit(EXIT_FAILURE);
	}

}
