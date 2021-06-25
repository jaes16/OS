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
/bin/bash ${FULL_PATH}/ata/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/pci/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/fat_fs/mk.sh "${FULL_PATH}" "${REL_PATH}"
/bin/bash ${FULL_PATH}/terminal/mk.sh "${FULL_PATH}" "${REL_PATH}"
