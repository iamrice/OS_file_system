#include "file_system.h"
#include <windows.h>
#include <iostream>
#include<cstdio>
using namespace std;

const char* command[] = { "help","createFile","deleteFile","createDir","deleteDir","changeDir","dir","cp","sum","cat","exit" };

int main_process();
int test_unit_1();
int test_unit_2();
int test_unit_3();
int test_unit_4();

int main(){
	main_process();
}

/**********************
测试单元：
1. 系统初始化
2. 创建一个文件，读取文件内容
3. 创建一个文件夹
4. 将2中的文件复制到3中的文件夹，列出该目录内容
4.5 重启文件系统
5. 删除2中的文件
6. 删除3中的文件夹
**********************/

int test_unit_4(){
	file_system* sys;

	sys = new file_system();
	sys->createDir("./dir1");
	sys->createFile("./dir1/file1",1);
	sys->listDir();

	//sys->changeDir("./dir1");
	//sys->listDir();
	sys->sum();

	system("pause");

	//sys->changeDir(".");
	sys->deleteDir("/dir1");
	sys->listDir();
	sys->sum();	

	remove("./system");
}


int test_unit_3(){
	file_system* sys;

	sys = new file_system();
	sys->createFile("./file1", 3);
	sys->listDir();
	sys->sum();

	sys->deleteFile("./file1");
	sys->listDir();
	sys->sum();


	system("pause");
	remove("./system");
}

int test_unit_2(){
	file_system* sys;

	sys = new file_system();
	sys->createFile("./file1", 3);
	sys->listDir();

	sys->cat("./file1");

	sys->createDir("/dir1");

	sys->listDir();

	sys->copy("./file1","./dir1/file2");

	sys->cat("./dir1/file2");

	system("pause");
	remove("./system");
}

int test_unit_1() //已通过
{
	cout << sizeof(sysNode) << " " << sizeof(inode) << " " << sizeof(fileNode)<<"\n";
	file_system* sys;

	sys = new file_system();
	sys->createFile("./file1", 3);
	sys->listDir();

	delete sys;

	sys = new file_system();
	sys->listDir();

	system("pause");
	remove("./system");
}

int main_process(){
	file_system system;

	char Command[30],name[30];
	int Com = -1,fileSize;
	bool exit = 0;

	while(true){
		printf("FileSystem@fileSystem:");
		scanf("%s", Command);

		for (int i = 0; i < 10; i++) {
			if (!strcmp(Command, command[i])) {
				Com = i;
				break;
			}
		}

		switch (Com) {
			case 0:
				system.help();
				break;
			case 1:
				scanf("%s%d", name, &fileSize);
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
			default:
				exit=1;
				break;
		}
			
		if (exit) {
			return 0;
		}

	}
}		