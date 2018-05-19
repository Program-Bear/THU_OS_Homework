CONCEPT: To avoid the deadlock problem of Algorithm 3, we periodically clear and reset our own flag while waiting for the other one.
INITIALIZATION:

	typedef char boolean;
	...
	shared boolean flags[n -1];
	...
	flags[i ] = FREE;
	...
	flags[j ] = FREE;
	...
ENTRY PROTOCOL (for Process i ):
	/* claim the resource */
	flags[i ] = BUSY;
-->
	/* wait if the other process is using the resource */
	while (flags[j ] == BUSY) {
		flags[i ] = FREE;
		delay a while ;
		flags[i ] = BUSY;
	}
EXIT PROTOCOL (for Process i ):
	/* release the resource */
	flags[i ] = FREE;

