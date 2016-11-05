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
