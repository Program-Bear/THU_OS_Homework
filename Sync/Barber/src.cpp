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
#define WORK_SPAN 4
#define WAITCHAIR 3

//shared variable
int waiting_chair;
int idle_barber;

//mutex
dispatch_semaphore_t c_mutex, b_mutex;

//Sync
dispatch_semaphore_t barber, customer;


struct arg_struct{
	arg_struct(int _id, int _start, int _work, string _indent): id(_id), start(_start), work(_work), indent(_indent){}
	arg_struct(int _id): id(_id), start(0), work(0), indent(string("")){}
	int id;
	int start;
	int work;
	string indent;	
};

//First role
void* Barber(void* argv){
	arg_struct arg = *(arg_struct*)argv;
	int id = arg.id;
	const char* indent = arg.indent.c_str();
	sleep(arg.start);
    int worktime = arg.work;
//start
    printf("%sSTART\n", indent);
    while(1){
        bool wake_up = false;
        if (waiting_chair == WAITCHAIR){
            printf("%sSLEEP\n", indent);
            dispatch_semaphore_wait(b_mutex, DISPATCH_TIME_FOREVER);
            idle_barber++;
            dispatch_semaphore_signal(b_mutex);
            wake_up = true;
        }
        
        dispatch_semaphore_wait(customer, DISPATCH_TIME_FOREVER); // Wait for customer
        
        if (wake_up == true){
            dispatch_semaphore_wait(b_mutex, DISPATCH_TIME_FOREVER);
            idle_barber--;
            dispatch_semaphore_signal(b_mutex);
        }
        
        printf("%ssCUT\n", indent);
        sleep(worktime);
        printf("%seCUT\n", indent);
        dispatch_semaphore_signal(barber);
    }
//end
    
	return NULL;
}

//Second role
void* Consumer(void* argv){
	arg_struct arg = *(arg_struct*)argv;
	int id = arg.id;
	const char* indent = arg.indent.c_str();
	sleep(arg.start);

//start
    printf("%sSTART\n", indent);
    dispatch_semaphore_signal(customer);
    
    if (idle_barber > 0){
        goto BAR;
    }
    
    if (waiting_chair <= 0){
        printf("%sLEAVE\n", indent);
        return NULL;
    }
    
    dispatch_semaphore_wait(c_mutex, DISPATCH_TIME_FOREVER);
    waiting_chair--;
    dispatch_semaphore_signal(c_mutex);
    
    printf("%sWAIT\n", indent);
    dispatch_semaphore_wait(barber, DISPATCH_TIME_FOREVER);

    dispatch_semaphore_wait(c_mutex, DISPATCH_TIME_FOREVER);
    waiting_chair++;
    dispatch_semaphore_signal(c_mutex);

BAR:
    printf("%sBAR\n", indent);
//end
    
	return NULL;
}

int main(int argc, char** argv) {
	srand((unsigned)time(NULL));
    
//init of shared variable
    waiting_chair = WAITCHAIR;
    idle_barber = 0;
//init of mutex
    c_mutex = dispatch_semaphore_create(1);
    b_mutex = dispatch_semaphore_create(1);
    barber = dispatch_semaphore_create(0);
    customer = dispatch_semaphore_create(0);
    
//init of role name ( First is Barber, Second is Consumer )
    #define First 0
	#define Second 1

    //init of thread num
    const int f_num = 2;
    const int s_num = 10;
    
    int t_num = f_num+s_num;
    
    int f_count = 0;
    int s_count = 0;
    
    //init of thread array
    pthread_t p_f[f_num], p_s[s_num];
    
    //init of role array
    char role[] = {'F','F','S','S','S','S','S','S','S','S','S','S'};
    int st_time[] = {0, 0, 1, 2, 3, 4, 10, 11, 20, 25, 35, 60};
    
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
			printf("B%d\t", tmp_p++);
		} else if (inst[i][0] == Second){
			printf("C%d\t", tmp_c++);
		}
	}
	printf("\n");

	/* Create Producers and Consumers according to $inst*/
	int rc;
	string indent("");
	for (int i = 0; i < t_num; i++){
		if (inst[i][0] == First){
			//printf("Main: Producer %d created\n", p_count);
			rc = pthread_create(p_f + f_count, NULL, Barber, new arg_struct(f_count, inst[i][1], inst[i][2], indent));
			if (rc) printf("ERROR\n");
			f_count++;
		} else if (inst[i][0] == Second){
			//printf("Main: Consumer %d created\n", c_count);
			rc = pthread_create(p_s + s_count, NULL, Consumer, new arg_struct(s_count, inst[i][1], inst[i][2], indent));
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
