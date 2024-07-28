#!/bin/sh
#SLN: All one needs to do to "CLEAN" a module is delete the cwin64 folder and rerun the
#"BUILD 64 BIT MSVC 2022 with CMAKE GUI.bat" file. So that's what this script does.

export PATH=/c/Program\ Files/Git:$PATH
#We spawn a different git-bash for each repo and run them all in parallel
#Reference URL to get the desired window behavior:
#https://stackoverflow.com/questions/51729777/git-for-windows-difference-between-git-bin-bash-exe-and-git-usr-bin-bash-exe
#https://mintty.github.io/mintty.1.html

echo "Removing previous USR module build and temporary files."
DIR_A="./cwin64"
if [ "$(ls -A $DIR_A)" ]; then
   rm -rf $DIR_A/*
   rm -rf $DIR_A/*.*
else
   echo $DIR_A is already empty, good...
fi


echo Removed old files in cwin64. Now let us make some fresh ones. Mmmmm... Fresh...

start "" /usr/bin/bash -c "./RUN\ CMAKE\ GUI.bat;"
#start cmake-gui -G "Visual Studio 17 2022" -A x64 -S ./src/ -B ./cwin64


#Release attempts to minimize the size of the output binary (make the exe or lib as few bytes as possible)
#cmake --build ./cwin64 --config Release
#Debug builds a release exe/lib which also has debug symbols so a crash will reveal a more helpful callstack / breakpoints can get triggered
#cmake --build ./cwin64 --config RelWithDebInfo

echo Rebuilt your MSVC solution .sln/.vcxproj for your module. You can now double click /cwin64/YourModule.sln to open MSVC 2022.
echo Press ANY key to close this window...
read -e
exit

