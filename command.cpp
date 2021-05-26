#include "file_system.h"
#include<regex>
using namespace std;


inode find_entry(inode dir, string file_name) {}//�ڸ�Ŀ¼������ļ������ҵ���Ӧ��Inode

inode get_path_inode(string pathname) {}//����һ��·��������·��ָ���inode�ڵ�
									  //����·�� / dir / file ����ָ��file��inode�ڵ�

inode get_path_inode_except_name(string pathname) {}//����һ��·��������·����һ��Ŀ¼��inode�ڵ㣨����ȥ�ļ���·��inode��
												   //����·�� / dir / file ����ָ��dir��inode�ڵ�

char* get_create_file_name(string pathname) {}//ר�����ڴ����ļ����ļ���, copy��������ʱ��û�õ���

char* get_name(inode dir, inode file_inode) {} // ����file_inode��dir�õ����ļ������֣�

string get_path(inode file_inode) {}//����file_inode�õ��ļ�/Ŀ¼��·��

void file_system::createFile(string path, int) {}

void file_system::deleteFile(string path) {}

void file_system::createDir(string path) {}

void file_system::deleteDir(string path) {}

void file_system::changeDir(string path) {}//�����������./:��ǰĿ¼��ʼ��/:�Ӹ�Ŀ¼��ʼ

void file_system::listDir() {}

void file_system::copy(string origin_path, string copy_path) {}

void file_system::sum() {}

void file_system::cat(string path) {}

//ͻȻ����������û���趨lastModify����Ϊ������û��Ҫ�ı��ļ����ݵĺ���

/*
�ڸ�Ŀ¼������ļ������ҵ���Ӧ��Inode
*/
inode find_entry(inode dir, string file_name) {
	inode node;
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); i != fileList.end(); it++) {
		if (it.name == file_name)
		{
			node = getINode(it.nodeAddr);
			return node;
		}
		return NULL;//�������������𣿸о��ֵֹ�
	}

}

/*����һ��·��������·��ָ���inode�ڵ�
����·�� /dir/file ����ָ��file��inode�ڵ�
*/
inode get_path_inode(string pathname) {
	inode node, dir;
	regex e1("(\/\\w{1,30})+");//�ж�/dir/file����
	regex e2("^.(\/\\w{1,30})+");//�ж�./dir/file���ͣ����ӵ�ǰĿ¼��ʼ
	if (!regex_match(s, e1) && !regex_match(s, e2) {
		cout << "Wrong path!" << endl;
			return NULL;
	}
	else if (regex_match(s, e2)) {
		dir = this->current;
			pathname.erase(0, 1);
	}
	else if (regx_match(s, e1)) {
		dir = = getINode((this->sys_node).rootINode);
	}
	regex e("\\w{1,30}");//
	smatch sm;
	string::const_iterator a = pathname.begin();
	string::const_iterator b = pathname.end();
	while (regex_search(a, b, sm, e)) {
		//cout << sm[0] << endl;
		dir = find_entry(dir, sm[0]);
		if (!dir) {//�����������ж�
			cout << "Wrong path!" << endl;
			return;
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
inode get_path_inode_except_name(string pathname) {
	size_t index = pathname.find_last_of("\/");
	pathname = pathname.erase(index);
	return get_path_inode(pathname);
}
//ר�����ڴ����ļ����ļ�������������,copy��������ʱ��û�õ���
char* get_create_file_name(string pathname) {
	size_t index = pathname.find_last_of("\/");
	string filename = pathname.substr(index);
	char file[30];
	strcpy_s(file, filename.c_str());
	return file;//������ַ����������⻹û�������������
}

//����file_inode��dir�õ����ļ������֣�
char* get_name(inode dir, inode file_inode) {
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); i != fileList.end(); it++) {
		if (file_inode.addr = it.nodeAddr)
			return  it.name;
	}
}

//����file_inode�õ��ļ�/Ŀ¼��·��
string get_path(inode file_inode) {//path��string filename��char*
	string path;
	string s = "/";
	inode dir = getINode(file_inode.parentAddr)
		s.append(get_name(dir, file_inode));
	path.insert(0, s);
	while (dir != getINode((this->sysnode).rootINode)) {
		file_inode = dir;
		dir = getINode(file_inode.parentAddr);
		s = "/";
		s.append(get_name(dir, file_inode));
		path.insert(0, s);
	}
	return path;
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
void fileSystem::createFile(string path, int size) {
	inode dir = get_path_inode_except_name(path);
	//fileNode dirFileNode;                 
	inode new_inode;
	new_inode.addr = applyINode();
	if (new_inode.addr == 0) {
		cout << "There is no space for a new file!" << endl;
		return;
	}
	int block_index;
	list<int> address_list;
	while (size--) {
		block_index = applyBlock();
		this->sys_node.blockUsed++;
		if (block_index == 0) {
			cout << "There is no space for a new file!" << endl;
			releaseINode(new_inode.addr);//�ͷŷ����inode�ڵ�
			list<int>::iterator it;//�ͷŷ����block
			for (it = address_list.begin(), it != address_list.end(); it++) {
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
	new_inode.parentAddr = dir;
	new_inode.size = size;
	if (size <= 10) {
		for (int i = 0; i < size; i++) {
			new_inode.directBlock[i] = address_list[i];
		}
	}
	else {
		for (int i = 0; i < 10; i++) {
			new_inode.directBlock[i] = address_list[i];
		}
		for (int j = 0; j < node.size - 10) {
			new_inode.indirectBlock = add_indirect_block_index(new_node.addr, j, block_index);//��������ô��ˣ�indirectBlockֻ��һ������ô�����ظ���ֵ�أ�
		}
		updateINode(new_inode);
		add_file_node(dir, new_inode);//��������һ�����⣬�����ʱ��nameû���õ�����ô������
	}
}

/*
��Ӧ�ļ���inode: node,��ӦĿ¼��inode: dir
*/
void file_system::deleteFile(string path) {
	inode node = get_path_inode(path);//�ж��Ƿ�Ϊ��ǰ�ļ�
	if (node == this->current) {
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
		for (int j = 0; j < node.size - 10) {
			indirect_address = get_indirect_block_index(node->indirectBlock, j);
			releaseBlock(indirect_address);
			releaseItem(indirect_address);
		}
	}
	char name[30];
	name = get_name(dir, node);
	releaseINode(node->addr);
	delete_file_node(dir, name);

}

/*
*/
void fileSystem::createDir(string path, int size) {//������Ҫ֧��Ƕ�׵ģ�
	inode dir;
	regex e1("(\/\\w{1,30})+");//�ж�/dir/file����
	regex e2("^.(\/\\w{1,30})+");//�ж�./dir/file���ͣ����ӵ�ǰĿ¼��ʼ
	if (!regex_match(s, e1) && !regex_match(s, e2) {
		cout << "Wrong path!" << endl;
			return;
	}
	else if (regex_match(s, e2)) {
		dir = this->current;
			pathname.erase(0, 1);
	}
	else if (regx_match(s, e1)) {
		dir = = getINode((this->sys_node).rootINode);
	}
	regex e("\\w{1,30}");//�����ʽ�������򻯵õ�����Ŀ¼
	smatch sm;
	string::const_iterator a = pathname.begin();
	string::const_iterator b = pathname.end();
	while (regex_search(a, b, sm, e)) {//
		//cout << sm[0] << endl;
		int block_index;
		if (find_entry(dir, sm[0])) {//�����һ��dir���ڣ�����������
			dir = find_entry(dir, sm[0]);
			a = sum[0].second;
			continue;
		}
		else {//��������ڣ���Ҫ����һ���µ�dir,��������ο�createFile,ֻ�ı�isDirection��size��Ϊ1
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
		}
		time(&new_inode.createTime);
		new_inode.isDirection = true;
		new_inode.parentAddr = dir;
		new_inode.size = 1;//size����Ϊ1
		new_inode.directBlock[0] = block_index;
		updateINode(new_inode);
		add_file_node(dir, new_inode);//��������һ�����⣬�����ʱ��nameû���õ�����ô������
	}//������Ŀ¼�ɹ�
	dir = new_inode;//�½���Ŀ¼��Ϊ��һ��Ƕ�׵�Ŀ¼
}
a = sm[0].second;
	}//while
}

/*
a. �ݹ����Ŀ¼�µ�Ŀ¼���ļ�������ɾ��
b. �ͷſ�ռ䣬ɾ��inode��㣬ɾ�������ļ����е�����
*/
void file_system::deleteDir(string path) {
	inode dir = get_path_inode(path);//��������path��һ����ַĿ¼������ֱ����get_path_inode�õ�Ŀ¼;
	if (this->current == dir) {
		cout << "Cannot delete current directory!" << endl;
		return;
	}
	//�����ж��Ƿ���Ŀ¼���棬���path�ǵ�ǰ·�����ִ����������ǰĿ¼������ɾ��
	string current_path = get_path(this->current)��

		if (current_path.find(path) == 0) {//���ǵ��������⣬���path��current_path��ͷ��ʼ���ִ�
			cout << "Cannot delete current directory!" << endl;
			return;
		}
	inode file_inode;
	list<fileNode> fileList = loadDir(dir);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); i != fileList.end(); it++) {
		file_inode = getINode(it.nodeAddr);
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
		for (int j = 0; j < dir.size - 10) {
			indirect_address = get_indirect_block_index(dir->indirectBlock, j);
			releaseBlock(indirect_address);
			releaseItem(indirect_address);
		}
	}
	char name[30];
	name = get_name(father_dir, dir);
	releaseINode(dir->addr);
	delete_file_node(father_dir, name);
}

/*changeDir����ƶ�����Ŀ¼
*/
void fileSystem::changeDir(string path)
{
	inode dir = get_path_inode(path);//�����pathҲ��һ��Ŀ¼·��,����path�Ƿ�Ϸ������⽻��get_path_inode����
	if (dir != NULL) {
		this->current = dir;
	}
}

void file_system::listDir() {
	inode file_inode;
	list<fileNode> fileList = loadDir(this->current);
	list<fileNode>::iterator it;
	for (it = fileList.begin(); i != fileList.end(); it++) {
		cout << it.name << "\t";
		file_inode = getINode(it.nodeAddr);
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
	new_inode.parentAddr = dir;
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
			for (it = address_list.begin(), it != address_list.end(); it++) {
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
		//����Ҫ���ļ����ݸ��ƹ�ȥ���ȶ���buffer���棬��֪���Բ���
		File* fp = fopen(origin_path, 'r');
		File* buffer;
		fread(buffer, node.size, 1, fp);
		fclose(fp);
		File *fp1 = fopen(copy_path, 'w');
		fwrite(buffer, node.size, 1, fp1);
		fclose(fp1);

	}
	if (new_inode.size <= 10) {
		for (int i = 0; i < size; i++) {
			new_inode.directBlock[i] = address_list[i];
		}
	}
	else {
		for (int i = 0; i < 10; i++) {
			new_inode.directBlock[i] = address_list[i];
		}
		for (int j = 0; j < node.size - 10) {
			new_inode.indirectBlock = add_indirect_block_index(new_node.addr, j, block_index);//��������ô��ˣ�indirectBlockֻ��һ������ô�����ظ���ֵ�أ�
		}
		updateINode(new_inode);
		add_file_node(dir, new_inode);//�����޷������ļ����ƣ�����
	}
}

void file_system::sum() {
	cout << "Total number of block:\t" << this->block_num << endl;
	cout << "Number of used block:\t" << this->sys_node.blockUsed << endl;
	cout << "Number of unused block:\t" << this->block_num - this->sys_node.blockUsed << endl;
}


void file_system::cat(string path) {
	File *fp, *buffer;
	fp = fopen(path, "r");//����ֱ��дpath������? FILE *fopen(const char *filename, const char *mode)
	inode node = get_path_inode(path);
	fread(buffer, node.size, 1, fp);
	cout << buffer << endl;//����������ӡ�𣿲����ϣ� char buffer[20];printf(%s\n, buffer);������C���԰汾��
	fclose(fp);
}
