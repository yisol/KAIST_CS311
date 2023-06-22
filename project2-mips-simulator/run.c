/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/

instruction* get_inst_info(uint32_t pc) 
{ 
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
	/** Implement this function */
    instruction* fetched_instr = get_inst_info(CURRENT_STATE.PC);
    uint32_t t;
    uint32_t immi;
    uint32_t a;
    uint32_t b;
    uint32_t concat;
    int signext;
	switch (fetched_instr->opcode)
	{
	case 0x0:
        switch (fetched_instr->func_code)
        {
        case 0x21:
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]= CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt] +CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs];
        CURRENT_STATE.PC+=4;
            break;
        case 0x24:
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt] &CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs];
        CURRENT_STATE.PC+=4;
            break;
        case 0x8:
        CURRENT_STATE.PC = CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs];
            break;
        case 0x27:
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]=~(CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]|CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs]);
        CURRENT_STATE.PC+=4;
            break;
        case 0x25:
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]|CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs];
        CURRENT_STATE.PC+=4;
            break;
        case 0x2b:
        if(CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt] > CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs])
        {
            CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]= 1;
        }
        else
        {
            CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]=0;
        }
        CURRENT_STATE.PC+=4;
            break;
        case 0x00:
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]<<fetched_instr->r_t.r_i.r_i.r.shamt;
        CURRENT_STATE.PC+=4;
            break;
        case 0x02:
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]=CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]>>fetched_instr->r_t.r_i.r_i.r.shamt;
        CURRENT_STATE.PC+=4;
            break;
        case 0x23:
        a=CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt];
        b=CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs];
        a=~a;
        a+=1;
        t=b+a;
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.r_i.r.rd]=t;
            CURRENT_STATE.PC+=4;
            break;    
        default:
            break;
        }
		break;
	case 0x9:
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt] = CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs] +(int)(fetched_instr->r_t.r_i.r_i.imm);
        CURRENT_STATE.PC+=4;
		break;
	case 0xf:
        concat = 0x0000FFFF & (uint32_t)(fetched_instr->r_t.r_i.r_i.imm);
        concat = concat <<16;
        concat = concat & 0xFFFF0000;
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]= concat;
        CURRENT_STATE.PC+=4;
		break;
	case 0x23:
        immi = (int)(fetched_instr->r_t.r_i.r_i.imm);
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]= mem_read_32(CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs]+immi);
        CURRENT_STATE.PC+=4;
		break;
	case 0xd:
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]=(CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs])| (((int)(fetched_instr->r_t.r_i.r_i.imm)) & 0x0000FFFF);
        CURRENT_STATE.PC+=4;
		break;
	case 0xb:
    signext = (int)(fetched_instr->r_t.r_i.r_i.imm);
    
    if (CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs]<(uint32_t)(signext))
    {
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]=1;
    }
    else
    {
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]=0;
    }
    CURRENT_STATE.PC+=4;
	break;
	case 0x2b:
        mem_write_32(CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs]+(int)(fetched_instr->r_t.r_i.r_i.imm),CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]);
        CURRENT_STATE.PC+=4;
		break;
	case 0xc:
        immi = (int)(fetched_instr->r_t.r_i.r_i.imm);
        immi &= 0x0000FFFF;
        
        CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt]=(CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs])& immi;
        CURRENT_STATE.PC+=4;
		break;
	case 0x4:
    if (CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt] == CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs])
    {
        signext = (int)(fetched_instr->r_t.r_i.r_i.imm);
        signext = signext << 2;
        CURRENT_STATE.PC =CURRENT_STATE.PC + signext+4 ;
    }
    else
    {
        CURRENT_STATE.PC +=4;
    }
	break;
	case 0x5:
    if (CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rt] != CURRENT_STATE.REGS[fetched_instr->r_t.r_i.rs])
    {
        signext = (int)(fetched_instr->r_t.r_i.r_i.imm);
        signext = signext << 2;
        CURRENT_STATE.PC =CURRENT_STATE.PC + signext+4 ;
    }
    else
    {
        CURRENT_STATE.PC +=4;
    }
	break;
	case 0x2:
    CURRENT_STATE.PC = ((0xF0000000 & (CURRENT_STATE.PC+4))) | ((fetched_instr->r_t.target<<2));
		break;
	case 0x3:
    CURRENT_STATE.REGS[31]=CURRENT_STATE.PC+4;
    CURRENT_STATE.PC = ((0xF0000000 & (CURRENT_STATE.PC+4))) | ((fetched_instr->r_t.target<<2));
		break;
    }
    if(CURRENT_STATE.PC == MEM_TEXT_START+NUM_INST*4)
    {
        RUN_BIT = FALSE;
    }
}