#!/bin/sh

../template/rasterisation-hat.sh 'shaggy-yeti' '--export-area-page' 'ShaggyYeti'

# see ../brain-slug/make-hat.sh
#TMP=`mktemp --suffix='.svg'`
TMP='tmp.svg'
cp shaggy-yeti.svg $TMP
# show beak
perl -0777 -i -pe 's/(inkscape:label="beak"\n[^"]*)style="display:none"/$1style="display:inline"/igs' $TMP
# yellow filter
perl -0777 -i -pe 's/(inkscape:label="yellow filter\. Layers: Blend mode: Multiply \(enable-background\)"\n[^"]*)style="display:none/$1style="display:inline/igs' $TMP
# hide mouth
perl -0777 -i -pe 's/(inkscape:label="mouth"\n[^"]*)style="display:inline"/$1style="display:none"/igs' $TMP
../template/rasterisation-hat.sh ${TMP%.*} '--export-area-page' 'zoo_chicken'
rm $TMP
