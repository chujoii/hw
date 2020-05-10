#!/bin/sh

../template/rasterisation-hat.sh 'shaggy-yeti' '--export-area-page' 'ShaggyYeti'

# see ../brain-slug/make-hat.sh
cp shaggy-yeti.svg tmp.svg
# show beak
perl -0777 -i -pe 's/(inkscape:label="beak"\n[^"]*)style="display:none"/$1style="display:inline"/igs' tmp.svg
# yellow filter
perl -0777 -i -pe 's/(inkscape:label="yellow filter\. Layers: Blend mode: Multiply \(enable-background\)"\n[^"]*)style="display:none/$1style="display:inline/igs' tmp.svg
# hide mouth
perl -0777 -i -pe 's/(inkscape:label="mouth"\n[^"]*)style="display:inline"/$1style="display:none"/igs' tmp.svg
../template/rasterisation-hat.sh 'tmp' '--export-area-page' 'zoo_chicken'
