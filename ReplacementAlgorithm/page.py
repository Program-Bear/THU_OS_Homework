import sys

work_set = []
access_list = []
threshold = 0

def load(file_name):
    file_object = open(file_name,'r')
    items = file_object.readline().split()
    ans = []
    for item in items:
        ans.append(int(item))
    return ans

if __name__ == "__main__":
    threshold = int(sys.argv[1])
    access_list = load("access_list")
    hit_time = 0
    last_time = 0
    access_time = 0
    for i in access_list:
        print("------Start to access %d------"%i)
        find = 0
        for j in work_set:
            if j[0] == i:
                print("Page Hit!")
                hit_time = hit_time + 1
                j[1] = 1
                find = 1
                break
        if find == 0:
            print("Page Fault!")
            if (access_time - last_time >= threshold):
                print("Start to delete")
                remove_list = []
                for j in work_set:
                    #print("Now check %d"%j[0])
                    if j[1] == 0:
                        print("Delete Page %d"%j[0])
                        remove_list.append(j)
                    else:
                        j[1] = 0
                #if len(remove_list) == 0:
                #    print("Delete Page %d"%work_set[0][0])
                #    work_set.remove(work_set[0])
                for k in remove_list:
                    work_set.remove(k)
            else:
                print("Will not delete")
            
            print("Add Page %d"%i)
            work_set.append([i,1])
            last_time = access_time
        c = ""
        for i in work_set:
            c = c + " " + str(i)
        print("Now Work Set: %s"%c)
        print("------Access End------")
        access_time = access_time + 1
    
    print("Hit time: %d access time: %d Hit rate: %s"%(hit_time, access_time, str(float(hit_time) / access_time)))
