#!/bin/bash



echo "cloning soundio\n"
 
git clone https://github.com/andrewrk/libsoundio.git
cd libsoundio
mkdir build
cd build  

echo "cmake command running ===>\n"

cmake ..

echo "make command running ===>\n"

make

echo "installing ===>\n"

make install

echo "copying object files ===>\n"

cp ./libsoundio.so.2 /usr/lib/

cp ./libsoundio.so.2 /lib/

echo "you are good to go !\n\n"
