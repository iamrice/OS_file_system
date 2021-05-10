#ifndef INODE_H
#define INODE_H
#include <string>

/***************
inode:
	a. 记录文件的属性：大小、修改时间、创建时间、是否为目录、读写执行rwx权限
		注：size属性以bit为单位，因为对目录inode 来说，添加新文件时需要size来定位。
		注：目录记录文件的方法需要斟酌，如果频繁新建和删除文件，目录内记录文件名的位置能否回收利用
	b. 使用direct block 和 indirect block 记录文件的块地址
	c. 在系统卸载时将inode的所有内容转为字符串，记录在文件内
	b. 系统重新载入时，解析字符串，重新转为inode对象
***************/

class inode{
public:
	int size;				
	int lastModify;			
	int createTime;			
	bool isDirection;		
	int directBlock[10];	
	int indirectBlock;
	bool read;
	bool write;
	bool execute;
	inode();				
	inode(std::string);	
	std::string dump();
};
#endif