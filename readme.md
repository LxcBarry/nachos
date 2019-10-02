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


