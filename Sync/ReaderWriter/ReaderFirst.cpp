#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <dispatch/dispatch.h>

using namespace std;

#define N 3
#define SLEEP_SPAN 5
#define WORK_SPAN 4

dispatch_semaphore_t write_mutex, counter_mutex;
int readcount = 0;
int data = 0;


struct arg_struct{
    arg_struct(int _id, int _start, int _work, string _indent): id(_id), start(_start), work(_work), indent(_indent){}
    arg_struct(int _id): id(_id), start(0), work(0), indent(string("")){}
    int id;
    int start;
    int work;
    string indent;
};

void* reader(void* argv){
    arg_struct arg = *(arg_struct*)argv;
    int id = arg.id;
    const char* indent = arg.indent.c_str();
    
    sleep(arg.start);
    
    printf("%sSTART\n", indent);
    
    dispatch_semaphore_wait(counter_mutex, DISPATCH_TIME_FOREVER);
    printf("%saCOUNT\n", indent);
    if (readcount == 0){
        dispatch_semaphore_wait(write_mutex, DISPATCH_TIME_FOREVER);
    }
    readcount++;
    dispatch_semaphore_signal(counter_mutex);
    printf("%srCOUNT\n", indent);
    
    printf("%ssREAD%d\n", indent, data);
    sleep(SLEEP_SPAN);
    printf("%sfREAD%d\n", indent, data);
    
    dispatch_semaphore_wait(counter_mutex, DISPATCH_TIME_FOREVER);
    printf("%saCOUNT\n", indent);
    readcount--;
    if (readcount == 0){
        dispatch_semaphore_signal(write_mutex);
    }
    dispatch_semaphore_signal(counter_mutex);
    printf("%srCOUNT\n", indent);
    
    return NULL;
}

void* writer(void* argv){
    arg_struct arg = *(arg_struct*)argv;
    int id = arg.id;
    const char* indent = arg.indent.c_str();
    
    sleep(arg.start);
    
    //printf("Consumer %d Waiting for FULL: %d\n", id, full_count);
    printf("%sSTART\n", indent);
    
    dispatch_semaphore_wait(write_mutex, DISPATCH_TIME_FOREVER);
    printf("%saWRITE\n",indent);
    sleep(SLEEP_SPAN);
    data++;
    printf("%sWRITE%d\n",indent, data);
    dispatch_semaphore_signal(write_mutex);
    printf("%srWRITE\n",indent);
    
    return NULL;
}

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    
    write_mutex = dispatch_semaphore_create(1);
    counter_mutex = dispatch_semaphore_create(1);
    
    pthread_t p_reader[2 * N], p_writer[2 * N];
    
    int r_count = 0, w_count = 0;
    
#define WRITER 0
#define READER 1
    
    /* For managed creation of 2 * N threads */
    
    int st_time = 0;
    int inst[2 * N][3] = {
        {READER, st_time += 0, rand()%WORK_SPAN},
        {READER, st_time += 1, rand()%WORK_SPAN},
        {READER, st_time += 0, rand()%WORK_SPAN},
        {READER, st_time += 4, rand()%WORK_SPAN},
        {READER, st_time += 0, rand()%WORK_SPAN},
        {WRITER, st_time += 0, rand()%WORK_SPAN}
    };
    
    /* Print the first line */
    int tmp_r = 0, tmp_w = 0;
    for (int i = 0; i < 2 * N; i++){
        if (inst[i][0] == READER){
            printf("R%d\t", tmp_r++);
        } else if (inst[i][0] == WRITER){
            printf("W%d\t", tmp_w++);
        }
    }
    printf("\n");
    
    /* Create Readers and Writers according to $inst*/
    int rc;
    string indent("");
    for (int i = 0; i < 2 * N; i++){
        if (inst[i][0] == READER){
            //printf("Main: Producer %d created\n", p_count);
            rc = pthread_create(p_reader + r_count, NULL, reader, new arg_struct(r_count, inst[i][1], inst[i][2], indent));
            if (rc) printf("ERROR\n");
            r_count++;
        } else if (inst[i][0] == WRITER){
            //printf("Main: Consumer %d created\n", c_count);
            rc = pthread_create(p_writer + w_count, NULL, writer, new arg_struct(w_count, inst[i][1], inst[i][2], indent));
            if (rc) printf("ERROR\n");
            w_count++;
        }
        indent += '\t';
    }
    
    /* wait until every thread finishes*/
    for (int i = 0; i < r_count; i++){
        pthread_join(p_reader[i], NULL);
    }
    for (int i = 0; i < w_count; i++){
        pthread_join(p_writer[i], NULL);
    }
    
    return 0;
}
