#!/bin/sh
echo "Create help doc for progs"
NAME_OF_PROJECT=bQella

DOC_NAME=$NAME_OF_PROJECT-doc
#OUTPUT_PATH=help-doc

cd ../resources/doc
qhelpgenerator $DOC_NAME.qhp -o $DOC_NAME.qch
#rm -rf ./$OUTPUT_PATH/
#mkdir $OUTPUT_PATH

qcollectiongenerator $DOC_NAME.qhcp -o $DOC_NAME.qhc
#mv $DOC_NAME.qhc ./$OUTPUT_PATH/
rm /home/files/Develop/git/bQella/next/bqella-build-desktop/build/bin/doc/$DOC_NAME.qhc
rm /home/files/Develop/git/bQella/next/bqella-build-desktop/build/bin/doc/$DOC_NAME.qch
cp $DOC_NAME.qhc ../../bin/doc/
cp $DOC_NAME.qch ../../bin/doc/
mv $DOC_NAME.qhc /home/files/Develop/git/bQella/next/bqella-build-desktop/build/bin/doc/
mv $DOC_NAME.qch /home/files/Develop/git/bQella/next/bqella-build-desktop/build/bin/doc/

#rm $DOC_NME.qch
