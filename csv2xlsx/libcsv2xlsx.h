#ifndef LIBCSV2XLSX_H
#define LIBCSV2XLSX_H

#include <stdio.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool csv2xlsx(FILE *csv, const char *output);
bool csv2xlsx_with_delimiter_and_quote(FILE *csv, const char *output, char delimiter, char quote);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCSV2XLSX_H */
