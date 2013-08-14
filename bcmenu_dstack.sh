#!/bin/bash
dirs|tr ' ' '\n'|tac|bcmenu_run
cd $(bcmenu_result)
