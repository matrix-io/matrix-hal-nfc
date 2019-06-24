#! /bin/bash
rm -rf build/examples
mkdir -p build/examples
cd build/examples
cmake ../../examples
make -j4