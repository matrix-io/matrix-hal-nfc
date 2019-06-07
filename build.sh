#! /bin/bash
mkdir -p build
cd build
cmake ..
sudo make install -j4
