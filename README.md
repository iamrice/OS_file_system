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

## 核心设计

1. block 空间管理：使用 bitmap 记录 block 的使用情况，并维护一个 offset 指针，记录上一次开辟新空间的位置，加快 block 空间申请。
2. inode 管理：
   1. 为了系统的弹性使用，不对 inode 预留存储空间，当 inode 空间不足时会开辟一个新的 block。
   2. 为了有效回收删除的 inode 空间，为每一个存储 inode 的 block 维护一个 4B 长度的 bitmap，记录该 block 的使用情况。
   3. 储存 inode 的 block 地址和其 bitmap 会组成为一个 6B 的字节段存在系统文件的头部，方便申请新 inode 时的查询。
3. 系统文件头部（7 block）：
   1. 第一部分：rootNode，存有 blockBitMap 的地址、inodeBitMap 的地址、根目录 inode 的地址，共6B。
   2. 第二部分：blockBitMap，系统有1024*16 个block，bitMap 占用 2048B。
   3. 第三部分：inodeBitMap，此部分动态增长，最大可达(1024*16 / 21) \* 6 B，因此预留 4.5 个block。

![](https://z3.ax1x.com/2021/05/17/g2OPCn.png)

## 待解决的问题

- [x] 目录如何记录文件名，文件删除时如何回收空间

   规定文件名不多于29个ASCII字符，用 char[30] 存储

- [x] 在系统关闭时，inode 如何以文本的形式记录

   fwrite 函数可以将 struct 或 class 结构以二进制的形式写入文件，fread 函数可以读取二进制文件并转为 struct 或 class 结构。

- [x] inode需要预留多少空间

   一个 inode 48 byte，1个block可以存约21个；假设每个文件都很小，只占用1个block，那至多有16384个inode，会占用688个block的空间；假设每个文件都很大，占用522个block，那至少有32个inode。为了文件系统更好地弹性存储，不预设inode 的空间，而是动态分配。

- [x] 空闲块如何维护

   使用 bitmap 相对简单，因为16MB 一共16384 个block，所以使用bitmap 是可行的。占用 64byte，可存在第一个block内。

   文件的存储单位是byte，如果使用bit的话需要有一定的转换。例如，1~8个block的state为x=11010011，如果要修改第3个block，则令 x=x | 00100000，得到11110011；如果要修改第7个block，则令 x= x & 11111101。

- [x] short 只有 2B，不足以表达 inode 的位置，inode 位置包括 block 和 offset，需要 14+5 bit。但这会破坏紧凑的结构
  有一种解决方法：跳转。inode 地址指向它所在的 bitmap，再通过 bitmap 上记录的地址去查找。使用到 inode 地址的只有目录文件。



## 可优化的内容

1. 文件系统登录时，使用树结构来存储文件结点，这样会比在文件上读写更快，也可以更好地应对频繁的 cd 命令。预判性的构建树状结构。
