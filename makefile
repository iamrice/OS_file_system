file_system.exe : file_system.cpp file_system.h inode.h main.cpp 
	g++ -std=c++11 file_system.cpp file_system.h inode.h main.cpp -o file_system.exe

clean:
	rm file_system.exe