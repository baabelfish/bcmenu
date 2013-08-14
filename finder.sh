#!/bin/bash
if [[ ! -e ~/.cache/fcf ]]; then
    mkdir -p ~/.cache/fcf
fi
./bcmenu ~/.cache/fcf/temp
