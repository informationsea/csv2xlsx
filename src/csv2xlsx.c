/**
 * CSV2Xlsx : CSV to XSLX converter
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

#include <argtable3.h>
#include <xlsxwriter.h>
#include <libcsv.h>
#include "libcsv2xlsx.h"

#define STRING_LITERAL(x) #x
#define STRING(x) STRING_LITERAL(x)

#define MAX_SHEETS 50
#define MAX_SHEET_NAME_LENGTH 31

typedef char sheet_name[MAX_SHEET_NAME_LENGTH + 1];
sheet_name assigned_sheet_names[MAX_SHEETS];

bool sheet_exists(const char *sheet_name, int sheets_count) {
    for (int i = 0; i < sheets_count; i++) {
        if (strcmp(assigned_sheet_names[i], sheet_name) == 0) {
            return true;
        }
    }

    return false;
}

void generate_unique_sheet_name(const char *sheet_name_candidate, char *sheet_name, size_t sheets_count) {
    int suffix = 2;
    strncpy(sheet_name, sheet_name_candidate, MAX_SHEET_NAME_LENGTH);
    sheet_name[MAX_SHEET_NAME_LENGTH] = '\0';

    while (sheet_exists(sheet_name, sheets_count)) {
        if (strlen(sheet_name_candidate) + csv2xlsx_int_length(suffix) >= MAX_SHEET_NAME_LENGTH) {
            fprintf(stderr, "Sheet name too long, truncating.\n");
            generate_unique_sheet_name("Sheet", sheet_name, sheets_count);

            return;
        }

        snprintf(sheet_name, MAX_SHEET_NAME_LENGTH, "%s%d", sheet_name_candidate, suffix++);
    }
}

int main(int argc, char **argv) {
    struct arg_lit *help, *table, *header, *auto_filter;
    struct arg_lit *convert_digit, *convert_number, *convert_boolean, *convert_percent, *convert_formula, *convert_url;
    struct arg_file *files;
    struct arg_str *csv_delimiter, *freeze_panes, *sheet_names, *columns, *header_column_formats, *temp_directory;
    struct arg_int *header_rows;
    struct arg_end *end;
	void *argtable[] = {
        files = arg_filen(NULL, NULL, NULL, 2, MAX_SHEETS, "Input TSV or CSV files"),
        arg_rem("<file>", "Output XLSX file"),
		help = arg_lit0("h", "help", "Display this help and exit"),
		csv_delimiter = arg_str0("D", "delimiter", NULL, "Configure delimiter used in the CSV file, defaults to ','"),
        table = arg_lit0("t", "table", "Enable table (do not enable this option to reduce memory usage)"),
        header = arg_lit0("H", "header", "Enable header (a shorthand to passing --header-rows=1)"),
        header_rows = arg_int0("r", "header-rows", NULL, "Enable header and set number of header rows."),
		auto_filter = arg_lit0("a", "auto-filter", "Enable AutoFilter"),
        freeze_panes = arg_str0("P", "freeze-panes", NULL, "Split and freeze a worksheet into panes in format 'row' or 'row,column'"),
        sheet_names = arg_strn("s", "sheet-name", NULL, 0, MAX_SHEETS, "Configure excel sheet names"),
        columns = arg_strn("c", "column", NULL, 0, 200, "Configure columns in format like '0=type: string' or '1=type: number; number-format: #,##0.00; color: #FF0000', definitions without column number are treated as defaults"),
        header_column_formats = arg_strn("F", "header-column", NULL, 0, 200, "Configure header column format in format like '0=background-color: #33333333; color: #FFFFFF', definitions without column number are treated as defaults"),
		convert_digit = arg_lit0("d", "convert-digit", "Enable automatic conversion to integer"),
		convert_number = arg_lit0("n", "convert-number", "Enable automatic convert to floating number"),
		convert_boolean = arg_lit0("b", "convert-bool", "Enable automatic convert to boolean"),
		convert_percent = arg_lit0("p", "convert-percent", "Enable automatic convert to percent"),
		convert_formula = arg_lit0("f", "convert-formula", "Enable automatic convert to formula"),
		convert_url = arg_lit0("u", "convert-url", "Enable automatic convert to url"),
        temp_directory = arg_str0("T", "temp-directory", NULL, "Configure alternative temporary files location to override system default"),
		end = arg_end(20),
	};

	// Verifies the argtable entries were allocated sucessfully.
	if (arg_nullcheck(argtable) != 0) {
		printf("%s: insufficient memory\n", argv[0]);

		return 1;
	}

    int errors_count = arg_parse(argc, argv, argtable);

    if (help->count > 0) {
		printf("csv2xlsx %s https://github.com/maryo/csv2xlsx\n\n", STRING(CSV2XLSX_VERSION));
		printf("Usage: %s", argv[0]);
        files->hdr.mincount = 1;
		arg_print_syntax(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "  %-29s %s\n");

		return 0;
	}

	if (errors_count > 0) {
		arg_print_errors(stdout, end, argv[0]);
		printf("Try '%s --help' for more information.\n", argv[0]);

		return 1;
	}

	csv2xlsx_config config = {
		.delimiter = csv_delimiter->count > 0 && *csv_delimiter->sval[0] != '\0' ? csv_delimiter->sval[0][0] : ',',
		.quote = '"',
		.sheet_name = "Sheet 1",
		.table = table->count == 1,
		.header_row_count = header_rows->count > 0 ? header_rows->ival[0] : header->count > 0,
		.auto_filter = auto_filter->count == 1,
        .auto_convert = (convert_digit->count > 0 ? CSV2XLSX_AUTO_CONVERT_DIGIT : 0)
            | (convert_number->count > 0 ? CSV2XLSX_AUTO_CONVERT_NUMBER : 0)
            | (convert_percent->count > 0 ? CSV2XLSX_AUTO_CONVERT_PERCENT : 0)
            | (convert_boolean->count > 0 ? CSV2XLSX_AUTO_CONVERT_BOOL : 0)
            | (convert_formula->count > 0 ? CSV2XLSX_AUTO_CONVERT_FORMULA : 0)
            | (convert_url->count > 0 ? CSV2XLSX_AUTO_CONVERT_URL : 0),
	};

	if (freeze_panes->count == 1) {
        if (!csv2xlsx_parse_freeze_panes((char *) freeze_panes->sval[0], &config.freeze_row, &config.freeze_column)) {
            fprintf(stderr, "%s\n", csv2xlsx_get_last_error());

            return 1;
        }
	}

	lxw_workbook *workbook = workbook_new_opt(files->filename[files->count - 1], &(lxw_workbook_options) {
        .constant_memory = config.table ? LXW_FALSE : LXW_TRUE, // TODO: FALSE produces smaller output, investigate if we should use it instead
        .tmpdir = temp_directory->count > 0 ? (char *) temp_directory->sval[0] : NULL,
    });
    config.column_config = csv2xlsx_column_config_create(workbook);
    bool success = csv2xlsx_parse_column_config(
        (char **) columns->sval,
        (char **) header_column_formats->sval,
        columns->count,
        header_column_formats->count,
        &config.column_config
    );

    if (!success) {
        fprintf(stderr, "%s\n", csv2xlsx_get_last_error());

        return 1;
    }

    csv2xlsx_format_set format_set = csv2xlsx_format_set_create(workbook);

    if (files->count - 1 > MAX_SHEETS) {
        fprintf(stderr, "Maximum number of %d sheets exceeded.", MAX_SHEETS);

        return 1;
    }

	for (size_t i = 0; i < files->count - 1; i++) {
        char sheet_name[MAX_SHEET_NAME_LENGTH + 1];

        if (sheet_names->count > i) {
            strcpy(sheet_name, sheet_names->sval[i]);
        } else {
            strncpy(sheet_name, files->basename[i], strlen(files->basename[i]) - strlen(files->extension[i]));
        }

        generate_unique_sheet_name(sheet_name, assigned_sheet_names[i], i);
		FILE *csv_file = fopen(files->filename[i], "rb");

		if (csv_file == NULL) {
			fprintf(stderr, "Cannot open %s: %s", files->filename[i], strerror(errno));

			return 1;
		}

		char delimiter = '\t';
		char quote = '\0';

		if (csv2xlsx_string_ends_with(files->filename[i], ".csv")) {
			delimiter = config.delimiter;
			quote = '"';
		}

		struct csv_reader *csv_reader = csv_reader_initialize(csv_file, delimiter, quote);

		if (csv_reader == NULL) {
			fprintf(stderr, "Cannot initialize CSV reader.\n");

			return 1;
		}

		lxw_worksheet *worksheet = workbook_add_worksheet(workbook, assigned_sheet_names[i]);
		lxw_error error = csv2xlsx_convert_csv_to_worksheet(worksheet, csv_reader, &config, &format_set);

        if (error != LXW_NO_ERROR) {
            fprintf(stderr, "libxlsxwriter: %s.\n", csv2xlsx_get_lxw_error_message(error));

            return 1;
        }
	}

	workbook_close(workbook);

	return 0;
}
