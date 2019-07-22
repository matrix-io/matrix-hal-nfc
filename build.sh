#! /bin/bash
mkdir -p build && cd build && cmake ../src && sudo make install -j4 && cd .. && sudo ldconfig && bash rebuild_examples.sh