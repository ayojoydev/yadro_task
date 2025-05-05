linux:
mkdir build
cd build
cmake ..
cmake --build .
./task path/to/test_file.txt


linux w/ GTest:
mkdir build
cd build
cmake ..
cmake --build .
ctest        

