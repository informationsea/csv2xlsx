/*
 * CSV2Xlsx : CSV to XLSX converter
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

#include <libcsv.h>
#include <xlsxwriter.h>
#include <math.h>
#include <stdarg.h>

#include "libcsv2xlsx.h"

bool csv2xlsx(FILE *csv, const char *output, csv2xlsx_config *config) {
    struct csv_reader *csv_reader = csv_reader_initialize(csv, config->delimiter, config->quote);

    if (csv_reader == NULL) {
        return false;
    }

    lxw_workbook_options options = {
        .constant_memory = true, // TODO: desirable? Set by default? Measure memory consumption & performance.
    };
    lxw_workbook *workbook = workbook_new_opt(output, &options);
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, config->sheet_name);
    csv2xlsx_format_set format_set = csv2xlsx_format_set_create(workbook);
    csv2xlsx_convert_csv_to_worksheet(worksheet, csv_reader, config, &format_set);
    workbook_close(workbook);

    return true;
}

csv2xlsx_config csv2xlsx_config_default() {
    return (csv2xlsx_config) {
        .delimiter = ',',
        .quote = '"',
        .sheet_name = "Sheet 1",
        .auto_convert = CSV2XLSX_AUTO_CONVERT_ALL,
    };
}

bool csv2xlsx_parse_cell_type(const char* text, csv2xlsx_cell_type *cell_type) {
    if (strcmp(text, "text") == 0) {
        *cell_type = CSV2XLSX_CELL_TYPE_TEXT;
    } else if (strcmp(text, "number") == 0) {
        *cell_type = CSV2XLSX_CELL_TYPE_NUMBER;
    } else if (strcmp(text, "digit") == 0) {
        *cell_type = CSV2XLSX_CELL_TYPE_DIGIT;
    } else if (strcmp(text, "percent") == 0) {
        *cell_type = CSV2XLSX_CELL_TYPE_PERCENT;
    } else if (strcmp(text, "bool") == 0 || strcmp(text, "boolean") == 0) {
        *cell_type = CSV2XLSX_CELL_TYPE_BOOL;
    } else if (strcmp(text, "formula") == 0) {
        *cell_type = CSV2XLSX_CELL_TYPE_FORMULA;
    } else if (strcmp(text, "url") == 0) {
        *cell_type = CSV2XLSX_CELL_TYPE_URL;
    } else {
        csv2xlsx_set_last_error("Unknown cell type '%s'.", text);

        return false;
    }

    return true;
}

csv2xlsx_format csv2xlsx_format_create() {
    return (csv2xlsx_format) {
        .color = CSV2XLSX_COLOR_UNDEFINED,
        .background_color = CSV2XLSX_COLOR_UNDEFINED,
        .align = CSV2XLSX_ALIGN_UNDEFINED,
        .underline = CSV2XLSX_UNDERLINE_UNDEFINED,
    };
}

csv2xlsx_column_definition csv2xlsx_column_definition_create() {
    return (csv2xlsx_column_definition) {
        .type = CSV2XLSX_CELL_TYPE_UNDEFINED,
        .width = CSV2XLSX_WIDTH_UNDEFINED,
        .max_width = CSV2XLSX_WIDTH_UNDEFINED,
        .header_format = csv2xlsx_format_create(),
        .data_format = csv2xlsx_format_create(),
    };
}

csv2xlsx_column_config csv2xlsx_column_config_create(lxw_workbook *workbook) {
    return (csv2xlsx_column_config) {
        .default_column_definition = csv2xlsx_column_definition_create(),
    };
}

csv2xlsx_column_definition* csv2xlsx_column_config_get_column_definition(
    csv2xlsx_column_config *config,
    unsigned int column
) {
    return config->column_definitions[column].initialized
        ? &config->column_definitions[column]
        : &config->default_column_definition;
}

unsigned int csv2xlsx_int_length(int value) {
    if (value == 0) {
        return 1;
    }

    int length = (unsigned int) log10(abs(value)) + 1;

    if (value < 0) {
        length++;
    }

    return length;
}

/**
 * Naive implementation, supports the most basic formatting only.
 */
size_t csv2xlsx_formatted_number_length(double value, const char* number_format)
{
    char *decimal_separator = strchr(number_format, '.');
    size_t zeroes_count = 0;

    if (decimal_separator != NULL) {
        zeroes_count = strspn(decimal_separator + 1, "0");
    }

    double rounded_value = round(value * pow(10, (double) zeroes_count)) / pow(10, (double) zeroes_count);
    size_t length = csv2xlsx_int_length((unsigned int) rounded_value);

    // Thousands separator
    if (strstr(number_format, "# ") != NULL) {
        length += (length - 1) / 3;
    }

    if (decimal_separator) {
        length += 1 + zeroes_count;
    }

    return length;
}

lxw_error csv2xlsx_convert_csv_to_worksheet(
    lxw_worksheet *worksheet,
    struct csv_reader *csv_reader,
    csv2xlsx_config *config,
    csv2xlsx_format_set *format_set
) {
    lxw_row_t row = 0;
    lxw_col_t column = 0;
    lxw_col_t max_column = 0;
    double column_widths[CSV2XLST_MAX_COLUMNS] = {0};
    lxw_error error;

    while (true) {
		bool end_of_line;
		const char *value = csv_read_next(csv_reader, &end_of_line);

		if (value == NULL) {
			break;
        }

        error = csv2xlsx_write_cell(worksheet, row, column++, value, config, format_set, column_widths);

		if (error != LXW_NO_ERROR) {
			return error;
		}

		if (end_of_line) {
			if (column > max_column) {
                max_column = column;
			}

			row++;
			column = 0;
		}
	}

	if (config->table) {
		lxw_table_options options = {.no_autofilter = config->auto_filter ? LXW_FALSE : LXW_TRUE};
		error = worksheet_add_table(worksheet, 0, 0, row - 1, max_column - 1, &options);
	} else if (config->auto_filter) {
		error = worksheet_autofilter(worksheet, 0, 0, row - 1, max_column - 1);
	}

    if (error != LXW_NO_ERROR) {
        return error;
    }

    if (config->freeze_row > 0 || config->freeze_column > 0) {
        worksheet_freeze_panes(worksheet, config->freeze_row, config->freeze_column);
    }

    for (lxw_col_t i = 0; i < max_column; i++) {
        if (column_widths[i] > 0) {
            worksheet_set_column(worksheet, i, i, column_widths[i] + 1, NULL);
        }
    }

	return LXW_NO_ERROR;
}

void csv2xlsx_complete_format(csv2xlsx_format *format, csv2xlsx_cell_type type) {
    static char number_format_empty[] = "";
    static char number_format_default[] = "@";
    static char number_format_digit[] = "0";
    static char number_format_percent[] = "0%";

    switch (type) {
        case CSV2XLSX_CELL_TYPE_BLANK:
        case CSV2XLSX_CELL_TYPE_BOOL:
        case CSV2XLSX_CELL_TYPE_FORMULA:
        case CSV2XLSX_CELL_TYPE_NUMBER:
            break;
        case CSV2XLSX_CELL_TYPE_DIGIT:
            if (format->number_format == NULL) {
                format->number_format = number_format_digit;
            }

            break;
        case CSV2XLSX_CELL_TYPE_PERCENT:
            if (format->number_format == NULL) {
                format->number_format = number_format_percent;
            }

            break;
        case CSV2XLSX_CELL_TYPE_URL:
            if (format->number_format == NULL) {
                format->number_format = number_format_default;
            }

            if (format->color == CSV2XLSX_COLOR_UNDEFINED) {
                format->color = CSV2XLSX_COLOR_LINK;
            }

            if (format->underline == CSV2XLSX_UNDERLINE_UNDEFINED) {
                format->underline = CSV2XLSX_UNDERLINE_SINGLE;
            }

            break;
        default:
            if (format->number_format == NULL) {
                format->number_format = number_format_default;
            }
	}

    if (format->number_format == NULL) {
        format->number_format = number_format_empty;
    }

    if (format->color == CSV2XLSX_COLOR_UNDEFINED) {
        format->color = CSV2XLSX_COLOR_UNSET;
    }

    if (format->background_color == CSV2XLSX_COLOR_UNDEFINED) {
        format->background_color = CSV2XLSX_COLOR_UNSET;
    }

    if (format->align == CSV2XLSX_ALIGN_UNDEFINED) {
        format->align = CSV2XLSX_ALIGN_NONE;
    }

    if (format->underline == CSV2XLSX_UNDERLINE_UNDEFINED) {
        format->underline = CSV2XLSX_UNDERLINE_NONE;
    }
}

csv2xlsx_format_set csv2xlsx_format_set_create(lxw_workbook *workbook) {
    csv2xlsx_format_set format_set = {
        .workbook = workbook,
        .formats_count = 1,
    };
    format_set.formats[CSV2XLST_NULL_FORMAT] = workbook_add_format(workbook);

    return format_set;
}

lxw_format* csv2xlsx_format_set_find(csv2xlsx_format_set *format_set, csv2xlsx_format *format) {
    size_t i;
    lxw_format *format_set_format;

    for (i = 0; i < format_set->formats_count; i++) {
        format_set_format = format_set->formats[i];

        if (strcmp(format_set_format->num_format, format->number_format) != 0) {
            continue;
        }

        if (format_set_format->font_color != format->color) {
            continue;
        }

        if (format_set_format->bg_color != format->background_color) {
            continue;
        }

        if (format_set_format->text_h_align != format->align) {
            continue;
        }

        if (format_set_format->underline != format->underline) {
            continue;
        }

        break;
    }

    if (i == format_set->formats_count) {
        return NULL;
    }

    return format_set_format;
}

lxw_format* csv2xlsx_format_set_add(csv2xlsx_format_set *format_set, csv2xlsx_format *format) {
    lxw_format *format_set_format = workbook_add_format(format_set->workbook);
    format_set_num_format(format_set_format, format->number_format);
    format_set_font_color(format_set_format, format->color);
    format_set_bg_color(format_set_format, format->background_color);
    format_set_align(format_set_format, format->align);
    format_set_underline(format_set_format, format->underline);
    format_set->formats[format_set->formats_count++] = format_set_format;

    return format_set_format;
}

lxw_format* get_column_format(
    csv2xlsx_column_definition *column_definition,
    csv2xlsx_format_set *format_set,
    csv2xlsx_cell_type type,
    bool header
) {
    csv2xlsx_format *format;

    if (header) {
        format = &column_definition->header_format;
        // TODO: Called multiple times
        csv2xlsx_complete_format(format, type);
    } else {
        format = column_definition->type_formats[type];
    }

    if (format == NULL) {
        // TODO: The only heap allocation, does it worth it?
        format = (csv2xlsx_format *) malloc(sizeof(csv2xlsx_format));
        *format = column_definition->data_format;
        csv2xlsx_complete_format(format, type);
        column_definition->type_formats[type] = format;
    }

    if (format->_format_set_format != NULL) {
        return format->_format_set_format;
    }

    lxw_format *format_set_format = csv2xlsx_format_set_find(format_set, format);

    if (format_set_format == NULL) {
        format_set_format = csv2xlsx_format_set_add(format_set, format);
    }

    format->_format_set_format = format_set_format;

    return format_set_format;
}

// TODO: use const where possible
lxw_error csv2xlsx_write_cell(
    lxw_worksheet *worksheet,
    lxw_row_t row,
    lxw_col_t column,
    const char *value,
    csv2xlsx_config *config,
    csv2xlsx_format_set *format_set,
    double *column_widths
) {
    csv2xlsx_column_definition *column_definition;
    column_definition = csv2xlsx_column_config_get_column_definition(&config->column_config, column);
    csv2xlsx_cell_value cell_value = csv2xlsx_convert_value(value, column_definition->type, config->auto_convert); // TODO: HEADER SHOULD IGNORE THIS
    lxw_format *format;
    format = get_column_format(column_definition, format_set, cell_value.type, config->header_row_count > row);
    const char *number_format = format->num_format;
    bool adjust_width = column_definition->width == CSV2XLSX_WIDTH_AUTO;
    lxw_error error;
    size_t length;

    if (format == format_set->formats[CSV2XLST_NULL_FORMAT]) {
        format = NULL;
    }

	switch (cell_value.type) {
        case CSV2XLSX_CELL_TYPE_BLANK:
            error = worksheet_write_blank(worksheet, row, column, format);
            length = 1;
            break;
        case CSV2XLSX_CELL_TYPE_DIGIT:
            error = worksheet_write_number(worksheet, row, column, cell_value.digit, format);
            length = adjust_width ? csv2xlsx_int_length(cell_value.digit) : 0;
            break;
        case CSV2XLSX_CELL_TYPE_NUMBER:
            error = worksheet_write_number(worksheet, row, column, cell_value.number, format);
            length = adjust_width ? csv2xlsx_formatted_number_length(cell_value.number, number_format) : 0;
            break;
        case CSV2XLSX_CELL_TYPE_PERCENT:
            error = worksheet_write_number(worksheet, row, column, cell_value.number, format);
            length = adjust_width ? csv2xlsx_formatted_number_length(cell_value.number, number_format) + 1 : 0;
            break;
        case CSV2XLSX_CELL_TYPE_BOOL:
            error = worksheet_write_boolean(worksheet, row, column, cell_value.boolean ? 1 : 0, format);
            length = (size_t) LXW_DEF_COL_WIDTH;
            break;
        case CSV2XLSX_CELL_TYPE_FORMULA:
            error = worksheet_write_formula(worksheet, row, column, cell_value.text, format);
            length = (size_t) LXW_DEF_COL_WIDTH;
            break;
        case CSV2XLSX_CELL_TYPE_URL:
            error = worksheet_write_url(worksheet, row, column, cell_value.text, format);
            length = adjust_width ? strlen(cell_value.text) : 0;
            break;
        default:
            error = worksheet_write_string(worksheet, row, column, cell_value.text, format);
            length = adjust_width ? strlen(cell_value.text) : 0;
            break;
	}

    if (adjust_width && length > column_widths[column]) {
	    double max_width = column_definition->max_width == CSV2XLSX_WIDTH_UNDEFINED
	        ? CSV2XLSX_MAX_WIDTH_DEFAULT
	        : column_definition->max_width;
        column_widths[column] = length > max_width ? max_width : length;
    }

	return error;
}

csv2xlsx_cell_value csv2xlsx_convert_value(const char *text, csv2xlsx_cell_type type, auto_convert auto_convert) {
	csv2xlsx_cell_value value = {
		.type = CSV2XLSX_CELL_TYPE_TEXT,
		.text = text,
		.digit = 0,
		.number = 0.,
		.boolean = false,
	};

	if (*text == '\0') {
		value.type = CSV2XLSX_CELL_TYPE_BLANK;

		return value;
	}

	if (type == CSV2XLSX_CELL_TYPE_TEXT) {
        return value;
	}

	if (type == CSV2XLSX_CELL_TYPE_BOOL) {
		if (strcmp(text, "1") == 0) {
			value.type = CSV2XLSX_CELL_TYPE_BOOL;
			value.boolean = true;

			return value;
		}

		if (strcmp(text, "0") == 0) {
			value.type = CSV2XLSX_CELL_TYPE_BOOL;
			value.boolean = false;

			return value;
		}
	}

	if (type == CSV2XLSX_CELL_TYPE_BOOL || auto_convert & CSV2XLSX_AUTO_CONVERT_BOOL) {
		if (csv2xlsx_string_compare_ignore_case(text, "true") == 0) {
			value.type = CSV2XLSX_CELL_TYPE_BOOL;
			value.boolean = true;

			return value;
		}

		if (csv2xlsx_string_compare_ignore_case(text, "false") == 0) {
			value.type = CSV2XLSX_CELL_TYPE_BOOL;
			value.boolean = false;

			return value;
		}
	}

    bool auto_convert_digit = type != CSV2XLSX_CELL_TYPE_NUMBER && auto_convert & CSV2XLSX_AUTO_CONVERT_DIGIT;

	if ((type == CSV2XLSX_CELL_TYPE_DIGIT || auto_convert_digit) && csv2xlsx_is_digit(text)) {
        if (csv2xlsx_translate_int(text, &value.digit)) {
            value.type = CSV2XLSX_CELL_TYPE_DIGIT;

            return value;
        }
	}

	if (
        (type == CSV2XLSX_CELL_TYPE_NUMBER || auto_convert & CSV2XLSX_AUTO_CONVERT_NUMBER)
        && csv2xlsx_is_number(text)
    ) {
        char *comma = strrchr(text, ',');

        if (comma) {
            *comma = '.';
        }

		char *end;
		double number = strtod(text, &end);

        if (comma) {
            *comma = ',';
        }

		if (*end == '\0') {
			value.type = CSV2XLSX_CELL_TYPE_NUMBER;
			value.number = number;

			return value;
		}
	}

	if (
        (type == CSV2XLSX_CELL_TYPE_PERCENT || auto_convert & CSV2XLSX_AUTO_CONVERT_PERCENT)
        && csv2xlsx_is_percent(text)
    ) {
		char *end;
		int number = strtol(text, &end, 10);

		if (*end == '%' && end[1] == '\0') {
			value.type = CSV2XLSX_CELL_TYPE_PERCENT;
			value.number = number / 100.0;

			return value;
		}
	}

	if (
        (type == CSV2XLSX_CELL_TYPE_FORMULA || auto_convert & CSV2XLSX_AUTO_CONVERT_FORMULA)
        && csv2xlsx_is_formula(text)
    ) {
		value.type = CSV2XLSX_CELL_TYPE_FORMULA;

		return value;
	}

	if ((type == CSV2XLSX_CELL_TYPE_URL || auto_convert & CSV2XLSX_AUTO_CONVERT_URL) && csv2xlsx_is_url(text)) {
		value.type = CSV2XLSX_CELL_TYPE_URL;

		return value;
	}

	return value;
}

bool csv2xlsx_is_digit(const char *text) {
	if (*text == '-' /* || *text == '+' || */) {
		text++;
	}

    if (*text == '\0') {
        return false;
    }

    do {
		if (*text < '0' || *text > '9') {
			return false;
		}
    } while (*++text != '\0');

	return true;
}

bool csv2xlsx_is_number(const char *text) {
    if (*text == '-' /* || *text == '+' || */) {
		text++;
	}

    if (*text == '\0') {
        return false;
    }

	int decimal_separator_count = 0;

	do {
		if (*text == '.' || *text == ',') {
            if (++decimal_separator_count > 1) {
                return false;
            }
		} else if (*text < '0' || *text > '9') {
			return false;
		}
	} while (*++text != '\0');

    return true;
}

bool csv2xlsx_is_percent(const char *text) {
    if (*text == '-' /* || *text == '+' || */) {
        text++;
    }

    if (*text == '\0') {
        return false;
    }

    do {
        if (*text < '0' || *text > '9') {
            return false;
        }
    } while (*++text != '\0');

    return *text == '%' && text[1] == '\0';
}

bool csv2xlsx_is_formula(const char *text) {
	return *text == '=';
}

bool csv2xlsx_is_url(const char *text) {
	if (strncmp("https://", text, 8) == 0) {
		return true;
	}

	if (strncmp("http://", text, 7) == 0) {
		return true;
	}

	return false;
}

bool csv2xlsx_parse_default_column_definition(
    char** column_definition_strings,
    int column_definition_count,
    csv2xlsx_column_definition *default_column_definition
) {
    for (int i = 0; i < column_definition_count; i++) {
        int column;
        char* definition_string = column_definition_strings[i];

        if (!csv2xlsx_parse_column_definition_column(definition_string, &column)) {
            return false;
        }

        if (column != CSV2XLSX_COLUMN_UNDEFINED) {
            continue;
        }

        return csv2xlsx_parse_column_definition(definition_string, default_column_definition);
    }

    return true;
}

bool csv2xlsx_parse_default_header_column_format(
    char** header_column_format_strings,
    int header_column_format_count,
    csv2xlsx_column_definition *default_column_definition
) {
    for (int i = 0; i < header_column_format_count; i++) {
        int column;
        char* definition_string = header_column_format_strings[i];
        csv2xlsx_parse_column_definition_column(definition_string, &column);

        if (column != CSV2XLSX_COLUMN_UNDEFINED) {
            continue;
        }

        if (csv2xlsx_parse_column_definition_format(definition_string, &default_column_definition->header_format)) {
            break;
        }
    }

    return true;
}

bool csv2xlsx_parse_column_definitions(
    char** column_definition_strings,
    int column_definition_count,
    csv2xlsx_column_config *column_config
) {
    for (int i = 0; i < column_definition_count; i++) {
        int column;
        char* definition_string = column_definition_strings[i];

        if (!csv2xlsx_parse_column_definition_column(definition_string, &column)) {
            return false;
        }

        if (column == CSV2XLSX_COLUMN_UNDEFINED) {
            continue;
        }

        csv2xlsx_column_definition *column_definition = &column_config->column_definitions[column];

        if (!column_definition->initialized) {
            *column_definition = column_config->default_column_definition;
        }

        if (csv2xlsx_parse_column_definition(definition_string, column_definition)) {
            column_definition->initialized = true;
        }
    }

    return true;
}

bool csv2xlsx_parse_header_column_formats(
    char** header_column_format_strings,
    int header_column_format_count,
    csv2xlsx_column_config *column_config
) {
    for (int i = 0; i < header_column_format_count; i++) {
        int column;
        char* definition_string = header_column_format_strings[i];
        csv2xlsx_parse_column_definition_column(definition_string, &column);

        if (column == CSV2XLSX_COLUMN_UNDEFINED) {
            continue;
        }

        csv2xlsx_column_definition *column_definition = &column_config->column_definitions[column];

        if (!column_definition->initialized) {
            *column_definition = column_config->default_column_definition;
            column_definition->initialized = true;
        }

        column_definition->header_format = column_config->default_column_definition.header_format;

        if (!csv2xlsx_parse_column_definition_format(definition_string, &column_definition->header_format)) {
            return false;
        }
    }

    return true;
}

bool csv2xlsx_parse_column_config(
    char** column_definition_strings,
    char** header_column_format_strings,
    int column_definition_count,
    int header_column_format_count,
    csv2xlsx_column_config *column_config
) {
    int success = csv2xlsx_parse_default_column_definition(
        column_definition_strings,
        column_definition_count,
        &column_config->default_column_definition
    );

    if (!success) {
        return false;
    }

    success = csv2xlsx_parse_default_header_column_format(
        header_column_format_strings,
        header_column_format_count,
        &column_config->default_column_definition
    );

    if (!success) {
        return false;
    }

    column_config->default_column_definition.initialized = true;

    if (!csv2xlsx_parse_column_definitions(column_definition_strings, column_definition_count, column_config)) {
        return false;
    }

    success = csv2xlsx_parse_header_column_formats(
        header_column_format_strings,
        header_column_format_count,
        column_config
    );

    return success;
}

bool csv2xlsx_parse_column_definition_column(const char* text, int* column) {
    char* delimiter = strchr(text, '=');

    // -c "definition", -c="definition"
    if (delimiter == NULL || delimiter == text) {
        *column = CSV2XLSX_COLUMN_UNDEFINED;

        return true;
    }

    char *end;
    long value = strtol(text, &end, 10);

    if (end != delimiter) {
        size_t column_length = delimiter - text;
        csv2xlsx_set_last_error(
            "Unable to parse column definition. Invalid column number '%.*s'.",
            column_length,
            text
        );

        return false;
    }

    *column = (int) value;

    return true;
}

bool csv2xlsx_parse_column_definition_format_property(char* text, csv2xlsx_format* format) {
    char* delimiter = strchr(text, ':');

    if (delimiter == NULL || delimiter == text) {
        csv2xlsx_set_last_error("Unable to parse column definition. Malformed format property definition '%s'.", text);

        return false;
    }

    size_t property_length = csv2xlsx_substring_trim_right_length(text, delimiter - 1);
    char* value = csv2xlsx_string_trim_left(delimiter + 1);
    bool success = false;

    if (csv2xlsx_prefix_equals(text, "color", property_length)) {
        success = csv2xlsx_parse_color(value, &format->color);
    } else if (csv2xlsx_prefix_equals(text, "background-color", property_length)) {
        success = csv2xlsx_parse_color(value, &format->background_color);
    } else if (csv2xlsx_prefix_equals(text, "align", property_length)) {
        success = csv2xlsx_parse_align(value, &format->align);
    } else if (csv2xlsx_prefix_equals(text, "number-format", property_length)) {
        format->number_format = value;
        success = true;
    } else {
        csv2xlsx_set_last_error("Unable to parse column definition. Unknown property '%.*s'.", property_length, text);
        success = false;
    }

    return success;
}

bool csv2xlsx_parse_column_definition_format(char* text, csv2xlsx_format* format) {
    char* delimiter = strchr(text, '=');
    char *format_string = delimiter != NULL ? delimiter + 1 : text;
    char* token = csv2xlsx_string_trim_left(strtok(format_string, ";"));

    while (token != NULL) {
        if (!csv2xlsx_parse_column_definition_format_property(token, format)) {
            return false;
        }

        token = csv2xlsx_string_trim_left(strtok(NULL, ";"));
    }

    return true;
}

bool csv2xlsx_parse_column_definition(char* text, csv2xlsx_column_definition *column_definition) {
    char* definition_delimiter = strchr(text, '=');
    char *property_string = definition_delimiter != NULL ? definition_delimiter + 1 : text;
    char* token = csv2xlsx_string_trim_left(strtok(property_string, ";"));

    while (token != NULL) {
        char* delimiter = strchr(token, ':');

        if (delimiter == NULL || delimiter == token) {
            csv2xlsx_set_last_error("Unable to parse column definition. Malformed property definition '%s'.", token);

            return false;
        }

        size_t property_length = csv2xlsx_substring_trim_right_length(token, delimiter - 1);
        char* value = csv2xlsx_string_trim_left(delimiter + 1);
        bool success = false;

        if (csv2xlsx_prefix_equals(token, "type", property_length)) {
            success = csv2xlsx_parse_cell_type(value, &column_definition->type);
        } else if (csv2xlsx_prefix_equals(token, "width", property_length)) {
            if (csv2xlsx_string_compare_ignore_case(value, "auto") == 0) {
                column_definition->width = CSV2XLSX_WIDTH_AUTO;
                success = true;
            } else {
                success = csv2xlsx_translate_double(value, &column_definition->width);

                if (!success) {
                    csv2xlsx_set_last_error(
                        "Unable to parse width value '%.*s', it must be a number without a unit or 'auto'.",
                        csv2xlsx_string_trim_right_length(value),
                        value
                    );
                }
            }
        } else if (csv2xlsx_prefix_equals(token, "max-width", property_length)) {
            success = csv2xlsx_translate_double(value, &column_definition->max_width);

            if (!success) {
                csv2xlsx_set_last_error(
                    "Unable to parse max-width value '%.*s', it must be a number without a unit.",
                    csv2xlsx_string_trim_right_length(value),
                    value
                );
            }
        } else {
            success = csv2xlsx_parse_column_definition_format_property(token, &column_definition->data_format);
        }

        if (!success) {
            return false;
        }

        token = csv2xlsx_string_trim_left(strtok(NULL, ";"));
    }

    return true;
}

bool csv2xlsx_parse_freeze_panes(char *text, unsigned int *row, unsigned int *column) {
    *row = 0;
    *column = 0;

    if (text == NULL) {
        csv2xlsx_set_last_error("Unable to parse freeze panes option.");

        return false;
    }

    char *token = strtok(text, ",");
    bool success = false;

    if (token == NULL) {
        csv2xlsx_set_last_error("Unable to parse freeze panes option.");

        return false;
    }

    if (text[0] == ',') {
        success = csv2xlsx_translate_unsigned_int(token, column);
    } else {
        success = csv2xlsx_translate_unsigned_int(token, row);
    }

    if (success) {
        token = strtok(NULL, ",");

        if (token == NULL) {
            return true;
        }

        success = csv2xlsx_translate_unsigned_int(token, column);
    }

    if (!success) {
        char *value = csv2xlsx_string_trim_left(token);
        csv2xlsx_set_last_error(
            "Unable to parse freeze panes option. Value '%.*s' is not unsigned integer.",
            csv2xlsx_string_trim_right_length(value),
            value
        );
    }

    return success;
}

bool csv2xlsx_parse_align(const char* text, csv2xlsx_align *align) {
    size_t length = csv2xlsx_string_trim_right_length(text);

    if (csv2xlsx_substring_compare_ignore_case(text, "none", length) == 0) {
        *align = CSV2XLSX_ALIGN_NONE;
    } else if (csv2xlsx_substring_compare_ignore_case(text, "left", length) == 0) {
        *align = CSV2XLSX_ALIGN_LEFT;
    } else if (csv2xlsx_substring_compare_ignore_case(text, "center", length) == 0) {
        *align = CSV2XLSX_ALIGN_CENTER;
    } else if (csv2xlsx_substring_compare_ignore_case(text, "right", length) == 0) {
        *align = CSV2XLSX_ALIGN_RIGHT;
    } else if (csv2xlsx_substring_compare_ignore_case(text, "justify", length) == 0) {
        *align = CSV2XLSX_ALIGN_JUSTIFY;
    } else {
        csv2xlsx_set_last_error("Unknown align '%.*s'.", length, text);

        return false;
    }

    return true;
}

bool csv2xlsx_parse_color(const char* text, csv2xlsx_color *color) {
    if (text[0] != '#') {
        csv2xlsx_set_last_error("Unable to parse color. Value '%s' does not start with hashbang.", text);

        return false;
    }

    unsigned int r, g, b;
    bool success = false;

    switch (strlen(text + 1)) {
        case 6:
            if (sscanf(text + 1, "%02x%02x%02x", &r, &g, &b) == 3) {
                success = true;
            }

            break;
        case 3:
            if (sscanf(text + 1, "%1x%1x%1x", &r, &g, &b) == 3) {
                success = true;
                r = (r << 4) | r;
                g = (g << 4) | g;
                b = (b << 4) | b;
            }

            break;
    }

    if (!success) {
        csv2xlsx_set_last_error(
            "Unable to parse color. Value '%s' does not conform to 3-digit or 6-digit hex notation.",
            text
        );

        return false;
    }

    *color = (r << 16) | (g << 8) | b;

    return true;
}

bool csv2xlsx_translate_int(const char* text, int *value) {
    if (text == NULL) {
        return false;
    }

    char *end;
    long long_value = strtol(text, &end, 10);

    if (*end != '\0' || long_value < INT_MIN || long_value > INT_MAX) {
        return false;
    }

    *value = (int) long_value;

    return true;
}

bool csv2xlsx_translate_unsigned_int(const char* text, unsigned int *value) {
    if (text == NULL) {
        return false;
    }

    char *end;
    long long_value = strtoul(text, &end, 10);

    if (*end != '\0' || long_value > UINT_MAX) {
        return false;
    }

    *value = (unsigned int) long_value;

    return true;
}

bool csv2xlsx_translate_double(const char* text, double *value) {
    if (text == NULL) {
        return false;
    }

    char *end;
    *value = strtod(text, &end);

    if (*end != '\0') {
        return false;
    }

    return true;
}

char *csv2xlsx_string_trim_left(char *text) {
    if (text == NULL || *text == '\0') {
        return text;
    }

    while (isblank(*text)) {
        text++;
    }

    return text;
}

size_t csv2xlsx_string_trim_right_length(const char* text) {
    size_t length = strlen(text);

    return csv2xlsx_substring_trim_right_length(text, text + length - 1);
}

size_t csv2xlsx_substring_trim_right_length(const char* start, const char* end) {
    const char* character = end;

    while (character >= start && isspace((unsigned char) *character)) {
        character--;
    }

    return character - start + 1;
}

bool csv2xlsx_string_ends_with(const char *text, const char *suffix) {
    if (text == NULL || suffix == NULL) {
        return false;
    }

    size_t length = strlen(text);
    size_t suffix_length = strlen(suffix);

    if (suffix_length > length) {
        return false;
    }

    return strcmp(text + length - suffix_length, suffix) == 0;
}

bool csv2xlsx_prefix_equals(const char* text, const char* prefix, size_t prefix_length) {
    size_t length = strlen(prefix);

    return length == prefix_length && strncmp(text, prefix, length) == 0;
}

static char csv2xlsx_last_error[256] = {'\0'};

const char* csv2xlsx_get_last_error() {
    return csv2xlsx_last_error;
}

void csv2xlsx_set_last_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(csv2xlsx_last_error, sizeof(csv2xlsx_last_error), format, args);
    va_end(args);
}

const char* csv2xlsx_get_lxw_error_message(lxw_error error) {
    switch (error) {
        case LXW_NO_ERROR:
            return "No error.";
        case LXW_ERROR_MEMORY_MALLOC_FAILED:
            return "Memory error, failed to malloc() required memory.";
        case LXW_ERROR_CREATING_XLSX_FILE:
            return "Error creating output xlsx file. Usually a permissions error.";
        case LXW_ERROR_CREATING_TMPFILE:
            return "Error encountered when creating a tmpfile during file assembly.";
        case LXW_ERROR_READING_TMPFILE:
            return "Error reading a tmpfile.";
        case LXW_ERROR_ZIP_FILE_OPERATION:
            return "Zip generic error ZIP_ERRNO while creating the xlsx file.";
        case LXW_ERROR_ZIP_PARAMETER_ERROR:
            return "Zip error ZIP_PARAMERROR while creating the xlsx file.";
        case LXW_ERROR_ZIP_BAD_ZIP_FILE:
            return "Zip error ZIP_BADZIPFILE (use_zip64 option may be required).";
        case LXW_ERROR_ZIP_INTERNAL_ERROR:
            return "Zip error ZIP_INTERNALERROR while creating the xlsx file.";
        case LXW_ERROR_ZIP_FILE_ADD:
            return "File error or unknown zip error when adding sub file to xlsx file.";
        case LXW_ERROR_ZIP_CLOSE:
            return "Unknown zip error when closing xlsx file.";
        case LXW_ERROR_FEATURE_NOT_SUPPORTED:
            return "Feature is not currently supported in this configuration.";
        case LXW_ERROR_NULL_PARAMETER_IGNORED:
            return "NULL function parameter ignored.";
        case LXW_ERROR_PARAMETER_VALIDATION:
            return "Function parameter validation error.";
        case LXW_ERROR_SHEETNAME_LENGTH_EXCEEDED:
            return "Worksheet name exceeds Excel's limit of 31 characters.";
        case LXW_ERROR_INVALID_SHEETNAME_CHARACTER:
            return "Worksheet name cannot contain invalid characters: '[ ] : * ? / \\'.";
        case LXW_ERROR_SHEETNAME_START_END_APOSTROPHE:
            return "Worksheet name cannot start or end with an apostrophe.";
        case LXW_ERROR_SHEETNAME_ALREADY_USED:
            return "Worksheet name is already in use.";
        case LXW_ERROR_32_STRING_LENGTH_EXCEEDED:
            return "Parameter exceeds Excel's limit of 32 characters.";
        case LXW_ERROR_128_STRING_LENGTH_EXCEEDED:
            return "Parameter exceeds Excel's limit of 128 characters.";
        case LXW_ERROR_255_STRING_LENGTH_EXCEEDED:
            return "Parameter exceeds Excel's limit of 255 characters.";
        case LXW_ERROR_MAX_STRING_LENGTH_EXCEEDED:
            return "String exceeds Excel's limit of 32,767 characters.";
        case LXW_ERROR_SHARED_STRING_INDEX_NOT_FOUND:
            return "Error finding internal string index.";
        case LXW_ERROR_WORKSHEET_INDEX_OUT_OF_RANGE:
            return "Worksheet row or column index out of range.";
        case LXW_ERROR_WORKSHEET_MAX_URL_LENGTH_EXCEEDED:
            return "Maximum hyperlink length (2079) exceeded.";
        case LXW_ERROR_WORKSHEET_MAX_NUMBER_URLS_EXCEEDED:
            return "Maximum number of worksheet URLs (65530) exceeded.";
        case LXW_ERROR_IMAGE_DIMENSIONS:
            return "Couldn't read image dimensions or DPI.";
        case LXW_MAX_ERRNO:
            return "Maximum error number reached.";
        default:
            return "Unknown error.";
    }
}
