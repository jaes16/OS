#!/bin/bash
LIST[0]="$2/interrupts/idt.s"
LIST[1]="$2/interrupts/idt.c"
LIST[2]="$2/interrupts/irq.s"
LIST[3]="$2/interrupts/irq.c"
LIST[4]="$2/interrupts/isr.s"
LIST[5]="$2/interrupts/isr.c"

echo -n "${LIST[*]}"
echo -n " "
