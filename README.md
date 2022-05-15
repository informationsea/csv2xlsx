# CSV2Excel

[![Build](https://github.com/informationsea/csv2xlsx/actions/workflows/build.yml/badge.svg)](https://github.com/informationsea/csv2xlsx/actions/workflows/build.yml)

Convert CSV files to Excel without worrying auto data format converter.

## License

GPL version 3 or later

## Features

* Convert CSV, TSV to Excel file
* Combine multiple 
* Configurable automatic format converter

## Usage

```
Usage: csv2xlsx [-hadnbpfu] -o <EXCEL> [-s <NAME>]... <CSV,TSV> [<CSV,TSV>]...
  -h, --help                display this help and exit
  -o, --output=<EXCEL>      Output xlsx file
  -t, --disable-table       Disable table (Please set this option to reduce memory usage)
  -a, --disable-autofilter  Disable autofilter
  -d, --disable-convert-digit Disable auto convert to integer
  -n, --disable-convert-number Disable auto convert to floating number
  -b, --disable-convert-bool Disable auto convert to boolean
  -p, --disable-convert-percent Disable auto convert to percent
  -f, --disable-convert-formula Disable auto convert to formula
  -u, --disable-convert-url Disable auto convert to url
  -s, --sheetname=<NAME>    Excel sheet names
  <CSV,TSV>                 Input TSV, CSV files
```

