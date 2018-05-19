CONCEPT: A shared variable named turn is used to keep track of whose turn it is to enter the critical section.
INITIALIZATION:

shared int turn;
...
turn = i ;
ENTRY PROTOCOL (for Process i ):
/* wait until it's our turn */
while (turn != i ) {
}
EXIT PROTOCOL (for Process i ):
/* pass the turn on */
turn = j ;
