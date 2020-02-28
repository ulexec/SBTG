all:
	gcc -m32 -static sbtg.c -o sbtg
	gcc -m32 test.c -o test

clean:
	rm sbtg test output.elf
