#!/bin/bash

DIR=icon.iconset
mkdir "$DIR"

for i in 16 32 128 256; do
    n=$(( i * 2 ))
    sips -z $i $i "$1" --out "$DIR/icon_${i}x${i}.png"
    sips -z $n $n "$1" --out "$DIR/icon_${i}x${i}@2x.png"
    if [[ $n -eq 512 ]]; then
        sips -z $n $n "$1" --out "$DIR/icon_${n}x${n}.png"
    fi
done
ls -R "$DIR"
iconutil --convert icns --output "$2" "$DIR"
