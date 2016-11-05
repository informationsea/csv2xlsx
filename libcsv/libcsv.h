/*
* CSV2Xlsx : CSV to Xslx converter
* Copyright(C) 2016 OKAMURA, Yasunobu
*
* This program is free software : you can redistribute it and / or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

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