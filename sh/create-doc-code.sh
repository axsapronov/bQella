#!/bin/sh

NAME_OF_PROJECT=bQella

DOC_NAME=$NAME_OF_PROJECT-documentation

echo Create documenation for code
# translate to hindi
#mv latex/Makefile ./
cd ../documentation
#rm -rf latex
doxygen
cd latex
rm Makefile
cp ../Makefile ./
mkdir output
mv refman.tex ./output/$DOC_NAME.tex
make
mv $DOC_NAME.* ./output/
cd output
mv $DOC_NAME.pdf ../../

cd ../../../
git checkout documentation/latex/
