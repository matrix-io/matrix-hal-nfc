#! /bin/bash
sudo rm -rf build
mkdir build
cd build
cmake ..
sudo make install -j4
cd ..
bash rebuild_examples.sh