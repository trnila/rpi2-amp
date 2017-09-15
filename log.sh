#!/bin/sh

log_addr=$(objdump -t app.elf  | grep _log | cut -f 1 -d ' ')

while true; do
	clear
	./tools/logcat "0x$log_addr"
	sleep 0.5
done
