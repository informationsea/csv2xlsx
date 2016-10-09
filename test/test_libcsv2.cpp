#include "libcsv.h"

#include <assert.h>
#include <string.h>
#include <gtest/gtest.h>

TEST(OpenCSV, OpenCSV)
{
	const char *csvfile = getenv("CSV1");
	ASSERT_TRUE(csvfile);
	ASSERT_TRUE(strlen(csvfile) > 0);

	FILE *file = fopen(csvfile, "rb");
	ASSERT_TRUE(file);

	struct csv_reader *reader = csv_reader_initialize(file, ',', '"');

	bool end_of_line;
	ASSERT_STREQ("Header 1", csv_read_next(reader, &end_of_line));
	ASSERT_FALSE(end_of_line);
	ASSERT_STREQ("Header 2", csv_read_next(reader, &end_of_line));
	ASSERT_TRUE(end_of_line);

	ASSERT_STREQ("12", csv_read_next(reader, &end_of_line));
	ASSERT_FALSE(end_of_line);
	ASSERT_STREQ("01", csv_read_next(reader, &end_of_line));
	ASSERT_TRUE(end_of_line);

	ASSERT_STREQ("hello, world", csv_read_next(reader, &end_of_line));
	ASSERT_FALSE(end_of_line);
	ASSERT_STREQ("ok", csv_read_next(reader, &end_of_line));
	ASSERT_TRUE(end_of_line);

	ASSERT_STREQ("", csv_read_next(reader, &end_of_line));
	ASSERT_TRUE(end_of_line);

	ASSERT_STREQ("quote \" included", csv_read_next(reader, &end_of_line));
	ASSERT_FALSE(end_of_line);
	ASSERT_STREQ("ng", csv_read_next(reader, &end_of_line));
	ASSERT_TRUE(end_of_line);

	ASSERT_FALSE(csv_read_next(reader, &end_of_line));
	csv_reader_free(reader);
}