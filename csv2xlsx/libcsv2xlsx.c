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

#include <xlsxwriter.h>
#include <libcsv.h>

#include "libcsv2xlsx.h"

bool csv2xlsx(FILE *csv, const char *output)
{
	return csv2xlsx_with_delimiter_and_quote(csv, output, ',', '"');
}

bool csv2xlsx_with_delimiter_and_quote(FILE *csv, const char *output, char delimiter, char quote)
{
	struct csv_reader *csv_reader = csv_reader_initialize(csv, delimiter, quote);
	if (csv_reader == NULL) return false;
	lxw_workbook *workbook = workbook_new(output);
	lxw_worksheet *sheet = workbook_add_worksheet(workbook, "Sheet 1");

	int row = 0;
	int col = 0;

	do {
		bool endofline;
		const char *str = csv_read_next(csv_reader, &endofline);
		if (str == NULL) break;
		worksheet_write_string(sheet, row, col++, str, NULL);
		if (endofline) {
			row += 1;
			col = 0;
		}
	} while (1);

	workbook_close(workbook);
	return true;
}
