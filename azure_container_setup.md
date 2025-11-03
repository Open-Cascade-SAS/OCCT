# Azure Container Instances (ACI) Safe OCCT Configuration
# This dockerfile and environment setup ensures OCCT works safely in Azure containers

# Dockerfile for Azure-safe OCCT deployment
FROM ubuntu:22.04

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libfreetype6-dev \
    libfreeimage-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libx11-dev \
    libxext-dev \
    libxmu-dev \
    libxi-dev \
    && rm -rf /var/lib/apt/lists/*

# Set environment variables for container-safe operation
ENV OCCT_DISABLE_SIGNALS=1
ENV OCC_CONVERT_SIGNALS=0
ENV DOCKER_CONTAINER=1

# Copy your OCCT application
COPY your_occt_app /usr/local/bin/
COPY libTK*.so /usr/local/lib/

# Set library path
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Create non-root user for security
RUN groupadd -r occtuser && useradd -r -g occtuser occtuser
USER occtuser

# Entry point that ensures safe signal handling
ENTRYPOINT ["/usr/local/bin/container_safe_entry.sh"]

---

#!/bin/bash
# container_safe_entry.sh - Entry point script for Azure container safety

# Disable core dumps in container environment
ulimit -c 0

# Set signal handling to ignore problematic signals
trap 'echo "Received SIGTERM, shutting down gracefully"; exit 0' TERM
trap 'echo "Received SIGINT, shutting down gracefully"; exit 0' INT

# Ensure OCCT doesn't override container signal handlers
export OCCT_DISABLE_SIGNALS=1
export OCC_CONVERT_SIGNALS=0

# Set Azure-specific environment variables if available
if [ -n "$ACI_RESOURCE_GROUP" ]; then
    echo "Running in Azure Container Instance"
    export AZURE_CONTAINER_ENVIRONMENT=1
fi

# Start your OCCT application with safe signal handling
exec "$@"
