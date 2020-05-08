#!/bin/sh

# usage:
# for convert file 'sauce-boat.svg'
# with layers '--export-id-only --export-id=layer2'
# to file 'sauce-boat_silver.png' :
# ./rasterisation-hat.sh 'sauce-boat' '--export-id-only --export-id=layer2' 'silver'
#
# for convert svg to png, and create animated gif
# add any argument to script (for example "a"):
# ./rasterisation-hat.sh 'sauce-boat' '--export-id-only --export-id=layer2' 'silver' a

SRC=$1
EXPORT_ONLY=$2
OUT=$3

inkscape --vacuum-defs $SRC.svg
sed -r -i -e 's/xlink:href="[^"]*templateol3.png"/xlink:href="..\/template\/templateol3.png"/g' $SRC.svg
sed -r -i -e 's/sodipodi:absref="[^"]*templateol3.png"/sodipodi:absref="\/home\/chujoii\/project\/game\/hw\/share\/hedgewars\/Data\/Graphics\/Hats\/template\/templateol3.png"/g' $SRC.svg


inkscape --file=$SRC.svg --export-area-page $EXPORT_ONLY --export-png=$OUT".png"
if [ ! -z "$4" ]; then
    ../template/animate-hedgehog.sh ../template/Idle.png $OUT".png"
fi
../template/hat-converter-1-to-2-row.sh $OUT".png"
mv  $OUT".png" ../
