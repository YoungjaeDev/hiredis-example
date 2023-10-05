# docker build -t redis-image .

FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG C.UTF-8

# Update package lists and install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    libhiredis-dev \
    redis-server \
    vim \
    gcc \
    gdb \
    build-essential \
    cmake \
    pkg-config \
    silversearcher-ag \
    git \
    libopencv-dev \
    libevent-dev
