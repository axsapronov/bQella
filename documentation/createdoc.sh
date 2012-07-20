#!/bin/sh

NAME_OF_DOCUMENTATION=bQella-documentation
cd ./latex
mkdir output
mv refman.tex $NAME_OF_DOCUMENTATION.tex
make
mv $NAME_OF_DOCUMENTATION.* ./output/
cd output
mv $NAME_OF_DOCUMENTATION.pdf ../../
