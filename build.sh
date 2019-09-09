#! /bin/bash
mkdir -p build
cd build
cmake ../src
sudo make install -j4
cd .. && sudo ldconfig
bash rebuild_examples.sh
printf "\n****************************\nMATRIX HAL NFC is Installed! \n****************************\n"
