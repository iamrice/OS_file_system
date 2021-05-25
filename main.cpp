#include "file_system.h"
#include <iostream>
#include<cstdio>
using namespace std;

const char* command[] = { "help","createFile","deleteFile","createDir","dir","cp","sum","cat","exit" };

int main(){
	file_system system;
	//system.test();
	
	char Command[30],name[30];
	int Com = -1,fileSize;
	bool exit = 0;
	
	while(true){
		printf("FileSystem@fileSystem:");
		scanf("%s", command);

		for (int i = 0; i < 9; i++) {
			if (!strcmp(Command, command[i])) {
				Com = i;
				break;
			}
		}
		printf("FileSystem@fileSystem:");

		switch (Com) {
			case 0:
				system.help();
				break;
			case 1:
				scanf("%s%d", name, fileSize);
				system.createFile(name, fileSize);
				break;
			case 2:
				scanf("%s", name);
				system.deleteFile(name);
				break;
			case 3:
				scanf("%s", name);
				system.createDir(name);
				break;
			case 4:
				scanf("%s", name);
				system.deleteDir(name);
				break;
			case 5:
				scanf("%s", name);
				system.changeDir(name);
				break;
			case 6://dir
				system.listDir();
				break;
			case 7:
				char name2[30];
				scanf("%s%s", name, name2);
				system.copy(name, name2);
				break;
			case 8:
				system.sum();
				break;
			case 9:
				scanf("%s", name);
				system.cat(name);
				break;
			case 10:
				exit = 1;
				break;
		}
			
		if (exit) {
			return 0;
		}

	}
}		