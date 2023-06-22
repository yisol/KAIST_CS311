#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define TEXT_ADDR 0x400000

#define DATA_ADDR 0x10000000


typedef struct dictionary
{
	char* key;
	int data;
	struct dictionary* next;
} dictionary;

typedef struct addr_book
{
	int cnt;
	dictionary* head;
} addr_book;

void book_add(addr_book* book, char* label, int val)
{
	book->cnt++;
	dictionary* curr =book->head;
	if (curr == NULL)
	{
		dictionary* new = malloc(sizeof(dictionary)+1);
		new->key = malloc(strlen(label)+1);
		memcpy(new->key,label,strlen(label));
		new->next = NULL;
		new->data = val;
		book->head = new;

	}
	else
	{
		while(curr)
		{
			if(curr->next == NULL)
			{
				dictionary* new = malloc(sizeof(dictionary));
				new->key = malloc(strlen(label)+1);
				memcpy(new->key,label,strlen(label));
				new->next = NULL;
				new->data = val;
				curr->next = new;
				break;
			}
			else
			{
				curr = curr->next;
			}
		}
	}
}
int book_find(addr_book* book, char* label)
{
	dictionary* curr =book->head;
	while(curr)
	{
		if(strcmp(curr->key,label) ==0)
		{
			return curr->data;
		}
		else
		{
			curr = curr->next;
		}
	}
	return -1;
}




void reg_to_binary(char* reg, char* binary)
{
	char **pos =NULL;
	char tmp[5] = {0};
	if(reg[strlen(reg)-1] == ',')
	{
		strncpy(tmp,reg+1,strlen(reg)-2);
	}
	else
	{
		strncpy(tmp,reg+1,strlen(reg)-1);
	}
	long long val = strtoll(tmp, pos,10);
	for (size_t i = 0; i < 5; i++)
	{
	    binary[4-i] = (val & 1) + '0';
    	val >>= 1;
	}
}

void copy_binary_R_format(char* ret ,char* op, char* rs, char* rt, char* rd, char* shamt, char* fnct)
{
	memcpy(ret, op,6);

	char reg_binary[6] = {0};
	reg_to_binary(rs,reg_binary);
	memcpy(ret+6, reg_binary,5);

	reg_to_binary(rt,reg_binary);
	memcpy(ret+11, reg_binary,5);

	reg_to_binary(rd,reg_binary);
	memcpy(ret+16, reg_binary,5);


	char **pos =NULL;
	char shamt_binary[5] = {0};
	long long val = strtoll(shamt, pos,10);
	for (size_t i = 0; i < 5; i++)
	{
	    shamt_binary[4-i] = (val & 1) + '0';
    	val >>= 1;
	}

	memcpy(ret+21, shamt_binary,5);	

	memcpy(ret+26, fnct,6);	
}
void copy_binary_I_format(char* ret,char* op, char* rs, char* rt, char* immi)
{
	memcpy(ret, op,6);
	char reg_binary[33] = {0};

	reg_to_binary(rs,reg_binary);
	memcpy(ret+6, reg_binary,5);

	reg_to_binary(rt,reg_binary);
	memcpy(ret+11, reg_binary,5);

	char **pos =NULL;
	char immi_binary[16] = {0};

	if(immi[0] == '-')
	{
		if(strncmp(immi+1,"0x",2) ==0)
		{
			long long val = strtoll(immi+1, pos,16);
			val = ~val;
			val +=1;
		//?안전한지
		//So you must implement the assembler to read the fields as signed-extended bits.
			for (size_t i = 0; i < 16; i++)
			{
				immi_binary[15-i] = (val & 1) + '0';
				val >>= 1;
			}

		}
		else
		{
			long long val = strtoll(immi+1, pos,10);
			val = ~val;
			val +=1;
	
			for (size_t i = 0; i < 16; i++)
			{
				immi_binary[15-i] = (val & 1) + '0';
				val >>= 1;
			}

		}
	}
	else if(strncmp(immi,"0x",2) ==0)
	{
		long long val = strtoll(immi, pos,16);
		for (size_t i = 0; i < 16; i++)
		{
			immi_binary[15-i] = (val & 1) + '0';
			val >>= 1;
		}
	}
	else
	{
		long long val = strtoll(immi, pos,10);
		for (size_t i = 0; i < 16; i++)
		{
			immi_binary[15-i] = (val & 1) + '0';
			val >>= 1;
		}
	}

	memcpy(ret+16, immi_binary,16);
}


void copy_binary_I_branch_format(char* ret,char* op, char* rs, char* rt, int immi)
{
	memcpy(ret, op,6);
	char reg_binary[33] = {0};

	reg_to_binary(rs,reg_binary);
	memcpy(ret+6, reg_binary,5);

	reg_to_binary(rt,reg_binary);
	memcpy(ret+11, reg_binary,5);

	char immi_binary[16] = {0};

	for (size_t i = 0; i < 16; i++)
	{
		immi_binary[15-i] = (immi & 1) + '0';
		immi >>= 1;
		
	}

	memcpy(ret+16, immi_binary,16);
}


void copy_binary_J_format(char* ret,char* op, int addr)
{
	memcpy(ret, op,6);
	char addr_binary[26] = {0};

	for (size_t i = 0; i < 26; i++)
	{
		addr_binary[25-i] = (addr & 1) + '0';
		addr >>= 1;
		
	}


	memcpy(ret+6, addr_binary,26);

}



int main(int argc, char* argv[]){

	if(argc != 2){
		printf("Usage: ./runfile <assembly file>\n"); //Example) ./runfile /sample_input/example1.s
		printf("Example) ./runfile ./sample_input/example1.s\n");
		exit(0);
	}
	else
	{

		// To help you handle the file IO, the deafult code is provided.
		// If we use freopen, we don't need to use fscanf, fprint,..etc. 
		// You can just use scanf or printf function 
		// ** You don't need to modify this part **
		// If you are not famailiar with freopen,  you can see the following reference
		// http://www.cplusplus.com/reference/cstdio/freopen/

		//For input file read (sample_input/example*.s)

		char *file=(char *)malloc(strlen(argv[1])+3);
		strncpy(file,argv[1],strlen(argv[1]));

		if(freopen(file, "r",stdin)==0){
			printf("File open Error!\n");
			exit(1);
		}

		char *file_write=(char *)malloc(strlen(argv[1])+3);
		strncpy(file_write,argv[1],strlen(argv[1]));
		file_write[strlen(file_write)-1] ='o';
		if(freopen(file_write, "w",stdout)==0){
			printf("File open Error!\n");
			exit(1);
		}

		char str[50]={0};
		char arg1[50]={0};
		char arg2[50]={0};
		char arg3[50]={0};
		//긴거생각?
		

		// char* ret_binary = calloc(33, sizeof(char)); 둘중 머?
		char ret_binary[33] = {0};


		addr_book book_data;
		book_data.cnt=0;
		book_data.head=NULL;
		addr_book book_label;
		book_label.cnt=0;
		book_label.head=NULL;
		addr_book book_data_value;
		book_data_value.cnt=0;
		book_data_value.head = NULL;
		
		int data_sec = 0;
		int text_sec = 0;
		int PC = TEXT_ADDR+4; //byte
		int addr_for_lable = TEXT_ADDR; //byte
		int addr_for_data = DATA_ADDR; //byte


		// char* currkey = malloc(50);
		char dataname[50] = {0};
		//
		//메인에 선언된 지역변수?


		while (scanf("%s", str) != -1)
		{
			if (strcmp(str,".data")==0)
			{
				data_sec = 1;
				text_sec = 0;
				continue;
			}
			else if (strcmp(str,".text")==0)
			{
				data_sec = 0;
				text_sec = 1;
				continue;
			}
			if(data_sec)
			{
				if(str[strlen(str)-1] == ':')
				{
					memset(dataname,0,50);
					strncpy(dataname,str,strlen(str)-1);
				}
				else
				{					
					if(strcmp(str, ".word")==0)
					{
						continue;
					}
					else
					{
						char** pos = NULL;
						long long val =0;
						if(strncmp(str,"0x",2) ==0)
						{
							val = strtoll(str, pos,16);
						}
						else
						{
							val = strtoll(str, pos,10);
						}

						char binary[33] = {0};
						for (size_t i = 0; i < 32; i++)
						{
							binary[31-i] = (val & 1) + '0';
							val >>= 1;
						}

						book_add(&book_data_value,binary,0);
						book_add(&book_data,dataname,addr_for_data);
						addr_for_data+=4;
						continue;
					}
				}
			}
			else if (text_sec)
			{				
				//handle text section
				char curr_label[50] = {0};
				if(str[strlen(str)-1] == ':')
				{	
					strncpy(curr_label,str,strlen(str)-1);						
					book_add(&book_label, curr_label,addr_for_lable);
					//label
				}
				else
				{
					if(strcmp(str,"la")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);

						long long val = book_find(&book_data,arg2);
						int	b_over_16=0;
						//la이 경우만 있나?
						if((val & 0x0000FFFF) == 0x00000000)
						{
							b_over_16=0;
						}
						// else if((val & 0xFFFF0000) == 0x00000000)
						// {
						// 	b_over_16=0;
						// }
						else
						{
							b_over_16=1;
						}
						if(b_over_16)
						{
							addr_for_lable+=8;
						}
						else
						{
							addr_for_lable+=4;
						}
					}
					else if((strcmp(str,"addiu")==0)||(strcmp(str,"addu")==0)||(strcmp(str,"and")==0)||(strcmp(str,"andi")==0)||(strcmp(str,"beq")==0)||(strcmp(str,"bne")==0)||(strcmp(str,"j")==0)||(strcmp(str,"jal")==0)||(strcmp(str,"jr")==0)||(strcmp(str,"lui")==0)||(strcmp(str,"lw")==0)||(strcmp(str,"nor")==0)||(strcmp(str,"or")==0)||(strcmp(str,"ori")==0)||(strcmp(str,"sltiu")==0)||(strcmp(str,"sltu")==0)||(strcmp(str,"sll")==0)||(strcmp(str,"srl")==0)||(strcmp(str,"sw")==0)||(strcmp(str,"subu")==0))
					{
						addr_for_lable+=4;
					}
				}
			}
		}
		memset(str,0,50);
		memset(arg1,0,50);
		memset(arg2,0,50);

		size_t text_size = addr_for_lable-TEXT_ADDR;
		char text_binary[33] = {0};
		for (size_t i = 0; i < 32; i++)
		{
			text_binary[31-i] = (text_size & 1) + '0';
			text_size >>= 1;
		}
		printf("%s",text_binary);

		size_t data_size = addr_for_data-DATA_ADDR;
		char data_binary[33] = {0};
		for (size_t i = 0; i < 32; i++)
		{
			data_binary[31-i] = (data_size & 1) + '0';
			data_size >>= 1;
		}
		printf("%s",data_binary);


		if(freopen(file, "r",stdin)==0){
			printf("File open Error!\n");
			exit(1);
		}

		data_sec = 0;
		text_sec = 0;

		while (scanf("%s", str) != -1)
		{
			if (strcmp(str,".text")==0)
			{
				data_sec = 0;
				text_sec = 1;
				continue;
			}
			if(text_sec)
			{		
				if(str[strlen(str)-1] == ':')
				{
					continue;
				}
				else
				{
					if(strcmp(str,"addiu")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);
						copy_binary_I_format(ret_binary,"001001", arg2,arg1,arg3);
						PC+=4;
					}
					else if(strcmp(str,"addu")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);
						copy_binary_R_format(ret_binary,"000000", arg2,arg3,arg1, "0","100001");
						PC+=4;
					}
					else if(strcmp(str,"and")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);
						copy_binary_R_format(ret_binary,"000000", arg2,arg3,arg1, "0","100100");
						PC+=4;
					}
					else if(strcmp(str,"andi")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);
						copy_binary_I_format(ret_binary,"001100", arg2,arg1,arg3);
						PC+=4;
					}
					else if(strcmp(str,"beq")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);
						int diff_ins= book_find(&book_label,arg3)-PC;
						diff_ins = diff_ins & 0x3FFFFFFF;
						copy_binary_I_branch_format(ret_binary,"000100", arg1,arg2,diff_ins >> 2);
						PC = PC+4;
						//빼는거 문제없음? 범위
					}
					else if(strcmp(str,"bne")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);
						int diff_ins= book_find(&book_label,arg3)-PC;
						diff_ins = diff_ins & 0x3FFFFFFF;
						copy_binary_I_branch_format(ret_binary,"000101", arg1,arg2,diff_ins >> 2);
						PC = PC+4;
					}
					else if(strcmp(str,"j")==0)
					{
						scanf("%s", arg1);
						int true_address = book_find(&book_label,arg1);
						int direct = true_address & 0x0FFFFFFF;
						direct = direct >> 2;
						copy_binary_J_format(ret_binary, "000010",direct);
						PC = PC+4;
					}
					else if(strcmp(str,"jal")==0)
					{
						scanf("%s", arg1);
						int true_address = book_find(&book_label,arg1);
						int direct = true_address & 0x0FFFFFFF;
						direct = direct >> 2;
						copy_binary_J_format(ret_binary, "000011",direct);
						PC = PC+4;
					}
					else if(strcmp(str,"jr")==0)
					{
						scanf("%s", arg1);
						copy_binary_R_format(ret_binary,"000000", arg1,"$0,","$0,", "0","001000");
						PC = PC+4;
					}
					else if(strcmp(str,"lui")==0)
					{	
						scanf("%s", arg1);
						scanf("%s", arg2);
						copy_binary_I_format(ret_binary,"001111", "$0,",arg1,arg2);
						//0넣는거?
						PC+=4;
					}
					else if(strcmp(str,"lw")==0)
					{			
						scanf("%s", arg1);
						scanf("%s", arg2);
						int idx=0;
						char offset[100] ={0};
						char rs[4] = {0};
						for (size_t i = 1; i < strlen(arg2); i++)
						{
								if(arg2[i] == '(')
								{
									idx=i;
									break;
								}
						}
						strncpy(offset,arg2,idx);
						strncpy(rs,arg2+idx+1,strlen(arg2)-2-idx);
						copy_binary_I_format(ret_binary,"100011",rs,arg1,offset);
						PC+=4;
					}
					else if(strcmp(str,"la")==0)
					{
			
						scanf("%s", arg1);
						scanf("%s", arg2);

						long long val = book_find(&book_data,arg2);
						char strtmp[50];
						char strtmp1[50];
						if((val & 0x0000FFFF) == 0x00000000)
						{
							int upperbit = val & 0xFFFF0000;
							upperbit = upperbit >>16;
							sprintf(strtmp, "%d", upperbit);
							copy_binary_I_format(ret_binary,"001111","$0,", arg1,strtmp );
							PC+=4;
						}
						//data segment 무조건 16비트 이상이라 필요없음
						// else if((val & 0xFFFF0000) == 0x00000000)
						// {
						// 	val = val & 0x0000FFFF;
						// 	sprintf(strtmp, "%d", val);
						// 	copy_binary_I_format(ret_binary,"001110",arg1,arg1,strtmp);
						// 	PC+=4;
						// }
						else
						{
							int upperbit = val & 0xFFFF0000;
							upperbit = upperbit >>16;
							sprintf(strtmp, "%d", upperbit);

							int lower = val & 0x0000FFFF;
							sprintf(strtmp1, "%d", lower);

							copy_binary_I_format(ret_binary,"001111","$0,", arg1,strtmp);
							printf("%s",ret_binary);

							copy_binary_I_format(ret_binary,"001101",arg1,arg1,strtmp1);
									printf("%s",ret_binary);

							PC+=8;
							continue;
						}
					}
					else if(strcmp(str,"nor")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);

						copy_binary_R_format(ret_binary,"000000", arg2,arg3,arg1, "0","100111");
						PC+=4;
					}
					else if(strcmp(str,"ori")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);
						copy_binary_I_format(ret_binary,"001101", arg2,arg1,arg3);
						PC+=4;
					}
					else if(strcmp(str,"or")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);

						copy_binary_R_format(ret_binary,"000000", arg2,arg3,arg1, "0","100101");
						PC+=4;
					}
					else if(strcmp(str,"sltiu")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);

						copy_binary_I_format(ret_binary,"001011", arg2,arg1,arg3);
						PC+=4;
					}
					else if(strcmp(str,"sltu")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);

						copy_binary_R_format(ret_binary,"000000", arg2,arg3,arg1, "0","101011");
						PC+=4;
					}
					else if(strcmp(str,"sll")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);

						copy_binary_R_format(ret_binary,"000000", "$0,",arg2,arg1, arg3,"000000");
						PC+=4;
					}
					else if(strcmp(str,"srl")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);

						copy_binary_R_format(ret_binary,"000000", "$0,",arg2,arg1, arg3,"000010");
						PC+=4;
					}
					else if(strcmp(str,"sw")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);

						int idx=0;
						char offset[100] ={0};
						char rs[4] = {0};
						for (size_t i = 1; i < strlen(arg2); i++)
						{
								if(arg2[i] == '(')
								{
									idx=i;
									break;
								}
						}
						strncpy(offset,arg2,idx);
						strncpy(rs,arg2+idx+1,strlen(arg2)-2-idx);
						copy_binary_I_format(ret_binary,"101011",rs,arg1,offset);
						PC+=4;
					}
					else if(strcmp(str,"subu")==0)
					{
						scanf("%s", arg1);
						scanf("%s", arg2);
						scanf("%s", arg3);

						copy_binary_R_format(ret_binary,"000000", arg2,arg3, arg1,"0","100011");
						PC+=4;
					}
					
					memset(str,0,50);		
					memset(arg1,0,50);
					memset(arg2,0,50);
					memset(arg3,0,50);
					printf("%s",ret_binary);
					}
				}
			}
		



		dictionary* curr =book_data_value.head;
		while(curr)
		{
			printf("%s",curr->key);
			curr = curr->next;
		}
		
		//From now on, if you want to read string from input file, you can just use scanf function.
		

		// For output file write 
		// You can see your code's output in the sample_input/example#.o 
		// So you can check what is the difference between your output and the answer directly if you see that file
		// make test command will compare your output with the answer


		//If you use printf from now on, the result will be written to the output file.

		// printf("Hello World!\n"); 
		

	}
	return 0;
}

