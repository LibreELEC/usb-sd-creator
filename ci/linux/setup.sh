#!/bin/sh

sudo add-apt-repository universe -y
sudo apt update -y
sudo apt install -y cmake \
  gcc \
  g++ \
  p7zip-full \
  patchelf \
  libgl1-mesa-dev \
  libxkbcommon-x11-0 \
  libxkbcommon-dev \
  libdbus-1-dev \
  zlib1g-dev

sudo wget -q -c https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O /usr/local/bin/linuxdeploy
sudo wget -q -c https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O /usr/local/bin/linuxdeploy-plugin-qt
sudo wget -q -c https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage -O /usr/local/bin/linuxdeploy-plugin-appimage
sudo chmod +x /usr/local/bin/linuxdeploy /usr/local/bin/linuxdeploy-plugin-qt /usr/local/bin/linuxdeploy-plugin-appimage

echo "CC=gcc" >> $GITHUB_ENV
echo "CXX=g++" >> $GITHUB_ENV
