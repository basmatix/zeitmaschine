#!/bin/sh

# this script generates a folder generated/ with one cpp file for
# each .h file found in ../.. containing just one line of code
# including that header file.
#
# together with an extra cmake project with the only purpose to
# bring all these files together unmet header dependencies
# can be identified by trying to compile all these files.
#
# additionally unnessesary compile time can be identified by compiling
# this extra project since no unneeded header files should be included
# in these files

# delete generated files
rm -rf generated
mkdir  generated

# list all header files in cpp files
for i in `find ../../ -iname *.h | grep -v "\/submodules\/" | sort | uniq `
do 
    echo $i 
    
    #we include twice to check proper recursive inclusion avoidance, too
    echo "#include \"../$i\"" >  generated/`basename $i`.cpp
    echo "#include \"../$i\"" >> generated/`basename $i`.cpp
done

# delete special cases
# NOTE: this should not be a tool to get rid of problems
#       please solve the root cause of the problems

##rm generated/ysboxplugin.h.cpp

echo
echo here all files should be preceded by a '1' - otherwise its duplicated
# warn of ambigeous file names
for i in `find ../../ -iname *.h | grep -v "\/submodules\/"`; do echo `basename $i`; done | sort | uniq -c | sort -g | tail

