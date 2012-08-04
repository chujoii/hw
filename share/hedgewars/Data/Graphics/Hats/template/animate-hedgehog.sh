#!/bin/sh

# Copyright (C) 2012 Roman V. Prikhodchenko
# Author: Roman V. Prikhodchenko <chujoii@gmail.com>
#
#   animate-hedgehog is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   animate-hedgehog is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with animate-hedgehog.  If not, see <http://www.gnu.org/licenses/>.



convert $1 -crop 32x32 +repage tile_%02d.png  # repage remove or set offset (+ or -)  ; strange, but "-repage 32x32+0+4" not used when composite
convert $2 -crop 32x32 +repage hat_tile_%02d.png  # repage remove or set offset (+ or -)

rm *tile_19.png *tile_2?.png *tile_3?.png



for i in tile*.png
do
  echo $i
  convert -size 32x36 xc:none\
      $i     -geometry +0+4 -composite \
      hat_$i -geometry +0+0 -composite \
      result_$i
done



convert -delay 20 -dispose Background -loop 0 result_tile*.png animated.gif

rm *tile*png

