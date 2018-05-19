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
------------------------>
	/* wait if the other process is using the resource */
	while (flags[j ] == BUSY) {
	}
EXIT PROTOCOL (for Process i ):
	/* release the resource */
	flags[i ] = FREE;
