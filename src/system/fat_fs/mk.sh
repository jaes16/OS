#!/bin/bash

LIST[0]="$2/fat_fs/fat_init.c"
LIST[1]="$2/fat_fs/fat_attr.c"
LIST[2]="$2/fat_fs/fat_rd.c"
LIST[3]="$2/fat_fs/fat_mk.c"
LIST[4]="$2/fat_fs/fat_rm.c"


echo -n "${LIST[*]}"
echo -n " "
