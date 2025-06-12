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

项目文件夹下编译运行源文件mpi_odd_even_sort.c

```bash
mpicc mpi_odd_even_sort.c -o mpi_odd_even_sort

mpirun mpi_odd_even_sort
```

# 输入说明

待排序序列在input.txt中存放。

待排序序列为INT类型，取值范围(INT32_MIN = -2147483648 , INT32_MAX = 2147483647)

待排序序列由项目中generate_input.py代码随机生成，详见源文件

**注**：

如果文件中出现非数字字符，程序将对非数字字符之前的数字序列进行排序。如：

```
//input.txt
-768142071 ssss dddd
-339388778
...

//stdout
>>mpiexec -n 8 .\mpi_odd_even_sort.exe
实际读取数据量: 1 
排序后的序列 (1 个元素):
-768142071
```

# 项目结构

