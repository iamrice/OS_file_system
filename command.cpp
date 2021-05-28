#include "pch.h"
#include "file_system.h"

#include <regex>
using namespace std;


/*
�ڸ�Ŀ¼������ļ������ҵ���Ӧ��Inode
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
	return null_node;//������null_node��ʾ�Ҳ�����Ӧ��Inode
}

/*����һ��·��������·��ָ���inode�ڵ�
����·�� /dir/file ����ָ��file��inode�ڵ�
*/
inode file_system::get_path_inode(string pathname) {
	inode node, dir, null_node;
	null_node.addr = NULL;
	regex e1("(/\\w{1,30})+");//�ж�/dir/file����
	regex e2("^.(/\\w{1,30})+");//�ж�./dir/file���ͣ����ӵ�ǰĿ¼��ʼ
	if (!regex_match(pathname, e1) && !regex_match(pathname, e2)) {
		cout << "Wrong path!" << endl;
		return null_node;//����÷���ʲô��
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
		if (dir.addr == NULL) {//�ж�
			cout << "Wrong path!" << endl;
			return null_node;
		}
		a = sm[0].second;
	}
	node = dir;
	return node;
}

//���ڴ����ļ����ļ���,copy
/*����һ��·��������·����һ��Ŀ¼��inode�ڵ㣨����ȥ�ļ���·��inode��
����·�� /dir/file ����ָ��dir��inode�ڵ�
*/
inode file_system::get_path_inode_except_name(string pathname) {
	size_t index = pathname.find_last_of("/");
	pathname = pathname.erase(index);
	return get_path_inode(pathname);
}
//ר�����ڴ����ļ����ļ�������������,copy��������ʱ��û�õ���
char* file_system::get_create_file_name(string pathname) {
	size_t index = pathname.find_last_of("/");
	string filename = pathname.substr(index);
	char file[30];
	strcpy_s(file, filename.c_str());
	return file;//������ַ����������⻹û�������������
}

//����file_inode��dir�õ����ļ������֣�
char* file_system::get_name(inode dir, inode file_inode) {
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		if (file_inode.addr = it->nodeAddr)
			return  it->name;
	}
}

//����file_inode�õ��ļ�/Ŀ¼��·��
string file_system::get_path(inode file_inode) {//path��string filename��char*
	string path;
	string s = "/";
	inode dir = getINode(file_inode.parentAddr);
	s.append(get_name(dir, file_inode));
	path.insert(0, s);
	while (dir.addr != (this->sys_node).rootINode) {//����ֱ���õ�ַ���ж��Ƿ�Ϊͬһ��Inode
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

/*��·���µ�Ŀ¼inode dir����Ŀ¼�����ݣ�dirFileNode
�ȸ��ļ�����inode �� block�����ݷ���ֵ�ж��Ƿ��пռ䣬
��Ҫһ��fileNode,��¼name �� nodeAddr
��¼��Inode�Ĵ���ʱ�䣺ֱ��time(&create_Time)
��¼bool isDirection;		//1B
	unsigned short addr;			//2B����¼��ǰλ��
	unsigned short parentAddr;		//2B
	unsigned short size;			//2B,��λ��block
	unsigned short indirectBlock = 0;//2B
	unsigned short directBlock[10] = { 0 };	//20B
	time_t lastModify;				//8B
	time_t createTime;
���update
�ο�
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
			releaseINode(new_inode.addr);//�ͷŷ����inode�ڵ�
			list<int>::iterator it;//�ͷŷ����block
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
				new_inode.indirectBlock = add_indirect_block_index(new_inode.addr, i - 10, block_index);//��������ô��ˣ�indirectBlockֻ��һ������ô�����ظ���ֵ�أ�
			}
		}
		//for (int j = 0; j < new_inode.size - 10;j++) {
		//	new_inode.indirectBlock = add_indirect_block_index(new_inode.addr, j, block_index);//��������ô��ˣ�indirectBlockֻ��һ������ô�����ظ���ֵ�أ�
		//}
		updateINode(new_inode);
		char* fname = get_create_file_name(path);
		fileNode fnode(new_inode.addr, fname);
		add_file_node(dir, fnode);
	}
}

/*
��Ӧ�ļ���inode: node,��ӦĿ¼��inode: dir
*/
void file_system::deleteFile(string path) {
	inode node = get_path_inode(path);//�ж��Ƿ�Ϊ��ǰ�ļ�
	if (node.addr == this->current.addr) {//����ֱ���õ�ַ���ж��Ƿ�Ϊͬһ��Inode
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
void file_system::createDir(string pathname) {//������Ҫ֧��Ƕ�׵ģ�
	inode dir;
	regex e1("(/\\w{1,30})+");//�ж�/dir/file����
	regex e2("^.(/\\w{1,30})+");//�ж�./dir/file���ͣ����ӵ�ǰĿ¼��ʼ
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
	regex e("\\w{1,30}");//�����ʽ�������򻯵õ�����Ŀ¼
	smatch sm;
	string::const_iterator a = pathname.begin();
	string::const_iterator b = pathname.end();
	while (regex_search(a, b, sm, e)) {//
		//cout << sm[0] << endl;
		int block_index;
		if (find_entry(dir, sm[0]).addr != NULL) {//�����һ��dir���ڣ�����������
			dir = find_entry(dir, sm[0]);
			a = sm[0].second;
			continue;
		}
		//��������ڣ���Ҫ����һ���µ�dir,��������ο�createFile,ֻ�ı�isDirection��size��Ϊ1
		inode new_inode;
		new_inode.addr = applyINode();
		if (new_inode.addr == 0) {
			cout << "There is no space for a new directory!" << endl;
			return;
		}
		block_index = applyBlock();
		if (block_index == 0) {
			cout << "There is no space for a new directory!" << endl;
			releaseINode(new_inode.addr);//�ռ䲻���ǵ��ͷ��ѷ����inode
			return;
		}
		else
			this->sys_node.blockUsed++;
		time(&new_inode.createTime);
		new_inode.isDirection = true;
		new_inode.parentAddr = dir.addr;
		new_inode.size = 1;//size����Ϊ1
		new_inode.directBlock[0] = block_index;
		updateINode(new_inode);
		char* fname = get_create_file_name(pathname);
		fileNode fnode(new_inode.addr, fname);
		add_file_node(dir, fnode);
		dir = new_inode;//�½���Ŀ¼��Ϊ��һ��Ƕ�׵�Ŀ¼ //������Ŀ¼�ɹ�	
		a = sm[0].second;
	}//while
}

/*
a. �ݹ����Ŀ¼�µ�Ŀ¼���ļ�������ɾ��
b. �ͷſ�ռ䣬ɾ��inode��㣬ɾ�������ļ����е�����
*/
void file_system::deleteDir(string path) {
	inode dir = get_path_inode(path);//��������path��һ����ַĿ¼������ֱ����get_path_inode�õ�Ŀ¼;
	if (this->current.addr == dir.addr) {//����ֱ���õ�ַ���ж��Ƿ�Ϊͬһ��Inode
		cout << "Cannot delete current directory!" << endl;
		return;
	}
	//�����ж��Ƿ���Ŀ¼���棬���path�ǵ�ǰ·�����ִ����������ǰĿ¼������ɾ��
	string current_path = get_path(this->current);

	if (current_path.find(path) == 0) {//���ǵ��������⣬���path��current_path��ͷ��ʼ���ִ�
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
	inode father_dir = getINode(dir.parentAddr);//Ȼ���ͷŸ�inode�ڵ���block��ɾ����Ŀ¼�����Ϣ
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

/*changeDir����ƶ�����Ŀ¼
*/
void file_system::changeDir(string path)
{
	inode dir = get_path_inode(path);//�����pathҲ��һ��Ŀ¼·��,����path�Ƿ�Ϸ������⽻��get_path_inode����
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
a. �ҵ������Ƶ�inode������һ�����󣬸��������ԣ�����ʱ����޸�ʱ�䲻���ƣ�
b. �ڸ��ƺ�·���ĸ���Ŀ¼�ϼ�¼�ļ�����inode��ַ
ԭ���ļ���inode:node,��Ҫ���Ƶ�·����dir
���ﻹ��һ�����⣬��Ȼ������block,��Ҫ������block��������ݸ��ƹ�ȥ������
*/
void file_system::copy(string origin_path, string copy_path) {
	inode node = get_path_inode(origin_path);
	inode dir = get_path_inode_except_name(copy_path);//�����copy_path����Ҳ�����ļ����ֵ�·��
	inode new_inode;
	new_inode.addr = applyINode();
	if (new_inode.addr == 0) {
		cout << "There is no space for copying a file!" << endl;
		return;
	}
	time(&new_inode.createTime);
	new_inode.isDirection = node.isDirection;
	new_inode.parentAddr = dir.addr;
	new_inode.size = node.size;//��������Ҫ��ӵ�ַ��Ҳ���Ƿ���block
	int block_index;
	list<int> address_list;
	while (new_inode.size--) {
		block_index = applyBlock();
		this->sys_node.blockUsed++;
		if (block_index == 0) {//����ռ䲻��
			cout << "There is no space for a new file!" << endl;
			releaseINode(new_inode.addr);//�ͷŷ����inode�ڵ�
			list<int>::iterator it;//�ͷŷ����block
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
				new_inode.indirectBlock = add_indirect_block_index(new_inode.addr, i - 10, block_index);//��������ô��ˣ�indirectBlockֻ��һ������ô�����ظ���ֵ�أ�
			}
		}
		//for (int j = 0; j < new_inode.size - 10;j++) {
		//	new_inode.indirectBlock = add_indirect_block_index(new_inode.addr, j, block_index);//��������ô��ˣ�indirectBlockֻ��һ������ô�����ظ���ֵ�أ�
		//}
		updateINode(new_inode);
		char* fname = get_create_file_name(copy_path);
		fileNode fnode(new_inode.addr, fname);
		add_file_node(dir, fnode);
	}
	int len = 0;
	FILE *pIn = NULL;
	FILE *pOut = NULL;
	char buff[8192] = { 0 };//�����8192�ο�����https://blog.csdn.net/Primeprime/article/details/105515059
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
	regex e1("(/\\w{1,30})+");//�ж�/dir/file����
	regex e2("^.(/\\w{1,30})+");//�ж�./dir/file���ͣ����ӵ�ǰĿ¼��ʼ
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
