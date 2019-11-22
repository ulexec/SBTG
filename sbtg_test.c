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
#define REG_EAX		0
#define REG_ECX		1
#define REG_EDX		2
#define REG_EBX		3
#define REG_ESP		4
#define REG_EBP		5
#define REG_ESI		6
#define REG_EDI		7

#define OP_PUSHAD	0x60
#define OP_PUSH		0x50
#define OP_PUSH_DREG	0x30ff
#define OP_POP          0x58
#define OP_POP_DREG	0x8f
#define OP_NOP		0x90
#define OP_XOR_REG_REG  0xc031	
#define OP_MOV_REG_REG  0xc089
#define OP_MOV_REG_DREG 0x008b
#define OP_MOV_DREG_REG 0x0089
#define OP_ENTER	0x000000c8
#define OP_LEAVE	0xc9
#define OP_BUILD2(op, dest, src) OP_SRC(op, src) | OP_DST(op, dest)
#define OP_SRC(x, y)	x | y << 11	
#define OP_DST(x, y)	x | y << 8	

void pushad_0 (uint8_t *, uint32_t *);
void pushad_1 (uint8_t *, uint32_t *);
void prelude_0 (uint8_t *, uint32_t *);
void prelude_1 (uint8_t *, uint32_t *);
void mov_reg_dreg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_reg_dreg_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_dreg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_dreg_reg_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void Sbtg_XOR (uint8_t *, size_t, uint8_t *, size_t, uint32_t *);
void (*pushad_variants_arr[])(uint8_t *, uint32_t *) = {pushad_0, pushad_1};
void (*prelude_variants_arr[])(uint8_t *, uint32_t *) = {prelude_0, prelude_1};
void (*mov_reg_dreg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_reg_dreg_0, mov_reg_dreg_1};
void (*mov_dreg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_dreg_reg_0, mov_dreg_reg_1};
void (*cipher_types_arr[])(uint8_t *, uint32_t, uint8_t *, uint32_t, uint32_t *) = {Sbtg_XOR};

void gensecuence(uint32_t *array, size_t size){
    int x;
    int count;
    int i=0;

    srand(time(NULL));

    for (count = 0; count < size; count++){
        array[count] = rand() % size;
    }

    while (i < size){
        int r = rand() % size;

        for (x = 0; x < i; x++) {
            if (array[x] == r) {
            	break;
            }
        }
        if( x == i){
            array[i++] = r;
        }
    }
}

uint32_t genrand(uint32_t limit) {
  int c, n = 0;
  srand(time(NULL) - rand());

  for (c = 0; c < rand() % 2; c++) {
    n += rand();
  }
  return n % limit;
}

void pushad_0 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint8_t*)(decryptor_buff + *offset) = OP_PUSHAD;
	*offset += 1;
}

void pushad_1 (uint8_t *decryptor_buff, uint32_t *offset) {
	for (int i = 0; i < 8; i++) {
		*(uint8_t*)(decryptor_buff + *offset) = OP_PUSH + i;
		*offset += 1;
	}	
}

void prelude_0 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint32_t*)(decryptor_buff + *offset) = OP_ENTER;
	*offset += 4;	
}

void prelude_1 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_PUSH | REG_EBP);
	*offset += 1;
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_REG_REG, REG_EBP, REG_ESP);
        *offset += 2;	
}

void mov_reg_dreg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_PUSH_DREG, reg1);
	*offset += 2;
	*(uint8_t*)(decryptor_buff + *offset) = (OP_POP | reg2);
	*offset += 1;

}

void mov_reg_dreg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_REG_DREG, reg1, reg2);
	*offset += 2;
}

void mov_dreg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_PUSH | reg1);
	*offset += 1;
	*(uint8_t*)(decryptor_buff + *offset) = OP_POP_DREG;
	*offset += 1;
	*(uint8_t*)(decryptor_buff + *offset) = reg2;
	*offset += 1;
}

void mov_dreg_reg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_REG_DREG, reg1, reg2);
	*offset += 2;
}


void Sbtg_XOR (uint8_t *decryptor_buff, size_t decryptor_buff_size, uint8_t *target_buff, size_t target_buff_size, uint32_t *code_offset) {
	void (*generic_fptr)(uint8_t *, uint32_t *);
	void (*mov_fptr)(uint8_t *, uint32_t *, uint32_t, uint32_t);
	int vregs[4];
	int idx;
	int loop_start;
	
	gensecuence(vregs, sizeof(vregs)/sizeof(uint32_t));
	
	*(uint16_t*)(decryptor_buff + *code_offset) = OP_SRC(0x458b, vregs[0]);		
	*code_offset += 2;
	*(uint8_t*)(decryptor_buff + *code_offset) = 0x08;				// MOV REG1, ARG1
	*code_offset += 1;

	*(uint16_t*)(decryptor_buff + *code_offset) = OP_SRC(0x458b, vregs[1]);
	*code_offset += 2;
	*(uint8_t*)(decryptor_buff + *code_offset) = 0x0C;				// MOV REG2, ARG2
	*code_offset += 1;

	*(uint8_t*)(decryptor_buff + *code_offset) = 0xb8 + vregs[2];			// MOV REG3, KEY
	*code_offset += 1;
	*(uint32_t*)(decryptor_buff + *code_offset) = genrand(0xff) + 1;
	*code_offset += 4;
	
	loop_start = *code_offset;
	idx = genrand(sizeof(prelude_variants_arr)/sizeof(uintptr_t));			
	mov_fptr = mov_reg_dreg_variants_arr[idx];
	(*mov_fptr)(decryptor_buff, code_offset, vregs[0], vregs[3]); 			// MOV REG4, [REG1]
	
	*(uint16_t*)(decryptor_buff + *code_offset) = OP_BUILD2(OP_XOR_REG_REG, vregs[3], vregs[2]);			// XOR REG3, KEY
	*code_offset += 2;
	idx = genrand(sizeof(prelude_variants_arr)/sizeof(uintptr_t));			
	mov_fptr = mov_dreg_reg_variants_arr[idx];
	(*mov_fptr)(decryptor_buff, code_offset, vregs[3], vregs[0]); 			// MOV REG4, [REG1]
	
	*(uint8_t*)(decryptor_buff + *code_offset) = 0xcc;

}

void Sbtg(uint8_t *decryptor_buff, size_t decryptor_buff_size, uint8_t *target_buff, size_t target_buff_size, uint32_t flags) {
	uint32_t code_offset = 0;
	void (*generic_fptr)(uint8_t *, uint32_t *);
	int idx;

	memset(decryptor_buff, OP_NOP, decryptor_buff_size);

	idx = genrand(sizeof(prelude_variants_arr)/sizeof(uintptr_t));			
	generic_fptr = prelude_variants_arr[idx];
	(*generic_fptr)(decryptor_buff, &code_offset); 				// PRELUDE
	
	if (flags & CONF_PRESERVE_REGISTERS) {
		idx = genrand(sizeof(pushad_variants_arr)/sizeof(uintptr_t));			
		generic_fptr = pushad_variants_arr[idx];
		(*generic_fptr)(decryptor_buff, &code_offset);
	}

	if (flags & CONF_RANDOM_CIPHER) {
		idx = genrand(sizeof(cipher_types_arr)/sizeof(uintptr_t));
		(*cipher_types_arr[idx])(decryptor_buff, decryptor_buff_size, target_buff, target_buff_size, &code_offset);
	} else {
		Sbtg_XOR(decryptor_buff, decryptor_buff_size, target_buff, target_buff_size, &code_offset);
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
