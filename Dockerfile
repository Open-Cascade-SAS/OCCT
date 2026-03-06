FROM buildpack-deps:jammy AS builder
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    make \
    tcl \
    tcl-dev \
    tk \
    tk-dev \
    libgl1-mesa-dev \
    libgl2ps-dev \
    libvtk9-dev \
    libxi-dev \
    libxmu-dev \
    rapidjson-dev \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /occt
COPY . .
RUN mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=release \
             -DUSE_RAPIDJSON=ON \
             .. \
    && make -j$(nproc) \
    && make install

FROM buildpack-deps:jammy
COPY --from=builder /usr/local/ /usr/local/
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
