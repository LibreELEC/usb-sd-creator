#!/bin/sh

sudo add-apt-repository universe -y
sudo apt-get update -y -qq
sudo apt-get install -y -qq cmake \
  gcc \
  g++ \
  libgl1-mesa-dev \
  qt6-base-dev \
  qt6-l10n-tools \
  qt6-tools-dev \
  qt6-tools-dev-tools \
  qt6-wayland-dev

echo "CC=gcc" >>$GITHUB_ENV
echo "CXX=g++" >>$GITHUB_ENV
