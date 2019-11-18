#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define PAGE_SIZE	0x1000
#define CONF_PRESERVE_REGISTERS	0b00000001
#define CONF_RANDOM_CIPHER	0b00000010

#define OP_PUSHAD	0x60
#define OP_PUSH		0x50
#define OP_NOP		0x90
#define OP_MOV_REG_REG  0b1000100111000000
#define OP_MOV_REG_DREG 0b1000101100000000
#define OP_MOV_DREG_REG 0b1000100100000000

void pushad_0 (uint8_t *, uint32_t *);
void pushad_1 (uint8_t *, uint32_t *);
void Sbtg_RC4 (uint8_t *, size_t, uint8_t *, size_t, uint32_t *);
void (*pushad_variants_arr[])(uint8_t *, uint32_t *) = {pushad_0, pushad_1};
void (*cipher_types_arr[])(uint8_t *, uint32_t, uint8_t *, uint32_t, uint32_t *) = {Sbtg_RC4};

uint32_t genrand(uint32_t limit) {
  int c, n = 0;
  struct timespec seed;
  
  srand(time(NULL));

  for (c = 0; c < 10; c++) {
    n += rand();
  }
  return n % limit;
}

void pushad_0 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint8_t*)decryptor_buff = OP_PUSHAD;
	*offset+=1;
}

void pushad_1 (uint8_t *decryptor_buff, uint32_t *offset) {
	for (int i = 0; i < 8; i++) {
		*(uint8_t*)(decryptor_buff + *offset) = OP_PUSH + i;
		*offset+=1;
	}	
}

void Sbtg_RC4 (uint8_t *decryptor_buff, size_t decryptor_buff_size, uint8_t *target_buff, size_t target_buff_size, uint32_t *code_offset) {

}

void Sbtg(uint8_t *decryptor_buff, size_t decryptor_buff_size, uint8_t *target_buff, size_t target_buff_size, uint32_t flags) {
	uint32_t code_offset = 0;
	void (*generic_fptr)(uint8_t *, uint32_t *);
	int idx;

	memset(decryptor_buff, OP_NOP, decryptor_buff_size);
	
	if (flags & CONF_PRESERVE_REGISTERS) {
		idx = genrand(sizeof(pushad_variants_arr)/sizeof(uintptr_t));			
		generic_fptr = pushad_variants_arr[idx];
		(*generic_fptr)(decryptor_buff, &code_offset);
	}
	
	if (flags & CONF_RANDOM_CIPHER) {
		idx = genrand(sizeof(cipher_types_arr)/sizeof(uintptr_t));
		(*cipher_types_arr[idx])(decryptor_buff, decryptor_buff_size, target_buff, target_buff_size, &code_offset);
	} else {
		Sbtg_RC4(decryptor_buff, decryptor_buff_size, target_buff, target_buff_size, &code_offset);
	}

	/*
	if (flags & CONF_PRESERVE_REGISTERS) {
		idx = genrand(sizeof(popad_variants_arr)/sizeof(uintptr_t));			
		generic_fptr = popad_variants_arr[idx];
		(*generic_fptr)(decryptor_buff, &code_offset);
	}*/
		
}

bool alloc_file(int *fd, struct stat *st, const char *filename, uint8_t **buf) { 
	if ((*fd = open(filename, O_RDWR)) < 0) {
       		return false;
   	}
               
  	if (fstat(*fd, st) < 0) {
       		return false;
   	}
                   
   	if ((*buf = mmap(NULL, st->st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, *fd, 0)) == MAP_FAILED) {
       		return false;
   	}   
   	return true;
}

int main(int argc, char **argv) {
	uint8_t * decryptor_buff;
	uint8_t * target_buff;
	uint32_t flags;
	size_t target_buff_size;
	size_t decryptor_buff_size;
	void (*decryptor_entry)(void);
	struct stat st;
	uint8_t *buf;
	int fd;

	if (!alloc_file(&fd, &st, argv[1], &buf)) {
       		printf("[+] Usage: %s <target binary>\n", argv[0]);
       		return -1;
   	}
	target_buff_size = st.st_size;

	decryptor_buff = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANON|MAP_PRIVATE, -1, 0);
	if (decryptor_buff == MAP_FAILED) {
		puts("[-] Failed to allocate memory for decryptor");
		return -1;
	}	
	decryptor_buff_size = PAGE_SIZE;
	flags = 0b00000111;

 	Sbtg(decryptor_buff, decryptor_buff_size, target_buff, target_buff_size, flags);
	
	decryptor_entry = (void (*)(void))decryptor_buff;
	(*decryptor_entry)();

	return 0;	
} 
