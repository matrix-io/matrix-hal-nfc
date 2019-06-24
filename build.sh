#! /bin/bash
mkdir -p build
cd build
cmake ..
sudo make install -j4
cd ..
bash rebuild_examples.sh