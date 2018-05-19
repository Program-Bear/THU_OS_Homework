//
//  Philosophy.c
//  
//
//  Created by victor on 08/05/2018.
//

#include "Philosophy.h"


#define N 5
semaphore fork[5];
void philosopher(int i ){
    while(TRUE){
        think();
        P(mutex);
        P(fork[i]);
        P(fork[i+1] %N);
        V(mutex);
        eat();
        V(fork[i]);
        V(fork[(i+1) %N]);
    }
}
