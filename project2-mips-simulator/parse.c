/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

void handle_I(instruction* instr, const char* buffer)
{ 
	char rs[6] = {0};
	char rt[6] = {0};
	char immi[17] = {0};

	memcpy(rs,buffer+6,5);
	instr->r_t.r_i.rs = (unsigned char)fromBinary(rs);
	memcpy(rt,buffer+11,5);
	instr->r_t.r_i.rt = (unsigned char)fromBinary(rt);
	memcpy(immi,buffer+16,16);
	instr->r_t.r_i.r_i.imm = (short)fromBinary(immi);
}

void handle_R(instruction* instr, const char* buffer)
{ 
	char rs[6] = {0};
	char rt[6] = {0};
	char rd[6] = {0};
	char shmt[6] = {0};
	char fnct[7] = {0};
	memcpy(rs,buffer+6,5);
	instr->r_t.r_i.rs = (unsigned char)fromBinary(rs);
	memcpy(rt,buffer+11,5);
	instr->r_t.r_i.rt = (unsigned char)fromBinary(rt);
	memcpy(rd,buffer+16,5);
	instr->r_t.r_i.r_i.r.rd = (unsigned char)fromBinary(rd);
	memcpy(shmt,buffer+21,5);
	instr->r_t.r_i.r_i.r.shamt = (unsigned char)fromBinary(shmt);
	memcpy(fnct,buffer+26,6);
	instr->func_code = (short)fromBinary(fnct);
}

void handle_J(instruction* instr, const char* buffer)
{
	char addr[27] = {0};
	memcpy(addr,buffer+6,26);
	instr->r_t.target = (uint32_t)fromBinary(addr);
}



instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;

	char opcode[7] = {0};

	memcpy(opcode,buffer,6);
	instr.opcode = (short)fromBinary(opcode);

	instr.value = (uint32_t)fromBinary(buffer);

	switch (instr.opcode)
	{
	case 0x0:
	handle_R(&instr,buffer);
		break;
	case 0x9:
	case 0xf:
	case 0x23:
	case 0xd:
	case 0xb:
	case 0x2b:
	case 0xc:
	case 0x4:
	case 0x5:
	handle_I(&instr, buffer);
		break;
	case 0x2:
	case 0x3:
	handle_J(&instr,buffer);
		break;
	default:
		break;
	}

	/** Implement this function */
    return instr;
}

void parsing_data(const char *buffer, const int index)
{
	// void mem_write_32(uint32_t address, uint32_t value)
	mem_write_32(MEM_DATA_START+index, fromBinary(buffer));


	/** Implement this function */
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
	printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
	printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	switch(INST_INFO[i].opcode)
	{
	    //I format
	    case 0x9:		//ADDIU
	    case 0xc:		//ANDI
	    case 0xf:		//LUI	
	    case 0xd:		//ORI
	    case 0xb:		//SLTIU
	    case 0x23:		//LW	
	    case 0x2b:		//SW
	    case 0x4:		//BEQ
	    case 0x5:		//BNE
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
		break;

    	    //R format
	    case 0x0:		//ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU if JR
		printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
		printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
		break;

    	    //J format
	    case 0x2:		//J
	    case 0x3:		//JAL
		printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
		break;

	    default:
		printf("Not available instruction\n");
		assert(0);
	}
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
	printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
	printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
