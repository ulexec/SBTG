#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

extern void SBTG(uint32_t *decrypt_buff, size_t decrypt_buff_size, uint32_t target_buff, size_t target_buff_size);

int main(int argc, char **argv) {
	SBTG(0, 1, 2, 3);
	return 0;	
} 
