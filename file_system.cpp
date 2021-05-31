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

	cout <<"root\n"<< current->to_string();

}

file_system::~file_system(){}

void file_system::createFileSystem(){
	fp = fopen(this->sysFile, "w");
	//fseek(fp,this->system_size-1,SEEK_SET);
	fseek(fp, this->test_size - 1, SEEK_SET);
	char end=EOF;
	fwrite(&end,1,1,fp);
	fclose(fp);
	
	sys_node.inodeBitMap = sizeof(sysNode)+ block_bitmap_size;
	sys_node.blockBitMap = sizeof(sysNode);

	//前7个block为系统空间
	setBitMap(sys_node.blockBitMap, 0, true);
	setBitMap(sys_node.blockBitMap, 1, true);
	setBitMap(sys_node.blockBitMap, 2, true);
	setBitMap(sys_node.blockBitMap, 3, true);
	setBitMap(sys_node.blockBitMap, 4, true);
	setBitMap(sys_node.blockBitMap, 5, true);
	setBitMap(sys_node.blockBitMap, 6, true);
	sys_node.blockUsed = 7;

	inode root;
	root.addr = applyINode();
	root.parentAddr = root.addr;
	root.isDirection = true;
	root.size = 0;
	updateINode(&root);
	//cout<<root.to_string();
	sys_node.rootINode = 0;

	fp = fopen(this->sysFile, "r+");
	fseek(fp, 0, SEEK_SET);
	fwrite(&sys_node, sizeof(sys_node), 1, fp);
	fclose(fp);

	//cout<<"root\n"<<root.to_string();

	current = this->getINode(sys_node.rootINode);
}

void file_system::openFileSystem(){
	fp = fopen(this->sysFile, "r+");
	fseek(fp, 0, SEEK_SET);
	fread(&sys_node, sizeof(sys_node), 1, fp);
	fclose(fp);
	
	current = this->getINode(sys_node.rootINode);
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


unsigned int file_system::applyBlock() {
	/*
	if (this->sys_node.blockUsed >= block_num) {
		return 0;
	}
	*/
	fp = fopen(this->sysFile, "r+");
	int blockAd = this->sys_node.blockBitMap;
	int blockAdd = 0;
	int ans = -1;
	for (int i = 0; i < block_bitmap_size; i++)
	{
		blockAdd = blockAd + i;
		fseek(fp, blockAdd, SEEK_SET);
		int n = fgetc(fp);
		int temp = 1;
		int tempp = 1;
		for (int j = 1; j <= 7; j++)
		{
			if (n&temp == 0)
			{
				ans = j + 8 * i;
				goto success;
			}
			else
				temp = tempp << j;
		}
	}

success:
	int i = ans / 8 + this->sys_node.blockBitMap;
	int j = ans % 8;
	setBitMap(i, j, 1);
	fclose(fp);
	return ans;
}


void file_system::releaseBlock(unsigned int blockId) {
	int blockAddr = blockId / 8 + this->sys_node.blockBitMap;
	int offset = blockId % 8;
	setBitMap(blockAddr, offset, false);
}

void file_system::releaseItem(unsigned int blockId) {
	int offset = blockId * block_size;
	fp = fopen(this->sysFile, "r+");
	fseek(fp, offset, SEEK_SET);
	for (int i = 0; i < block_size; i++)
	{
		fputc(0, fp);
	}
	fclose(fp);
}


void file_system::writeItem(unsigned short blockId){
	int offset=blockId*block_size;
	fp = fopen(this->sysFile, "r+");
	fseek(fp, offset,SEEK_SET);
	char c=rand()%94+33;//随机写入
	for (int i = 0; i < block_size; ++i)
	{
		//cout<<offset+i<<" ";
		fwrite(&c,sizeof(char),1,fp);
	}
	fclose(fp);
}

void file_system::copyItem(unsigned short src,unsigned short dst){
	int offset1 = src*block_size;
	FILE* fp1 = fopen(this->sysFile, "r");
	fseek(fp1, offset1,SEEK_SET);

	int offset2 = dst*block_size;
	FILE* fp2 = fopen(this->sysFile, "r+");
	fseek(fp2, offset2,SEEK_SET);

	char c;
	for (int i = 0; i < block_size; ++i)
	{
		fread(&c,sizeof(char),1,fp1);
		fwrite(&c,sizeof(char),1,fp2);
	}

	fclose(fp1);
	fclose(fp2);
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
			goto success;
		}
		else {
			for (int t = 0; t < inode_in_block; t++) {
				int i = t / 8;
				int j = t % 8;
				char temp = node.bitmap[i] | (1 << (7-j));
				//cout<<"temp:"<<hex<<int(temp)<<" vs "<<hex<<int(node.bitmap[i])<<"\n";
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
	//cout<<node.to_string();
	//cout<<"applyINode "<<nodeIndex<<" "<<offset<<endl;
	return nodeIndex;
}

inode* file_system::getINode(unsigned short addr){
	fp = fopen(this->sysFile, "r+");
	inodeBitMap map;
	int offset = this->sys_node.inodeBitMap + (addr / inode_in_block) * sizeof(inodeBitMap);
	fseek(fp, offset, SEEK_SET);
	fread(&map, sizeof(map), 1, fp);
	inode* node=new inode();

	int offset2 = map.addr * block_size + (addr % inode_in_block) * sizeof(inode);
	fseek(fp, offset2, SEEK_SET);
	fread(node, sizeof(*node), 1, fp);
	fclose(fp);

	//cout<<map.to_string();
	//cout<<"get inode: offset1:"<<offset<<" offset2:"<<offset2<<"\n";

	return node;
}

void file_system::updateINode(inode* node){
	fp = fopen(this->sysFile, "r+");
	inodeBitMap map;
	int offset = this->sys_node.inodeBitMap + (node->addr/inode_in_block)*sizeof(inodeBitMap);
	fseek(fp, offset, SEEK_SET);
	fread(&map, sizeof(map), 1, fp);

	int offset2 = map.addr * block_size + (node->addr % inode_in_block) * sizeof(inode);
	//cout<<"updateINode "<<map.addr<<" "<<map.bitmap<<" "<<offset2<<endl;
	fseek(fp, offset2, SEEK_SET);
	fwrite(node, sizeof((*node)), 1, fp);

	//cout<<map.to_string();
	//cout<<"update inode: offset1:"<<offset<<" offset2:"<<offset2<<" "<<sizeof((*node))<<"\n";
	//cout<<node->to_string();
	fclose(fp);
}

void file_system::releaseINode(unsigned short addr){
	int node_addr = this->sys_node.inodeBitMap + (addr / inode_in_block) * sizeof(inodeBitMap);
	int offset = addr % inode_in_block;
	setBitMap(node_addr + 2 + offset / 8, offset % 8, false);
}

int file_system::get_indirect_block_index(int addr,int block_count){
	fp = fopen(this->sysFile, "r");
	unsigned short block_index;
	fseek(fp,addr*block_size+block_count*sizeof(block_index),SEEK_SET);
	fread(&block_index,sizeof(block_index),1,fp);
	fclose(fp);
	return block_index;
}


void file_system::add_indirect_block_index(int addr,int block_count,unsigned short block_index){
	fp = fopen(this->sysFile, "r+");
	fseek(fp,addr*block_size+block_count*sizeof(block_index),SEEK_SET);
	fwrite(&block_index,sizeof(block_index),1,fp);
	fclose(fp);
}

list<fileNode> file_system::loadDir(inode *dirNode){
	list<fileNode> list;
	int block_count=0;
	while(block_count<dirNode->size){
		int block_index;
		if(block_count<10){
			block_index=dirNode->directBlock[block_count];
		}else{
			block_index=get_indirect_block_index(dirNode->indirectBlock,block_count-10);
		}
		block_count++;
		cout<<"block_index: "<<block_index<<endl;
		fp = fopen(this->sysFile, "r");
		fileNode node;
		fseek(fp,block_index*block_size, SEEK_SET);
		for(int i=0;i<fileNode_in_block;i++){
			fread(&node,sizeof(node),1,fp);
			if(node.nodeAddr!=0){
				list.push_back(node);
			}
		}
		fclose(fp);
		//system("pause");
	}

	return list;
}


void file_system::add_file_node(inode* dirNode,fileNode new_node){
	int block_count=0;
	int new_block_index;
	while(block_count<dirNode->size){
		int block_index;
		if(block_count<10){
			block_index=dirNode->directBlock[block_count];
		}else{
			block_index=get_indirect_block_index(dirNode->indirectBlock,block_count-10);
		}
		block_count++;

		fp = fopen(this->sysFile, "r+");
		for(int i=0;i<fileNode_in_block;i++){
			fseek(fp, block_index*block_size+i*sizeof(fileNode), SEEK_SET);
			fileNode node;
			fread(&node,sizeof(node),1,fp);
			if(node.nodeAddr==0){
				fseek(fp, block_index*block_size+i*sizeof(fileNode), SEEK_SET);
				fwrite(&new_node,sizeof(new_node),1,fp);
				fclose(fp);
				goto success;
			}
		}
		fclose(fp);
	}
	fail:
	new_block_index=applyBlock();
	if(dirNode->size<10){
		dirNode->directBlock[dirNode->size]=new_block_index;
	}else{
		fp = fopen(this->sysFile, "r+");
		fseek(fp,dirNode->indirectBlock*block_size+(dirNode->size-10)*sizeof(new_block_index),SEEK_SET);
		fwrite(&new_block_index,sizeof(new_block_index),1,fp);
		fclose(fp);
	}
	dirNode->size++;
	updateINode(dirNode);

	fp = fopen(this->sysFile, "r+");
	fseek(fp,new_block_index*block_size,SEEK_SET);
	fwrite(&new_node,sizeof(new_node),1,fp);
	fclose(fp);

	success:
	return;
}

void file_system::delete_file_node(inode* dirNode,char* file_name){
	int block_count=0;
	while(block_count<dirNode->size){
		int block_index;
		if(block_count<10){
			block_index=dirNode->directBlock[block_count];
		}else{
			block_index=get_indirect_block_index(dirNode->indirectBlock,block_count-10);
		}
		block_count++;

		fp = fopen(this->sysFile, "r+");
		for(int i=0;i<fileNode_in_block;i++){
			fseek(fp, block_index*block_size+i*sizeof(fileNode), SEEK_SET);
			fileNode node;
			fread(&node,sizeof(node),1,fp);
			if(node.nodeAddr!=0 && strcmp(node.name,file_name)==0){
				node.nodeAddr=0;
				fseek(fp, block_index*block_size+i*sizeof(fileNode), SEEK_SET);
				fwrite(&node,sizeof(node),1,fp);
				fclose(fp);
				goto success;
			}
		}
		fclose(fp);
	}
	success:
	return;
}
//test

void file_system::catFile(inode* file){
	int block_count=0;
	while(block_count<file->size){
		int block_index;
		if(block_count<10){
			block_index=file->directBlock[block_count];
		}else{
			block_index=get_indirect_block_index(file->indirectBlock,block_count-10);
		}
		block_count++;

		fp = fopen(this->sysFile, "r");
		fseek(fp,block_index*block_size,SEEK_SET);
		char c;
		for(int i=0;i<1024;i++){
			fread(&c,sizeof(char),1,fp);
			cout<<c;
		}
		cout<<endl;
	}

}

void file_system::test() {
}

void file_system::help()
{
}
