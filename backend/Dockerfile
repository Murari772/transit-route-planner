FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libasio-dev \
    python3-pip \
    unzip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN pip3 install gdown && \
    mkdir -p data && cd data && \
    gdown 18rQnS3L1Gs18XN_XYcdHnwAgV8cm88L2 -O gtfs.zip && \
    unzip -o gtfs.zip && \
    rm gtfs.zip

RUN mkdir -p build && cd build && cmake .. && make -j$(nproc)

EXPOSE 8080

CMD ["./build/TransitRoutePlanner"]
