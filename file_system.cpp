#include "file_system.h"
#include <fstream>
#include <iostream>
#include <time.h>
using namespace std;

file_system::file_system(){
	cout<<"MiniOS file system 1.0.0 developed by 陈泰佑(201830570057),韦泽晟(),邱颖()"<<endl;
	cout<<"Copyright (c) 2021 Taiyou Chen, Zesheng Wei and Ying Qiu. All rights reserved."<<endl;
	cout<<"Type \"help\" or \"lisence\" for more informations"<<endl;
}

file_system::~file_system(){}

void file_system::createFileSystem(){
	FILE *fp=fopen(this->sysFile,"w");
	fseek(fp,1024*1024*16-1,SEEK_SET);
	char end=EOF;
	fwrite(&end,1,1,fp);	
	
	sys_node.blockBitMap = 6;
	sys_node.inodeBitMap = 70;
	sys_node.rootINode = applyINode();
	fseek(fp, 0, SEEK_SET);
	fwrite(&sys_node, sizeof(sys_node), 1, fp);

	inode root;
	root.addr = sys_node.rootINode;
	root.parentAddr = sys_node.rootINode;
	root.isDirection = true;
	root.size = 0;
	time(&root.createTime);
	time(&root.lastModify);
	updateINode(root);

	fclose(fp);
}

void file_system::openFileSystem(){
	FILE *fp = fopen(this->sysFile, "r+");
	fseek(fp, 0, SEEK_SET);
	fread(&sys_node, sizeof(sys_node), 1, fp);
	fclose(fp);
}

void file_system::setBitMap(unsigned short addr,int offset, bool bit) {
	FILE *fp = fopen(this->sysFile, "r+");
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

void file_system::loadBitMap(unsigned short addr){}

void file_system::dumpBitMap(unsigned short addr){}

unsigned short file_system::applyBlock(){}

void file_system::releaseBlock(unsigned short addr){}

unsigned short file_system::applyINode() {
	int offset = this->sys_node.inodeBitMap;
	FILE *fp = fopen(this->sysFile, "r+");
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
			goto success;
		}
		else {
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
	return nodeIndex;
}

inode file_system::getINode(unsigned short addr){
	FILE *fp = fopen(this->sysFile, "r+");
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
	FILE *fp = fopen(this->sysFile, "r+");
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