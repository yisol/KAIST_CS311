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
instruction* get_inst_info(uint32_t pc) { 
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}


char Is_Branch(uint32_t pc)
{
    instruction* instr = get_inst_info(pc);
    return (instr->opcode == 0x4 ||  instr->opcode == 0x5);
}

void IF_Stage(){


    if(CURRENT_STATE.PC == MEM_TEXT_START+NUM_INST*4)
    {
        CURRENT_STATE.PIPE[0] = 0;
        CURRENT_STATE.IF_ID_INST = 0;
        CURRENT_STATE.IF_ID_NPC = 0;
        return;
    }

    instruction* fetched_instr = get_inst_info(CURRENT_STATE.PC);
    CURRENT_STATE.PIPE[0] = CURRENT_STATE.PC;
    if(CURRENT_STATE.IF_ID_WRITE)
    {
        CURRENT_STATE.IF_ID_INST = fetched_instr->value;
        CURRENT_STATE.IF_ID_NPC = CURRENT_STATE.PC+4;
        CURRENT_STATE.IF_ID_WRITE =1;
    }

    if(CURRENT_STATE.PIPE_STALL[0])
    {
        CURRENT_STATE.PIPE[0] = 0;
        CURRENT_STATE.PIPE_STALL[0] = FALSE;
        CURRENT_STATE.IF_ID_INST = 0;
        CURRENT_STATE.IF_ID_NPC = 0;
    }
    else
    {
        if(CURRENT_STATE.PC_WRITE)
        {if(CURRENT_STATE.EX_MEM_BR_TAKE)
            CURRENT_STATE.PC = CURRENT_STATE.BRANCH_PC;
        else if(CURRENT_STATE.ID_EX_JMP)
            CURRENT_STATE.PC = CURRENT_STATE.JUMP_PC;
        else
            CURRENT_STATE.PC = CURRENT_STATE.IF_ID_NPC;
        CURRENT_STATE.PC_WRITE =1;
        }


        // if(CURRENT_STATE.IF_ID_JMP)
        // {
        //     CURRENT_STATE.IF_PC = CURRENT_STATE.IF_ID_JMP_TARGET;
        // }

        // if(fetched_instr->opcode == 0x2||fetched_instr->opcode == 0x3)
        // {
        //     CURRENT_STATE.IF_ID_JMP = 1; 
        //     CURRENT_STATE.IF_ID_JMP_TARGET = fetched_instr->r_t.target;
        // }
        // if(fetched_instr->opcode == 0x0 && fetched_instr->func_code == 0x8)
        // {
        //     CURRENT_STATE.IF_ID_JMP = 1; 
        //     CURRENT_STATE.IF_ID_JMP_TARGET = fetched_instr->r_t.r_i.rs;
        // }
        // CURRENT_STATE.EX_MEM_BR_TAKE
    }

    //right pc to next fetch


}


void ID_Stage(){
    // instruction* ID_instr = get_inst_info(CURRENT_STATE.PIPE[1]);
    //move it to next stage
    if(CURRENT_STATE.IF_ID_WRITE)
        CURRENT_STATE.PIPE[1] = CURRENT_STATE.PIPE[0];

    // if(CURRENT_STATE.IF_ID_JMP)
    // {

    // }
    CURRENT_STATE.PC_WRITE = 1;
    CURRENT_STATE.IF_ID_WRITE=1;


    unsigned char calc_rs = (0x03E00000 & CURRENT_STATE.IF_ID_INST) >> 21;
    unsigned char calc_rt = (0x001F0000 & CURRENT_STATE.IF_ID_INST) >> 16;
    unsigned char calc_rd = (0x0000F800 & CURRENT_STATE.IF_ID_INST) >> 11;
    unsigned char calc_op = (0xFC000000 & CURRENT_STATE.IF_ID_INST) >> 26;
    unsigned char calc_fnct = CURRENT_STATE.IF_ID_INST & 0x0000003F;
    short calc_imm = 0x0000FFFF & CURRENT_STATE.IF_ID_INST;
    uint32_t calc_relative = 0x03FFFFFF & CURRENT_STATE.IF_ID_INST;



    if (CURRENT_STATE.PIPE_STALL[1] )
    {
        //stall함수로
        CURRENT_STATE.PIPE_STALL[1]=FALSE;
        CURRENT_STATE.PIPE[1] = 0;
        CURRENT_STATE.ID_EX_REG_DEST=0;
        CURRENT_STATE.ID_EX_MEM_WRITE=0;
        CURRENT_STATE.ID_EX_MEM_READ=0;
        CURRENT_STATE.ID_EX_ALU_SRC=0;
        CURRENT_STATE.ID_EX_BRANCH=0;
        CURRENT_STATE.ID_EX_REG_WRITE=0;
        CURRENT_STATE.ID_EX_JMP = 0;
	    CURRENT_STATE.ID_EX_ALU_OP = 0;
	    CURRENT_STATE.ID_EX_MEMTOREG = 0;

    }
    else if((CURRENT_STATE.ID_EX_MEM_READ && ((CURRENT_STATE.ID_EX_RT == calc_rt && (calc_op == 0x0 && calc_fnct != 0x8) ) ||CURRENT_STATE.ID_EX_RT ==calc_rs)))
    {
        //hazard detection unit
        CURRENT_STATE.PIPE_STALL[2]=TRUE;
        CURRENT_STATE.ID_EX_REG_DEST=0;
        CURRENT_STATE.ID_EX_MEM_WRITE=0;
        CURRENT_STATE.ID_EX_MEM_READ=0;
        CURRENT_STATE.ID_EX_ALU_SRC=0;
        CURRENT_STATE.ID_EX_BRANCH=0;
        CURRENT_STATE.ID_EX_REG_WRITE=0;
        CURRENT_STATE.ID_EX_JMP = 0;
        CURRENT_STATE.PC_WRITE = 0;
        CURRENT_STATE.IF_ID_WRITE=0;
    }
    else
    {

        CURRENT_STATE.ID_EX_RS = calc_rs;
        CURRENT_STATE.ID_EX_RT = calc_rt;
        CURRENT_STATE.ID_EX_RD = calc_rd;
        CURRENT_STATE.ID_EX_IMM = (int)calc_imm;
        CURRENT_STATE.ID_EX_REG1 = CURRENT_STATE.REGS[calc_rs];
        CURRENT_STATE.ID_EX_REG2 = CURRENT_STATE.REGS[calc_rt];
        CURRENT_STATE.ID_EX_MEMTOREG = 0;

        CURRENT_STATE.ID_EX_MEM_WRITE = 0;
        CURRENT_STATE.ID_EX_MEM_READ = 0;
        CURRENT_STATE.ID_EX_BRANCH = 0;
        CURRENT_STATE.ID_EX_JMP = 0;
        CURRENT_STATE.ID_EX_ALU_SRC = 0;
        CURRENT_STATE.ID_EX_REG_DEST = 0;
        CURRENT_STATE.ID_EX_REG_WRITE = 0;
        CURRENT_STATE.ID_EX_ALU_OP=0;
        if (calc_op ==0x23)
        {
            //lw
            CURRENT_STATE.ID_EX_MEM_READ = 1;
            CURRENT_STATE.ID_EX_ALU_SRC = 1;
            CURRENT_STATE.ID_EX_REG_WRITE = 1;
            CURRENT_STATE.ID_EX_MEMTOREG =1; 
        }
        else if (calc_op ==0x2b)
        {
            //sw
            CURRENT_STATE.ID_EX_MEM_WRITE = 1;
            CURRENT_STATE.ID_EX_ALU_SRC = 1;
        }
        else if (calc_op == 0x0)
        {
            if(calc_fnct == 0x8)
            {
                //jr
                CURRENT_STATE.JUMP_PC = CURRENT_STATE.REGS[calc_rs];      
                CURRENT_STATE.ID_EX_JMP =1;
                CURRENT_STATE.PIPE_STALL[1] = TRUE;
            }
            else
            {
                CURRENT_STATE.ID_EX_ALU_OP=2;
                CURRENT_STATE.ID_EX_REG_DEST = 1;
                CURRENT_STATE.ID_EX_REG_WRITE = 1;
            }
        }
        else if(calc_op == 0x4 )
        {
            //beq
            CURRENT_STATE.ID_EX_BRANCH = 1;
            CURRENT_STATE.ID_EX_ALU_OP=1;

        }
        else if(calc_op == 0x5)
        {
            //bne
            CURRENT_STATE.ID_EX_BRANCH = 1;
            CURRENT_STATE.ID_EX_ALU_OP=8;
        }
        else if(calc_op == 0x2)
        {
            //j
            uint32_t first4bits = (0xF0000000 & CURRENT_STATE.IF_ID_NPC);
            CURRENT_STATE.JUMP_PC = first4bits| (calc_relative<<2);        
            CURRENT_STATE.PIPE_STALL[1] = TRUE;
            CURRENT_STATE.ID_EX_JMP = 1;
        }
        else if(calc_op == 0x3)
        {

            //jal
            uint32_t first4bits = (0xF0000000 & CURRENT_STATE.IF_ID_NPC);
            CURRENT_STATE.JUMP_PC = first4bits| (calc_relative<<2);       
            CURRENT_STATE.REGS[31] = CURRENT_STATE.IF_ID_NPC; 
            CURRENT_STATE.PIPE_STALL[1] = TRUE;
            CURRENT_STATE.ID_EX_JMP = 1;
        }
        else if(calc_op == 0x9)
        {
            //addiu
            CURRENT_STATE.ID_EX_REG_WRITE=1;
            CURRENT_STATE.ID_EX_ALU_OP = 3;
            CURRENT_STATE.ID_EX_ALU_SRC = 1;        
        }
        else if(calc_op == 0xf)
        {
            //lui
            CURRENT_STATE.ID_EX_REG_WRITE=1;
            CURRENT_STATE.ID_EX_ALU_OP = 4;
            CURRENT_STATE.ID_EX_ALU_SRC = 1;        
        }
        else if(calc_op == 0xd)
        {
            //ori
            CURRENT_STATE.ID_EX_REG_WRITE=1;
            CURRENT_STATE.ID_EX_ALU_OP = 5;
            CURRENT_STATE.ID_EX_ALU_SRC = 1;        
        }
        else if(calc_op == 0xb)
        {
            //sltiu
            CURRENT_STATE.ID_EX_REG_WRITE=1;
            CURRENT_STATE.ID_EX_ALU_OP = 6;
            CURRENT_STATE.ID_EX_ALU_SRC = 1;        
        }
        else if(calc_op == 0xc)
        {
            //andi
            CURRENT_STATE.ID_EX_REG_WRITE=1;
            CURRENT_STATE.ID_EX_ALU_OP = 7;
            CURRENT_STATE.ID_EX_ALU_SRC = 1;        
        }
        CURRENT_STATE.ID_EX_NPC = CURRENT_STATE.IF_ID_NPC;
    }

}

void EX_Stage(){
    
    //move it to next stage
    CURRENT_STATE.PIPE[2] = CURRENT_STATE.PIPE[1];

    CURRENT_STATE.EX_MEM_FORWARD_VALUE= CURRENT_STATE.EX_MEM_ALU_OUT;
    CURRENT_STATE.EX_MEM_FORWARD_REG = CURRENT_STATE.EX_MEM_DEST;

    if(CURRENT_STATE.PIPE_STALL[2])
    {
        CURRENT_STATE.PIPE[2] = 0;
        CURRENT_STATE.PIPE_STALL[2] = FALSE;

        CURRENT_STATE.EX_MEM_REG_WRITE = 0;
        CURRENT_STATE.EX_MEM_BRANCH = 0;
        CURRENT_STATE.EX_MEM_DEST =0;
        CURRENT_STATE.EX_MEM_MEM_READ=0;
        CURRENT_STATE.EX_MEM_MEM_WRITE=0;
        CURRENT_STATE.EX_MEM_MEMTOREG=0;
        CURRENT_STATE.EX_MEM_BR_TAKE =0;
    }
    else
    {
        uint32_t ALU1 = CURRENT_STATE.ID_EX_REG1;
        uint32_t ALU2 = CURRENT_STATE.ID_EX_REG2;

        if (CURRENT_STATE.ID_EX_ALU_SRC ==0)
        {
            ALU2 = CURRENT_STATE.ID_EX_REG2;
        }
        else if(CURRENT_STATE.ID_EX_ALU_SRC ==1)
        {
            ALU2 = CURRENT_STATE.ID_EX_IMM;
        }

        if (CURRENT_STATE.EX_MEM_REG_WRITE && CURRENT_STATE.EX_MEM_FORWARD_REG!= 0 && CURRENT_STATE.EX_MEM_FORWARD_REG== CURRENT_STATE.ID_EX_RS)
        {
            ALU1 = CURRENT_STATE.EX_MEM_FORWARD_VALUE;
        }
        if (CURRENT_STATE.EX_MEM_REG_WRITE && CURRENT_STATE.EX_MEM_FORWARD_REG!= 0 && CURRENT_STATE.EX_MEM_FORWARD_REG== CURRENT_STATE.ID_EX_RT)
        {
            //lw r1, add , r1 -> hazard detection unit이 stall로 해결
            
            //add r1, addiu r1,
            //lw r1, addiu r1,
            //add r1, lw r1,
            if(CURRENT_STATE.ID_EX_REG_DEST == 1)
            {
                //add r1, sw r1,
                ALU2 = CURRENT_STATE.EX_MEM_FORWARD_VALUE;
            }
        }
        if (CURRENT_STATE.MEM_WB_REG_WRITE_FORWARD && CURRENT_STATE.MEM_WB_FORWARD_REG!= 0 && !(CURRENT_STATE.EX_MEM_REG_WRITE && CURRENT_STATE.EX_MEM_FORWARD_REG !=0 && CURRENT_STATE.EX_MEM_FORWARD_REG== CURRENT_STATE.ID_EX_RS) && CURRENT_STATE.MEM_WB_FORWARD_REG== CURRENT_STATE.ID_EX_RS)
        {
            ALU1 = CURRENT_STATE.MEM_WB_FORWARD_VALUE;
        }
        if (CURRENT_STATE.MEM_WB_REG_WRITE_FORWARD && CURRENT_STATE.MEM_WB_FORWARD_REG!= 0 && !(CURRENT_STATE.EX_MEM_REG_WRITE && CURRENT_STATE.EX_MEM_FORWARD_REG !=0 && CURRENT_STATE.EX_MEM_FORWARD_REG== CURRENT_STATE.ID_EX_RT) && CURRENT_STATE.MEM_WB_FORWARD_REG== CURRENT_STATE.ID_EX_RT )
        {
            if(CURRENT_STATE.ID_EX_REG_DEST == 1)
            {
                ALU2 = CURRENT_STATE.MEM_WB_FORWARD_VALUE;
            }
        }
        
        CURRENT_STATE.BRANCH_PC = CURRENT_STATE.ID_EX_NPC + (CURRENT_STATE.ID_EX_IMM<<2);




        unsigned char fnct = CURRENT_STATE.ID_EX_IMM & 0x0000003F;
        unsigned char calc_shmt = (0x000007C0 & CURRENT_STATE.ID_EX_IMM) >> 6;

        if(CURRENT_STATE.ID_EX_ALU_OP == 0)
        {
            CURRENT_STATE.EX_MEM_ALU_OUT =  ALU1+ALU2;
        }
        else if (CURRENT_STATE.ID_EX_ALU_OP == 1)
        {
            CURRENT_STATE.EX_MEM_BR_TAKE = ALU1 == ALU2 ? 1 : 0;
        }
        else if(CURRENT_STATE.ID_EX_ALU_OP == 2)
        {
            switch (fnct)
            {
            case 0x21:
            //addu
                CURRENT_STATE.EX_MEM_ALU_OUT =  ALU1+ALU2;
                break;
            case 0x27:
                CURRENT_STATE.EX_MEM_ALU_OUT =  ~(ALU1|ALU2);
                break;
            case 0x2b:
                if(ALU2 > ALU1)
                {
                    CURRENT_STATE.EX_MEM_ALU_OUT= 1;
                }
                else
                {
                    CURRENT_STATE.EX_MEM_ALU_OUT=0;
                }
                break;
            case 0x00:
                //sll
                CURRENT_STATE.EX_MEM_ALU_OUT = ALU2 << calc_shmt;
                break;
            case 0x02:
                //srl
                CURRENT_STATE.EX_MEM_ALU_OUT = ALU2 >> calc_shmt;
                break;
            case 0x23:
                ALU2 = ~ALU2;
                ALU2 +=1;
                CURRENT_STATE.EX_MEM_ALU_OUT = ALU1 + ALU2;
                break;    
            case 0x24:
                CURRENT_STATE.EX_MEM_ALU_OUT =  ALU1&ALU2;
                break;
            case 0x25:
                //or
                CURRENT_STATE.EX_MEM_ALU_OUT =  ALU1|ALU2;
                break;
            default:
                break;
            }        
        }
        else if(CURRENT_STATE.ID_EX_ALU_OP == 3)
        {
            CURRENT_STATE.EX_MEM_ALU_OUT =  ALU1 + ALU2;
        }
        else if(CURRENT_STATE.ID_EX_ALU_OP == 4)
        {
            ALU2 = 0x0000FFFF & ALU2;
            ALU2 = ALU2 <<16;
            ALU2 = ALU2 & 0xFFFF0000;
            CURRENT_STATE.EX_MEM_ALU_OUT = ALU2;
        }
        else if(CURRENT_STATE.ID_EX_ALU_OP == 5)
        {
            CURRENT_STATE.EX_MEM_ALU_OUT = ALU1 | (ALU2 & 0x0000FFFF);
        }
        else if(CURRENT_STATE.ID_EX_ALU_OP == 6)
        {
            if (ALU1<ALU2)
            {
                CURRENT_STATE.EX_MEM_ALU_OUT=1;
            }
            else
            {
                CURRENT_STATE.EX_MEM_ALU_OUT=0;
            }
        }
        else if(CURRENT_STATE.ID_EX_ALU_OP == 7)
        {
            ALU2 &= 0x0000FFFF;        
            CURRENT_STATE.EX_MEM_ALU_OUT = ALU1 & ALU2;
        }
        else if(CURRENT_STATE.ID_EX_ALU_OP == 8)
        {
            CURRENT_STATE.EX_MEM_ALU_OUT =  ALU1-ALU2;
            CURRENT_STATE.EX_MEM_BR_TAKE = ALU1 != ALU2 ? 1 : 0;
        }



        if(CURRENT_STATE.ID_EX_REG_DEST==1)
        {
            
            CURRENT_STATE.EX_MEM_DEST = CURRENT_STATE.ID_EX_RD;
        }
        else
        {
            CURRENT_STATE.EX_MEM_DEST = CURRENT_STATE.ID_EX_RT;
        }

        CURRENT_STATE.EX_MEM_BRANCH = CURRENT_STATE.ID_EX_BRANCH;
        CURRENT_STATE.EX_MEM_REG_WRITE = CURRENT_STATE.ID_EX_REG_WRITE;
        CURRENT_STATE.EX_MEM_W_VALUE = CURRENT_STATE.ID_EX_REG2;
        CURRENT_STATE.EX_MEM_MEMTOREG = CURRENT_STATE.ID_EX_MEMTOREG;
        CURRENT_STATE.EX_MEM_MEM_READ = CURRENT_STATE.ID_EX_MEM_READ;
        CURRENT_STATE.EX_MEM_MEM_WRITE = CURRENT_STATE.ID_EX_MEM_WRITE;
    } 
}

void MEM_Stage(){

    CURRENT_STATE.PIPE[3] = CURRENT_STATE.PIPE[2];

    CURRENT_STATE.MEM_WB_REG_WRITE_FORWARD =  CURRENT_STATE.MEM_WB_REG_WRITE;
    CURRENT_STATE.MEM_WB_FORWARD_REG = CURRENT_STATE.MEM_WB_DEST;


    if(CURRENT_STATE.EX_MEM_BR_TAKE)
    {
        CURRENT_STATE.PIPE_STALL[0] = TRUE;
        CURRENT_STATE.PIPE_STALL[1] = TRUE;
        CURRENT_STATE.PIPE_STALL[2] = TRUE;
        // CURRENT_STATE.PC = CURRENT_STATE.EX_MEM_BR_TARGET;
    }

    if (CURRENT_STATE.EX_MEM_MEM_WRITE)
    {
        //jal wb에서 써야함?
        //memtoreg ok?원래는 mem_write인데
        //EX_MEM_DEST 꼼수 원래는 sw r1, 의 r1이 필요한데 없으니까 EX_MEM_DEST를 sw시에 r1으로 설정
        if (CURRENT_STATE.MEM_WB_MEMTOREG && CURRENT_STATE.MEM_WB_DEST !=0 && CURRENT_STATE.MEM_WB_DEST == CURRENT_STATE.EX_MEM_DEST)
        {
            mem_write_32(CURRENT_STATE.EX_MEM_ALU_OUT,CURRENT_STATE.MEM_WB_FORWARD_VALUE);    	
        }
        else
        {
            mem_write_32(CURRENT_STATE.EX_MEM_ALU_OUT,CURRENT_STATE.EX_MEM_W_VALUE);    	
        }
    }
    else if(CURRENT_STATE.EX_MEM_MEM_READ)
    {
        CURRENT_STATE.MEM_WB_MEM_OUT = mem_read_32(CURRENT_STATE.EX_MEM_ALU_OUT);
    }
  	CURRENT_STATE.MEM_WB_DEST = CURRENT_STATE.EX_MEM_DEST;
    CURRENT_STATE.MEM_WB_ALU_OUT = CURRENT_STATE.EX_MEM_ALU_OUT;
    CURRENT_STATE.MEM_WB_MEMTOREG = CURRENT_STATE.EX_MEM_MEMTOREG;
    CURRENT_STATE.MEM_WB_REG_WRITE = CURRENT_STATE.EX_MEM_REG_WRITE;

}


void WB_Stage(){
    //move it to next stage
    CURRENT_STATE.PIPE[4] = CURRENT_STATE.PIPE[3];
    if(CURRENT_STATE.PIPE[4]!=0)
    {
        INSTRUCTION_COUNT++;
    }

    if(CURRENT_STATE.MEM_WB_MEMTOREG == 1)
    {
        //혹시 forward가 필요할 수 있으니 데이터 백업용 
        CURRENT_STATE.MEM_WB_FORWARD_VALUE = CURRENT_STATE.MEM_WB_MEM_OUT;
    }
    else if(CURRENT_STATE.MEM_WB_MEMTOREG == 0)
    {
        CURRENT_STATE.MEM_WB_FORWARD_VALUE = CURRENT_STATE.MEM_WB_ALU_OUT;
    }
    if(CURRENT_STATE.MEM_WB_REG_WRITE==1)
    {
        CURRENT_STATE.REGS[CURRENT_STATE.MEM_WB_DEST] = CURRENT_STATE.MEM_WB_FORWARD_VALUE;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
    WB_Stage();	
    MEM_Stage();
    EX_Stage();
    ID_Stage();
    IF_Stage();

    if(CURRENT_STATE.PIPE[0] == 0 &&CURRENT_STATE.PIPE[1] == 0&& CURRENT_STATE.PIPE[2] == 0 && CURRENT_STATE.PIPE[3] == 0)
    {
        RUN_BIT = FALSE;
    }

    /** Your implementation here */
}
