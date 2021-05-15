# OS_file_system

<div align='left' >
<img src="https://img.shields.io/github/license/iamrice/OS_file_system.svg"/>
<img  src="https://img.shields.io/github/last-commit/iamrice/OS_file_system.svg"/>
<img src="https://img.shields.io/badge/language-c++-\#F34B7D.svg"/>
<img src="https://img.shields.io/github/repo-size/iamrice/OS_file_system.svg"/>
</div>
This project is a simple file system implemented by c++.

## License

Copyright (C) 2021 Taiyou Chen, Zesheng Wei, Ying Qiu

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

## Usage

1. 将项目拉取到本地
2. 编译，执行

```
mingw32-make //on windows
make //on linux

file_system.exe
```

## 未解决的问题

1. 目录如何记录文件名，文件删除时如何回收空间

   规定文件名不多于29个ASCII字符，用 char[30] 存储

2. 在系统关闭时，inode 如何以文本的形式记录

   fwrite 函数可以将 struct 或 class 结构以二进制的形式写入文件，fread 函数可以读取二进制文件并转为 struct 或 class 结构。

3. inode需要预留多少空间

   一个 inode 43 byte，因为系统是8字节对齐，所以实际上占用48byte，12个 block。为了文件系统更好地弹性存储，不预设inode 的空间，而是动态分配。

4. 空闲块如何维护

   使用 bitmap 相对简单，因为16MB 一共也才16384 个block，所以使用bitmap 是可行的。占用 64byte，16 个block。

   文件的存储单位是byte，如果使用bit的话需要有一定的转换。例如，1~8个block的state为x=11010011，如果要修改第3个block，则令 x=x | 00100000，得到11110011；如果要修改第7个block，则令 x= x & 11111101。

## 可优化的内容

1. 文件系统登录时，使用树结构来存储文件结点，这样会比在文件上读写更快，也可以更好地应对频繁的 cd 命令。