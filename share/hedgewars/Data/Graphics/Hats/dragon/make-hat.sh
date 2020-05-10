#!/bin/sh

../template/rasterisation-hat.sh 'dragon' '--export-area-page' 'Dragon'




# see ../brain-slug/make-hat.sh
#TMP=`mktemp --suffix='.svg'`
TMP='tmp.svg'
cp dragon.svg $TMP
# show laminaria (shifted dragon whiskers)
perl -0777 -i -pe 's/style="display:none"(\n[^"]*inkscape:label="laminaria")/style="display:inline"$1/igs' $TMP
# hide dragon hat
perl -0777 -i -pe 's/style="display:inline"(\n[^"]*inkscape:label="hat")/style="display:none"$1/igs' $TMP
# hide dragon whiskers
perl -0777 -i -pe 's/(inkscape:label="dragon-whiskers"\n[^"]*)style="display:inline"/$1style="display:none"/igs' $TMP
../template/rasterisation-hat.sh ${TMP%.*} '--export-area-page' 'Laminaria'
rm $TMP
