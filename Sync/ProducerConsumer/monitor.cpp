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



int buffer[BUFFER_SIZE] = {};
int g_write_index = 0;
int g_read_index = 0;

pthread_mutex_t lock;
pthread_cond_t consume_cond, produce_cond;

int p_task_done = -1;
int c_task_done = 1;
int empty_count, full_count;
int data_num = 0;
int num = 0;

struct arg_struct{
    arg_struct(int _id, int _start, int _work, string _indent): id(_id), start(_start), work(_work), indent(_indent){}
    arg_struct(int _id): id(_id), start(0), work(0), indent(string("")){}
    int id;
    int start;
    int work;
    string indent;
};


void * produce(void * argv){
    arg_struct arg = *(arg_struct*) argv;
    int id = arg.id;
    const char* indent = arg.indent.c_str();
    
    sleep(arg.start);
    
    printf("%sSTART\n", indent);
    
    pthread_mutex_lock(&lock);
    printf("%saMUTEX\n", indent);
    bool first = true;
    while((g_write_index + 1) % BUFFER_SIZE == g_read_index){
        if (first){
            printf("%sWAIT\n",indent);
            first = false;
        }
        pthread_cond_wait(&produce_cond, &lock);
    }
    
    printf("%ssPRO%d\n", indent, g_write_index);
    
    sleep(arg.work);
    p_task_done++;
    buffer[g_write_index] = p_task_done;
    empty_count--;
    full_count++;
    printf("%sePRO%d\n",indent, g_write_index);
    g_write_index = (g_write_index + 1) % BUFFER_SIZE;
    
    pthread_cond_signal(&consume_cond);
    
    pthread_mutex_unlock(&lock);
    printf("%srMUTEX\n",indent);
    return NULL;
}

void * consume(void * argv){
    arg_struct arg = *(arg_struct*) argv;
    int id = arg.id;
    const char* indent = arg.indent.c_str();
    
    sleep(arg.start);
    
    printf("%sSTART\n", indent);
    
    pthread_mutex_lock(&lock);
    printf("%saMUTEX\n", indent);
    bool first = true;
    while(g_read_index == g_write_index){
        if (first){
            printf("%sWAIT\n",indent);
            first = false;
        }
        pthread_cond_wait(&consume_cond, &lock);
    }
    
    printf("%ssCON%d\n", indent, g_read_index);
    sleep(arg.work);
    c_task_done++;
    buffer[g_read_index] = c_task_done;
    printf("%seCON%d\n",indent, g_read_index);
    g_read_index = (g_read_index + 1) % BUFFER_SIZE;
    empty_count--;
    full_count++;
    
    pthread_cond_signal(&produce_cond);
    pthread_mutex_unlock(&lock);
    printf("%srMUTEX\n",indent);
    return NULL;
}

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    
    memset(buffer, 0, sizeof(int) * BUFFER_SIZE);
    
    //    sem_open(&mutex, 0, 1);
    //    sem_open(&empty, 0, BUFFER_SIZE);
    //    sem_open(&full, 0, 0);
    
    //mutex = dispatch_semaphore_create(1);
    //empty = dispatch_semaphore_create(BUFFER_SIZE);
    //full  = dispatch_semaphore_create(0);
    
    pthread_cond_init(&consume_cond, 0);
    pthread_cond_init(&produce_cond, 0);
    pthread_mutex_init(&lock, 0);
    
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
            rc = pthread_create(p_producer + p_count, NULL, produce, new arg_struct(p_count, inst[i][1], inst[i][2], indent));
            if (rc) printf("ERROR\n");
            p_count++;
        } else if (inst[i][0] == CONSUMER){
            //printf("Main: Consumer %d created\n", c_count);
            rc = pthread_create(p_consumer + c_count, NULL, consume, new arg_struct(c_count, inst[i][1], inst[i][2], indent));
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

