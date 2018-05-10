//
//  11th.c
//  
//
//  Created by victor on 07/05/2018.
//

#include "11th.h"
//CONCEPT: Both status values and turn values are used. The status array is expanded to an integer value for each process, which is used to track that process' progress in scanning the status of other processes. The turn value is also expanded to an integer array. Its values represent the relative ordering for each pair of processes.


//INITIALIZATION:

shared int flags[NUMPROCS];
shared int turn[NUMPROCS - 1];
int index;

for (index = 0; index < (NUMPROCS); index++) {
    
    flags[index] = -1
    }
    
    
    for (index = 0; index < (NUMPROCS-1); index++) {
        turn[index] = 0;
    }
    
    ENTRY PROTOCOL (for Process i):
    /* repeat for all partners */
    for (count = 0; count < (NUMPROCS-1); count++) {
        
        flags[i] = count;
        turn[count] = i;
        
        "wait until (for all k != i, flag[k]<count) or (turn[count] != i)"
        
    }
    
    EXIT PROTOCOL (for Process i):
    /* tell everyone we are finished */
    flags[i] = -1;
