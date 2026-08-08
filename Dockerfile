# Use an official Ubuntu image. Docker on Apple Silicon runs the ARM64 version automatically.
FROM ubuntu:24.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential GCC compiler, CMake, and debugging tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gdb \
    git \
    && rm -rf /var/lib/apt/lists/*q

