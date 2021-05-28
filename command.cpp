#include "pch.h"
#include "file_system.h"

#include <regex>
using namespace std;

/*
在父目录里根据文件名字找到对应的Inode
*/
inode file_system::find_entry(inode dir, string file_name) {

	inode node, null_node;
	null_node.addr = NULL;
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		if (it->name == file_name)
		{
			node = getINode(it->nodeAddr);
			return node;
		}
	}
	return null_node;//这里用null_node表示找不到对应的Inode
}

/*给出一个路径，返回路径指向的inode节点
对于路径 /dir/file 返回指向file的inode节点
*/
inode file_system::get_path_inode(string pathname) {

	inode node, dir, null_node;
	null_node.addr = NULL;
	regex e1("(/\\w{1,30})+");//判断/dir/file类型
	regex e2("^.(/\\w{1,30})+");//判断./dir/file类型：即从当前目录开始
	if (!regex_match(pathname, e1) && !regex_match(pathname, e2)) {

		cout << "Wrong path!" << endl;
		return null_node;//这里该返回什么？
	}
	else if (regex_match(pathname, e2)) {
		dir = this->current;
		pathname.erase(0, 1);
	}
	else if (regex_match(pathname, e1)) {
		dir = getINode((this->sys_node).rootINode);
	}
	regex e("\\w{1,30}");//
	smatch sm;
	string::const_iterator a = pathname.begin();
	string::const_iterator b = pathname.end();
	while (regex_search(a, b, sm, e)) {
		//cout << sm[0] << endl;
		dir = find_entry(dir, sm[0]);
		if (dir.addr == NULL) {//判断
			cout << "Wrong path!" << endl;
			return null_node;
		}
		a = sm[0].second;
	}
	node = dir;
	return node;
}

//用于创建文件和文件夹,copy
/*给出一个路径，返回路径上一层目录的inode节点（即除去文件的路径inode）
对于路径 /dir/file 返回指向dir的inode节点
*/
inode file_system::get_path_inode_except_name(string pathname) {

	size_t index = pathname.find_last_of("/");

	pathname = pathname.erase(index);
	return get_path_inode(pathname);
}
//专门用于创建文件和文件夹两个函数；,copy，但是暂时还没用到过
char* file_system::get_create_file_name(string pathname) {

	size_t index = pathname.find_last_of("/");

	string filename = pathname.substr(index);
	char file[30];
	strcpy_s(file, filename.c_str());
	return file;//这里的字符串乱码问题还没解决！！！！！
}

//根据file_inode和dir得到该文件的名字；
char* file_system::get_name(inode dir, inode file_inode) {
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		if (file_inode.addr = it->nodeAddr)
			return  it->name;
	}
}

//根据file_inode得到文件/目录的路径
string file_system::get_path(inode file_inode) {//path用string filename用char*
	string path;
	string s = "/";
	inode dir = getINode(file_inode.parentAddr);

	s.append(get_name(dir, file_inode));
	path.insert(0, s);
	while (dir.addr != (this->sys_node).rootINode) {//这里直接用地址来判断是否为同一个Inode

		file_inode = dir;
		dir = getINode(file_inode.parentAddr);
		s = "/";
		s.append(get_name(dir, file_inode));
		path.insert(0, s);
	}
	return path;
}

void file_system::copyfile(FILE*to, FILE*from)
{
	int c;
	while ((c = getc(from)) != EOF)
		putc(c, to);
}

/*该路径下的目录inode dir，和目录的内容：dirFileNode
先给文件分配inode 和 block，根据返回值判断是否有空间，
需要一个fileNode,记录name 和 nodeAddr
记录该Inode的创建时间：直接time(&create_Time)
记录bool isDirection;		//1B
	unsigned short addr;			//2B，记录当前位置
	unsigned short parentAddr;		//2B
	unsigned short size;			//2B,单位是block
	unsigned short indirectBlock = 0;//2B
	unsigned short directBlock[10] = { 0 };	//20B
	time_t lastModify;				//8B
	time_t createTime;
最后update
参考
cout << "inode " << sizeof(inode) << " " << sizeof(inodeBitMap) << " sysnode " << sizeof(sysNode) << "\n";
	inode a = getINode(0);
	cout << a.to_string();
	inode b;
	b.addr = applyINode();
	b.parentAddr = a.addr;
	b.isDirection = false;
	updateINode(b);
	cout << b.to_string();
*/
void file_system::createFile(string path, int size) {
	inode dir = get_path_inode_except_name(path);
	//fileNode dirFileNode;                 
	inode new_inode;
	new_inode.addr = applyINode();
	if (new_inode.addr == 0) {
		std::cout << "There is no space for a new file!" << endl;
		return;
	}
	int block_index;
	list<int> address_list;
	while (size--) {
		block_index = applyBlock();
		this->sys_node.blockUsed++;
		if (block_index == 0) {
			std::cout << "There is no space for a new file!" << endl;
			releaseINode(new_inode.addr);//释放分配的inode节点
			list<int>::iterator it;//释放分配的block
			for (it = address_list.begin(); it != address_list.end(); *it++) {
				if (*it != 0) {
					releaseBlock(*it);
					this->sys_node.blockUsed--;
				}
				else return;
			}
			return;
		}
		else {
			address_list.push_back(block_index);
		}
	}
	time(&new_inode.createTime);
	new_inode.isDirection = false;
	new_inode.parentAddr = dir.addr;
	new_inode.size = size;
	list<int>::iterator it;
	if (size <= 10) {
		int i = 0;
		for (it = address_list.begin(); it != address_list.end(); it++) {
			new_inode.directBlock[i] = *it;
			i++;
			if (i > size) break;
		}
	}
	else {
		int i = 0;
		for (it = address_list.begin(); it != address_list.end(); it++) {
			new_inode.directBlock[i] = *it;
			i++;
			if (i > 10) {
				new_inode.indirectBlock = add_indirect_block_index(new_inode.addr, i - 10, block_index);//这里可能用错了，indirectBlock只有一个，怎么可以重复赋值呢？
			}

		}
		//for (int j = 0; j < new_inode.size - 10;j++) {
		//	new_inode.indirectBlock = add_indirect_block_index(new_inode.addr, j, block_index);//这里可能用错了，indirectBlock只有一个，怎么可以重复赋值呢？
		//}
		updateINode(new_inode);
		char* fname = get_create_file_name(path);
		fileNode fnode(new_inode.addr, fname);
		add_file_node(dir, fnode);
	}
}

/*
对应文件的inode: node,对应目录的inode: dir
*/
void file_system::deleteFile(string path) {
	inode node = get_path_inode(path);//判断是否为当前文件
	if (node.addr == this->current.addr) {//这里直接用地址来判断是否为同一个Inode
		cout << "Cannot delete current file!" << endl;
		return;
	}
	inode dir = get_path_inode_except_name(path);
	int indirect_address;
	if (node.size <= 10) {
		for (int i = 0; i < node.size; i++) {
			releaseBlock(node.directBlock[i]);
			releaseItem(node.directBlock[i]);
		}
	}
	else {
		for (int i = 0; i < 10; i++) {
			releaseBlock(node.directBlock[i]);
			releaseItem(node.directBlock[i]);
		}
		for (int j = 0; j < node.size - 10; j++) {
			indirect_address = get_indirect_block_index(node.indirectBlock, j);
			releaseBlock(indirect_address);
			releaseItem(indirect_address);
		}
	}
	char* name;
	name = get_name(dir, node);
	releaseINode(node.addr);
	delete_file_node(dir, name);

}

/*
*/
void file_system::createDir(string pathname) {//这里需要支持嵌套的，

	inode dir;
	regex e1("(/\\w{1,30})+");//判断/dir/file类型
	regex e2("^.(/\\w{1,30})+");//判断./dir/file类型：即从当前目录开始
	if (!regex_match(pathname, e1) && !regex_match(pathname, e2)) {
		cout << "Wrong path!" << endl;
		return;
	}
	else if (regex_match(pathname, e2)) {
		dir = this->current;
		pathname.erase(0, 1);
	}
	else if (regex_match(pathname, e1)) {
		dir = getINode((this->sys_node).rootINode);
	}
	regex e("\\w{1,30}");//输入格式无误，正则化得到各个目录
	smatch sm;
	string::const_iterator a = pathname.begin();
	string::const_iterator b = pathname.end();
	while (regex_search(a, b, sm, e)) {//
		//cout << sm[0] << endl;
		int block_index;
		if (find_entry(dir, sm[0]).addr != NULL) {//如果下一个dir存在，继续往下找
			dir = find_entry(dir, sm[0]);
			a = sm[0].second;
			continue;
		}
		//如果不存在，则要创建一个新的dir,创建代码参考createFile,只改变isDirection，size设为1
		inode new_inode;
		new_inode.addr = applyINode();
		if (new_inode.addr == 0) {
			cout << "There is no space for a new directory!" << endl;
			return;
		}
		block_index = applyBlock();
		if (block_index == 0) {
			cout << "There is no space for a new directory!" << endl;
			releaseINode(new_inode.addr);//空间不够记得释放已分配的inode
			return;
		}
		else
			this->sys_node.blockUsed++;
		time(&new_inode.createTime);
		new_inode.isDirection = true;
		new_inode.parentAddr = dir.addr;
		new_inode.size = 1;//size设置为1
		new_inode.directBlock[0] = block_index;
		updateINode(new_inode);
		char* fname = get_create_file_name(pathname);
		fileNode fnode(new_inode.addr, fname);
		add_file_node(dir, fnode);
		dir = new_inode;//新建的目录作为下一个嵌套的目录 //创建新目录成功	
		a = sm[0].second;
	}//while
}

/*
a. 递归遍历目录下的目录和文件，挨个删除
b. 释放块空间，删除inode结点，删除父级文件夹中的名称
*/
void file_system::deleteDir(string path) {
	inode dir = get_path_inode(path);//由于这里path是一个地址目录，所以直接用get_path_inode得到目录;
	if (this->current.addr == dir.addr) {//这里直接用地址来判断是否为同一个Inode
		cout << "Cannot delete current directory!" << endl;
		return;
	}
	//这里判断是否在目录里面，如果path是当前路径的字串，则包含当前目录，不能删除
	string current_path = get_path(this->current);

	if (current_path.find(path) == 0) {//考虑到重名问题，如果path是current_path从头开始的字串
		cout << "Cannot delete current directory!" << endl;
		return;
	}
	inode file_inode;
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		file_inode = getINode(it->nodeAddr);
		if (file_inode.isDirection)
			deleteDir(get_path(file_inode));
		else
			deleteFile(get_path(file_inode));
	}
	inode father_dir = getINode(dir.parentAddr);//然后释放该inode节点与block，删除父目录里的信息
	int indirect_address;
	if (dir.size <= 10) {
		for (int i = 0; i < dir.size; i++) {
			releaseBlock(dir.directBlock[i]);
			releaseItem(dir.directBlock[i]);
		}
	}
	else {
		for (int i = 0; i < 10; i++) {
			releaseBlock(dir.directBlock[i]);
			releaseItem(dir.directBlock[i]);
		}
		for (int j = 0; j < dir.size - 10; j++) {
			indirect_address = get_indirect_block_index(dir.indirectBlock, j);
			releaseBlock(indirect_address);
			releaseItem(indirect_address);
		}
	}
	char* name;
	name = get_name(father_dir, dir);
	releaseINode(dir.addr);
	delete_file_node(father_dir, name);
}

/*changeDir命令，移动工作目录
*/
void file_system::changeDir(string path)
{
	inode dir = get_path_inode(path);//这里的path也是一个目录路径,对于path是否合法的问题交给get_path_inode函数
	if (dir.addr != NULL) {
		this->current = dir;
	}
}

void file_system::listDir() {
	inode file_inode;
	list<fileNode> fileList = loadDir(this->current);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		cout << it->name << "\t";
		file_inode = getINode(it->nodeAddr);
		cout << "size: " << file_inode.size << "\t" << "create Time: " << file_inode.createTime << endl;
	}
}

/*
a. 找到被复制的inode，创建一个对象，复制其属性（创建时间和修改时间不复制）
b. 在复制后路径的父级目录上记录文件名和inode地址
原来文件的inode:node,需要复制的路径：dir
这里还有一个问题，虽然分配了block,但要怎样把block里面的内容复制过去？？？
*/
void file_system::copy(string origin_path, string copy_path) {
	inode node = get_path_inode(origin_path);
	inode dir = get_path_inode_except_name(copy_path);//这里把copy_path当作也带有文件名字的路径
	inode new_inode;
	new_inode.addr = applyINode();
	if (new_inode.addr == 0) {
		cout << "There is no space for copying a file!" << endl;
		return;
	}
	time(&new_inode.createTime);
	new_inode.isDirection = node.isDirection;
	new_inode.parentAddr = dir.addr;
	new_inode.size = node.size;//接下来还要添加地址，也就是分配block
	int block_index;
	list<int> address_list;
	while (new_inode.size--) {
		block_index = applyBlock();
		this->sys_node.blockUsed++;
		if (block_index == 0) {//如果空间不够
			cout << "There is no space for a new file!" << endl;
			releaseINode(new_inode.addr);//释放分配的inode节点
			list<int>::iterator it;//释放分配的block
			for (it = address_list.begin(); it != address_list.end(); it++) {
				if (*it != 0) {
					releaseBlock(*it);
					this->sys_node.blockUsed--;
				}
				else return;
			}
			return;
		}
		else {
			address_list.push_back(block_index);
		}
	}
	list<int>::iterator it;
	if (new_inode.size <= 10) {
		int i = 0;
		for (it = address_list.begin(); it != address_list.end(); it++) {
			new_inode.directBlock[i] = *it;
			i++;
			if (i > new_inode.size) break;
		}
	}
	else {
		int i = 0;
		for (it = address_list.begin(); it != address_list.end(); it++) {
			new_inode.directBlock[i] = *it;
			i++;
			if (i > 10) {
				new_inode.indirectBlock = add_indirect_block_index(new_inode.addr, i - 10, block_index);//这里可能用错了，indirectBlock只有一个，怎么可以重复赋值呢？
			}
		}
		//for (int j = 0; j < new_inode.size - 10;j++) {
		//	new_inode.indirectBlock = add_indirect_block_index(new_inode.addr, j, block_index);//这里可能用错了，indirectBlock只有一个，怎么可以重复赋值呢？
		//}
		updateINode(new_inode);
		char* fname = get_create_file_name(copy_path);
		fileNode fnode(new_inode.addr, fname);
		add_file_node(dir, fnode);
	}
	int len = 0;
	FILE *pIn = NULL;
	FILE *pOut = NULL;
	char buff[8192] = { 0 };//这里的8192参考的是https://blog.csdn.net/Primeprime/article/details/105515059
	if ((pIn = fopen(origin_path.c_str(), "r")) == NULL)
	{
		printf("Open File %s Failed...\n", origin_path.c_str());
		return;
	}
	if ((pOut = fopen(copy_path.c_str(), "w")) == NULL)
	{
		printf("Copy File Failed...\n");
		return;
	}
	while ((len = fread(buff, sizeof(char), sizeof(buff), pIn)) > 0)
	{
		fwrite(buff, sizeof(char), len, pOut);
	}
	fclose(pOut);
	fclose(pIn);
}

void file_system::sum() {
	cout << "Total number of block:\t" << this->block_num << endl;
	cout << "Number of used block:\t" << this->sys_node.blockUsed << endl;
	cout << "Number of unused block:\t" << this->block_num - this->sys_node.blockUsed << endl;
}


void file_system::cat(string path) {
	regex e1("(/\\w{1,30})+");//判断/dir/file类型
	regex e2("^.(/\\w{1,30})+");//判断./dir/file类型：即从当前目录开始
	if (!regex_match(path, e1) && !regex_match(path, e2)) {
		cout << "Wrong path!" << endl;
		return;
	}
	FILE *fp;
	if ((fp = fopen(path.c_str(), "r")) == NULL)
	{
		printf("cat: can't open file %s\n", path.c_str());
		return;
	}
	else
	{
		copyfile(stdout, fp);
		fclose(fp);
	}
}
