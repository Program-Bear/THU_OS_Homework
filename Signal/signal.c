#include <stdio.h>
#include <signal.h>
void sigproc(){
	signal(SIGINT, sigproc);
	printf("you have pressed ctrl-c\n");
}
void quitproc(){
	printf("quit!\n");
	exit(0);
}
int main(){
	signal(SIGINT, sigproc);
	signal(SIGQUIT, quitproc);
	while(1);
}
