#define _CRT_SECURE_NO_WARNINGS
#include "file_system.h"
#include <fstream>
#include <iostream>
#include <time.h>

using namespace std;

file_system::file_system(){
	cout<<"MiniOS file system 1.0.0"<<endl;
	cout<<"Copyright (c) 2021 Taiyou Chen, Zesheng Wei and Ying Qiu. All rights reserved."<<endl;
	cout<<"Type \"help\" or \"lisence\" for more informations"<<endl;
	sysFile = "./system";
	/*fp=fopen(this->sysFile, "r");
	if (fp) {
		fclose(fp);
		openFileSystem();
	}
	else {
		createFileSystem();
	}*/
	createFileSystem();
}

file_system::~file_system(){}

void file_system::createFileSystem(){
	fp = fopen(this->sysFile, "w");
	fseek(fp,1024*16-1,SEEK_SET);
	char end=EOF;
	fwrite(&end,1,1,fp);
	fclose(fp);
	
	sys_node.blockBitMap = 6;

	//前四个block为系统空间
	setBitMap(6, 0, true);
	setBitMap(6, 1, true);
	setBitMap(6, 2, true);
	setBitMap(6, 3, true);

	sys_node.inodeBitMap = 70;

	cout<<"flag1"<<endl;
	inode root;
	root.addr = applyINode();
	root.parentAddr = root.addr;
	root.isDirection = true;
	root.size = 0;
	time(&root.createTime);
	time(&root.lastModify);
	updateINode(root);

	sys_node.rootINode = root.addr;

	fp = fopen(this->sysFile, "w");
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

unsigned short file_system::applyBlock(){
	return 4;
}

void file_system::releaseBlock(unsigned short blockIndex){
	int addr, offset;
	addr = 6 + blockIndex / 8;
	offset = blockIndex % 8;
	setBitMap(addr, offset, true);
}

unsigned short file_system::applyINode() {
	cout<<"flag2"<<endl;
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
			cout<<"new INode block "<<node.addr<<" "<<node.bitmap[0]<<node.bitmap[1]<<node.bitmap[2]<<endl;
			goto success;
		}
		else {
			cout<<"exist INode block "<<node.addr<<" "<<node.bitmap<<endl;
			for (int t = 0; t < inode_in_block; t++) {
				int i = t / 8;
				int j = t % 8;
				bool temp = node.bitmap[i] | (1 << j);
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
	cout<<"applyINode "<<nodeIndex<<" "<<offset;
	return nodeIndex;
}

inode file_system::getINode(unsigned short addr){
	fp = fopen(this->sysFile, "r+");
	inodeBitMap map;
	int offset = this->sys_node.inodeBitMap + (addr / inode_in_block) * 5;
	fseek(fp, offset, SEEK_SET);
	fread(&map, sizeof(map), 1, fp);

	inode node;

	int offset2 = map.addr * blockSize + (addr % inode_in_block) * sizeof(inode);
	fseek(fp, offset, SEEK_SET);
	fwrite(&node, sizeof(node), 1, fp);
	fclose(fp);

	return node;
}

void file_system::updateINode(inode node){
	fp = fopen(this->sysFile, "r+");
	inodeBitMap map;
	int offset = this->sys_node.inodeBitMap + (node.addr/inode_in_block)*5;
	fseek(fp, offset, SEEK_SET);
	fread(&map, sizeof(map), 1, fp);

	int offset2 = map.addr * blockSize + (node.addr % inode_in_block) * sizeof(inode);
	fseek(fp, offset, SEEK_SET);
	fwrite(&node, sizeof(node), 1, fp);

	fclose(fp);
}

void file_system::releaseINode(unsigned short addr){
	int node_addr = this->sys_node.inodeBitMap + (addr / inode_in_block) * 5;
	int offset = addr % inode_in_block;
	setBitMap(node_addr + 2 + offset / 8, offset % 8, false);
}

void file_system::loadDir(inode* dirNode,list<fileNode>* list){}

void file_system::test() {

}