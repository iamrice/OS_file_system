#ifndef INODE_H
#define INODE_H
#include <string>

class file_system
{
public:
	/***************
	requirement 1:
		a. 构造函数调用时：
			输出welcome语句，声明copyright，输出组员的姓名和学号
			读取16MB的大文件内容，从根结点开始解析文件系统
		b. 解构函数调用时，将文件系统的inode部分转为字符串存入文件中
	***************/
	file_system();
	~file_system();

private:

	/***************
	requirement 2:
		a. 维护空闲块,可选用的方法有bitmap、空闲链表法、空闲表法
		b. 在申请空间时查找空闲块，返回地址并标记为已占用
		c. 在释放空间时将块标记为空闲
	***************/
	bool free_block[2>>14];
	int applyBlock();
	void releaseBlock(int);

	/******************
	createFile:
		a. 创建inode
		b. 若磁盘空间不足或inode空间不足，则报错
		c. 在父级目录中记录文件名和inode地址
	deleteFile:
		a. 释放inode指向的块空间
		b. 删除inode
		c. 删除父级文件夹中的名称
	*******************/
	void createFile(std::string,int);
	void deleteFile(std::string);

	/******************
	createdir：
		a. 创建目录inode
		b. 目录所需要的空间不明确，先申请1个block，当空间不足时再额外申请。
	deleteDir：
		a. 递归遍历目录下的目录和文件，挨个删除
		b. 释放块空间，删除inode结点，删除父级文件夹中的名称
	*******************/
	void createDir(std::string);
	void deleteDir(std::string);

	/******************
	changedir：
		a. 使用一个属性来标记即可
	listDir：
		a. 遍历目录下的目录和文件，并输出文件名或目录名，且输出inode的各种属性
	*******************/
	void changeDir(std::string);
	void listDir();

	/******************
	copy：
		a. 找到被复制的inode，创建一个对象，复制其属性（创建时间和修改时间不复制）
		b. 在复制后路径的父级目录上记录文件名和inode地址
	cat：
		a. 找到对应文件的size和block地址，并输出。
	sum：
		a. 查看空闲块的情况，具体方法要取决于管理空闲块
	*******************/

	void copy(std::string,std::string);
	void cat(std::string);
	void sum();
};
#endif