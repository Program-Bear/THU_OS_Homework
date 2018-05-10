#include <stdio.h>
#include <unistd.h>
int main(){
	int i ;
	int cnt = 0;
	for(i = 0; i < 10; i++)
		fork();
	printf("here\n");
	
}
