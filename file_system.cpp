#define _CRT_SECURE_NO_WARNINGS
#include "file_system.h"
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

file_system::file_system(){
	cout<<"MiniOS file system 1.0.0"<<endl;
	cout<<"Copyright (c) 2021 Taiyou Chen, Zesheng Wei and Ying Qiu. All rights reserved."<<endl;
	cout<<"Type \"help\" or \"lisence\" for more informations"<<endl;
	sysFile = "./system";

	fp=fopen(this->sysFile, "r");
	if (fp) {
		fclose(fp);
		openFileSystem();
	}
	else {
		createFileSystem();
	}
}

file_system::~file_system(){}

void file_system::createFileSystem(){
	fp = fopen(this->sysFile, "w");
	fseek(fp,1024*16-1,SEEK_SET);
	char end=EOF;
	fwrite(&end,1,1,fp);
	fclose(fp);
	
	sys_node.blockBitMap = sizeof(sysNode);

	//前7个block为系统空间
	setBitMap(6, 0, true);
	setBitMap(6, 1, true);
	setBitMap(6, 2, true);
	setBitMap(6, 3, true);
	setBitMap(6, 4, true);
	setBitMap(6, 5, true);
	setBitMap(6, 6, true);

	sys_node.inodeBitMap = sizeof(sysNode)+ block_bitmap_size;
	sys_node.blockBitMap = sizeof(sysNode);

	inode root;
	root.addr = applyINode();
	root.parentAddr = root.addr;
	root.isDirection = true;
	root.size = 0;
	updateINode(root);
	cout<<root.to_string();
	sys_node.rootINode = root.addr;

	fp = fopen(this->sysFile, "r+");
	fseek(fp, 0, SEEK_SET);
	fwrite(&sys_node, sizeof(sys_node), 1, fp);
	fclose(fp);
}

void file_system::openFileSystem(){
	fp = fopen(this->sysFile, "r+");
	fseek(fp, 0, SEEK_SET);
	fread(&sys_node, sizeof(sys_node), 1, fp);
	fclose(fp);
}

void file_system::setBitMap(unsigned short addr,int offset, bool bit) {
	fp = fopen(this->sysFile, "r+");
	fseek(fp, addr, SEEK_SET);
	unsigned char word;
	fread(&word, sizeof(word), 1, fp);
	if (bit == true) {
		word = word | (1 << (7 - offset));
	}
	else {
		word = word & (255 - 1 << (7 - offset));
	}

	fseek(fp, addr, SEEK_SET);
	fwrite(&word, sizeof(word), 1, fp);
	fclose(fp);
}

//unsigned short file_system::getAvaiableBlock(unsigned short addr, ) {}

unsigned int file_system::applyBlock(){
	if (this->sys_node.blockUsed >= block_num) {
		return 0;
	}
	int blockAd = this->sys_node.rootINode;
	int ans = -1;
	for (int i = 0; i < block_bitmap_size; i++)
	{
		blockAd += i;
		fseek(fp, blockAd, SEEK_SET);
		int n = fgetc(fp);
		int temp = 1; 
		for (int j = 1; j <= 7; j++)
		{
			if (n&temp == 0)
			{
				ans = j * (i + 1);
				goto success;
			}
			else
				temp <<= j;
		}
	}

success:
	fclose(fp);
	return ans;
}


void file_system::releaseBlock(unsigned int blockId){
	int blockAddr = blockId / 8 + this->sys_node.rootINode;
	int offset = blockId % 8;
	setBitMap(blockAddr, offset , false);
}

void file_system::releaseItem(unsigned int blockId){
	int offset = this->sys_node.blockBitMap + this->sys_node.rootINode + this->sys_node.inodeBitMap + blockId * block_size;
	fp = fopen(this->sysFile, "r+");
	fseek(fp, offset,SEEK_SET);
	for (int i = 0; i < block_num; i++)
	{
		fputc(0, fp);
	}
	fclose(fp);
}

unsigned short file_system::applyINode() {
	int offset = this->sys_node.inodeBitMap;
	fp = fopen(this->sysFile, "r+");
	inodeBitMap node;
	int nodeIndex=0;
	while (true) {
		fseek(fp, offset, SEEK_SET);
		fread(&node, sizeof(node), 1, fp);
		if (node.addr == 0) {
			unsigned short blockIndex = applyBlock();
			node.addr = blockIndex;
			memset(node.bitmap, 0, sizeof(node.bitmap));
			node.bitmap[0] = 128;
			fseek(fp, offset, SEEK_SET);
			fwrite(&node, sizeof(node), 1, fp);
			cout<<node.to_string();
			goto success;
		}
		else {
			for (int t = 0; t < inode_in_block; t++) {
				int i = t / 8;
				int j = t % 8;
				char temp = node.bitmap[i] | (1 << (7-j));
				cout<<"temp:"<<hex<<int(temp)<<" vs "<<hex<<int(node.bitmap[i])<<"\n";
				if (temp != node.bitmap[i]) {
					node.bitmap[i] = temp;
					fseek(fp, offset, SEEK_SET);
					fwrite(&node, sizeof(node), 1, fp);
					nodeIndex += t;
					goto success;
				}				
			}
		}
		offset += sizeof(node);
		nodeIndex += inode_in_block;
	}
success:
	fclose(fp);
	//cout<<"applyI	Node "<<nodeIndex<<" "<<offset<<endl;
	return nodeIndex;
}

inode file_system::getINode(unsigned short addr){
	fp = fopen(this->sysFile, "r+");
	inodeBitMap map;
	int offset = this->sys_node.inodeBitMap + (addr / inode_in_block) * sizeof(inodeBitMap);
	fseek(fp, offset, SEEK_SET);
	fread(&map, sizeof(map), 1, fp);
	cout<<map.to_string();
	inode node;

	int offset2 = map.addr * block_size + (addr % inode_in_block) * sizeof(inode);
	fseek(fp, offset2, SEEK_SET);
	fread(&node, sizeof(node), 1, fp);
	fclose(fp);

	return node;
}

void file_system::updateINode(inode node){
	fp = fopen(this->sysFile, "r+");
	inodeBitMap map;
	int offset = this->sys_node.inodeBitMap + (node.addr/inode_in_block)*sizeof(inodeBitMap);
	fseek(fp, offset, SEEK_SET);
	fread(&map, sizeof(map), 1, fp);


	int offset2 = map.addr * block_size + (node.addr % inode_in_block) * sizeof(inode);

	//cout<<"updateINode "<<map.addr<<" "<<map.bitmap<<" "<<offset2<<endl;
	fseek(fp, offset2, SEEK_SET);
	fwrite(&node, sizeof(node), 1, fp);

	fclose(fp);
}

void file_system::releaseINode(unsigned short addr){
	int node_addr = this->sys_node.inodeBitMap + (addr / inode_in_block) * sizeof(inodeBitMap);
	int offset = addr % inode_in_block;
	setBitMap(node_addr + 2 + offset / 8, offset % 8, false);
}

void file_system::loadDir(inode* dirNode,list<fileNode>* list){}

void file_system::test() {
	cout<<"inode "<<sizeof(inode)<<" "<<sizeof(inodeBitMap)<<"\n";
	inode a=getINode(0);
	cout<<a.to_string();
	inode b;
	b.addr=applyINode();
	b.parentAddr=a.addr;
	b.isDirection=false;
	updateINode(b);
	cout<<b.to_string();
}

void file_system::help()
{
	printf("command: \n\
    help       ---  show help menu \n\
    createFile ---  Create a file: createFile fileName fileSize \n\
    deleteFile ---  Delete a file: deleteFile fileName \n\
    createDir  ---  Create a directory：createDir directoryPath \n\
    deleteDir  ---  Delete a directory: deleteDir directoryPath \n\
    changeDir  ---  Change current working direcotry:changeDir path \n\
    dir        ---  List all the files and sub-directories under current working directory \n\
    cp         ---  Copy a file: cp file1 file2 \n\
    sum        ---  Display the usage of storage space \n\
    cat        ---  Print out the file contents:cat fileName \n\
    exit       ---  exit this system ");
}