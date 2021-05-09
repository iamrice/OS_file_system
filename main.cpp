#include "file_system.h"
#include <iostream>
using namespace std;

int main(){
	file_system system;
	string str;
	cout<<">>>";
	while(true){
		getline(cin,str);
		
		/****************
		requirement: 指令处理器
			a. 按空格分割指令，并传递参数给对应的函数
			b. 如果参数数量正确，传递给对应的函数,输出">>>"，等待下一个指令；
			c. 如果参数数量不足，则缓存已有参数，输出"...",等待用户继续输入；
			d. 如果参数数量过多，则报错。
		****************/
	}
}		