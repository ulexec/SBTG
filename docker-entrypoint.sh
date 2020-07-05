#!/bin/sh

while true; do
	make
	./sbtg ./test
	./output.elf
	sleep 1

	if ./output.elf | grep -q 'Hello World'; then
	   break
	fi
done
