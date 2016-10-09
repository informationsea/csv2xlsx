#include "libcsv2xlsx.h"

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "%s INPUT.csv OUTPUT.xlsx", argv[0]);
		return 1;
	}

	FILE *file = fopen(argv[1], "r");
	if (file == NULL) {
		perror("cannot open csv file");
		return 1;
	}

	csv2xlsx_with_delimiter_and_quote(file, argv[2], ',', '"');

	fclose(file);
}