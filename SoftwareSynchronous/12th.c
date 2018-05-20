CONCEPT: A process waiting to enter its critical section chooses a number. This number must be greater than all other numbers currently in use. There is a global shared array of current numbers for each process. The entering process checks all other processes sequentially, and waits for each one which has a lower number. Ties are possible; these are resolved using process IDs.

INITIALIZATION:

	typedef char boolean;
	...
	shared boolean choosing[n]
	shared int num[n];
	...
	for (j=0; j < n; j++) {
		num[j] = 0;
	}
	...
ENTRY PROTOCOL (for Process i):
	/* choose a number */
	choosing[i] = TRUE;
	num[i] = max(num[0], ..., num[n-1]) + 1;
	choosing[i] = FALSE;
	
	/* for all other processes */
	for (j=0; j < n; j++) {
	
		/* wait if the process is currently choosing */
		while (choosing[j]) {}
		
		/* wait if the process has a number and comes ahead of us */
		if ((num[j] > 0) &&
		  ((num[j] < num[i]) ||
		  (num[j] == num[i]) && (j < i))) {
			while (num[j] > 0) {}
		}
	}
		
EXIT PROTOCOL (for Process i):
	/* clear our number */
	num[i] = 0;
