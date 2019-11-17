all:
	nasm -f elf32 ./SBTG.asm
	gcc -m32 sbtg_test.c SBTG.o -o sbtg_test

clean:
	rm SBTG.o
	rm sbtg_test
