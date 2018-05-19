import java.util.*;
public class Process {
	int r_num;
	int id;
	int[][] Resource; // For Max, All, Request
	
	Scanner scn = new Scanner(System.in);
	
	Process(int i, int r){
		id = i+1;
		r_num = r;
		Resource = new int[3][r];
		System.out.println("Process " + id + " has been established");
		while(true) {
			System.out.println("Please input data: ");
			String path = scn.nextLine();
			String[] ss = path.split(";");
			assert(ss.length == 2);
			/*
			if (ss.length != 2) {
				System.out.println("Wrong input");
				continue;
			}
			*/
			
			String[] Max = ss[0].split(",");
			String[] Alloc = ss[1].split(",");
			
			assert(Max.length == Alloc.length);
			assert(Max.length == r);
			
			/*
			if (Max.length != Alloc.length || Max.length != r) {
				System.out.println("Wrong input");
				continue;
			}
			*/
			
			for (int j = 0; j < Max.length; j++) {
				Resource[0][j] = Integer.parseInt(Max[j]);
				Resource[1][j] = Integer.parseInt(Alloc[j]);
				assert(Resource[0][j] > Resource[1][j]);
				Resource[2][j] = Resource[0][j] - Resource[1][j];
			}
			break;
		}
	}
	public int get_num(int i, int j) {
		return Resource[i][j];
	}
	public int get_max(int j) {
		return Resource[0][j];
	}
	public int get_alloc(int j) {
		return Resource[1][j];
	}
	public int get_remain(int j) {
		return Resource[2][j];
	}
	public boolean can_finish(int[] Remaining) {
		assert(Remaining.length == r_num);
		for (int i = 0; i < r_num; i++) {
			if (get_remain(i) > Remaining[i]) {
				return false;
			}
		}
		return true;
	}
}
