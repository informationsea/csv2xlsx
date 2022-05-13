FROM debian:11 AS build
RUN apt-get update -y && apt-get install -y gcc g++ libz-dev cmake git
COPY . /csv2xlsx
RUN mkdir -p /csv2xlsx/linux-build
WORKDIR /csv2xlsx/linux-build
RUN cmake .. && cmake --build .

FROM debian:11-slim
COPY --from=build /csv2xlsx/linux-build/csv2xlsx /usr/local/bin
