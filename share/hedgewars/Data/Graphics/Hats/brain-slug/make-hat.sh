#!/bin/sh

# strange, but in Inkscape 0.92 export-id for many id`s not work
# inkscape --file=brain-slug.svg --export-area-page --export-id-only --export-id='layer2 layer4' --export-png=scif_BrainSlug.png
# ... --export-id=layer2\ layer4 ...
# ... --export-id="layer2,layer4" ...
# ... --export-id="layer2;layer4" ...
# always error:
# Object with id="layer2;layer4" was not found in the document. Nothing exported.
#
# if select different key
# ... --export-id=layer2 --export-id=layer4 ...
# Inkscape export only last selected
#
# fixme:
#../template/rasterisation-hat.sh 'brain-slug' '--export-id-only --export-id="layer2;layer4"' 'scif_BrainSlug'
#../template/rasterisation-hat.sh 'brain-slug' '--export-id-only --export-id="layer2;layer3;layer4"' 'scif_BrainSlug2'



# for export hat automation, we have to do the sunrise manually:
#
# -0777 causes perl to slurp files whole https://perldoc.perl.org/perlrun.html
#
# close mouth (display:none)
cp brain-slug.svg tmp.svg
perl -0777 -i -pe 's/(inkscape:label="mouth"\n[^"]*)style="display:inline"/$1style="display:none"/igs' tmp.svg
../template/rasterisation-hat.sh 'tmp' '' 'scif_BrainSlug'

# open mouth
../template/rasterisation-hat.sh 'brain-slug' '' 'scif_BrainSlug2'
