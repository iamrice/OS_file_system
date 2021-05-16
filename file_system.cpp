#include "file_system.h"
#include <fstream>
#include <iostream>
using namespace std;

file_system::file_system(){
	cout<<"MiniOS file system 1.0.0 developed by 陈泰佑(201830570057),韦泽晟(),邱颖()"<<endl;
	cout<<"Copyright (c) 2021 Taiyou Chen, Zesheng Wei and Ying Qiu. All rights reserved."<<endl;
	cout<<"Type \"help\" or \"lisence\" for more informations"<<endl;
}

file_system::~file_system(){}

void file_system::createFileSystem(){
	FILE *fp=fopen("./file.txt","w");
	fseek(fp,1024*1024*16-1,SEEK_SET);
	char end=EOF;
	fwrite(&end,1,1,fp);	
	fclose(fp);
	
	int bitmap[2 << 9];
	
	memset(bitmap, 0, sizeof(bitmap));

	rootNode root;
}

void file_system::openFileSystem(){}

void file_system::loadBitMap(unsigned short addr){}

void file_system::dumpBitMap(unsigned short addr){}

int file_system::applyBlock(){}

void file_system::releaseBlock(unsigned short addr){}

void file_system::getINode(inode* node,unsigned short addr){}

void file_system::updateINode(inode* node){}

void file_system::releaseINode(unsigned short addr){}

void file_system::loadDir(inode* dirNode,list<fileNode>* list){}