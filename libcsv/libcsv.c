#include "libcsv.h"

#include <string.h>

#define DEFAULT_BUFFER_SIZE 1024

enum CSV_READER_MODE {
	NORMAL, IN_QUOTE, AFTER_QUOTE
};

struct csv_reader* csv_reader_initialize(FILE *file, char delimiter, char quote_char)
{
	struct csv_reader* reader = malloc(sizeof(struct csv_reader));
	memset(reader, 0, sizeof (struct csv_reader));
	reader->file = file;
	reader->delimiter = delimiter;
	reader->quote_char = quote_char;
	reader->buf = malloc(DEFAULT_BUFFER_SIZE);
	reader->current_buffer_size = DEFAULT_BUFFER_SIZE;
	reader->last_charator_is_return = false;
	return reader;
}

static size_t reader_add_charactor(struct csv_reader* reader, char ch, size_t current_size) {
	if (reader->current_buffer_size <= current_size) {
		char *buf = realloc(reader->buf, reader->current_buffer_size + DEFAULT_BUFFER_SIZE);
		if (buf == NULL)
			return 0;
		reader->current_buffer_size += DEFAULT_BUFFER_SIZE;
		reader->buf = buf;
	}
	reader->buf[current_size++] = ch;
	return current_size;
}

const char *csv_read_next(struct csv_reader* reader, bool *lineend)
{
	enum CSV_READER_MODE mode = NORMAL;
	size_t current_size = 0;
	*lineend = false;
	bool finish_flag = false;
	do {
		int ch = fgetc(reader->file);
		if (ch < 0) {
			finish_flag = true;
			break;
		}

		switch (mode) {
		case NORMAL: {
			if (ch == reader->delimiter) {
				reader->last_charator_is_return = false;
				goto finish;
			}
			else if (ch == '\n' && current_size == 0 && reader->last_charator_is_return) {
				continue; // skip
			}
			else if (ch == '\r' || ch == '\n') {
				reader->last_charator_is_return = false;
				*lineend = true;
				if (ch == '\r') reader->last_charator_is_return = true;
				goto finish;
			}
			else if (ch == reader->quote_char && current_size == 0) {
				reader->last_charator_is_return = false;
				mode = IN_QUOTE;
			}
			else {
				reader->last_charator_is_return = false;
				current_size = reader_add_charactor(reader, ch, current_size);
				if (current_size == 0) return 0; // Error
			}
			break;
		}
		case IN_QUOTE:
			if (ch == reader->quote_char) {
				mode = AFTER_QUOTE;
			}
			else {
				current_size = reader_add_charactor(reader, ch, current_size);
				if (current_size == 0) return 0; // Error
			}
			break;
		case AFTER_QUOTE:
			if (ch == reader->quote_char) {
				mode = IN_QUOTE;
				current_size = reader_add_charactor(reader, ch, current_size);
				if (current_size == 0) return 0; // Error
			}
			else if (ch == '\r' || ch == '\n') {
				*lineend = true;
				goto finish;
			}
			else if (ch == reader->delimiter) {
				goto finish;
			}
			break;
		}
	} while (true);
finish:
	reader->buf[current_size] = '\0';
	if (current_size == 0 && finish_flag)
		return NULL;
	return reader->buf;
}

void csv_reader_free(struct csv_reader *reader)
{
	free(reader->buf);
	free(reader);
}