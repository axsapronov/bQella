#!/bin/sh

NAME_OF_DOCUMENTATION=bQella-documentation

# translate to hindi
#mv latex/Makefile ./
rm -rf latex
doxygen
cd latex
rm Makefile
cp ../Makefile ./
mkdir output
mv refman.tex ./output/$NAME_OF_DOCUMENTATION.tex
make
mv $NAME_OF_DOCUMENTATION.* ./output/
cd output
mv $NAME_OF_DOCUMENTATION.pdf ../../
