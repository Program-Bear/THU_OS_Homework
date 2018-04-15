import sys
if __name__ == "__main__":
    file_object = open(str(sys.argv[1]),'r')
    lines = file_object.readlines()
    file_object.close()
    strings = []
    for line in lines:
        array = line.split(' ')[2:-1]
        string = ""
        print(line)
        print(" ----> ")
        print(line.split(' '))
        for i in array:
            string = string + i
        strings.append(string)
    
    file_object = open(str(sys.argv[2]),'w+')
    for string in strings:
        file_object.write(string)
    file_object.close()
        
