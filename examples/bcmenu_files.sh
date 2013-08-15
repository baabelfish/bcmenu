#!/bin/bash
IGNORE='.git'
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
find $1|grep -v "$IGNORE"|awk '{ print length($0), $0|"sort -n" }'|cut -f2- -d' '|$DIR/../bcmenu_run
