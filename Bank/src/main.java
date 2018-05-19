import java.util.*;

public class main {
	static int resource;
	static int process;
	public static void main(String argv[]) {
		Scanner scn = new Scanner(System.in);
		
		System.out.println("Bank Algorithm");
		
		System.out.println("Please input the num of resource");
		resource = Integer.parseInt(scn.nextLine());
		System.out.println("Please input the num of process");
		process = Integer.parseInt(scn.nextLine());
		
		boolean[] pro_finish = new boolean[process];
		ArrayList<Process> pl = new ArrayList<Process>();
		for(int i = 0; i < process; i++) {
			pl.add(new Process(i, resource));
			pro_finish[i] = false;
		}
		
		int[] remaining = new int[resource];
		System.out.println("Please input the remaining resource");
		String[] rems = scn.nextLine().split(",");
		assert(rems.length == resource);
		for (int i = 0; i < rems.length; i++) {
			remaining[i] = Integer.parseInt(rems[i]); 
		}
		
		boolean deadlock = true;
		ArrayList<Integer> safe_queue = new ArrayList<Integer>();
		
		while(true) {
			boolean all_true = true;
			
			for(int i = 0; i < process; i++) {
				if (pro_finish[i] == false) {
					all_true = false;
					break;
				}
			}
			
			if (all_true) {
				deadlock = false;
				break;
			}
			
			boolean find = false;
			
			for (int i = 0; i < process; i++) {
				if (pro_finish[i]) {
					continue;
				}
				Process now = pl.get(i);
				if (now.can_finish(remaining)) {
					for(int j = 0; j < resource; j++) {
						remaining[j] += now.get_alloc(j);
					}
					pro_finish[i] = true;
					find = true;
					safe_queue.add(now.id);
				}
			}
			if (!find) {
				break;
			}
		}
	if (deadlock) {
		System.out.println("Dead Lock");
	}else {
		assert(safe_queue.size() == process);
		System.out.println("There is not a Dead Lock and the safe queue is as follow");
		for (int i = 0; i < safe_queue.size(); i++) {
			System.out.print(safe_queue.get(i) + " ");
		}
	}
	}
}
