#!/bin/sh

../template/rasterisation-hat.sh 'dragon' '--export-area-page' 'Dragon'




# see ../brain-slug/make-hat.sh
cp dragon.svg tmp.svg
# show laminaria (shifted dragon whiskers)
perl -0777 -i -pe 's/style="display:none"(\n[^"]*inkscape:label="laminaria")/style="display:inline"$1/igs' tmp.svg
# hide dragon hat
perl -0777 -i -pe 's/style="display:inline"(\n[^"]*inkscape:label="hat")/style="display:none"$1/igs' tmp.svg
# hide dragon whiskers
perl -0777 -i -pe 's/(inkscape:label="dragon-whiskers"\n[^"]*)style="display:inline"/$1style="display:none"/igs' tmp.svg
../template/rasterisation-hat.sh 'tmp' '--export-area-page' 'Laminaria'
