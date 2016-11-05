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