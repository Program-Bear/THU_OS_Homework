//
//  10th.c
//  
//
//  Created by victor on 07/05/2018.
//

#include "10th.h"
CONCEPT: The turn variable and status flags are used as in Dekker's algorithm for the 2-process case.
The flags now have three possible values:

WAITING for a process in the entry protocol,waiting for the resource'
ACTIVE for a process in the critical section, using the resource
IDLE for other cases.

Process priority is maintained in circular order beginning with the one holding the turn. Each process begins the entry protocol by scanning all processes from the one with the turn up to itself. These are the only processes that might have to go first if there is competition.

If the scan finds all processes idle, the process advances tentatively to the ACTIVE state. However, it is still possible that another process which started scanning later but belongs before us will also reach this state. We check one more time to be sure there are no active processes.

INITIALIZATION:

shared enum states {IDLE, WAITING, ACTIVE} flags[n -1];
shared int turn;
int index;    /* not shared! */
...
turn = 0;
...
for (index=0; index<n; index++) {
    flags[index] = IDLE;
}
ENTRY PROTOCOL (for Process i ):

repeat {
    
    /* announce that we need the resource */
    flags[i] = WAITING;
    
    /* scan processes from the one with the turn up to ourselves. */
    /* repeat if necessary until the scan finds all processes idle */
    index = turn;
    while (index != i) {
        if (flag[index] != IDLE) index = turn;
        else index = index+1 mod n;
    }
    
    /* now tentatively claim the resource */
    flags[i] = ACTIVE;
    
    /* find the first active process besides ourselves, if any */
    index = 0;
    while ((index < n) && ((index == i) || (flags[index] != ACTIVE))) {
        index = index+1;
    }
    
    /* if there were no other active processes, AND if we have the turn
     or else whoever has it is idle, then proceed.  Otherwise, repeat
     the whole sequence. */
} until ((index >= n) && ((turn == i) || (flags[turn] == IDLE)));

/* claim the turn and proceed */
turn = i;
EXIT PROTOCOL (for Process i ):

/* find a process which is not IDLE */
/* (if there are no others, we will find ourselves) */
index = turn+1 mod n;
while (flags[index] == IDLE) {
    index = index+1 mod n;
}

/* give the turn to someone that needs it, or keep it */
turn = index;

/* we're finished now */
flag[i] = IDLE;
