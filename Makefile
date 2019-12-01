all:
	gcc -m32 -N -static -fPIE sbtg_test.c -o sbtg_test
	gcc -m32 test.c -o test

clean:
	rm sbtg_test test
