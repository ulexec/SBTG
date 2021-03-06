#define _GNU_SOURCE
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
#include <stdarg.h>
#include <elf.h>

#define __NR_execveat	  358
#define __NR_memfd_create 356
#define __NR_write			4

#define PAGE_SIZE 		 4096
#define	PAGE_ALIGN(k) 		 (((k)+((PAGE_SIZE)-1))&(~((PAGE_SIZE)-1)))

#define REG_AL_MASK			0x0
#define REG_AH_MASK			0x2000
#define REG_SI_MASK			0x3000
#define REG_DI_MASK			0x3800
#define REG_EAX				0x0
#define REG_ECX				0x1
#define REG_EDX				0x2
#define REG_EBX				0x3
#define REG_ESP				0x4
#define REG_EBP				0x5
#define REG_ESI				0x6
#define REG_EDI				0x7
#define CJMP_NZ_			0x1
#define CJMP_Z_				0x0
#define SIB_MASK			0x4000
#define OP_PUSHAD			0x60
#define OP_PUSH				0x50
#define OP_PUSH_DREG		0x30ff
#define OP_POPAD			0x61
#define OP_POP          	0x58
#define OP_POP_DREG			0x8f
#define OP_NOP				0x90
#define OP_NOT				0xd0f7
#define OP_AND				0xc021
#define OP_DEC				0x48
#define OP_INC				0x40
#define OP_LEA_REG_REG		0x018d
#define OP_DIV_REG			0xf0f7
#define OP_IDIV_REG			0xf8f7
#define OP_ADD_REG_B_IMM	0xc083
#define OP_ADD_REG_IMM		0xc081
#define OP_ADD_REG_REG		0xc001
#define OP_ADD_B_REG_REG	0xc000
#define OP_SUB_REG_IMM		0xe883
#define OP_SUB_REG_REG		0xc029
#define OP_SUB_B_REG_REG	0xc028
#define OP_CMP_REG_IMM		0xf883
#define OP_TEST_REG_REG		0xc085
#define OP_XOR_REG_REG  	0xc031	
#define OP_XOR_B_REG_B_REG  0xc030	
#define OP_MOV_REG_REG  	0xc089
#define OP_MOV_REG_DREG 	0x008b
#define OP_MOV_DREG_REG 	0x0089
#define OP_MOV_B_REG_DREG 	0x008a
#define OP_MOV_B_DREG_REG 	0x0088
#define OP_MOV_REG_IMM 		0x00b8
#define OP_MOVZX_REG_REG 	0xb60f
#define OP_FINIT			0xe3db9b
#define OP_FLD1  			0xe8d9
#define OP_FLDZ 			0xeed9
#define OP_FLDN2  			0xedd9
#define OP_FLDLG2  			0xecd9
#define OP_FLDPI			0xebd9
#define OP_FNSTENV 			0x30d9
#define OP_ENTER			0x000000c8
#define OP_LEAVE			0xc9
#define OP_CJMP_NEAR_IMM 	0x74
#define OP_JMP_NEAR_IMM 	0xe9
#define OP_CALL_NEAR_IMM 	0xe8
#define OP_INT_x80			0x80cd 
#define OP_RET				0xc3
#define OP_BUILD2(op, dest, src) OP_SRC(op, src) | OP_DST(op, dest)
#define OP_SRC(x, y)		x | y << 11	
#define OP_DST(x, y)		x | y << 8	
#define INVOKE_RANDFUNC(arr, ...)	arr[genrand(sizeof(arr)/sizeof(uintptr_t))](__VA_ARGS__)
#define CONF_PRESERVE_REGISTERS	0x1
#define CONF_RC4_CIPHER		0x2
#define CONF_OUTPUT			0x10
#define CONF_RAWSIZE		0x10000
#define CONF_START			0x100
#define CONF_END			0x1000

static uint32_t g_TargetBuff;
static uint32_t g_TargetBuff_size;

void pushad_0 (uint8_t *, uint32_t *);
void pushad_1 (uint8_t *, uint32_t *);
void popad_0 (uint8_t *, uint32_t *);
void popad_1 (uint8_t *, uint32_t *);
void prelude_0 (uint8_t *, uint32_t *);
void prelude_1 (uint8_t *, uint32_t *);
void epiloge_0 (uint8_t *, uint32_t *);
void epiloge_1 (uint8_t *, uint32_t *);
void push_0 (uint8_t *, uint32_t *, uint32_t);
void push_1 (uint8_t *, uint32_t *, uint32_t);
void pop_0 (uint8_t *, uint32_t *, uint32_t);
void pop_1 (uint8_t *, uint32_t *, uint32_t);
void zero_reg_0 (uint8_t *, uint32_t *, uint32_t);
void zero_reg_1 (uint8_t *, uint32_t *, uint32_t);
void zero_reg_2 (uint8_t *, uint32_t *, uint32_t);
void zero_reg_3 (uint8_t *, uint32_t *, uint32_t);
void div_reg_0 (uint8_t *, uint32_t *, uint32_t);
void div_reg_1 (uint8_t *, uint32_t *, uint32_t);
void delta_0 (uint8_t *, uint32_t *, uint32_t, uint32_t*);
void delta_1 (uint8_t *, uint32_t *, uint32_t, uint32_t*);
void dec_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void dec_reg_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void dec_reg_2 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void inc_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void inc_reg_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void inc_reg_2 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void cmp_reg_0_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void cmp_reg_0_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_reg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_reg_reg_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void xor_b_reg_b_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void xor_reg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void xor_reg_reg_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void sub_reg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void sub_b_reg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void sub_reg_imm_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void sub_reg_imm_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void sub_reg_imm_2 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void add_reg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void add_b_reg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void add_b_reg_h_reg_0 (uint8_t *, uint32_t *, uint32_t, uint32_t);
void add_reg_imm_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void add_reg_b_imm_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void add_reg_b_imm_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void add_reg_b_imm_2 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_reg_dreg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_reg_dreg_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_b_dreg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_b_reg_dreg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_h_dreg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_h_reg_dreg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_dreg_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void mov_dreg_reg_1 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void movzx_reg_b_reg_0 (uint8_t *, uint32_t*, uint32_t, uint32_t);
void sbtg_write (uint8_t *, uint32_t *, size_t , uint8_t *);
int sbtg_get_random_str(uint8_t *, size_t );

//void (*cipher_types_arr[])(uint8_t *, uint32_t, uint32_t *) = {Sbtg_XOR};
void (*popad_variants_arr[])(uint8_t *, uint32_t *) = {popad_0, /*popad_1*/};
void (*pushad_variants_arr[])(uint8_t *, uint32_t *) = {pushad_0, /*pushad_1*/};
void (*push_variants_arr[])(uint8_t *, uint32_t *, uint32_t) = {push_0, /*push_1*/};
void (*div_variants_arr[])(uint8_t *, uint32_t *, uint32_t) = {div_reg_0, div_reg_1}; 
void (*delta_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t*) = {delta_0, delta_1};
void (*pop_variants_arr[])(uint8_t *, uint32_t *, uint32_t) = {pop_0, /*pop_1*/}; 
void (*zero_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t) = {zero_reg_0, zero_reg_1, zero_reg_2};
void (*prelude_variants_arr[])(uint8_t *, uint32_t *) = {prelude_0, prelude_1};
void (*epilogue_variants_arr[])(uint8_t *, uint32_t *) = {epiloge_0, epiloge_1};
void (*cmp_reg_0_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {cmp_reg_0_0, cmp_reg_0_1};
void (*dec_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {dec_reg_0, dec_reg_1, dec_reg_2};
void (*inc_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {inc_reg_0, inc_reg_1, inc_reg_2};
void (*xor_reg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {xor_reg_reg_0, xor_reg_reg_1};
void (*xor_b_reg_b_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {xor_b_reg_b_reg_0};
void (*mov_reg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_reg_reg_0, mov_reg_reg_1};
void (*movzx_reg_b_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {movzx_reg_b_reg_0}; // add more
void (*mov_reg_dreg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_reg_dreg_0, mov_reg_dreg_1};
void (*mov_dreg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_dreg_reg_0, mov_dreg_reg_1};
void (*mov_b_dreg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_b_dreg_reg_0}; //add more
void (*mov_b_reg_dreg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_b_reg_dreg_0}; //add more
void (*mov_h_reg_dreg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_h_reg_dreg_0}; //add more
void (*mov_h_dreg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {mov_h_dreg_reg_0}; //add more
void (*sub_reg_imm_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {sub_reg_imm_0, sub_reg_imm_1, sub_reg_imm_2};
void (*sub_reg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {sub_reg_reg_0}; //add more
void (*sub_b_reg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {sub_b_reg_reg_0}; //add more
void (*add_reg_b_imm_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {add_reg_b_imm_0, add_reg_b_imm_1, add_reg_b_imm_2};
void (*add_reg_imm_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {add_reg_imm_0}; //add more
void (*add_reg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {add_reg_reg_0}; //add more
void (*add_b_reg_reg_variants_arr[])(uint8_t *, uint32_t *, uint32_t, uint32_t) = {add_b_reg_reg_0}; //add more

void gensecuence(uint32_t *array, size_t size){
	int x;
    	int count;
    	int i=0;

    	for (count = 0; count < size; count++){
		int val = REG_ESP;
		while (val == REG_ESP) {
	       		val = rand() % size;
		}
        	array[count] = val;
    	}

    	while (i < size){
		int r = REG_ESP;
		while (r == REG_ESP) {
	       		r = rand() % size;
		}

        	for (x = 0; x < i; x++) {
            		if (array[x] == r) {
            			break;
            		}
        	}
        	if (x == i){
            		array[i++] = r;
        	}
    	}
}

uint32_t genrand(uint32_t limit) {
	int c, n = 0;
  	//srand(time(NULL) - rand());

  	for (c = 0; c < rand() % 2; c++) {
    		n += rand();
  	}
  	return n % limit;
}

void div_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_DIV_REG, reg);
	*offset += sizeof(uint16_t);
}

void div_reg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_IDIV_REG, reg);
	*offset += sizeof(uint16_t);
}

void push_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_PUSH | reg);
	*offset += sizeof(uint8_t);
}

void push_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {	
	uint16_t op;

	INVOKE_RANDFUNC(sub_reg_imm_variants_arr, decryptor_buff, offset, REG_ESP, 4);

	switch(reg) {
		case REG_EAX:
			op = 0x0489;
			break;
		case REG_EBX:
			op = 0x1c89; 
			break;
		case REG_ECX:
			op = 0x0c89;
			break;
		case REG_EDX:
			op = 0x1489;
			break;
		case REG_ESI:
			op = 0x3489;
			break;
		case REG_EDI:
			op = 0x3c89;
			break;
		case REG_EBP:
			op = 0x2c89;
			break;
	}
	*(uint16_t*)(decryptor_buff + *offset) = op;
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = 0x24;
	*offset += sizeof(uint8_t);
}

void pop_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_POP | reg);
	*offset += sizeof(uint8_t);
}

void pop_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {	
	uint16_t op;

	switch(reg) {
		case REG_EAX:
			op = 0x048b;
			break;
		case REG_EBX:
			op = 0x1c8b; 
			break;
		case REG_ECX:
			op = 0x0c8b;
			break;
		case REG_EDX:
			op = 0x148b;
			break;
		case REG_ESI:
			op = 0x348b;
			break;
		case REG_EDI:
			op = 0x3c8b;
			break;
		case REG_EBP:
			op = 0x2c8b;
			break;
	}
	*(uint16_t*)(decryptor_buff + *offset) = op;	
	*offset += sizeof(uint16_t);	
	*(uint8_t*)(decryptor_buff + *offset) = 0x24;	
	*offset += sizeof(uint8_t);	

	INVOKE_RANDFUNC(add_reg_b_imm_variants_arr, decryptor_buff, offset, REG_ESP, 4);	
}

void pushad_0 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint8_t*)(decryptor_buff + *offset) = OP_PUSHAD;
	*offset += sizeof(uint8_t);
}

void pushad_1 (uint8_t *decryptor_buff, uint32_t *offset) {
	for (int i = 0; i < 8; i++) {
		INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, i);
	}	
}

void popad_0 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint8_t*)(decryptor_buff + *offset) = OP_POPAD;
	*offset += sizeof(uint8_t);
}

void popad_1 (uint8_t *decryptor_buff, uint32_t *offset) {
	for (int i = 7; i >= 0; i--) {
		INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, i);
	}	
}

void prelude_0 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint32_t*)(decryptor_buff + *offset) = OP_ENTER;
	*offset += sizeof(uint32_t);	
}

void prelude_1 (uint8_t *decryptor_buff, uint32_t *offset) {
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, REG_EBP);
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_REG_REG, REG_EBP, REG_ESP);
        *offset += sizeof(uint16_t);	
}

void epiloge_0 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint8_t*)(decryptor_buff + *offset) = OP_LEAVE;
	*offset += sizeof(uint8_t);	
}

void epiloge_1 (uint8_t *decryptor_buff, uint32_t *offset) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_REG_REG, REG_ESP, REG_EBP);
        *offset += sizeof(uint16_t);	
	*(uint8_t*)(decryptor_buff + *offset) = (OP_POP | REG_EBP);
	*offset += sizeof(uint8_t);
}

void mov_reg_dreg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_PUSH_DREG, reg1);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = (OP_POP | reg2);
	*offset += sizeof(uint8_t);
}

void mov_reg_dreg_disp_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2, int disp) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_REG_DREG | SIB_MASK, reg1, reg2);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = disp;
	*offset += sizeof(uint8_t);
}

void mov_reg_imm_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t imm) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_MOV_REG_IMM | reg1);
	*offset += sizeof(uint8_t);
	*(uint32_t*)(decryptor_buff + *offset) = imm;
	*offset += sizeof(uint32_t);
}

void mov_reg_dreg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_REG_DREG, reg1, reg2);
	*offset += sizeof(uint16_t);
}

void mov_h_reg_dreg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_B_REG_DREG|REG_AH_MASK, reg1, reg2);
	*offset += sizeof(uint16_t);
}

void mov_h_dreg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_B_DREG_REG|REG_AH_MASK, reg1, reg2);
	*offset += sizeof(uint16_t);
}


void mov_b_reg_dreg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_B_REG_DREG, reg1, reg2);
	*offset += sizeof(uint16_t);
}

void mov_b_dreg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_B_DREG_REG, reg1, reg2);
	*offset += sizeof(uint16_t);
}

void mov_dreg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_PUSH | reg1);
	*offset += sizeof(uint8_t);
	*(uint8_t*)(decryptor_buff + *offset) = OP_POP_DREG;
	*offset += sizeof(uint8_t);
	*(uint8_t*)(decryptor_buff + *offset) = reg2;
	*offset += sizeof(uint8_t);
}

void mov_dreg_reg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_DREG_REG, reg2, reg1);
	*offset += sizeof(uint16_t);
}

void movzx_reg_b_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {	
	*(uint8_t*)(decryptor_buff + *offset) = 0x0f;
	*offset += sizeof(uint8_t);
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(0xc0b6, reg2, reg1);	
	*offset += sizeof(uint16_t);
}

void mov_reg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {	
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_MOV_REG_REG, reg2, reg1);	
	*offset += sizeof(uint16_t);
}

void mov_reg_reg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_PUSH | reg1);
	*offset += sizeof(uint8_t);
	*(uint8_t*)(decryptor_buff + *offset) = (OP_POP | reg2);
	*offset += sizeof(uint8_t);
}

void xor_b_reg_b_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_XOR_B_REG_B_REG, reg2, reg1);	
	*offset += sizeof(uint16_t);
}

void xor_reg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_XOR_REG_REG, reg2, reg1);	
	*offset += sizeof(uint16_t);
}

void xor_reg_reg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	// A XOR B = (NOT(A AND B)) AND (NOT(NOT A AND NOT B))
	void (*mov_fptr)(uint8_t *, uint32_t *, uint32_t, uint32_t);
	int vregs[4];
	uint32_t zregs[2] = {REG_ESI, REG_EDI};
	uint32_t zregs_order[2];
	int idx;

	gensecuence(vregs, sizeof(vregs)/sizeof(uint32_t));
	gensecuence(zregs_order, sizeof(zregs_order)/sizeof(uint32_t));

	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, zregs[zregs_order[1]]);

	for (int i = 0; i < sizeof(vregs)/sizeof(uint32_t); i++) {
		*(uint8_t*)(decryptor_buff + *offset) = (OP_PUSH | vregs[i]);		
		*offset += sizeof(uint8_t);
	}
	
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, reg1, zregs[zregs_order[0]]);
	
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, reg2, zregs[zregs_order[1]]);
	
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, reg2, vregs[0]);
	
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_AND, vregs[0], zregs[zregs_order[0]]);
	*offset += sizeof(uint16_t);	

	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_NOT, vregs[0]);
	*offset += sizeof(uint16_t);	
	
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, zregs[zregs_order[0]], vregs[1]);

	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_NOT, vregs[1]);
	*offset += sizeof(uint16_t);	
		
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, zregs[zregs_order[1]], vregs[2]);

	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_NOT, vregs[2]);
	*offset += sizeof(uint16_t);	
	
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_AND, vregs[1], vregs[2]);
	*offset += sizeof(uint16_t);	

	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_NOT, vregs[1]);
	*offset += sizeof(uint16_t);	
	
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_AND, vregs[0], vregs[1]);
	*offset += sizeof(uint16_t);	
	
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, vregs[0], zregs[zregs_order[1]]);
	
	for (int i = sizeof(vregs)/sizeof(uint32_t) -1; i >= 0; i--) {
		*(uint8_t*)(decryptor_buff + *offset) = (OP_POP | vregs[i]);		
		*offset += sizeof(uint8_t);
	}	
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, zregs[zregs_order[1]], reg2);
	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, zregs[zregs_order[0]]);
	
}

void dec_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_DEC | reg1);
	*offset += sizeof(uint8_t);
}	

void dec_reg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_ADD_REG_B_IMM, reg1);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = 0xff;
	*offset += sizeof(uint8_t);
}

void dec_reg_2 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_SUB_REG_IMM, reg1);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = 0x01;
	*offset += sizeof(uint8_t);
}

void inc_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint8_t*)(decryptor_buff + *offset) = (OP_INC | reg1);
	*offset += sizeof(uint8_t);
}	

void inc_reg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_SUB_REG_IMM, reg1);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = 0xff;
	*offset += sizeof(uint8_t);
}

void inc_reg_2 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_ADD_REG_B_IMM, reg1);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = 0x01;
	*offset += sizeof(uint8_t);
}

void cmp_reg_0_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t val) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_CMP_REG_IMM, reg1);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = val;
	*offset += sizeof(uint8_t);
}

void cmp_reg_0_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_TEST_REG_REG, reg1, reg1);
	*offset += sizeof(uint16_t);
}

void sub_b_reg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_SUB_B_REG_REG, reg1, reg2);
	*offset += sizeof(uint16_t);

}
void sub_reg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_SUB_REG_REG, reg1, reg2);
	*offset += sizeof(uint16_t);
}

void sub_reg_imm_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t val) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_SUB_REG_IMM, reg1);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = val;
	*offset += sizeof(uint8_t);
}

void sub_reg_imm_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t val) {
	for (int i = 0; i < val; i++) {
		INVOKE_RANDFUNC(dec_reg_variants_arr, decryptor_buff, offset, reg1, 0);
	}
}

void sub_reg_imm_2 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t val) {
	if (val % 2) {
		sub_reg_imm_0 (decryptor_buff, offset, reg1, val);
		
	} else {
		for (int i = 0; i < val/2; i++) {
			INVOKE_RANDFUNC(dec_reg_variants_arr, decryptor_buff, offset, reg1, 0);
		}
		sub_reg_imm_0 (decryptor_buff, offset, reg1, val/2);
	}
}

void add_b_reg_h_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_ADD_B_REG_REG | REG_AH_MASK, reg1, reg2);
	*offset += sizeof(uint16_t);
}

void add_b_reg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_ADD_B_REG_REG, reg1, reg2);
	*offset += sizeof(uint16_t);
}

void add_reg_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t reg2) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_BUILD2(OP_ADD_REG_REG, reg1, reg2);
	*offset += sizeof(uint16_t);
}

void add_reg_imm_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t val) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_ADD_REG_IMM, reg1);
	*offset += sizeof(uint16_t);
	*(uint32_t*)(decryptor_buff + *offset) = val;
	*offset += sizeof(uint32_t);
}

void add_reg_b_imm_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t val) {
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_ADD_REG_B_IMM, reg1);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = val;
	*offset += sizeof(uint8_t);
}

void add_reg_b_imm_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t val) {
	for (int i = 0; i < val; i++) {
		INVOKE_RANDFUNC(inc_reg_variants_arr, decryptor_buff, offset, reg1, 0);
	}
}

void add_reg_b_imm_2 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg1, uint32_t val) {
	if (val % 2 != 0) {
		add_reg_b_imm_0 (decryptor_buff, offset, reg1, val);
		
	} else {
		for (int i = 0; i < val/2; i++) {
			INVOKE_RANDFUNC(inc_reg_variants_arr, decryptor_buff, offset, reg1, 0);
		}
		add_reg_b_imm_0(decryptor_buff, offset, reg1, val/2);
	}
}

void zero_reg_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {	
	mov_reg_imm_0 (decryptor_buff, offset, reg, 0x0);
}

void zero_reg_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {	
	INVOKE_RANDFUNC(xor_reg_reg_variants_arr, decryptor_buff, offset, reg, reg);	
}

void zero_reg_2 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {	
	INVOKE_RANDFUNC(sub_reg_reg_variants_arr, decryptor_buff, offset, reg, reg);	
}

void zero_reg_3 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg) {
	uint32_t randreg = reg;
	while(randreg == reg){
		randreg = genrand(4);
	} 
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, randreg);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, reg, randreg);

	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_NOT, randreg);
	*offset += sizeof(uint16_t);

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, offset, reg, randreg);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, randreg);
}

void cjmp_near_imm_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t dest, int mask) {
	*(uint8_t*)(decryptor_buff + *offset) = OP_CJMP_NEAR_IMM | mask;
	*offset += sizeof(uint8_t);
	*(uint8_t*)(decryptor_buff + *offset) = dest - *offset - sizeof(uint8_t);		
	*offset += sizeof(uint8_t);
}

void Sbtg_XOR (uint8_t *decryptor_buff, size_t decryptor_buff_size, uint32_t *code_offset, uint8_t* key) {
	void (*generic_fptr)(uint8_t *, uint32_t *);
	void (*mov_fptr)(uint8_t *, uint32_t *, uint32_t, uint32_t);
	int idx;
	int vregs[4];
	int loop_start;
	uint32_t ikey = 0;

	for (int i = 0; i < 4; i++){
    	ikey = 10 * ikey + key[i];
	}
	
	gensecuence(vregs, sizeof(vregs)/sizeof(uint32_t));
	
	// MOV REG1, ARG1
	mov_reg_dreg_disp_0 (decryptor_buff, code_offset, REG_EBP, vregs[0], 0x8);

	// MOV REG2, ARG2
	mov_reg_dreg_disp_0 (decryptor_buff, code_offset, REG_EBP, vregs[1], 0xc);

	// MOV REG3, KEY
	mov_reg_imm_0 (decryptor_buff, code_offset, vregs[2], ikey);

	loop_start = *code_offset;
	
	// MOV REG4, [REG1]
	INVOKE_RANDFUNC(mov_reg_dreg_variants_arr, decryptor_buff, code_offset, vregs[0], vregs[3]);
	
	// XOR REG4, REG3
	INVOKE_RANDFUNC(xor_reg_reg_variants_arr, decryptor_buff, code_offset, vregs[2], vregs[3]);
	
	// MOV [REG1], REG4
	INVOKE_RANDFUNC(mov_dreg_reg_variants_arr, decryptor_buff, code_offset, vregs[3], vregs[0]);
	
	// DEC REG2
	INVOKE_RANDFUNC(sub_reg_imm_variants_arr, decryptor_buff, code_offset, vregs[1], 4);

	// INC REG1
	INVOKE_RANDFUNC(add_reg_b_imm_variants_arr, decryptor_buff, code_offset, vregs[0], 4);
	
	// CMP REG2, 0
	INVOKE_RANDFUNC(cmp_reg_0_variants_arr, decryptor_buff, code_offset, vregs[1], 0);
	
	// JNZ IMM
	cjmp_near_imm_0(decryptor_buff, code_offset, loop_start, CJMP_NZ_);
}


void jmp_near_imm_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t dest) {
	*(uint8_t*)(decryptor_buff + *offset) = OP_JMP_NEAR_IMM;
	*offset += sizeof(uint8_t);
	*(uint32_t*)(decryptor_buff + *offset) = dest - *offset - sizeof(uint32_t);		
	*offset += sizeof(uint32_t);

}

void sbtg_memset (uint8_t *decryptor_buff, uint32_t *offset, size_t size, uint8_t value) {
	for(int i = 0; i < size; i++) {
		*(uint8_t*)(decryptor_buff + *offset) = value;
		*offset += sizeof(uint8_t);
	}
} 

void call_near_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t dest){
	*(uint8_t*)(decryptor_buff + *offset) = OP_CALL_NEAR_IMM;
	*offset += sizeof(uint8_t);
	*(uint32_t*)(decryptor_buff + *offset) = dest - *offset - sizeof(uint32_t) + sizeof(uint8_t);		
	*offset += sizeof(uint32_t);	
}

void delta_1 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg, uint32_t *delta) {
	uint16_t finst[] = {OP_FLDZ, OP_FLDPI, OP_FLDN2, OP_FLDLG2};
	int idx;

	for (int i=0; i < genrand(20) + 5; i++) {
		idx = genrand(sizeof(finst)/sizeof(uint16_t));

		*delta = *offset;
		*(uint16_t*)(decryptor_buff + *offset) = finst[idx];
		*offset += sizeof(uint16_t);
	}

	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, idx);
	INVOKE_RANDFUNC(sub_reg_imm_variants_arr, decryptor_buff, offset, REG_ESP, 32);
	
	*(uint16_t*)(decryptor_buff + *offset) = OP_SRC(0x048d, idx);
	*offset += sizeof(uint16_t);
	*(uint8_t*)(decryptor_buff + *offset) = 0x24;
	*offset += sizeof(uint8_t);
	
	*(uint16_t*)(decryptor_buff + *offset) = OP_DST(OP_FNSTENV, idx);
	*offset += sizeof(uint16_t);

	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, REG_ESP, reg);
	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, offset, reg, 12);
	INVOKE_RANDFUNC(mov_reg_dreg_variants_arr, decryptor_buff, offset, reg, reg);
	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, offset, REG_ESP, 32);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, idx);
}

void delta_0 (uint8_t *decryptor_buff, uint32_t *offset, uint32_t reg, uint32_t *delta) {
	call_near_0(decryptor_buff, offset, *offset+sizeof(uint32_t));
	*delta = *offset;
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, reg);	
}

void sbtg_write (uint8_t *decryptor_buff, uint32_t *offset, size_t size, uint8_t *value) {
	for (int i = 0; i < size; i++) {
		*(uint8_t*)(decryptor_buff + *offset) = value[i];
		*offset += sizeof(uint8_t);
	}
}

/*
void sbtg_memfd_cre(uint8_t *decryptor_buff, uint32_t *offset, uint32_t sysn, uint32_t nargs, ...) {
	va_list vl;
  	va_start(vl, nargs);
	uint32_t val;

	uint32_t params[] = {REG_EBX, REG_ECX, REG_EDX, REG_ESI, REG_EDI, REG_EBP};
	
	mov_reg_imm_0 (decryptor_buff, offset, REG_EAX, sysn);

	for (int i = 0; i < nargs; i++) {
		val = va_arg(vl, uint32_t);
		mov_reg_imm_0 (decryptor_buff, offset, params[i], val);
	}
	va_end(vl);

	*(uint16_t*)(decryptor_buff + *offset) = OP_INT_x80;
	*offset += sizeof(uint16_t);
}*/

int sbtg_get_random_str(uint8_t *buff, size_t size) {
	int fd;
	int result;

	if ((fd = open("/dev/urandom", O_RDONLY)) < -1) {
		return -1;
	}

    	if ((result = read(fd, buff, size)) < 0){
		return -1;
    	}
	return 0;
}

void Sbtg_RC4 (uint8_t *decryptor_buff, size_t decryptor_buff_size, uint32_t *code_offset, uint8_t *key) {
	void (*generic_fptr)(uint8_t *, uint32_t *);
	void (*mov_fptr)(uint8_t *, uint32_t *, uint32_t, uint32_t);
	uint32_t idx;
	uint32_t vregs[4];
	uint32_t zregs[2] = {REG_ESI, REG_EDI};
	uint32_t zregs_order[2];
	uint32_t config_order[3];
	uint32_t loop_start;
	uint32_t loop_start1;
	uint32_t S;
	uint32_t K;
	uint32_t delta;
	uint32_t key_offset;
	uint8_t rand_chunk[256];
	
	gensecuence(vregs, sizeof(vregs)/sizeof(uint32_t));
	gensecuence(zregs_order, sizeof(zregs_order)/sizeof(uint32_t));
	gensecuence(config_order, sizeof(config_order)/sizeof(uint32_t));

	INVOKE_RANDFUNC(delta_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], &delta); 

	jmp_near_imm_0(decryptor_buff, code_offset, *code_offset+(256*2)+16+sizeof(uint32_t)+sizeof(uint8_t));

	for (int i=0; i < sizeof(config_order)/sizeof(uint32_t); i++) {
		switch(i) {
			case 0:
				S = *code_offset;
				sbtg_get_random_str(rand_chunk, sizeof(rand_chunk));
				sbtg_write(decryptor_buff, code_offset, sizeof(rand_chunk), rand_chunk);
				break;
			case 1:
				K = *code_offset;
				sbtg_get_random_str(rand_chunk, sizeof(rand_chunk));
				sbtg_write(decryptor_buff, code_offset, sizeof(rand_chunk), rand_chunk);
				break;
			case 2:
				key_offset = *code_offset;
				sbtg_write(decryptor_buff, code_offset, 0x10, key);
				break;
		}
	}
	mov_reg_dreg_disp_0 (decryptor_buff, code_offset, REG_EBP, vregs[1], 0x8); // arg1

	mov_reg_dreg_disp_0 (decryptor_buff, code_offset, REG_EBP, vregs[2], 0xc); // arg2

	for(int i = 1; i < 3; i++) {
		INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, vregs[i]);
	} 

	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], vregs[1]);
	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, code_offset, vregs[1], (S - delta));
	
	mov_reg_imm_0(decryptor_buff, code_offset, vregs[2], 256);

	loop_start = *code_offset;
	
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);

	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[0]);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);

	INVOKE_RANDFUNC(sub_b_reg_reg_variants_arr, decryptor_buff, code_offset, vregs[0], vregs[2]);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, vregs[1], vregs[3]);

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, vregs[3], vregs[0]);
	INVOKE_RANDFUNC(mov_b_dreg_reg_variants_arr, decryptor_buff, code_offset, vregs[3], vregs[0]);

	INVOKE_RANDFUNC(sub_reg_imm_variants_arr, decryptor_buff, code_offset, vregs[2], 1);
	INVOKE_RANDFUNC(cmp_reg_0_variants_arr, decryptor_buff, code_offset, vregs[2], 0);
	
	cjmp_near_imm_0(decryptor_buff, code_offset, loop_start, CJMP_NZ_);

	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[1]);
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[2]);

	loop_start = *code_offset;	
		
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], (K - delta));

	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, vregs[3]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, vregs[0]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, vregs[2]);
	mov_reg_imm_0 (decryptor_buff, code_offset, REG_ECX, 0x10);
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, REG_EDX);

	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, REG_ECX, REG_EAX);
	INVOKE_RANDFUNC(div_variants_arr, decryptor_buff, code_offset, REG_ECX);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, REG_EDX, REG_EBP);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, vregs[2]);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, vregs[0]);
	
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, REG_EBP, vregs[1]);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, vregs[3]);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);

	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	
	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], (key_offset - delta));

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], vregs[1]);
	
	INVOKE_RANDFUNC(mov_b_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], vregs[3]);	
	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);

	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], (K - delta));

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], vregs[2]);

	INVOKE_RANDFUNC(mov_b_dreg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], vregs[3]);
	
	INVOKE_RANDFUNC(inc_reg_variants_arr, decryptor_buff, code_offset, vregs[1], 0);		
	INVOKE_RANDFUNC(inc_reg_variants_arr, decryptor_buff, code_offset, vregs[2], 0);		
	
	mov_reg_imm_0 (decryptor_buff, code_offset, REG_EBP, 0x100);
	
	INVOKE_RANDFUNC(sub_reg_reg_variants_arr, decryptor_buff, code_offset, REG_EBP, vregs[2]);			
	INVOKE_RANDFUNC(cmp_reg_0_variants_arr, decryptor_buff, code_offset, REG_EBP, 0);
	
	cjmp_near_imm_0(decryptor_buff, code_offset, loop_start, CJMP_NZ_);

	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);

	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[0]);
	
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[3]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, vregs[3]);
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[1]);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, vregs[3]);

	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, REG_EBP);
	
	loop_start = *code_offset;

	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);

	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], (S - delta));

	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], (K - delta));

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], vregs[0]);
	
	INVOKE_RANDFUNC(mov_b_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], vregs[2]);
	
	INVOKE_RANDFUNC(add_b_reg_reg_variants_arr, decryptor_buff, code_offset, vregs[3], vregs[2]);
	
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	
	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[0]);
	
	INVOKE_RANDFUNC(mov_b_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);

	INVOKE_RANDFUNC(add_b_reg_reg_variants_arr, decryptor_buff, code_offset, vregs[3], vregs[2]);

	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[0]);
	INVOKE_RANDFUNC(mov_b_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);
		
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[3]);
	
	INVOKE_RANDFUNC(mov_h_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);
	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[0]);
	INVOKE_RANDFUNC(mov_h_dreg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);
		
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);

	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[3]);
 	
	INVOKE_RANDFUNC(mov_b_dreg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);
	
	INVOKE_RANDFUNC(inc_reg_variants_arr, decryptor_buff, code_offset, vregs[0], 0);
	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	
	mov_reg_imm_0 (decryptor_buff, code_offset, REG_EBP, 0x100);
	
	INVOKE_RANDFUNC(sub_reg_reg_variants_arr, decryptor_buff, code_offset, REG_EBP, vregs[0]);			

	INVOKE_RANDFUNC(cmp_reg_0_variants_arr, decryptor_buff, code_offset, REG_EBP, 0);
	
	cjmp_near_imm_0(decryptor_buff, code_offset, loop_start, CJMP_NZ_);
	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, vregs[1]); 
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, REG_EBP); 

	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, vregs[1]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, REG_EBP);

	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[0]);
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[3]);
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, code_offset, vregs[2]);

	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, REG_EBP);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, vregs[1]);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]]);

	loop_start = *code_offset;

	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, vregs[1]);
	INVOKE_RANDFUNC(movzx_reg_b_reg_variants_arr, decryptor_buff, code_offset, vregs[1], vregs[0]);
	INVOKE_RANDFUNC(inc_reg_variants_arr, decryptor_buff, code_offset, vregs[1], 0);

	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[0]], zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(add_reg_imm_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], (S - delta));
	
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	
	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[1]); //ecx

	INVOKE_RANDFUNC(mov_h_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);

	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	
	add_b_reg_h_reg_0(decryptor_buff, code_offset, vregs[3], vregs[2]);
	
	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[3]);
	
	INVOKE_RANDFUNC(mov_b_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);
	
	jmp_near_imm_0(decryptor_buff, code_offset, *code_offset+sizeof(uint32_t)+sizeof(uint8_t) + sizeof(uint16_t));
	
	loop_start1 = *code_offset;
	
	cjmp_near_imm_0(decryptor_buff, code_offset, loop_start, CJMP_NZ_);
	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	
	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[1]);
	INVOKE_RANDFUNC(mov_b_dreg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);
	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	
	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[3]);
	INVOKE_RANDFUNC(mov_h_dreg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);

	add_b_reg_h_reg_0(decryptor_buff, code_offset, vregs[2], vregs[2]);
	INVOKE_RANDFUNC(movzx_reg_b_reg_variants_arr, decryptor_buff, code_offset, vregs[2], vregs[2]);

	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	
	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);
	INVOKE_RANDFUNC(mov_h_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[2]);

	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, REG_EBP, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(add_reg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[0]);
	INVOKE_RANDFUNC(mov_b_reg_dreg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[1]);
	
	INVOKE_RANDFUNC(xor_b_reg_b_reg_variants_arr, decryptor_buff, code_offset, vregs[2], vregs[1]);
	INVOKE_RANDFUNC(mov_b_dreg_reg_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]], vregs[1]);
	
	INVOKE_RANDFUNC(inc_reg_variants_arr, decryptor_buff, code_offset, vregs[0], 0);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, zregs[zregs_order[1]]);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, code_offset, vregs[1]);
	
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, code_offset, vregs[1], vregs[2]);

	INVOKE_RANDFUNC(sub_reg_reg_variants_arr, decryptor_buff, code_offset, vregs[2], vregs[0]);			
	
	INVOKE_RANDFUNC(cmp_reg_0_variants_arr, decryptor_buff, code_offset, vregs[2], 0);
	
	cjmp_near_imm_0(decryptor_buff, code_offset, loop_start1, CJMP_NZ_);
}

static inline int _execveat(int dirfd, const char *pathname, char *const argv[], char *const envp[], int flags) {
	return syscall(__NR_execveat, dirfd, pathname, argv, envp, flags);
}

static inline int _memfd_create(const char *name, unsigned int flags) {
	return syscall(__NR_memfd_create, name, flags);
}


	/*
	fd = _memfd_create("", 0);
	write(fd, target_buff, target_buff_size);
	_execveat(fd, "", NULL, NULL, AT_EMPTY_PATH);
	*/

void sbtg_memfd_creat(uint8_t *decryptor_buff, uint32_t *offset) {
	uint32_t vreg = REG_EAX;
	
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, offset, REG_ECX);

	mov_reg_imm_0 (decryptor_buff, offset, vreg,  0x00414141);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, vreg);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, REG_ESP, REG_EBX);

	mov_reg_imm_0 (decryptor_buff, offset, vreg,  __NR_memfd_create);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, vreg);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, REG_EAX);

	*(uint16_t*)(decryptor_buff + *offset) = OP_INT_x80;
	*offset += sizeof(uint16_t);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, REG_EAX);

	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, REG_EAX, REG_EBX);
	mov_reg_imm_0 (decryptor_buff, offset, vreg,  g_TargetBuff);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, vreg, REG_ECX);
	mov_reg_imm_0 (decryptor_buff, offset, vreg,  g_TargetBuff_size);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, vreg, REG_EDX);
	
	mov_reg_imm_0 (decryptor_buff, offset, vreg,  __NR_write);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, vreg);
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, REG_EAX);

	*(uint16_t*)(decryptor_buff + *offset) = OP_INT_x80;
	*offset += sizeof(uint16_t);
	
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, offset, REG_EDX);
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, offset, REG_ESI);	
	INVOKE_RANDFUNC(pop_variants_arr, decryptor_buff, offset, REG_EBX);
	
	INVOKE_RANDFUNC(zero_reg_variants_arr, decryptor_buff, offset, REG_ECX);
	INVOKE_RANDFUNC(push_variants_arr, decryptor_buff, offset, REG_ECX);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, REG_ESP, REG_ECX);
	
	mov_reg_imm_0 (decryptor_buff, offset, vreg,  AT_EMPTY_PATH);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, vreg, REG_EDI);
	mov_reg_imm_0 (decryptor_buff, offset, vreg,  __NR_execveat);
	INVOKE_RANDFUNC(mov_reg_reg_variants_arr, decryptor_buff, offset, vreg, REG_EAX);

	*(uint16_t*)(decryptor_buff + *offset) = OP_INT_x80;
	*offset += sizeof(uint16_t);

	mov_reg_imm_0 (decryptor_buff, offset, REG_EAX,  1);
	*(uint16_t*)(decryptor_buff + *offset) = OP_INT_x80;
	*offset += sizeof(uint16_t);
}

void Sbtg(uint8_t *decryptor_buff, size_t *decryptor_buff_size, uint8_t *key, uint32_t target_buff, uint32_t target_buff_size, uint32_t flags) {
	uint32_t code_offset = 0;
	void (*generic_fptr)(uint8_t *, uint32_t *);
	int idx;

	memset(decryptor_buff, OP_NOP, *decryptor_buff_size);

	if (flags & CONF_START ) {
		uint32_t vreg = genrand(4);
		mov_reg_imm_0 (decryptor_buff, &code_offset, vreg,  target_buff_size);
		push_0(decryptor_buff, &code_offset, vreg);
		mov_reg_imm_0 (decryptor_buff, &code_offset, vreg,  target_buff);
		push_0(decryptor_buff, &code_offset, vreg);
		call_near_0(decryptor_buff, &code_offset, code_offset+sizeof(uint32_t));
	}

	INVOKE_RANDFUNC(prelude_variants_arr, decryptor_buff, &code_offset);

	if (flags & CONF_PRESERVE_REGISTERS) {
		INVOKE_RANDFUNC(pushad_variants_arr, decryptor_buff, &code_offset);
	}

	if (flags & CONF_RC4_CIPHER) {
		Sbtg_RC4(decryptor_buff, *decryptor_buff_size, &code_offset, key);
	} else {
		Sbtg_XOR(decryptor_buff, *decryptor_buff_size, &code_offset, key);
	}

	if (flags & CONF_PRESERVE_REGISTERS) {
		INVOKE_RANDFUNC(popad_variants_arr, decryptor_buff, &code_offset);
	}

	INVOKE_RANDFUNC(epilogue_variants_arr, decryptor_buff, &code_offset);

	if (flags & CONF_OUTPUT) {
		char byte;
		sbtg_get_random_str(&byte, sizeof(char));

		INVOKE_RANDFUNC(cmp_reg_0_variants_arr, decryptor_buff, &code_offset, REG_ESP, byte);
		if (byte % 2) { 
			cjmp_near_imm_0(decryptor_buff, &code_offset, code_offset-(rand() & 0xff), CJMP_Z_);
		} else {
			cjmp_near_imm_0(decryptor_buff, &code_offset, code_offset+(rand() & 0xff), CJMP_Z_);

		}
	} else {
		*(uint8_t*)(decryptor_buff + code_offset) = OP_RET;
		code_offset += sizeof(uint8_t);
	}

	if (flags & CONF_END) {
		sbtg_memfd_creat(decryptor_buff, &code_offset);
	}
	if (~flags & CONF_RAWSIZE) {
		*decryptor_buff_size = code_offset;
	}
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

int craft_decryptor(uint8_t *target_buff, int target_buff_size) {
	uint32_t flag;
	void (*decryptor_entry)(uint8_t*, size_t);
	uint8_t *decryptor_buff;
	uint8_t *xor_key;
	uint8_t *rc4_key;
	uint8_t *iteration_order;
	size_t decryptor_buff_size;
	uint32_t output_file_offset;
	int iterations;
	uint8_t **rc4_keys;
	uint8_t **xor_keys;
	Elf32_Ehdr ehdr;
	Elf32_Phdr phdr;
	int fd;
	uint32_t random_base;
	int nxor=0;
	int nrc4=0;

	decryptor_buff = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANON|MAP_PRIVATE, -1, 0);
	if (decryptor_buff == MAP_FAILED) {
		puts("[-] Failed to allocate memory for decryptor");
		return -1;
	}	
	
	decryptor_entry = (void (*)(uint8_t*, size_t))decryptor_buff;
	decryptor_buff_size = PAGE_SIZE;
 	
	if((fd = open("output.elf", O_RDWR|O_CREAT, 0777)) == -1) {
		perror("open");
		return -1; 

	}
	sbtg_get_random_str((char*)&random_base, sizeof(random_base));
	random_base = PAGE_ALIGN(random_base);

	srand(time(NULL));
	iterations = 100 + rand() % 200;
	//iterations = 2;

	iteration_order = calloc(iterations, sizeof(uint8_t));
	xor_keys = calloc(200, sizeof(uintptr_t));
	rc4_keys = calloc(200, sizeof(uintptr_t));

	for (int i = 0; i < iterations; i++) {
		iteration_order[i] = rand() % 3;
		//iteration_order[i] = 1;

		switch(iteration_order[i]) {
			case 0: 
				xor_key = calloc(1, sizeof(uint8_t) * 4);
				rc4_key = calloc(1, sizeof(uint8_t) * 16);

				sbtg_get_random_str(xor_key,  sizeof(uint8_t) * 4);
				sbtg_get_random_str(rc4_key, sizeof(uint8_t) * 16);

				xor_keys[i] = xor_key;
				rc4_keys[i] = rc4_key;

 				Sbtg(decryptor_buff, &decryptor_buff_size, rc4_keys[i], g_TargetBuff, g_TargetBuff_size, CONF_PRESERVE_REGISTERS|CONF_RC4_CIPHER);
				(*decryptor_entry)(target_buff, target_buff_size);
				Sbtg(decryptor_buff, &decryptor_buff_size, xor_keys[i], g_TargetBuff, g_TargetBuff_size, CONF_PRESERVE_REGISTERS);
				(*decryptor_entry)(target_buff, target_buff_size);
				nxor++;
				nrc4++;
				break;

			case 1:
				rc4_key = calloc(1, sizeof(uint8_t) * 16);
				sbtg_get_random_str(rc4_key, sizeof(uint8_t) * 16);
				rc4_keys[i] = rc4_key;

				Sbtg(decryptor_buff, &decryptor_buff_size, rc4_keys[i], g_TargetBuff, g_TargetBuff_size, CONF_PRESERVE_REGISTERS|CONF_RC4_CIPHER);
				(*decryptor_entry)(target_buff, target_buff_size);
				nrc4++;
				break;

			case 2:
				xor_key = calloc(1, sizeof(uint8_t) * 4);
				sbtg_get_random_str(xor_key, sizeof(uint8_t) * 4);
				xor_keys[i] = xor_key;

				Sbtg(decryptor_buff, &decryptor_buff_size, xor_keys[i], g_TargetBuff, g_TargetBuff_size, CONF_PRESERVE_REGISTERS);
				(*decryptor_entry)(target_buff, target_buff_size);
				nxor++;
				break;
		}
	}

	output_file_offset = sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
	pwrite(fd, target_buff, target_buff_size, output_file_offset);
	output_file_offset += target_buff_size;
	
	g_TargetBuff = random_base + sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
	g_TargetBuff_size = target_buff_size;

	for (int i = iterations-1; i >= 0; i--) {
		int flags = CONF_PRESERVE_REGISTERS|CONF_OUTPUT|CONF_START;

		if (i == 0) {
			flags |= CONF_END;
		} 

		switch(iteration_order[i]) {
			case 0:
				Sbtg(decryptor_buff, &decryptor_buff_size, xor_keys[i], g_TargetBuff, g_TargetBuff_size, flags);
				pwrite(fd, decryptor_buff, decryptor_buff_size, output_file_offset);
				output_file_offset += decryptor_buff_size;

				rc4_key = rc4_keys[i];
				Sbtg(decryptor_buff, &decryptor_buff_size, rc4_keys[i], g_TargetBuff, g_TargetBuff_size, flags|CONF_RC4_CIPHER);
				pwrite(fd, decryptor_buff, decryptor_buff_size, output_file_offset);
				output_file_offset += decryptor_buff_size;
				break;

			case 1:
				{
				uint8_t *poly_buff;
				size_t poly_buff_size;
				int code_offset=0;
				uint32_t vreg = genrand(4);

				flags |= CONF_RC4_CIPHER;
				
				// creating decryptor buff
				Sbtg(decryptor_buff, &decryptor_buff_size, rc4_keys[i], g_TargetBuff, g_TargetBuff_size, flags);
				
				// saving it
				poly_buff = calloc(1, decryptor_buff_size);
				poly_buff_size = decryptor_buff_size;
				memcpy(poly_buff, decryptor_buff, poly_buff_size);
				
				// creating decryptor that will decrypt the previous one
				Sbtg(decryptor_buff, &decryptor_buff_size, rc4_keys[i], 0, 0, 
					CONF_PRESERVE_REGISTERS|CONF_RC4_CIPHER|CONF_START|CONF_OUTPUT);
				
				//patching buffer to update address of poly_buffer address in decryptor
				mov_reg_imm_0 (decryptor_buff, &code_offset, vreg,  poly_buff_size);
				push_0 (decryptor_buff, &code_offset, vreg);
				mov_reg_imm_0 (decryptor_buff, &code_offset, vreg,  random_base+output_file_offset+decryptor_buff_size);
				push_0 (decryptor_buff, &code_offset, vreg);
				call_near_0(decryptor_buff, &code_offset, code_offset+sizeof(uint32_t));

				// writing decryptor decrypter
				pwrite(fd, decryptor_buff, decryptor_buff_size, output_file_offset);
				output_file_offset += decryptor_buff_size;

				// crafting a decryptor to encrypt the poly_buff
				Sbtg(decryptor_buff, &decryptor_buff_size, rc4_keys[i], 0, 0, 
					CONF_PRESERVE_REGISTERS|CONF_RC4_CIPHER);

				// encrypting it	
				(*decryptor_entry)(poly_buff, poly_buff_size);
				
				// writitng the encrypted instance
				pwrite(fd, poly_buff, poly_buff_size, output_file_offset);
				output_file_offset += poly_buff_size;
				free(poly_buff);
				break;
				}

			case 2:
				Sbtg(decryptor_buff, &decryptor_buff_size, xor_keys[i], g_TargetBuff, g_TargetBuff_size, flags);
				pwrite(fd, decryptor_buff, decryptor_buff_size, output_file_offset);
				output_file_offset += decryptor_buff_size;
				break;
		}
	}

	ehdr.e_ident[EI_MAG0] = 0x7f;
	ehdr.e_ident[EI_MAG1] = 'E';
	ehdr.e_ident[EI_MAG2] = 'L';
	ehdr.e_ident[EI_MAG3] = 'F';
	ehdr.e_ident[EI_CLASS] = ELFCLASS32;
	ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
	ehdr.e_ident[EI_VERSION] = EV_CURRENT;
	ehdr.e_ident[EI_OSABI] = ELFOSABI_LINUX;
	ehdr.e_ident[EI_ABIVERSION] = 0;
	ehdr.e_type = ET_EXEC;
	ehdr.e_machine = EM_386;
	ehdr.e_version = EV_CURRENT;
	ehdr.e_entry = random_base + sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) + target_buff_size;
	ehdr.e_ehsize = sizeof(Elf32_Ehdr);
	ehdr.e_flags = 0;
	ehdr.e_phoff = sizeof(Elf32_Ehdr);
	ehdr.e_phnum = 1;
	ehdr.e_phentsize = sizeof(Elf32_Phdr);
	ehdr.e_shnum = 0;
	ehdr.e_shoff = 0;
	ehdr.e_shentsize = 0;
	ehdr.e_shstrndx = 0;

	pwrite(fd, &ehdr, sizeof(Elf32_Ehdr), 0);

	phdr.p_align = 0x1000;
	phdr.p_offset = 0;
	phdr.p_flags = PF_R | PF_W | PF_X;
	phdr.p_type = PT_LOAD;
	phdr.p_filesz = PAGE_ALIGN(output_file_offset);
	phdr.p_memsz = PAGE_ALIGN(output_file_offset);
	phdr.p_paddr = random_base;
	phdr.p_vaddr = random_base;
	
	pwrite(fd, &phdr,sizeof(Elf32_Phdr), sizeof(Elf32_Ehdr));
	close(fd);

	printf("[SBTG] - XOR layers : %d\n[SBTG] - RC4 layers: %d\n[SBTG] - Total layers : %d\n", nxor, nrc4, nxor+nrc4);
	
	return 0;

}

int main(int argc, char **argv, char **envp) {
	uint8_t * decryptor_buff;
	uint8_t *target_buff;
	uint32_t flags;
	size_t target_buff_size;
	size_t decryptor_buff_size;
	void (*decryptor_entry)(uint8_t*, size_t);
	struct stat st;
	int fd;

	if (!alloc_file(&fd, &st, argv[1], &target_buff)) {
       		printf("[+] Usage: %s <target binary>\n", argv[0]);
       		return -1;
   	}
	target_buff_size = st.st_size;

	craft_decryptor(target_buff, target_buff_size);
	
	return 0;	
} 
