#!/bin/sh

../template/rasterisation-hat.sh 'rag-doll' '--export-area-page' 'rag-doll-2'

# see ../brain-slug/make-hat.sh
#TMP=`mktemp --suffix='.svg'`
TMP='tmp.svg'
cp rag-doll.svg $TMP
# show 4 holes flat (sew-through) buttons eyes
perl -0777 -i -pe 's/(inkscape:label="hat4"\n[^"]*)style="display:none"/$1style="display:inline"/igs' $TMP
# hide 2 holes flat (sew-through) buttons eyes
perl -0777 -i -pe 's/(inkscape:label="hat2"\n[^"]*)style="display:inline"/$1style="display:none"/igs' $TMP
../template/rasterisation-hat.sh ${TMP%.*} '--export-area-page' 'rag-doll-4'
rm $TMP
