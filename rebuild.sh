#! /bin/bash
sudo rm -rf build
mkdir build
cd build
cmake ..
sudo make install -j4
