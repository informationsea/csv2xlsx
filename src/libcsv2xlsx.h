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
#include <float.h>
#include <xlsxwriter.h>
#include <libcsv.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_COLUMNS 1000
#define MAX_FORMATS 200
#define MAX_COLUMN_DEFINITIONS 100
#define CSV2XLSX_COLUMN_UNDEFINED -1
#define CSV2XLSX_COLOR_UNDEFINED 0xFFFFFFFF
#define CSV2XLSX_COLOR_UNSET LXW_COLOR_UNSET
#define CSV2XLSX_COLOR_LINK LXW_COLOR_BLUE
#define CSV2XLSX_WIDTH_UNDEFINED (-1.0)
#define CSV2XLSX_WIDTH_AUTO (-2.0)
#define CSV2XLSX_MAX_WIDTH_DEFAULT 30.0

    //     PREFIXED CONSTANTS?
//    #define WIDTH_DEFAULT LXW_DEF_COL_WIDTH
//    #define WIDTH_UNSET -1.0
//    #define WIDTH_AUTO -2.0
//    #define MAX_WIDTH_UNSET -1.0
//    #define MAX_WIDTH_DEFAULT 30.0

typedef lxw_color_t csv2xlsx_color;

typedef enum _csv2xlsx_align {
    CSV2XLSX_ALIGN_UNDEFINED = -1,
    CSV2XLSX_ALIGN_NONE = LXW_ALIGN_NONE,
    CSV2XLSX_ALIGN_LEFT = LXW_ALIGN_LEFT,
    CSV2XLSX_ALIGN_CENTER = LXW_ALIGN_CENTER,
    CSV2XLSX_ALIGN_RIGHT = LXW_ALIGN_RIGHT,
    CSV2XLSX_ALIGN_JUSTIFY = LXW_ALIGN_JUSTIFY,
} csv2xlsx_align;

typedef enum _csv2xlsx_underline {
    CSV2XLSX_UNDERLINE_UNDEFINED = -1,
    CSV2XLSX_UNDERLINE_NONE = LXW_UNDERLINE_NONE,
    CSV2XLSX_UNDERLINE_SINGLE = LXW_UNDERLINE_SINGLE,
    CSV2XLSX_UNDERLINE_DOUBLE = LXW_UNDERLINE_DOUBLE,
    CSV2XLSX_UNDERLINE_SINGLE_ACCOUNTING = LXW_UNDERLINE_SINGLE_ACCOUNTING,
    CSV2XLSX_UNDERLINE_DOUBLE_ACCOUNTING = LXW_UNDERLINE_DOUBLE_ACCOUNTING
} csv2xlsx_underline;

typedef enum _csv2xlsx_cell_type {
    CSV2XLSX_CELL_TYPE_UNDEFINED = -1,
    CSV2XLSX_CELL_TYPE_BLANK,
    CSV2XLSX_CELL_TYPE_DIGIT,
    CSV2XLSX_CELL_TYPE_NUMBER,
    CSV2XLSX_CELL_TYPE_PERCENT,
    CSV2XLSX_CELL_TYPE_BOOL,
    CSV2XLSX_CELL_TYPE_URL,
    CSV2XLSX_CELL_TYPE_FORMULA,
    CSV2XLSX_CELL_TYPE_TEXT,
} csv2xlsx_cell_type;

typedef struct _csv2xlsx_format {
    char *number_format;
    csv2xlsx_color color;
    csv2xlsx_color background_color;
    csv2xlsx_align align;
    csv2xlsx_underline underline;
    lxw_format *_format_set_format;
} csv2xlsx_format;

csv2xlsx_format csv2xlsx_format_create();

typedef struct _csv2xlsx_column_definition {
    bool initialized; // required?
    csv2xlsx_cell_type type;
    double width;
    double max_width;
    csv2xlsx_format header_format;
    csv2xlsx_format data_format;
    csv2xlsx_format *type_formats[CSV2XLSX_CELL_TYPE_TEXT + 1];
} csv2xlsx_column_definition;

csv2xlsx_column_definition csv2xlsx_column_definition_create();

typedef struct _csv2xlsx_column_config {
    csv2xlsx_column_definition default_column_definition;
    csv2xlsx_column_definition column_definitions[MAX_COLUMN_DEFINITIONS];
//    size_t count;
} csv2xlsx_column_config;

csv2xlsx_column_config csv2xlsx_column_config_create(lxw_workbook *workbook);

typedef int auto_convert;

enum auto_convert_flags {
    CSV2XLSX_AUTO_CONVERT_NONE = 0,
    CSV2XLSX_AUTO_CONVERT_DIGIT = 1 << 0,
    CSV2XLSX_AUTO_CONVERT_NUMBER = 1 << 1,
    CSV2XLSX_AUTO_CONVERT_PERCENT = 1 << 2,
    CSV2XLSX_AUTO_CONVERT_BOOL = 1 << 3,
    CSV2XLSX_AUTO_CONVERT_FORMULA = 1 << 4,
    CSV2XLSX_AUTO_CONVERT_URL = 1 << 5,
    CSV2XLSX_AUTO_CONVERT_ALL = (auto_convert) ~ 0,
};

typedef struct _csv2xlsx_config {
    char delimiter;
    char quote;
    const char *sheet_name;
    bool table;
    unsigned int header_row_count;
    unsigned int freeze_row;
    unsigned int freeze_column;
    csv2xlsx_column_config column_config;
    bool auto_filter;
    auto_convert auto_convert;
} csv2xlsx_config;

csv2xlsx_config csv2xlsx_config_default();


//csv2xlsx_column_config csv2xlsx_column_config_create(lxw_workbook *workbook);
//csv2xlsx_column_definition* csv2xlsx_column_config_get_column(unsigned int column);

typedef struct _csv2xlsx_format_set {
    lxw_workbook *workbook;
    lxw_format *formats[MAX_FORMATS];
    size_t formats_count;
} csv2xlsx_format_set;

csv2xlsx_format_set csv2xlsx_format_set_create(lxw_workbook *workbook);

typedef struct _csv2xlsx_cell_value {
    csv2xlsx_cell_type type;
    const char *text;
    int digit;
    double number;
    bool boolean;
} csv2xlsx_cell_value;

csv2xlsx_cell_value csv2xlsx_convert_value(const char *text, csv2xlsx_cell_type type, auto_convert auto_convert);

bool csv2xlsx(FILE *csv, const char *output, csv2xlsx_config *config);
lxw_error csv2xlsx_convert_csv_to_worksheet(lxw_worksheet *worksheet, struct csv_reader *csv_reader, csv2xlsx_config *config, csv2xlsx_format_set *format_set);
lxw_error csv2xlsx_write_cell(lxw_worksheet *worksheet, lxw_row_t row, lxw_col_t column, const char *value, csv2xlsx_config *config, csv2xlsx_format_set *format_set, double *column_widths);


bool csv2xlsx_parse_cell_type(const char* text, csv2xlsx_cell_type *cell_type);

bool csv2xlsx_is_digit(const char *text);
bool csv2xlsx_is_number(const char *text);
bool csv2xlsx_is_percent(const char *text);
bool csv2xlsx_is_formula(const char *text);
bool csv2xlsx_is_bool(const char *text);
bool csv2xlsx_is_url(const char *text);

bool csv2xlsx_parse_freeze_panes(char *text, unsigned int *row, unsigned int *column);

bool csv2xlsx_parse_align(const char* text, csv2xlsx_align *align);
bool csv2xlsx_parse_color(const char* text, csv2xlsx_color *color);

bool csv2xlsx_translate_int(const char* text, int *value);
bool csv2xlsx_translate_unsigned_int(const char* text, unsigned int *value);
bool csv2xlsx_translate_double(const char* text, double *value);

bool csv2xlsx_parse_column_config(
    char** column_definition_strings,
    char** header_column_format_strings,
    int column_definition_count,
    int header_column_format_count,
    csv2xlsx_column_config *column_config
);
bool csv2xlsx_parse_default_column_definition(
    char** column_definition_strings,
    int column_definition_count,
    csv2xlsx_column_definition *default_column_definition
);
bool csv2xlsx_parse_default_header_column_format(
    char** header_column_format_strings,
    int header_column_format_count,
    csv2xlsx_column_definition *default_column_definition
);
bool csv2xlsx_parse_column_definitions(
    char** column_definition_strings,
    int column_definition_count,
    csv2xlsx_column_config *column_config
);
bool csv2xlsx_parse_header_column_formats(
    char** header_column_format_strings,
    int header_column_format_count,
    csv2xlsx_column_config *column_config
);
bool csv2xlsx_parse_column_definition(char* text, csv2xlsx_column_definition *column_definition);
bool csv2xlsx_parse_column_definition_column(const char* text, int* column);
bool csv2xlsx_parse_column_definition_format_property(char* text, csv2xlsx_format* format);
bool csv2xlsx_parse_column_definition_format(char* text, csv2xlsx_format* format);

char *csv2xlsx_string_trim_left(char *text);
size_t csv2xlsx_string_trim_right_length(const char* text);
size_t csv2xlsx_substring_trim_right_length(const char* start, const char* end);
bool csv2xlsx_string_ends_with(const char *text, const char *suffix);
bool csv2xlsx_prefix_equals(const char* text, const char* prefix, size_t prefix_length);

const char* csv2xlsx_get_last_error();
void csv2xlsx_set_last_error(const char *format, ...);
const char* csv2xlsx_get_lxw_error_message(lxw_error error);

#if defined(_WIN32) && defined(_MSC_VER)
#define csv2xlsx_string_compare_ignore_case _stricmp
#define csv2xlsx_substring_compare_ignore_case _strnicmp
#else
#define csv2xlsx_string_compare_ignore_case strcasecmp
#define csv2xlsx_substring_compare_ignore_case strncasecmp
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCSV2XLSX_H */
