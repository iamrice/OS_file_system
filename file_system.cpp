#include "file_system.h"
#include <iostream>
using namespace std;

/******************
file_system:
	a. 读取文件缓存内容
	b. 解析缓存内容，加载文件系统
*******************/	
file_system::file_system(){
	cout<<"operating system project 1.0.0"<<endl;
	cout<<"type 'help', 'copyright' for more information."<<endl;
	return;
}

/******************
~file_system:
	a. 将文件系统转为字符串，存入文件
*******************/
file_system::~file_system(){
}

/******************
createFile:
	a. 创建inode
	b. 若磁盘空间不足或inode空间不足，则报错
	c. 在父级目录中记录文件名和inode地址
*******************/
void file_system::createFile(string path,int){}

/******************
deleteFile:
	a. 释放inode指向的块空间
	b. 删除inode
	c. 删除父级文件夹中的名称
*******************/
void file_system::deleteFile(string path){}

/******************
createDir:
*******************/
void file_system::createDir(string path){}

/******************
deleteDir:
*******************/
void file_system::deleteDir(string path){}

/******************
changeDir:
*******************/
void file_system::changeDir(string path){}

/******************
listDir:
*******************/
void file_system::listDir(){}

/******************
copy:
*******************/
void file_system::copy(string origin_path,string copy_path){}

/******************
sum:
*******************/
void file_system::sum(){}

/******************
cat:
*******************/
void file_system::cat(string path){}