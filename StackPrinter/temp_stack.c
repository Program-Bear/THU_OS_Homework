#include<stdio.h>
#include<stdlib.h>
#include<execinfo.h>
void get_ebp(unsigned long *ebp){
    asm volatile("mov %%ebp, %0 \r\n"
                 :"=m"(*ebp)
                 ::"memory");
}
void print_stackframe(){
    printf("----------start print stack----------\n");
    void * buffer[100] = {NULL};
    char ** trace = NULL;
    int size = backtrace(buffer,100);
    trace = backtrace_symbols(buffer,size);
    if(trace == NULL) return;
    for(int i = 0; i < size; i++){
        printf("%s\n",trace[i]);
    }
    printf("---------end print stack---------\n");
    return;
}
int factor(int i){
	print_stackframe();
	if (i == 1) return 1;
	return i * factor(i - 1);
}

int main(){
	print_stackframe();
    factor(6);
}
