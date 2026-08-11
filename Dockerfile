# Use an official Ubuntu image. Docker on Apple Silicon runs the ARM64 version automatically.
FROM ubuntu:24.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential GCC compiler, CMake, debugging tools, and Qt6 for Phase 3 UI.
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gdb \
    git \
    libsqlite3-dev \
    qt6-base-dev \
    libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*
