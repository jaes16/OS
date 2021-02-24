#!/bin/bash

FULL_PATH=$1
REL_PATH=$2

/bin/bash $FULL_PATH/system/mk.sh $FULL_PATH $REL_PATH
/bin/bash $FULL_PATH/lib/mk.sh $FULL_PATH $REL_PATH
