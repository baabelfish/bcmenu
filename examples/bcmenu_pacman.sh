#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
package-query -Af"%r/%n %v %d" -s $1 | awk '{ printf "%-30s",$1; printf "%-10s",$2; $1=$2=""; print }'|$DIR/../bcmenu
