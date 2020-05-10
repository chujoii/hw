#!/bin/sh

../template/rasterisation-hat.sh 'rag-doll' '--export-area-page' 'rag-doll-2'

# see ../brain-slug/make-hat.sh
cp rag-doll.svg tmp.svg
# show 4 holes flat (sew-through) buttons eyes
perl -0777 -i -pe 's/(inkscape:label="hat4"\n[^"]*)style="display:none"/$1style="display:inline"/igs' tmp.svg
# hide 2 holes flat (sew-through) buttons eyes
perl -0777 -i -pe 's/(inkscape:label="hat2"\n[^"]*)style="display:inline"/$1style="display:none"/igs' tmp.svg
../template/rasterisation-hat.sh 'tmp' '--export-area-page' 'rag-doll-4'
