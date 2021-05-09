#ifndef INODE_H
#define INODE_H
#include <string>

class file_system
{
public:
	file_system();
	~file_system();

private:
	void createFile(std::string,int);
	void deleteFile(std::string);
	void createDir(std::string);
	void deleteDir(std::string);
	void changeDir(std::string);
	void listDir();
	void copy(std::string,std::string);
	void sum();
	void cat(std::string);
};
#endif