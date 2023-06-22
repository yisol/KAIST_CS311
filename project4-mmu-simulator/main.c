
/* main.c 

Page Table Entry (PTE) format:

Width: 32 bits

Bit 31 ~ 12     : 20-bit physical page number for the 2nd-level page table node or the actual physical page.
Bit 1           : Dirty bit
Bit 0           : Valid bit

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "util.h"  /* DO NOT DELETE */


/* addr_t type is the 32-bit address type defined in util.h */
char** str_split(char *a_str, const char a_delim){
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp){
	if (a_delim == *tmp){
	    count++;
	    last_comma = tmp;
	}
	tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
     *        knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result){
	size_t idx  = 0;
	char* token = strtok(a_str, delim);

	while (token){
	    assert(idx < count);
	    *(result + idx++) = strdup(token);
	    token = strtok(0, delim);
	}
	assert(idx == count - 1);
	*(result + idx) = 0;
    }

    return result;
}

typedef struct TLB_row
{
    int index;
    struct elem* head;
    struct TLB_row* next;
    struct elem* seqeunce;
    int num_elem;

} TLB_row;

typedef struct elem
{   
    int tag;
    int ppn;
    char dirty;
    char valid;
    struct elem* next;
} elem;

typedef struct TLB
{
    struct TLB_row* head;
    int nway;
} TLB;


elem* find_by_tag(elem* head, int tag)
{
    elem* look = head;
    while (look != NULL)
    {
        if(look->tag ==tag)
        {
            return look;
        }
        else
        {
            look = look->next;
        }
    }
    return NULL;
}    

elem* lookup_TLB(TLB* arg_TLB, int idx, int tag)
{
    TLB_row* look = arg_TLB->head;
    while (look!=NULL)
    {
        if(look->index == idx)
        {
            return find_by_tag(look->head,tag);
        }
        else
        {
            look = look->next;
        }
    }
    return NULL;    
}

void add_TLB_elem(TLB* arg_TLB, int TLB_idx, int TLB_tag, int ppn, int is_write)
{
    TLB_row* look = arg_TLB->head;
    while (look!=NULL)
    {
        if(look->index == TLB_idx)
        {
            if (look->num_elem < arg_TLB->nway)
            {
                elem* iter = look->head;
                while (iter != NULL)
                {
                    if(iter->valid ==0)
                    {
                        iter->tag = TLB_tag;
                        iter->dirty = is_write;
                        iter->ppn = ppn;
                        iter->valid=1;
                        look->num_elem++;
                        break;
                    }
                    else
                        iter = iter->next;
                }
                //update lru
                elem* LRU_iter = look->seqeunce;
                int my_tag = 0;
                int my_valid = 0;
                int prev_tag = TLB_tag;
                int prev_valid = 1;
                while (LRU_iter != NULL)
                {   
                    my_tag = LRU_iter->tag;
                    my_valid = LRU_iter->valid;
                    LRU_iter->valid = prev_valid;
                    LRU_iter->tag = prev_tag;
                    prev_tag = my_tag;
                    prev_valid = my_valid;

                    LRU_iter = LRU_iter->next;                
                }

            }
            else
            {

                //evict LRU
                elem* LRU_iter = look->seqeunce;
                int LRU_tag = LRU_iter->tag;

                while (LRU_iter !=NULL)
                {
                    LRU_tag= LRU_iter->tag;
                    LRU_iter = LRU_iter->next;
                }
                
                elem* iter = look->head;
                while (iter != NULL)
                {
                    if(iter->tag == LRU_tag)
                    {
                        if(iter->dirty)
                        {
                            // Write_back
                        }
                        iter->dirty = is_write;
                        iter->tag = TLB_tag;
                        iter->ppn = ppn;
                        break;
                    }
                    else
                        iter = iter->next;
                }
                //update lru
                LRU_iter = look->seqeunce;
                int my_tag = 0;
                int my_valid = 0;
                int prev_tag = TLB_tag;
                int prev_valid = 1;
                while (LRU_iter != NULL)
                {   
                    my_tag = LRU_iter->tag;
                    my_valid = LRU_iter->valid;
                    LRU_iter->valid = prev_valid;
                    LRU_iter->tag = prev_tag;
                    prev_tag = my_tag;
                    prev_valid = my_valid;

                    LRU_iter = LRU_iter->next;                
                }
            }
            break;
        }
        else
        {
            look = look->next;
        }
    }
}


void init_TLB(TLB* arg_TLB, int depth, int nway)
{
    TLB_row* prev_row;
    arg_TLB->nway = nway;
    for (size_t i = 0; i < depth; i++)
    {
        TLB_row* new_row = malloc(sizeof(TLB_row)+1);
        new_row->index=i;
        new_row->num_elem =0;
        new_row->head = malloc(sizeof(elem)+1);
        new_row->head->valid = 0;
        elem* prev = new_row->head;
        for (size_t j = 1; j < nway; j++)
        {
            elem* new_elem= malloc(sizeof(elem)+1);
            new_elem->valid=0;

            prev->next = new_elem;
            prev = new_elem;
        }
        prev->next = NULL;

        new_row->seqeunce = malloc(sizeof(elem)+1);
        new_row->seqeunce->valid = 0;
        prev = new_row->seqeunce;
        for (size_t j = 1; j < nway; j++)
        {
            elem* new_elem= malloc(sizeof(elem));
            new_elem->valid=0;

            prev->next = new_elem;
            prev = new_elem;
        }
        prev->next = NULL;


        if(i==0)
        {
            arg_TLB->head = new_row;
            prev_row = new_row;
            prev_row->next = NULL;
        }
        else
        {
            prev_row->next = new_row;
            prev_row = new_row;
            prev_row->next = NULL;
        }

    }
    
}

uint32_t walk_pte_and_set_dirty(addr_t addr)
{

    addr_t L1PTN = addr & 0xffc00000;
    addr_t L2PTN =  addr & 0x003ff000;

    L1PTN = L1PTN>>22;
    L2PTN = L2PTN>>12;

    addr_t base_reg = page_table_base_addr();
    uint32_t L1PTE = mem_read_word32(base_reg + L1PTN*4);

    uint32_t L2PTE = mem_read_word32((L1PTE & 0xfffff000) + L2PTN*4);

    //set dirty
    mem_write_word32((L1PTE & 0xfffff000) + L2PTN*4,L2PTE|0x00000002);
    return L2PTE; 
}

int main(int argc, char *argv[]) {

    int count = 1;
    char** tokens;
    int entry_num=0;
    int way_n=0;
    int TLB_dump_set = 0;
    while(count != argc-1){
    if(strcmp(argv[count], "-c") == 0){
        tokens = str_split(argv[++count],':');

        entry_num = (int)strtol(*(tokens), NULL, 10);
        way_n = (int)strtol(*(tokens+1), NULL, 10);
    }
    else if(strcmp(argv[count], "-x") == 0)
        TLB_dump_set = 1;
    count++;
    }
    init(); /* DO NOT DELETE. */
    
    
    TLB new_TLB;
    new_TLB.head= NULL;

    int depth = entry_num/way_n;
    init_TLB(&new_TLB,depth,way_n);

    int cnt=0;
    while(depth!=1)
    {
        depth = depth/2;
        cnt++;
    }

    char str[50] = {0};
    int is_write = 0;
    uint32_t L1PTN = 0;
    uint32_t L2PTN = 0;
    uint32_t str_to_int=0;
    uint32_t TLB_idx=0;
    uint32_t TLB_tag = 0;



    int reads=0;
    int writes=0;
    int tlb_hits=0;
    int tlb_misses=0;
    int page_walks=0;
    int total_page_faults=0;

    /* Open program file. */
	if(freopen(argv[argc-1], "r",stdin)==0){
			printf("File open Error!\n");
			exit(1);
	}

    while (scanf("%s", str) != -1)
    {
        str_to_int = (int) strtol(str, NULL, 16);
        if(strcmp(str,"R")==0)
        {
            reads++;
            is_write = 0;
        }
        else if(strcmp(str,"W")==0)
        {
            writes++;
            is_write = 1;
        }
        else
        {
            L1PTN = str_to_int & 0xffc00000;
            L1PTN = L1PTN>>22;
            L2PTN =  str_to_int & 0x003ff000;
            L2PTN = L2PTN>>12;

            TLB_idx = str_to_int >> 12;
            TLB_idx = TLB_idx<<12;
            TLB_idx = TLB_idx<<(20-cnt);
            TLB_idx = TLB_idx>>(32-cnt);

            TLB_tag = str_to_int >> (12+cnt);
            elem* matched_entry = lookup_TLB(&new_TLB,TLB_idx,TLB_tag);
            if(matched_entry!=NULL)
            {
                
                tlb_hits++;
                //update LRU
                TLB_row* look = new_TLB.head;
                while (look!=NULL)
                {
                    if(look->index == TLB_idx)
                    {

                        elem* LRU_iter = look->seqeunce;
                        int my_tag = 0;
                        int my_valid = 0;
                        int prev_tag = TLB_tag;
                        int prev_valid = 1;
                        while (LRU_iter != NULL)
                        {   
                            if(LRU_iter->tag==TLB_tag)
                            {
                                LRU_iter->valid = prev_valid;
                                LRU_iter->tag = prev_tag;
                                break;
                            }
                            my_tag = LRU_iter->tag;
                            my_valid = LRU_iter->valid;
                            LRU_iter->valid = prev_valid;
                            LRU_iter->tag = prev_tag;
                            prev_tag = my_tag;
                            prev_valid = my_valid;

                            LRU_iter = LRU_iter->next;                
                        }
                        break;
                    }
                    else
                    {
                        look = look->next;
                    }
                }

                if(is_write)
                {
                    if(matched_entry->dirty ==0)
                        {
                            matched_entry->dirty = 1;
                            page_walks++;
                            walk_pte_and_set_dirty(str_to_int);
                        }
                }
            }
            else
            {
                tlb_misses++;
                page_walks++;
                addr_t base_reg = page_table_base_addr();
                uint32_t L1PTE = mem_read_word32(base_reg + L1PTN*4);
                if ((L1PTE & 0x00000001) == 0)
                {
                    L1PTE = get_new_page_table_node();
                    mem_write_word32(base_reg + L1PTN*4, L1PTE | 0x0000001);
                }
                uint32_t L2PTE = mem_read_word32((L1PTE & 0xfffff000) + L2PTN*4);
                if ((L2PTE & 0x00000001) == 0)
                {
                    total_page_faults++;
                    L2PTE = get_new_physical_page();
                    if(is_write)
                    {
                        //set dirty
                        mem_write_word32((L1PTE & 0xfffff000) + L2PTN*4, L2PTE | 0x00000003);
                    }
                    else
                    {
                        mem_write_word32((L1PTE & 0xfffff000) + L2PTN*4, L2PTE | 0x00000001);
                    }

                    int ppn = L2PTE&0xfffff000;
                    ppn = (ppn >> 12);
                    add_TLB_elem(&new_TLB,TLB_idx, TLB_tag, ppn,is_write);

                    tlb_misses++;
                    page_walks++;
                }
                else
                {
                    int ppn = L2PTE&0xfffff000;
                    ppn = (ppn >> 12);
                    add_TLB_elem(&new_TLB,TLB_idx, TLB_tag, ppn,(L2PTE&0x00000002)>>1);
                }
                    
            }
        }
        memset(str,0,50);
    }
    cdump(entry_num,way_n);
    sdump(reads+writes, reads, writes, tlb_hits+tlb_misses, tlb_hits, tlb_misses,page_walks,total_page_faults);
    if(TLB_dump_set)
    {
        printf("TLB Content:\n");
        printf("-------------------------------------\n");
        printf("    ");
        for (int i = 0; i < way_n; i++)
        {
            printf("      WAY[%d]",i);
        }
        printf("\n");
        int row=0;
        TLB_row* iter = new_TLB.head;
        while (iter != NULL)
        {
            printf("SET[%d]:   ",row);
            elem* iter_elem = iter->head;
            while (iter_elem != NULL)
            {
                printf(" (v=%d tag=0x%05x ppn=0x%05x d=%d) |",iter_elem->valid,iter_elem->tag, iter_elem->ppn,iter_elem->dirty);
                iter_elem=iter_elem->next;
            }
            iter = iter->next;
            row++;
            printf("\n");
        } 
        dump_page_table_area();

    }


    return 0;
}



