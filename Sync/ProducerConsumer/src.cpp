#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <dispatch/dispatch.h>

using namespace std;

#define BUFFER_SIZE 2
#define SLEEP_SPAN 5
#define WORK_SPAN 4

int iflag = 0;
int oflag = 0;
//sem_t empty, full, mutex;
dispatch_semaphore_t mutex,empty,full;
int empty_count, full_count;
int data_num = 0;
int num = 0;

int buffer[BUFFER_SIZE] = {};

int p_task_done = -1;
int c_task_done = -1;	

struct arg_struct{
	arg_struct(int _id, int _start, int _work, string _indent): id(_id), start(_start), work(_work), indent(_indent){}
	arg_struct(int _id): id(_id), start(0), work(0), indent(string("")){}
	int id;
	int start;
	int work;
	string indent;	
};

void* producer(void* argv){
	arg_struct arg = *(arg_struct*)argv;
	int id = arg.id;
	const char* indent = arg.indent.c_str();

	sleep(arg.start);

	//printf("Producer %d waiting for EMPTY: %d\n", id, empty_count);
	printf("%sSTART\n", indent);
    
    //sem_wait(&empty);
    dispatch_semaphore_wait(empty, DISPATCH_TIME_FOREVER);
	
    //printf("Producer %d Waiting for MUTEX\n", id);
	printf("%saEMPTY\n", indent);

	//sem_wait(&mutex);
    dispatch_semaphore_wait(mutex, DISPATCH_TIME_FOREVER);
	//printf("Producer %d entered Critical\n", id);
	printf("%saMUTEX\n", indent);

	printf("%sENTER\n", indent);
	
	int time = rand()%SLEEP_SPAN;
	//sleep(time);
	sleep(arg.work);

	p_task_done++;
	//printf("Producer %d Produced: %d\n", id, p_task_done);
	printf("%sProd %d\n", indent, p_task_done);

	buffer[iflag] = p_task_done;

	if (empty_count == 0) printf("Error: Produce while no empty\n");
	iflag = (iflag + 1) % BUFFER_SIZE;
	empty_count--;
	full_count++;

	//printf("Producer %d exited Critical\n", id);
	printf("%sEXIT\n", indent);
	
    //sem_post(&mutex);
    dispatch_semaphore_signal(mutex);
	printf("%srMUTEX\n", indent);	
	//printf("Producer %d released MUTEX\n", id);

    //sem_post(&full);
	dispatch_semaphore_signal(full);
	printf("%srFULL\n", indent);
	//printf("Producer %d released FULL: %d\n", id, full_count);

	return NULL;
}

void* consumer(void* argv){
	arg_struct arg = *(arg_struct*)argv;
	int id = arg.id;
	const char* indent = arg.indent.c_str();

	sleep(arg.start);
    
	//printf("Consumer %d Waiting for FULL: %d\n", id, full_count);
	printf("%sSTART\n", indent);

	//sem_wait(&full);
	dispatch_semaphore_wait(full, DISPATCH_TIME_FOREVER);
    
    //printf("Consumer %d Waiting for MUTEX\n", id);
	printf("%saFULL\n", indent);

	//sem_wait(&mutex);
	dispatch_semaphore_wait(mutex, DISPATCH_TIME_FOREVER);
    
    printf("%saMUTEX\n", indent);

	//printf("Consumer %d entered Critical\n", id);
	printf("%sENTER\n", indent);	

	sleep(arg.work);
		
	++c_task_done;
	if (full_count == 0) printf("Error: Consume while no full\n");

	int tmp = buffer[oflag];
	//printf("Consumer %d consumed: %d\n", id, tmp);
	printf("%sCons %d\n", indent, tmp);	

	oflag = (oflag + 1) % BUFFER_SIZE;
	if (c_task_done != tmp) printf("Error: Consume data wrong\n");
	if (c_task_done > p_task_done) printf("Error: Over-consume!\n");

	full_count--;
	empty_count++;
		
	//printf("Consumer %d exited Critical\n", id);
	printf("%sEXIT\n", indent);
	
	//sem_post(&mutex);
	dispatch_semaphore_signal(mutex);
    
    //printf("Consumer %d released MUTEX\n", id);
	printf("%srMUTEX\n", indent);

	//sem_post(&empty);
	dispatch_semaphore_signal(empty);
    
    //printf("Consumer %d released EMPTY: %d\n", id, empty_count);
	printf("%srEMPTY\n", indent);

	return NULL;
}

int main(int argc, char** argv) {
	srand((unsigned)time(NULL));

	memset(buffer, 0, sizeof(int) * BUFFER_SIZE);

//	sem_open(&mutex, 0, 1);
//	sem_open(&empty, 0, BUFFER_SIZE);
//	sem_open(&full, 0, 0);
    
    mutex = dispatch_semaphore_create(1);
    empty = dispatch_semaphore_create(BUFFER_SIZE);
    full  = dispatch_semaphore_create(0);
    
	empty_count = BUFFER_SIZE;
	full_count = 0;

	#define N 3
	pthread_t p_consumer[2 * N], p_producer[2 * N];

	int c_count = 0, p_count = 0;

	#define PRODUCER 0
	#define CONSUMER 1

	/* For managed creation of 2 * N threads */

	#define TEST1

	#ifdef TEST1
	int st_time = 0;
	int inst[2 * N][3] = {
		/* { Consumer or Producer to be create?, 
		     When does it start to work after being created?, st_stime += N means it starts N seconcds later than the previous P/C
		     How long does it work after it enters critical zone? } */
		{CONSUMER, st_time += 0, rand()%WORK_SPAN}, 
		{CONSUMER, st_time += 1, rand()%WORK_SPAN}, 
		{CONSUMER, st_time += 0, rand()%WORK_SPAN}, 
		{PRODUCER, st_time += 4, rand()%WORK_SPAN}, 
		{PRODUCER, st_time += 0, rand()%WORK_SPAN}, 
		{PRODUCER, st_time += 0, rand()%WORK_SPAN}
	};
	#else
	int inst[2 * N][3] = {
		{PRODUCER, st_time += 0, rand()%WORK_SPAN}, 
		{PRODUCER, st_time += 1, rand()%WORK_SPAN}, 
		{PRODUCER, st_time += 0, rand()%WORK_SPAN}, 
		{CONSUMER, st_time += 4, rand()%WORK_SPAN}, 
		{CONSUMER, st_time += 0, rand()%WORK_SPAN}, 
		{CONSUMER, st_time += 0, rand()%WORK_SPAN}
	};
	#endif

	/* Print the first line */
	int tmp_c = 0, tmp_p = 0;
	for (int i = 0; i < 2 * N; i++){
		if (inst[i][0] == PRODUCER){
			printf("P%d\t", tmp_p++);
		} else if (inst[i][0] == CONSUMER){
			printf("C%d\t", tmp_c++);
		}
	}
	printf("\n");

	/* Create Producers and Consumers according to $inst*/
	int rc;
	string indent("");
	for (int i = 0; i < 2 * N; i++){
		if (inst[i][0] == PRODUCER){
			//printf("Main: Producer %d created\n", p_count);
			rc = pthread_create(p_producer + p_count, NULL, producer, new arg_struct(p_count, inst[i][1], inst[i][2], indent));
			if (rc) printf("ERROR\n");
			p_count++;
		} else if (inst[i][0] == CONSUMER){
			//printf("Main: Consumer %d created\n", c_count);
			rc = pthread_create(p_consumer + c_count, NULL, consumer, new arg_struct(c_count, inst[i][1], inst[i][2], indent));
			if (rc) printf("ERROR\n");
			c_count++;
		}
		indent += '\t';
	}

	/* wait until every thread finishes*/
	for (int i = 0; i < p_count; i++){
		pthread_join(p_producer[i], NULL);
	}
	for (int i = 0; i < c_count; i++){
		pthread_join(p_consumer[i], NULL);	
	}

	return 0;
}
