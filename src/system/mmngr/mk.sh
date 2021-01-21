#!/bin/bash
LIST[1]="$2/mmngr/mmngr_phys.c"
LIST[2]="$2/mmngr/mmngr_virtual.c"
LIST[3]="$2/mmngr/vmmngr_pde.c"
LIST[4]="$2/mmngr/vmmngr_pte.c"
LIST[5]="$2/mmngr/paging.s"

echo -n "${LIST[*]}"
echo -n " "
