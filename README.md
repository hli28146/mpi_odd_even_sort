# 运行程序

## 1. Windows操作系统

**方法一：**

项目文件夹下编译运行源文件mpi_odd_even_sort.c

控制台打印输出结果、output.txt记录输出结果

```bash
g++ 文件名.cpp -o 文件名 -I mpi的Include路径 -L mpi的Lib/x64路径 -lmsmpi
mpiexec test 
//mpiexec -n 4 test  //也可以指定进程数
```

**方法二：**

运行可执行文件 mpi_odd_even_sort.exe

控制台打印输出结果、output.txt记录输出结果

```bash
//第一种方法
点击运行mpi_odd_even_sort.exe

//第二种方法
点击运行脚本mpi_odd_even_sort.bat
//(注：脚本中程序默认进程数为4；cmd编码方式修改为UTF-8)
```

**注**：

程序编码方式为UTF-8，出现中文乱码方式请修改终端的编码方式（VScode的默认编码方式为UTF-8，中国地区下cmd的编码方式GBK）

## 2. Linux

