#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <string.h>
#include <sstream>
#include <list>
#include <time.h>
#include <iostream>


struct fileNode
{
	unsigned short nodeAddr;//2B
	char name[30];//30B

	fileNode(unsigned short addr, char* fname) {
		nodeAddr = addr;
		strcpy(name, fname);
	}
	fileNode() {
		nodeAddr = 0;
	}
};

struct sysNode
{
	unsigned short inodeBitMap;//2B,地址单位是字节
    unsigned short blockBitMap;//2B,bitmap 位置，地址单位是字节
    unsigned short rootINode;//2B 第一个inode的位置
	unsigned short blockUsed;

};

struct inodeBitMap
{
	char bitmap[4];			//4B
	unsigned short addr;	//2B
	std::string to_string() {
		std::stringstream ss;
		ss.str("");
		ss << "--------inodeBitMap---------" << "\n";
		ss << "addr " << addr << "\n";
		ss << std::hex << int(bitmap[0]) << " " << int(bitmap[1]) << " " << int(bitmap[2]) << " " << int(bitmap[3]) << "\n";
		ss << "----------------------------" << "\n";
		return ss.str();
	}

};

struct inode
{
	bool isDirection;		//1B
	unsigned short addr;			//2B，记录当前位置
	unsigned short parentAddr;		//2B
	unsigned short size;			//2B
	unsigned short indirectBlock = 0;//2B
	unsigned short directBlock[10] = { 0 };	//20B
	time_t lastModify;				//8B
	time_t createTime;				//8B
	std::string to_string() {
		std::stringstream ss;
		ss.str("");
		ss << "--------inode---------" << "\n";
		ss << "addr " << addr << "\n";
		ss << "parentAddr " << parentAddr << "\n";
		ss << "isDirection " << isDirection << "\n";
		ss << "size " << size << "\n";
		ss << "createTime " << createTime << "\n";
		ss << "lastModify " << lastModify << "\n";
		for (int i = 0; i < 10; i++) {
			if (directBlock[i] == 0)
				break;
			ss << "direct block " << i << ": " << directBlock[i] << "\n";
		}
		if (indirectBlock > 0)
			ss << "indirectBlock: " << indirectBlock << "\n";
		ss << "---------------------" << "\n";
		return ss.str();
	}

	inode() {
		time(&createTime);
		time(&lastModify);
	}
};

class file_system
{
public:
	/***************
	constructor:
		a. 构造函数调用时：
			输出welcome语句，声明copyright，输出组员的姓名和学号
			读取16MB的大文件内容，从根结点开始解析文件系统
		b. 解构函数调用时，将文件系统的inode部分转为字符串存入文件中
	***************/
	file_system();
	~file_system();

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
	void createFile(std::string, int);
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

	void copy(std::string, std::string);
	void cat(std::string);
	void sum();

	void test();
	void help();

private:
	int block_size = 1024;
	int test_size = 1024 * 16;
	int system_size = 1024 * 1024 * 16;
	int block_num = 1024 * 16;
	int block_bitmap_size = 1024 * 2;
	int inode_in_block = 21;
	int fileNode_in_block = 64;

	const char* sysFile;
	sysNode sys_node;
	FILE *fp;
	inode* current;
	unsigned int blockFree;//空闲块数量
	unsigned char* blockBitmap;
	/***************
	createFileSystem: 当系统第一次运行时调用，创建16MB 的文件，并初始化bitmap和根目录结点，初始化第一个inodebitmap
	openFileSystem: 当系统文件已存在时调用，载入系统内容。
	***************/
	void createFileSystem();
	void openFileSystem();

	/***************
	loadBitMap\dumpBitMap:
		a. 从addr位置开始，读取 64 个block的信息，将bitmap 转为 bool 数组
		b. 将 bool 数组转为bitmap存入文件。
	applyBlock\releaseBlock:
		a. 维护空闲块,可选用的方法有bitmap、空闲链表法、空闲表法
		b. 在申请空间时查找空闲块，返回地址并标记为已占用
		c. 在释放空间时将块标记为空闲
	***************/
	void setBitMap(unsigned short addr, int offset, bool bit);
	/*
	void loadBitMap(unsigned short addr);
	void dumpBitMap(unsigned short addr);
	*/
	unsigned int applyBlock();
	void releaseBlock(unsigned int addr);
	void releaseItem(unsigned int blockId);
	void writeItem(unsigned short blockId);

	/***************
	getINode: 根据地址在文件中读取内容，转为inode结构
	updateINode：将INode重新写入文件
	releaseINode：删除INode
	***************/
	unsigned short applyINode();
	inode* getINode(unsigned short addr);
	void updateINode(inode* node);
	void releaseINode(unsigned short blockId);


	/***************
	get_indirect_block_index:
	add_indirect_block_index:
	***************/
	int get_indirect_block_index(int addr, int block_count);
	void add_indirect_block_index(int addr, int block_count, unsigned short block_index);
	void catFile(inode* file);
	void copyItem(unsigned short src,unsigned short dst);

	/***************
	loadDir: 给定一个目录，inode，根据size去读取目录内容，整理成list。
	add_file_node:
	delete_file_node:
	***************/
	std::list<fileNode> loadDir(inode *dirNode);
	void add_file_node(inode* dirNode, fileNode new_node);
	void delete_file_node(inode* dirNode, char* file_name);



	inode* find_entry(inode* dir, std::string file_name);//在父目录里根据文件名字找到对应的Inode
	inode* get_path_inode(std::string pathname);//给出一个路径，返回路径指向的inode节点
										  //对于路径 / dir / file 返回指向file的inode节点
	inode* get_path_inode_except_name(std::string pathname);//给出一个路径，返回路径上一层目录的inode节点（即除去文件的路径inode）
													   //对于路径 / dir / file 返回指向dir的inode节点
	std::string get_create_file_name(std::string pathname);//专门用于创建文件和文件夹, copy，但是暂时还没用到过
	char* get_name(inode *dir, inode *file_inode); // 根据file_inode和dir得到该文件的名字；
	std::string get_path(inode *file_inode);//根据file_inode得到文件/目录的路径
};
#endif