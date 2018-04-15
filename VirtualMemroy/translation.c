#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define INDEX(x,y) (x << 5)+y
#define PDBR 0xd80
#define PAGE_NUM(x) x >> 5
#define PAGE_OFFSET(x) x & 0x1f
#define PDE_INDEX(x) x >> 10
#define PTE_INDEX(x) (x >> 5) & 0x1f
#define VALID(x) (x >> 7) & 0x1
#define NUM(x) x & 0x7f
#define ADDR(x,y) (x << 5)+y

int array[3] = {1,2,3};
int hxnum(char temp){
	switch(temp){
		case 'a': return 10;
		case 'b': return 11;
		case 'c': return 12;
		case 'd': return 13;
		case 'e': return 14;
		case 'f': return 15;
		default: return temp - '0';
	
	}
}
int str2x(char * str,int len){
	//printf("str: %s, len: %d\n", str, len);
	assert(len > 0);
	int sum = 0;
	
	for (int i = 0; i < len; i++){
		//printf("char is: %c, exp is: %d, num is: %d\n",str[i], len-i+1, hxnum(str[i]));
		sum += pow(16,(len - i - 1)) * hxnum(str[i]);
	}

	return sum;
}
int get2(char* buffer, FILE* stream){
    char s;

    for(int i = 0; i < 2; i++){
        if ((s = fgetc(stream)) == -1) return -1;
        //printf("char is: %c ", s);
        buffer[i] = s;
    }
    return 0;
}
int memory[128*32];
int disk[128*32];
void load(char* file_name, int * target){
    FILE* stream;
    char temp[2];
    int row = 0;
    int column = -1;
    if((stream = fopen(file_name,"r")) != NULL){
        while(get2(temp,stream) != -1){
            column++;
            if (column == 32){
                column = 0;
                row++;
            }
            int ans = str2x(temp,2);
            //printf("row: %d, column: %d, content: %c%c, num: %#X\n", row, column,temp[0],temp[1],ans);
            target[INDEX(row,column)] = ans;
        }
    }
   
}


int main(int argc, char** argv){
	//printf("%d, %s\n",argc, argv[1]);
    load("M.txt",memory);
    load("D.txt",disk);
    
    int num = str2x(argv[1]+2,strlen(argv[1]) - 2);
    int pde_index = PDE_INDEX(num);
    int pte_index = PTE_INDEX(num);
    int offset = PAGE_OFFSET(num);
    
    //Look up page dictionary
    int pde_base = PDBR;
    int pde_address = PDBR + pde_index;
    int pde_row = PAGE_NUM(pde_address);
    int pde_column = PAGE_OFFSET(pde_address);
    int pde_content = memory[INDEX(pde_row,pde_column)];
    if(pde_content == 0x7f){
        printf("Wrong pte address!\n");
        return 0;
    }
    //Look up page table
    int pte_valid = VALID(pde_content);
    int pte_num = NUM(pde_content);
    int pte_row = pte_num;
    int pte_column = pte_index;
    int pte_content = pte_valid ? memory[INDEX(pte_row,pte_column)] : disk[INDEX(pte_row,pte_column)];
    if (pte_content == 0x7f){
        printf("Wrong page number!\n");
        return 0;
    }
    
    //Get page
    int valid = VALID(pte_content);
    int number = NUM(pte_content);
    int row = number;
    int column = offset;
    int address = ADDR(number,offset);
    int answer = -1;
    if (number != 0x7f) {
        answer = valid ? memory[INDEX(row,column)] : disk[INDEX(row,column)];
    }
    
    printf("Virtual Address %#x:\n",num);
    printf("  --> pde index:%#x pde pos:(page %#x, offset %#x) pde contents:(%#x valid %d, pfn %#x)\n", pde_index,pde_row, pde_column, pde_content,pte_valid,pte_num);
    printf("    --> pte index:%#x pte pos:(page %#x, offset %#x) pte contents:(%#x valid %d, pfn %#x)\n", pte_index, pte_row, pte_column,pte_content, valid, number);
    printf("      --> To Physical Address %#x --> ",address);
    answer == -1 ? printf("Does Not Exist A Page\n") : printf("Value: %#x\n",answer);
    
   
    
    //printf("answer: %#X\n",disk[INDEX(24,3)]);
    return 0;
}
