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