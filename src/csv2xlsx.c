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

#include "libcsv2xlsx.h"
#include <argtable3.h>
#include <libcsv.h>

struct arg_lit *help, *autofilter, *convert_digit, *convert_number, *convert_boolean, *convert_percent, *convert_formula, *convert_url, *disable_table;
struct arg_file *output, *input;
struct arg_str *sheet_names;
struct arg_end *end;

#define STR(x) #x
#define STR2(x) STR(x)

int main(int argc, char **argv)
{
	/* the global arg_xxx structs are initialised within the argtable */
	void *argtable[] = {
		help = arg_lit0("h", "help", "display this help and exit"),
		output = arg_file1("o", "output", "<EXCEL>", "Output xlsx file"),
		disable_table = arg_lit0("t", "disable-table", "Disable table (Please set this option to reduce memory usage)"),
		autofilter = arg_lit0("a", "disable-autofilter", "Disable autofilter"),
		convert_digit = arg_lit0("d", "disable-convert-digit", "Disable auto convert to integer"),
		convert_number = arg_lit0("n", "disable-convert-number", "Disable auto convert to floating number"),
		convert_boolean = arg_lit0("b", "disable-convert-bool", "Disable auto convert to boolean"),
		convert_percent = arg_lit0("p", "disable-convert-percent", "Disable auto convert to percent"),
		convert_formula = arg_lit0("f", "disable-convert-formula", "Disable auto convert to formula"),
		convert_url = arg_lit0("u", "disable-convert-url", "Disable auto convert to url"),
		sheet_names = arg_strn("s", "sheetname", "<NAME>", 0, 100, "Excel sheet names"),
		input = arg_filen(NULL, NULL, "<CSV,TSV>", 1, 100, "Input TSV, CSV files"),
		end = arg_end(20),
	};

	/* verify the argtable[] entries were allocated sucessfully */
	if (arg_nullcheck(argtable) != 0)
	{
		/* NULL entries were detected, some allocations must have failed */
		printf("%s: insufficient memory\n", argv[0]);
		return 1;
	}

	int nerrors = arg_parse(argc, argv, argtable);

	if (help->count > 0)
	{
		printf("csv2xlsx " STR2(CSV2XLSX_VERSION) " https://github.com/informationsea/csv2xlsx\n\n");
		printf("Usage: %s", argv[0]);
		arg_print_syntax(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "  %-25s %s\n");
		return 0;
	}

	/* If the parser returned any errors then display them and exit */
	if (nerrors > 0)
	{
		/* Display the error details contained in the arg_end struct.*/
		arg_print_errors(stdout, end, argv[0]);
		printf("Try '%s --help' for more information.\n", argv[0]);
		return 1;
	}

	csv2xlsx_config xlsx_config = {
		.delimiter = ',',
		.quote = '"',
		.sheet_name = "Sheet 1",
		.table = disable_table->count == 0,
		.autofilter = autofilter->count == 0,
		.auto_convert_digit = convert_digit->count == 0,
		.auto_convert_number = convert_number->count == 0,
		.auto_convert_scientific_number = false,
		.auto_convert_percent = convert_percent->count == 0,
		.auto_convert_boolean = convert_boolean->count == 0,
		.auto_convert_formula = convert_formula->count == 0,
		.auto_convert_url = convert_url->count == 0,
	};

	lxw_workbook_options options = {.constant_memory = xlsx_config.table ? LXW_FALSE : LXW_TRUE,
									.tmpdir = NULL};
	lxw_workbook *workbook = workbook_new_opt(output->filename[0], &options);
	csv2xlsx_format_set format_set = csv2xlsx_create_format_set(workbook);

	for (size_t i = 0; i < input->count; i++)
	{
		char sheetname[32];
		if (i < sheet_names->count)
		{
			snprintf(sheetname, sizeof(sheetname), "%s", sheet_names->sval[i]);
		}
		else
		{
			snprintf(sheetname, sizeof(sheetname), "%s", input->basename[i]);
		}
		FILE *csv_file = fopen(input->filename[i], "rb");
		if (csv_file == NULL)
		{
			fprintf(stderr, "Cannot open %s: ", input->filename[i]);
			perror("Cannot open file");
			return 1;
		}
		size_t filename_len = strlen(input->filename[i]);

		char delimiter = '\t';
		char quote = '\0';
		if (strcmp(input->filename[i] + filename_len - 4, ".csv") == 0)
		{
			delimiter = ',';
			quote = '"';
		}

		struct csv_reader *csv_reader = csv_reader_initialize(csv_file, delimiter, quote);
		if (csv_reader == NULL)
		{
			fprintf(stderr, "Cannot initialize CSV reader\n");
			return 1;
		}

		lxw_worksheet *sheet = workbook_add_worksheet(workbook, sheetname);
		convert_csv_to_sheet(sheet, csv_reader, &format_set, &xlsx_config);
	}

	workbook_close(workbook);

	return 0;
}