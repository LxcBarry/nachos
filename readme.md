# nachos
[toc]

智能系大三os课程  
之前看到一个学长(学姐)的代码，里面有这个课程的三个实验的代码，具体可参考
[这里](https://github.com/aksudya/nachos)
使用环境：ubuntu 18.04 64位  
  

由于nachos使用的是32位系统，在本系统下会有兼容性的问题，具体如何在64位系统下使用nachos3.4,可以
参考[这篇文章](https://blog.csdn.net/qq_37595769/article/details/81872625)，code下的代码
已经在ubuntu18.04测试过了，如果提示缺少依赖，可以尝试这几个命令  
```shell
sudo apt-get install lib32ncurses5 lib32z1
sudo apt-get install zlib1g:i386 libstdc++6:i386
sudo apt-get install libc6:i386 libncurses5:i386
sudo apt-get install libgcc1:i386 gcc-4.8-base:i386 libstdc++5:i386
```

## 工作日志

### 2019-11-24
[lab2](https://github.com/LxcBarry/nachos/tree/lab2)

**使用说明**  
在thread下输入以下命令
- ./nachos -q 2 -m 2 2 -e 3 
演示双向链表缺少互斥机制的情况(需要注释掉threadtest.cc下的ThreadTest2的sleep_lock) -m后命令为T N,-e后参数为错误类型(最多为4,0为无错误)  
- ./nachos -q 3 
table表的生产者消费者模型演示

- ./nachos -q 4
dllist的生产者消费者模型演示

- ./nachos -q 5 
boundedbuffer的生产者消费者模型演示  
(加-rs命令可以随机切换线程,加-b max_size b_in_size b_out_size，控制buffer区大小和每次读入大小，读出大小)  



