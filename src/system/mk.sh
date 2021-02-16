#!/bin/bash

FULL_PATH="$1/system"
REL_PATH="$2/system"

/bin/bash ${FULL_PATH}/booter/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/gdt/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/interrupts/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/keyboard/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/mmngr/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/screen/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/timer/mk.sh "${FULL_PATH}" "${REL_PATH}"
#/bin/bash ${FULL_PATH}/dmac/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/ATA/mk.sh "${FULL_PATH}" "${REL_PATH}"
