#!/bin/sh

# Copyright (C) 2012 Roman V. Prikhodchenko
# Author: Roman V. Prikhodchenko <chujoii@gmail.com>
#
#   hat-converter-1-to-2-row.sh is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   hat-converter-1-to-2-row.sh is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with hat-converter-1-to-2-row.sh.  If not, see <http://www.gnu.org/licenses/>.

convert $1 -crop 32x512 tile_%d.png
mv $1 original_$1
convert tile_* +append $1
rm tile*png
# mkdir -p ~/.hedgewars/Data/Graphics/Hats
# cp $1 ~/.hedgewars/Data/Graphics/Hats
