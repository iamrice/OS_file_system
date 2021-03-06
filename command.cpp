
#include "file_system.h"
#include <iomanip>
#include <regex>
using namespace std;

/*
在父目录里根据文件名字找到对应的Inode
*/
inode* file_system::find_entry(inode* dir, string file_name) {

	inode *node, *null_node=nullptr;
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	//cout<<fileList.size()<<endl;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		//cout<<it->name<<endl;
		if (strcmp(file_name.c_str(),it->name)==0)//$
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
inode* file_system::get_path_inode(string pathname) {

	inode *node, *dir, *null_node=nullptr;
	if (pathname[0]=='/') {
		dir = getINode((this->sys_node).rootINode);
	}else if(pathname[0]=='.' && pathname[1]=='.'){
		dir = getINode(this->current->parentAddr);
	}
	else{
		dir = this->current;
	}

	//cout<<pathname<<endl;
	regex e("\\w{1,30}");
	smatch sm;
	string::const_iterator a = pathname.begin();
	string::const_iterator b = pathname.end();
	while (regex_search(a, b, sm, e)) {
		//cout << sm[0] << endl;
		dir = find_entry(dir, sm[0]);
		if (dir==nullptr ) {//判断
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
inode* file_system::get_path_inode_except_name(string pathname) {

	size_t index = pathname.find_last_of("/");

	pathname = pathname.erase(index);
	return get_path_inode(pathname);
}
//专门用于创建文件和文件夹两个函数；,copy，但是暂时还没用到过
string file_system::get_create_file_name(string pathname) {

	size_t index = pathname.find_last_of("/");

	string filename = pathname.substr(index+1);
	return filename;//这里的字符串乱码问题还没解决！！！！！
}

//根据file_inode和dir得到该文件的名字；
char* file_system::get_name(inode* dir, inode* file_inode) {
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		if (file_inode->addr == it->nodeAddr)
			return  it->name;
	}
}

//根据file_inode得到文件/目录的路径
string file_system::get_path(inode* file_inode) {//path用string filename用char*

	string path="/";
	inode *dir = file_inode;
	while (dir->addr != (this->sys_node).rootINode) {//这里直接用地址来判断是否为同一个Inode
		file_inode = dir;
		dir = getINode(file_inode->parentAddr);
		path.insert(0, get_name(dir, file_inode) );
		path.insert(0,"/");
	}
	return path;
}

inode* file_system::createFile(string path, int size) {   
	if (size > 74 || size > block_num - this->sys_node.blockUsed) {
		return nullptr;
	}

	inode* new_inode=new inode();
	new_inode->addr = applyINode();
	if (new_inode->addr == 0) {
		std::cout << "There is no space for a new file!" << endl;
		return nullptr;
	}

	int block_count=0;
	while(block_count<size){
		int block_index=applyBlock();
		writeItem(block_index);
		if(block_count<10){
			new_inode->directBlock[block_count]=block_index;
		}else{
			add_indirect_block_index(new_inode->indirectBlock,block_count-10,block_index);
		}
		block_count++;
	}

	inode *dir = get_path_inode_except_name(path);
	time(&new_inode->createTime);
	new_inode->isDirection = false;
	new_inode->parentAddr = dir->addr;
	new_inode->size = size;
	
	updateINode(new_inode);

	string fname = get_create_file_name(path);
	char name[30];
	strcpy(name, fname.c_str());
	fileNode fnode(new_inode->addr, name);
	//cout<<"dir:\n"<<dir->to_string();
	add_file_node(dir, fnode);
	//cout<<"dir:\n"<<dir->to_string();


	//cout << "current: \n" << current->to_string();
	//cout << "dir: \n" << dir->to_string();
	//cout << "new file: \n" << new_inode->to_string();

	return new_inode;
}

/*
对应文件的inode: node,对应目录的inode: dir
*/
void file_system::deleteFile(string path) {
	inode *node = get_path_inode(path);//判断是否为当前文件
	if (node->addr == this->current->addr) {//这里直接用地址来判断是否为同一个Inode
		cout << "Cannot delete current file!" << endl;
		return;
	}
	inode *dir = get_path_inode_except_name(path);
	int indirect_address;
	if (node->size <= 10) {
		for (int i = 0; i < node->size; i++) {
			releaseBlock(node->directBlock[i]);
			releaseItem(node->directBlock[i]);
		}
	}
	else {
		for (int i = 0; i < 10; i++) {
			releaseBlock(node->directBlock[i]);
			releaseItem(node->directBlock[i]);
		}
		for (int j = 0; j < node->size - 10; j++) {
			indirect_address = get_indirect_block_index(node->indirectBlock, j);
			releaseBlock(indirect_address);
			releaseItem(indirect_address);
		}
	}
	char* name;
	name = get_name(dir, node);
	releaseINode(node->addr);
	delete_file_node(dir, name);

}


void file_system::createDir(string path) {   

	inode new_inode;
	new_inode.addr = applyINode();
	if (new_inode.addr == 0) {
		std::cout << "There is no space for a new file!" << endl;
		return;
	}

	inode *dir = get_path_inode_except_name(path);
	time(&new_inode.createTime);
	new_inode.isDirection = true;
	new_inode.parentAddr = dir->addr;
	new_inode.size = 0;
	
	updateINode(&new_inode);

	string fname = get_create_file_name(path);
	char name[30];
	strcpy(name, fname.c_str());
	fileNode fnode(new_inode.addr, name);
	add_file_node(dir, fnode);

	//cout << "current: \n" << current->to_string();
	//cout << "dir: \n" << dir->to_string();
	//cout << "new dir: \n" << new_inode.to_string();

	return;
}

/*
a. 递归遍历目录下的目录和文件，挨个删除
b. 释放块空间，删除inode结点，删除父级文件夹中的名称
*/
void file_system::deleteDir(string path) {
	inode *dir = get_path_inode(path);//由于这里path是一个地址目录，所以直接用get_path_inode得到目录;
	if (this->current->addr == dir->addr) {//这里直接用地址来判断是否为同一个Inode
		cout << "Cannot Delete current directory!" << endl;
		return;
	}
	//这里判断是否在目录里面，如果path是当前路径的字串，则包含当前目录，不能删除
	string current_path = get_path(this->current);
	string delete_path=get_path(dir);
	cout<<"current_path:"<<current_path<<" \ndelete path:"<<delete_path<<endl;
	//cout<<current_path.find(delete_path)<<" \nnpos:"<<std::string::npos;
	if (current_path.find(delete_path) != std::string::npos) {//考虑到重名问题，如果path是current_path从头开始的字串
		cout << "cannot delete current directory!" << endl;
		return;
	}

	inode *file_inode;
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		file_inode = getINode(it->nodeAddr);
		if (file_inode->isDirection)
			deleteDir(get_path(file_inode));
		else
			deleteFile(get_path(file_inode));
	}

	int indirect_address;
	if (dir->size <= 10) {
		for (int i = 0; i < dir->size; i++) {
			releaseBlock(dir->directBlock[i]);
			releaseItem(dir->directBlock[i]);
		}
	}
	else {
		for (int i = 0; i < 10; i++) {
			releaseBlock(dir->directBlock[i]);
			releaseItem(dir->directBlock[i]);
		}
		for (int j = 0; j < dir->size - 10; j++) {
			indirect_address = get_indirect_block_index(dir->indirectBlock, j);
			releaseBlock(indirect_address);
			releaseItem(indirect_address);
		}
	}

	inode *father_dir = getINode(dir->parentAddr);//然后释放该inode节点与block，删除父目录里的信息
	char* name;
	name = get_name(father_dir, dir);
	releaseINode(dir->addr);
	delete_file_node(father_dir, name);
}

/*changeDir命令，移动工作目录
*/
void file_system::changeDir(string path)
{
	inode *dir = get_path_inode(path);//这里的path也是一个目录路径,对于path是否合法的问题交给get_path_inode函数
	if (dir != nullptr) {
		this->current = dir;
		//cout<<"change current:\n"<<current->to_string();
	}
}

void file_system::listDir() {
	inode *file_inode;
	list<fileNode> fileList = loadDir(this->current);
	list<fileNode>::iterator it;
	//cout << "current:\n" << current->to_string();
	int len=18;
	cout << fileList.size() << " items\n";
	cout<<"----------------------------------------------------------\n";
	cout<<left<<setw(len)<<"name";
	cout<<left<<setw(len)<<"create";
	cout<<left<<setw(len)<<"type";
	cout<<left<<setw(len)<<"size";
	//cout<<left<<setw(len)<<"node-id";
	//cout<<left<<setw(len)<<"block-id";
	cout<<"\n";
	cout<<"----------------------------------------------------------\n";
	for (it = fileList.begin(); it != fileList.end(); it++) {
		cout <<left<<setw(len)<< it->name;
		file_inode = getINode(it->nodeAddr);

		struct tm time1;
		char s[40];
		time1 = *localtime(&file_inode->createTime);
		strftime(s, sizeof(s), "%Y-%m-%d %H:%M", &time1);

		cout<<left<<setw(len)<<s;
		if(file_inode->isDirection){
			cout<<left<<setw(len)<<"directory";
		}else{
			cout<<left<<setw(len)<<"file";
		}
		cout<<left<<setw(len)<<file_inode->size;
		//cout<<left<<setw(len)<<file_inode->addr;
		//cout<<left<<setw(len)<<file_inode->directBlock[0];
		cout<<"\n";
		//cout<<file_inode->to_string();
		//cout << "size: " << file_inode->size << "\t" << "create Time: " << file_inode->createTime << endl;
	}
	cout<<"----------------------------------------------------------\n";
}

/*
a. 找到被复制的inode，创建一个对象，复制其属性（创建时间和修改时间不复制）
b. 在复制后路径的父级目录上记录文件名和inode地址
原来文件的inode:node,需要复制的路径：dir
这里还有一个问题，虽然分配了block,但要怎样把block里面的内容复制过去？？？
*/
void file_system::copy(string origin_path, string copy_path) {
	inode *node = get_path_inode(origin_path);
	inode *new_inode=createFile(copy_path,node->size);

	int block_count=0;
	while(block_count<new_inode->size){
		//cout<<"block_count "<<block_count<<endl;
		if(block_count<10){
			copyItem(node->directBlock[block_count] , new_inode->directBlock[block_count]);
		}else{
			int src=get_indirect_block_index(node->indirectBlock,block_count-10);
			int dst=get_indirect_block_index(new_inode->indirectBlock,block_count-10);
			copyItem(src,dst);
		}
		block_count++;
	}
}

void file_system::sum() {
	cout << "Total number of block:\t" << this->block_num << endl;
	cout << "Number of used block:\t" << this->sys_node.blockUsed << endl;
	cout << "Number of unused block:\t" << this->block_num - this->sys_node.blockUsed << endl;
}

void file_system::cat(string path) {
	inode *file=get_path_inode(path);
	catFile(file);
}