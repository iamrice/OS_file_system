#include "file_system.h"
#include <iostream>
using namespace std;

file_system::file_system(){}

file_system::~file_system(){}

void createFileSystem(){}

void openFileSystem(){}

void loadBitMap(unsigned short addr){}

void dumpBitMap(unsigned short addr){}

int applyBlock(){}

void releaseBlock(unsigned short addr){}

void getINode(inode* node,unsigned short addr){}

void updateINode(inode* node){}

void releaseINode(unsigned short addr){}

void loadDir(inode* dirNode,list<fileNode>* list){}