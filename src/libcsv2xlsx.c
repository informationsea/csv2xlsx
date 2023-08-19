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
#include <ctype.h>

#include "libcsv2xlsx.h"

csv2xlsx_config csv2xlsx_default()
{
	struct _csv2xlsx_config data =
		{
			.delimiter = ',',
			.quote = '"',
			.sheet_name = "Sheet 1",
			.autofilter = true,
			.auto_convert_digit = true,
			.auto_convert_number = true,
			.auto_convert_scientific_number = true,
			.auto_convert_percent = true,
			.auto_convert_boolean = true,
			.auto_convert_formula = true,
			.auto_convert_url = true,
		};
	return data;
}

bool csv2xlsx(FILE *csv, const char *output)
{
	csv2xlsx_config config = csv2xlsx_default();
	return csv2xlsx_with_config(csv, output, &config);
}

bool csv2xlsx_with_config(FILE *csv, const char *output, const csv2xlsx_config *config)
{
	struct csv_reader *csv_reader = csv_reader_initialize(csv, config->delimiter, config->quote);
	if (csv_reader == NULL)
		return false;
	lxw_workbook_options options = {.constant_memory = LXW_TRUE,
									.tmpdir = NULL};
	lxw_workbook *workbook = workbook_new_opt(output, &options);

	char sheetname[32];
	memset(sheetname, 0, sizeof(sheetname));
	strncpy(sheetname, config->sheet_name, 31);
	lxw_worksheet *sheet = workbook_add_worksheet(workbook, sheetname);
	csv2xlsx_format_set format_set = csv2xlsx_create_format_set(workbook);

	convert_csv_to_sheet(sheet, csv_reader, &format_set, config);

	workbook_close(workbook);
	return true;
}

lxw_error convert_csv_to_sheet(lxw_worksheet *sheet, struct csv_reader *csv_reader, const csv2xlsx_format_set *format_set, const csv2xlsx_config *config)
{
	int row = 0;
	int col = 0;
	int max_col = 0;

	do
	{
		bool endofline;
		const char *str = csv_read_next(csv_reader, &endofline);
		if (str == NULL)
			break;
		lxw_error e = write_cell(sheet, row, col++, str, format_set, config);
		if (e != LXW_NO_ERROR)
		{
			return e;
		}

		if (endofline)
		{
			if (col > max_col)
			{
				max_col = col;
			}
			row += 1;
			col = 0;
		}
	} while (1);

	if (config->table)
	{
		lxw_table_options options = {.no_autofilter = config->autofilter ? LXW_FALSE : LXW_TRUE};
		lxw_error e = worksheet_add_table(sheet, 0, 0, row - 1, max_col - 1, &options);
		if (e != LXW_NO_ERROR)
		{
			return e;
		}
	}
	else if (config->autofilter)
	{
		lxw_error e = worksheet_autofilter(sheet, 0, 0, row - 1, max_col - 1);
		if (e != LXW_NO_ERROR)
		{
			return e;
		}
	}

	return LXW_NO_ERROR;
}

csv2xlsx_format_set csv2xlsx_create_format_set(lxw_workbook *workbook)
{
	lxw_format *percent_format = workbook_add_format(workbook);
	format_set_num_format(percent_format, "0%");

	lxw_format *digit_format = workbook_add_format(workbook);
	format_set_num_format(digit_format, "0_ ");

	lxw_format *text_format = workbook_add_format(workbook);
	format_set_num_format(text_format, "@");

	lxw_format *url_format = workbook_add_format(workbook);
	format_set_num_format(url_format, "@");
	format_set_underline(url_format, LXW_UNDERLINE_SINGLE);
	format_set_font_color(url_format, LXW_COLOR_BLUE);

	csv2xlsx_format_set set = {
		.workbook = workbook,
		.default_format = NULL,
		.digit_format = digit_format,
		.number_format = NULL,
		.percent_format = percent_format,
		.text_format = text_format,
		.url_format = url_format,
	};
	return set;
}

lxw_error write_cell(lxw_worksheet *worksheet, lxw_row_t row, lxw_col_t col, const char *value, const csv2xlsx_format_set *format_set, const csv2xlsx_config *config)
{
	converted_value v = convert_value(value, config);

	switch (v.type)
	{
	case CSV2XLSX_BLANK:
		return worksheet_write_blank(worksheet, row, col, format_set->default_format);
	case CSV2XLSX_DIGIT:
		return worksheet_write_number(worksheet, row, col, v.digit, format_set->digit_format);
	case CSV2XLSX_NUMBER:
	case CSV2XLSX_SCIENTIFIC_NUMBER:
		return worksheet_write_number(worksheet, row, col, v.number, format_set->default_format);
	case CSV2XLSX_PERCENT:
		return worksheet_write_number(worksheet, row, col, v.number, format_set->percent_format);
	case CSV2XLSX_BOOLEAN:
		return worksheet_write_boolean(worksheet, row, col, v.boolean ? 1 : 0, format_set->default_format);
	case CSV2XLSX_FORMULA:
		return worksheet_write_formula(worksheet, row, col, v.text, format_set->default_format);
	case CSV2XLSX_URL:
		return worksheet_write_url(worksheet, row, col, v.text, format_set->url_format);
	default:
		return worksheet_write_string(worksheet, row, col, value, format_set->text_format);
	}
}

converted_value convert_value(const char *text, const csv2xlsx_config *config)
{
	size_t text_len = strlen(text);
	converted_value value = {
		.type = CSV2XLSX_TEXT,
		.text = text,
		.digit = 0,
		.number = 0.,
		.boolean = false,
	};

	if (text_len == 0)
	{
		value.type = CSV2XLSX_BLANK;
		return value;
	}

	// check boolean
	if (config->auto_convert_boolean)
	{
		if (strcmp(text, "true") == 0)
		{
			value.type = CSV2XLSX_BOOLEAN;
			value.boolean = true;
			return value;
		}
		if (strcmp(text, "TRUE") == 0)
		{
			value.type = CSV2XLSX_BOOLEAN;
			value.boolean = true;
			return value;
		}
		if (strcmp(text, "True") == 0)
		{
			value.type = CSV2XLSX_BOOLEAN;
			value.boolean = true;
			return value;
		}
		if (strcmp(text, "FALSE") == 0)
		{
			value.type = CSV2XLSX_BOOLEAN;
			value.boolean = false;
			return value;
		}
		if (strcmp(text, "false") == 0)
		{
			value.type = CSV2XLSX_BOOLEAN;
			value.boolean = false;
			return value;
		}
		if (strcmp(text, "False") == 0)
		{
			value.type = CSV2XLSX_BOOLEAN;
			value.boolean = false;
			return value;
		}
	}

	// check digit
	if (config->auto_convert_digit && is_digit(text))
	{
		char *end;
		long digit = strtol(text, &end, 10);
		if (*end == '\0')
		{
			value.type = CSV2XLSX_DIGIT;
			value.digit = digit;
			return value;
		}
	}

	// convert number
	if (config->auto_convert_number && is_number(text))
	{
        char *comma = strrchr(text, ',');
        if (comma)
        {
            *comma = '.';
        }

		char *end;
		double number = strtod(text, &end);
		if (*end == '\0')
		{
			value.type = CSV2XLSX_NUMBER;
			value.number = number;
			return value;
		}
	}

	// check percent
	if (config->auto_convert_percent && is_percent(text))
	{
		char *end;
		int number = strtol(text, &end, 10);
		if (*end == '%' && end[1] == '\0')
		{
			value.type = CSV2XLSX_PERCENT;
			value.number = number / 100.;
			return value;
		}
	}

	// formula
	if (config->auto_convert_formula && is_formula(text))
	{
		value.type = CSV2XLSX_FORMULA;
		return value;
	}

	// check URL
	if (config->auto_convert_url && is_url(text))
	{
		value.type = CSV2XLSX_URL;
		return value;
	}

	return value;
}

bool is_digit(const char *text)
{
	if (text[0] == '+' || text[0] == '-')
	{
		text++;
	}

	while (*text)
	{
		if (*text < '0' || '9' < *text)
		{
			return false;
		}
		text++;
	}
	return true;
}

bool is_number(const char *text)
{

	if (text[0] == '+' || text[0] == '-')
	{
		text++;
	}

	int comma_count = 0;

	while (*text)
	{
		if (*text == '.' || *text == ',')
		{
			comma_count += 1;
		}
		else if (*text < '0' || '9' < *text)
		{
			return false;
		}
		text++;
	}
	return comma_count <= 1;
}

bool is_scientific_number(const char *text)
{
	return false;
}

bool is_percent(const char *text)
{
	while (*text)
	{
		if (*text < '0' || '9' < *text)
		{
			break;
		}
		text++;
	}

	if (*text == '%')
	{
		text++;
		if (*text == '\0')
		{
			return true;
		}
	}

	return false;
}

bool is_formula(const char *text)
{
	return text[0] == '=';
}

bool is_url(const char *text)
{
	if (strncmp("https://", text, 8) == 0)
	{
		return true;
	}

	if (strncmp("http://", text, 7) == 0)
	{
		return true;
	}

	return false;
}