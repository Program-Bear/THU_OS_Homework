#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <dispatch/dispatch.h>
#include <assert.h>
using namespace std;

#define BUFFER_SIZE 2
#define SLEEP_SPAN 5
#define WORK_SPAN 2
#define WAITCHAIR 3

//shared variable

//mutex
dispatch_semaphore_t t_mutex;

//Sync
dispatch_semaphore_t smook, paper, match;


struct arg_struct{
	arg_struct(int _id, int _start, int _work, string _indent): id(_id), start(_start), work(_work), indent(_indent){}
	arg_struct(int _id): id(_id), start(0), work(0), indent(string("")){}
	int id;
	int start;
	int work;
	string indent;	
};

//First role
void* Smooker(void* argv){
	arg_struct arg = *(arg_struct*)argv;
	int id = arg.id;
	const char* indent = arg.indent.c_str();
	sleep(arg.start);
    int worktime = arg.work;
    
    printf("%sSTART\n", indent);
//start
    while(1){
        printf("%sWAIT\n", indent);
        switch(id){
            case 0:{ //smook
                dispatch_semaphore_wait(smook, DISPATCH_TIME_FOREVER);
                break;
            }
            case 1:{ //paper
                dispatch_semaphore_wait(paper, DISPATCH_TIME_FOREVER);
                break;
            }
            case 2:{ //match
                dispatch_semaphore_wait(match, DISPATCH_TIME_FOREVER);
                break;
            }
        }
        printf("%sDO\n", indent);
        sleep(arg.work);
        dispatch_semaphore_signal(t_mutex);
    }
//end
    
	return NULL;
}

//Second role

void* Producer(void* argv){
	arg_struct arg = *(arg_struct*)argv;
	int id = arg.id;
	const char* indent = arg.indent.c_str();
	sleep(arg.start);
    printf("%sSTART\n", indent);

//start
    while(1){
        printf("%sWAIT\n", indent);
        dispatch_semaphore_wait(t_mutex, DISPATCH_TIME_FOREVER);
        int bitmap[] = {0,0,0};
        bitmap[rand() % 3] = 1;
        while(true){
            int temp = rand() % 3;
            if (bitmap[temp] != 1){
                bitmap[temp] = 1;
                break;
            }
        }
        
        //sleep(arg.work);
        if (bitmap[0] == 1 && bitmap[1] == 1){ // SMOOK and PAPER
            printf("%sSMOOK\n", indent);
            printf("%sPAPER\n", indent);
            dispatch_semaphore_signal(match);
        }
        if (bitmap[0] == 1 && bitmap[2] == 1){ // SMOOK and MATCH
            printf("%sSMOOK\n", indent);
            printf("%sMATCH\n", indent);
            dispatch_semaphore_signal(paper);
        }
        if (bitmap[1] == 1 && bitmap[2] == 1){ // PAPER and MATCH
            printf("%sPAPER\n", indent);
            printf("%sMATCH\n", indent);
            dispatch_semaphore_signal(smook);
        }
    }
    
//end
    
	return NULL;
}

int main(int argc, char** argv) {
	srand((unsigned)time(NULL));
    
//init of shared variable

//init of mutex
    t_mutex = dispatch_semaphore_create(1);
    smook = dispatch_semaphore_create(0);
    paper = dispatch_semaphore_create(0);
    match = dispatch_semaphore_create(0);
    
//init of role name ( First is Barber, Second is Consumer )
    #define First 0
	#define Second 1

    //init of thread num
    const int f_num = 1;
    const int s_num = 3;
    
    int t_num = f_num+s_num;
    
    int f_count = 0;
    int s_count = 0;
    
    //init of thread array
    pthread_t p_f[f_num], p_s[s_num];
    
    //init of role array
    char role[] = {'F','S','S','S'};
    int st_time[] = { 0, 1, 1, 1 };
    
	/* For managed creation of t_num threads */
    int inst[t_num][3];
    for (int i = 0; i < t_num; i++){
        bool finish = false;
        if (role[i] == 'F'){
            inst[i][0] = First;
            inst[i][1] = st_time[i];
            inst[i][2] = WORK_SPAN;
            finish = true;
        }
        if (role[i] == 'S'){
            inst[i][0] = Second;
            inst[i][1] = st_time[i];
            inst[i][2] = WORK_SPAN;
            finish = true;
         }
        assert(finish == true);
        
    }

	/* Print the first line */
	int tmp_c = 0, tmp_p = 0;
	for (int i = 0; i < t_num; i++){
		if (inst[i][0] == First){
			printf("P%d\t", tmp_p++);
		} else if (inst[i][0] == Second){
			printf("S%d\t", tmp_c++);
		}
	}
	printf("\n");

	/* Create Producers and Consumers according to $inst*/
	int rc;
	string indent("");
	for (int i = 0; i < t_num; i++){
		if (inst[i][0] == First){
			//printf("Main: Producer %d created\n", p_count);
			rc = pthread_create(p_f + f_count, NULL, Producer, new arg_struct(f_count, inst[i][1], inst[i][2], indent));
			if (rc) printf("ERROR\n");
			f_count++;
		} else if (inst[i][0] == Second){
			//printf("Main: Consumer %d created\n", c_count);
			rc = pthread_create(p_s + s_count, NULL, Smooker, new arg_struct(s_count, inst[i][1], inst[i][2], indent));
			if (rc) printf("ERROR\n");
			s_count++;
		}
		indent += '\t';
	}

	/* wait until every thread finishes*/
	for (int i = 0; i < f_count; i++){
		pthread_join(p_f[i], NULL);
	}
	for (int i = 0; i < s_count; i++){
		pthread_join(p_s[i], NULL);
	}

	return 0;
}
