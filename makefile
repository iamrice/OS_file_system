file_system.exe : file_system.cpp file_system.h inode.h storage.h main.cpp 
	g++ file_system.cpp file_system.h inode.h main.cpp -o file_system.exe

clean:
	rm file_system.exe