for thread i
do{
	flag[i] = true;
	turn = j;
	while(flag[j] && turn == j);
	
	....

	flag[i] = false;


}while(true)
