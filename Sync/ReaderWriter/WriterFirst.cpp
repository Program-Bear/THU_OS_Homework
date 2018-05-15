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
#define N 5


int buffer[BUFFER_SIZE] = {};
int g_write_index = 0;
int g_read_index = 0;

pthread_mutex_t lock;
pthread_cond_t okToRead, okToWrite;

int AR = 0; // active readers
int AW = 0; // active writers
int WR = 0; // waiting readers
int WW = 0; // waiting writers

int data = 0;

struct arg_struct{
    arg_struct(int _id, int _start, int _work, string _indent): id(_id), start(_start), work(_work), indent(_indent){}
    arg_struct(int _id): id(_id), start(0), work(0), indent(string("")){}
    int id;
    int start;
    int work;
    string indent;
};


void * reader(void * argv){
    arg_struct arg = *(arg_struct*) argv;
    int id = arg.id;
    const char* indent = arg.indent.c_str();
    
    sleep(arg.start);
    
    printf("%sSTART\n", indent);
    
    //Start Read
    pthread_mutex_lock(&lock); //Acquire the lock
    printf("%saMUTEX\n", indent);
    bool first = true;
    while((AW + WW) > 0){
        WR++;
        if (first){
            printf("%sWAIT\n", indent);
            first = false;
        }
        pthread_cond_wait(&okToRead, &lock);
        WR--;
    }
    AR++;
    
    pthread_mutex_unlock(&lock); //Release the lock
    printf("%srMUTEX\n", indent);
  
    //Read
    printf("%ssRead:%d\n", indent, data);
    sleep(arg.work);
    printf("%seRead:%d\n", indent, data);
    
    //End Read
    pthread_mutex_lock(&lock); // Acquire the lock
    printf("%saMUTEX\n", indent);
    AR--;
    if (AR == 0 && WW > 0){
        pthread_cond_signal(&okToWrite);
    }
    pthread_mutex_unlock(&lock); //Release the lock
    printf("%srMUTEX\n", indent);
    
    return NULL;
}

void * writer(void * argv){
    arg_struct arg = *(arg_struct*) argv;
    int id = arg.id;
    const char* indent = arg.indent.c_str();
    
    sleep(arg.start);
    
    printf("%sSTART\n", indent);
    
    //Start Write
    pthread_mutex_lock(&lock);
    printf("%saMUTEX\n", indent);
    bool first = true;
    while((AW + AR) > 0){
        WW++;
        if (first){
            printf("%sWAIT\n", indent);
            first = false;
        }
        pthread_cond_wait(&okToWrite, &lock);
        WW--;
    }
    AW++;
    
    pthread_mutex_unlock(&lock);
    printf("%srMUTEX\n", indent);
    
    //Write
    printf("%ssWrite:%d\n", indent, data);
    data++;
    sleep(arg.work);
    printf("%seWrite:%d\n", indent, data);
    
    //End Write
    pthread_mutex_lock(&lock);
    printf("%saMUTEX\n", indent);
    AW--;
    if (WW > 0){
        pthread_cond_signal(&okToWrite);
    }else if (WR > 0){
        pthread_cond_broadcast(&okToRead);
    }
    pthread_mutex_unlock(&lock);
    printf("%srMUTEX\n", indent);
    
    return NULL;
}

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    
    pthread_mutex_init(&lock,0);
    pthread_cond_init(&okToWrite,0);
    pthread_cond_init(&okToRead,0);
    
    pthread_t p_reader[2 * N], p_writer[2 * N];
    
    int r_count = 0, w_count = 0;
    
#define WRITER 0
#define READER 1
    
    /* For managed creation of 2 * N threads */
    
    int st_time = 0;
//#define TEST
#ifdef TEST
    int inst[2 * N][3] = {
        {READER, st_time += 0, rand()%WORK_SPAN},
        {READER, st_time += 1, rand()%WORK_SPAN},
        {READER, st_time += 0, rand()%WORK_SPAN},
        {WRITER, st_time += 4, rand()%WORK_SPAN},
        {READER, st_time += 0, rand()%WORK_SPAN},
        {WRITER, st_time += 1, rand()%WORK_SPAN},
        {READER, st_time += 0, rand()%WORK_SPAN},
        {READER, st_time += 0, rand()%WORK_SPAN},
        {WRITER, st_time += 1, rand()%WORK_SPAN},
        {READER, st_time += 0, rand()%WORK_SPAN}
    };
#else
    int inst[2 * N][3] = {
        {READER, st_time += 0, 2},
        {WRITER, st_time += 1, 2},
        {READER, st_time += 2, 6},
        {READER, st_time += 2, 6},
        {READER, st_time += 1, 6},
        {WRITER, st_time += 2, 2},
       // {READER, st_time += 0, rand()%WORK_SPAN},
       // {READER, st_time += 0, rand()%WORK_SPAN},
       // {WRITER, st_time += 1, rand()%WORK_SPAN},
       // {READER, st_time += 0, rand()%WORK_SPAN}
    };
#endif
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
            rc = pthread_create(p_reader + r_count, NULL, reader, new arg_struct(r_count, inst[i][1], inst[i][2], indent));
            if (rc) printf("ERROR\n");
            r_count++;
        } else if (inst[i][0] == WRITER){
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
