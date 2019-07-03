#! /bin/bash
sudo rm -rf build && mkdir -p build && cd build && cmake .. && sudo make install -j4 && cd .. && bash rebuild_examples.sh