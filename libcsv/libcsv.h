#ifndef LIBCSV_H
#define LIBCSV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct csv_reader {
     FILE *file;
     char delimiter;
     char quote_char;
	 char *buf;
	 size_t current_buffer_size;
	 bool last_charator_is_return;
};

struct csv_reader* csv_reader_initialize(FILE *file, char delimiter, char quote_char);
const char *csv_read_next(struct csv_reader* reader, bool *lineend); 
void csv_reader_free(struct csv_reader *reader);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCSV_H */