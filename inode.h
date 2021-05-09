#ifndef INODE_H
#define INODE_H

class inode{
public:
	int size;				//以bit为单位,因为文件夹在添加文件时需要找到准确的位置去插入文件名和inode位置
	int lastModify;			//上一次修改的时间戳
	int createTime;			//创建时间（时间戳）
	bool isDirection;		//目录标识
	int directBlock[10];	//一共有2^14个block，所以使用int记录块地址是足够的
	int indirectBlock;
	bool read;
	bool write;
	bool execute;
	inode();				//新建inode
	inode(string storage);	//从内存中读取inode
	string dump();			//将inode转为字符串，用于存储
};
#endif