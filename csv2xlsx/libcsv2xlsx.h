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
#include <xlsxwriter.h>
#include <libcsv.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

	struct _csv2xlsx_config
	{
		char delimiter;
		char quote;
		const char *sheet_name;
		bool autofilter;
		bool auto_convert_digit;
		bool auto_convert_number;
		bool auto_convert_scientific_number;
		bool auto_convert_percent;
		bool auto_convert_boolean;
		bool auto_convert_formula;
		bool auto_convert_url;
	};

	typedef struct _csv2xlsx_config csv2xlsx_config;

	csv2xlsx_config csv2xlsx_default();
	bool csv2xlsx(FILE *csv, const char *output);
	bool csv2xlsx_with_config(FILE *csv, const char *output, const csv2xlsx_config *config);

	struct _csv2xlsx_format_set
	{
		lxw_workbook *workbook;
		lxw_format *default_format;
		lxw_format *number_format;
		lxw_format *digit_format;
		lxw_format *percent_format;
		lxw_format *text_format;
		lxw_format *url_format;
	};
	typedef struct _csv2xlsx_format_set csv2xlsx_format_set;
	csv2xlsx_format_set csv2xlsx_create_format_set(lxw_workbook *workbook);
	lxw_error convert_csv_to_sheet(lxw_worksheet *sheet, struct csv_reader *csv_reader, const csv2xlsx_format_set *format_set, const csv2xlsx_config *config);

	lxw_error
	write_cell(lxw_worksheet *worksheet, lxw_row_t row, lxw_col_t col, const char *value, const csv2xlsx_format_set *format_set, const csv2xlsx_config *config);

	enum _converted_value_type
	{
		CSV2XLSX_BLANK,
		CSV2XLSX_DIGIT,
		CSV2XLSX_NUMBER,
		CSV2XLSX_SCIENTIFIC_NUMBER,
		CSV2XLSX_PERCENT,
		CSV2XLSX_BOOLEAN,
		CSV2XLSX_URL,
		CSV2XLSX_FORMULA,
		CSV2XLSX_TEXT,
	};
	typedef enum _converted_value_type converted_value_type;

	struct _converted_value
	{
		converted_value_type type;
		const char *text;
		int digit;
		double number;
		bool boolean;
	};
	typedef struct _converted_value converted_value;

	converted_value convert_value(const char *text, const csv2xlsx_config *config);

	bool is_digit(const char *text);
	bool is_number(const char *text);
	bool is_scientific_number(const char *text);
	bool is_percent(const char *text);
	bool is_formula(const char *text);
	bool is_boolean(const char *text);
	bool is_url(const char *text);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCSV2XLSX_H */
