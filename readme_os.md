# os lab

## 64位ubuntu编译nachos问题收集


**缺少依赖先装以下库**  
(好像有些是不必要的,不太记得了,反正全装了就是了)
- sudo dpkg --add-architecture i386 #是否打开32位架构的支持
- sudo apt-get update
- g++-multilib # 编译32程序
- lib32ncurses 
- zlib1g:i386 
- libstdc++6:i386
- lib32z1
- libc6:i386 
- libncurses5:i386 
- libstdc++6:i386
64位的系统上做实验，在Makefile.common中的 g++、gcc加上-m32，as加上-32

如果嫌虚拟机慢可以安装wsl使用(windows的linux子系统)   
如果使用wsl，32位的程序会运行不了  
使用以下命令进行修改
```shell
sudo apt update
sudo apt install qemu-user-static
sudo update-binfmts --install i386 /usr/bin/qemu-i386-static --magic '\x7fELF\x01\x01\x01\x03\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x03\x00\x01\x00\x00\x00' --mask '\xff\xff\xff\xff\xff\xff\xff\xfc\xff\xff\xff\xff\xff\xff\xff\xff\xf8\xff\xff\xff\xff\xff\xff\xff'

# 每次重启后需要以下命令才能运行32位程序
sudo service binfmt-support start



```
### 安装依赖库后的问题
**conflicting types for ‘malloc’**  
高版本gcc编译低版本代码的问题，可以修改nachos代码（这里发现是coff2noff.c文件的malloc函数那里报错)  
```cpp
//修改为
void *malloc();//<----char *malloc();
```
**发现会出现halt编译不出来的问题**  
```
Syntax error: "(" unexpected
```
[解决方法](https://blog.csdn.net/breeze5428/article/details/27353583)  

---
以下方法不一定采用  
**安装低版本的gcc**(好像效果和高版本的一样-_-)
参考[**这个**](https://ywnz.com/linuxjc/3736.html)安装
安装命令，先卸载之前的gcc g++  
装gcc4和g++5,这个和学校服务器的版本一样，但是发现版本太老的，已经没资源了  
只好装5了  
```shell
sudo apt-get install -y gcc-5
sudo apt-get install -y g++-5
```

把**建议的包**(大礼包)装了也行(当作练习打字)   
```shell
Suggested packages:
  gcc-5-multilib gcc-5-doc gcc-5-locales libgcc1-dbg libgomp1-dbg libitm1-dbg libatomic1-dbg libasan2-dbg liblsan0-dbg    libtsan0-dbg libubsan0-dbg libcilkrts5-dbg libmpx0-dbg libquadmath0-dbg
```
**重新建立软链接**
```shell
cd /usr/bin 
sudo ln -sf gcc-5 gcc // 建立gcc-5的软链接
sudo ln -sf g++-5 g++  //同上
```