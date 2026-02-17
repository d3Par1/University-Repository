# Dockerfile for building and testing all SSA lab tasks
# Build: docker build -t ssa-labs .
# Run:   docker run -it --rm ssa-labs
# Or mount source: docker run -it --rm -v $(pwd):/src -w /src ssa-labs bash

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    make \
    cmake \
    gdb \
    valgrind \
    ltrace \
    strace \
    binutils \
    musl-tools \
    linux-tools-common \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . /src/

# Build all tasks
RUN bash build_all.sh || true

CMD ["bash"]
