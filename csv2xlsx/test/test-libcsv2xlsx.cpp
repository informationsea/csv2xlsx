#include <gtest/gtest.h>
#include "libcsv2xlsx.h"

#include <stdio.h>

TEST(convert, is_digit)
{
    ASSERT_EQ(is_digit("100"), true);
    ASSERT_EQ(is_digit("1"), true);
    ASSERT_EQ(is_digit("+200"), true);
    ASSERT_EQ(is_digit("-300"), true);
    //ASSERT_EQ(is_digit("1,000"), true);
}

TEST(convert, convert_data)
{
    csv2xlsx_config config = csv2xlsx_default();
    {
        auto converted = convert_value("100", &config);
        ASSERT_EQ(converted.digit, 100);
        ASSERT_EQ(converted.type, CSV2XLSX_DIGIT);
    }
}

TEST(convert, testdata)
{
    csv2xlsx_config config = csv2xlsx_default();

    FILE *csv = fopen("testfiles/testdata.csv", "r");
    if (csv == nullptr)
    {
        perror("Cannot open csv file");
        ASSERT_TRUE(false);
    }
    csv2xlsx_with_config(csv, "testfiles/testdata.xlsx", &config);
}

TEST(convert, testdata2)
{
    csv2xlsx_config config = csv2xlsx_default();
    config.delimiter = '\t';
    config.quote = '\0';

    FILE *csv = fopen("testfiles/gene_group", "r");
    if (csv == nullptr)
    {
        perror("Cannot open csv file");
        ASSERT_TRUE(false);
    }
    csv2xlsx_with_config(csv, "testfiles/gene_group.xlsx", &config);
}