#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
dirs|tr ' ' '\n'|tac|$DIR/../bcmenu_run
cd $(bcmenu_result)
