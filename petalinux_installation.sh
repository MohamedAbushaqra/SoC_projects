#!/bin/bash

# Default Petalinux version
version=2023.1

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --petalinuxversion) version="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# Environment Variables
export DEBIAN_FRONTEND=noninteractive
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8

# Create directories
sudo mkdir -p /opt/petalinux

# Change ownership
sudo chown -R $(whoami):$(whoami) /opt/petalinux

# Update and install packages
sudo dpkg --add-architecture i386
sudo apt-get update -y
sudo apt-get clean all
sudo apt-get install -y -qq iputils-ping sudo rsync apt-utils x11-utils

# Required tools and libraries of Petalinux
sudo apt-get install -y -qq --no-install-recommends \
    iproute2 \
    python3 \
    git-core \
    automake \
    zlib1g:i386 \
    python3-pip \
    python3-pexpect \
    xz-utils \
    debianutils \
    python3-git \
    python3-jinja2 \
    libegl1-mesa \
    less \
    tofrodos \
    gawk \
    xvfb \
    gcc \
    wget \
    build-essential \
    tftpd \
    git \
    make \
    update-inetd \
    net-tools \
    libncurses5-dev \
    zlib1g-dev \
    libssl-dev \
    flex \
    bison \
    libselinux1 \
    gnupg \
    nano \
    diffstat \
    chrpath \
    socat \
    xterm \
    autoconf \
    libtool \
    tar \
    unzip \
    texinfo \
    gcc-multilib \
    libsdl1.2-dev \
    libglib2.0-dev \
    screen \
    pax \
    gzip \
    language-pack-en \
    libtool-bin \
    cpio \
    lib32z1 \
    lsb-release \
    vim-common \
    libgtk2.0-dev \
    libstdc++6:i386 \
    libtinfo5 \
    device-tree-compiler \
    bc \
    expect


# Ensure the current user can use sudo without password
sudo echo "$(whoami) ALL=(ALL) NOPASSWD:ALL" | sudo tee -a /etc/sudoers

# Add Petalinux settings to bashrc
sudo echo "source /opt/petalinux/settings.sh" >> ~/.bashrc

# Install Petalinux (Assumes the installer is in the same directory as this script)
chmod a+x ./petalinux-v${version}-final-installer.run
./petalinux-v${version}-final-installer.run --dir /opt/petalinux --skip_license
