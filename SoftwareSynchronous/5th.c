INITIALIZATION:

	typedef char boolean;
	...
	shared boolean flags[n - 1];
	...
	flags[i] = FREE;
	...
	flags[j] = FREE;
	...
ENTRY PROTOCOL (for Process i ):
	/* wait while the other process is in its CS */
	while (flags[j ] == BUSY) {
	}
--------------------------------->
	/* claim the resource */
	flags[i ] = BUSY;
EXIT PROTOCOL (for Process i ):
	/* release the resource */
	flags[i ] = FREE;
