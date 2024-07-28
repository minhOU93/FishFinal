#!/bin/sh
#SLN: All one needs to do to "CLEAN" a module is delete the cwin64 folder and rerun the
#"BUILD 64 BIT MSVC 2022 with CMAKE GUI.bat" file. So that's what this script does.

export PATH=/c/Program\ Files/Git:$PATH
#We spawn a different git-bash for each repo and run them all in parallel
#Reference URL to get the desired window behavior:
#https://stackoverflow.com/questions/51729777/git-for-windows-difference-between-git-bin-bash-exe-and-git-usr-bin-bash-exe
#https://mintty.github.io/mintty.1.html

cmake --build ./cwin64 --target GTest --config Release
#ctest -C Release ###can run this command which is built into CMake, but doesn't have cool colors.
cd cwin64/GTest
./Release/GTest.exe
read -e  ###Keeps the window open after the test completes so you can see the output
