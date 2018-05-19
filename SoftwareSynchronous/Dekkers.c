for thread i
do {
	flag[i] = true;
	while (flag[j] == true){
		if(turn != i){
			flag[i] = false;
			while(turn != i){}
			flag[i] = true;
		}
	}

	...
	
	turn = j;
	flag[i] = false;

}while(true)
