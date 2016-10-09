#ifndef LIBCSV2XLSX_H
#define LIBCSV2XLSX_H

#include <stdio.h>
#include <stdbool.h>

bool csv2xlsx_with_delimiter_and_quote(FILE *csv, const char *output, char delimiter, char quote);

#endif /* LIBCSV2XLSX_H */
