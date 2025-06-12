详见README.md
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

# 输出说明

排序后序列在output.txt中存放。output.txt中每行存放20个数据。

# 项目结构

```
mpi_odd_even_sort/
├── results			         # 结果截图
├── generate_input.py        # 生成输入数据的Python脚本
├── input.txt                # 输入数据文件
├── mpi_odd_even_sort.c      # 主要的MPI奇偶排序实现代码
├── mpi_odd_even_sort.bat    # Windows批处理文件，用于编译和运行程序
├── mpi_odd_even_sort.exe    # 编译后的可执行文件（Windows）
├── mpi_odd_even_sort        # 编译后的可执行文件（Linux）
├── output.txt               # 排序后的输出文件
└── README.md                # 项目说明文件
```

# 结果截图

## 1. 数据量n=1000

* **output.txt**

  <img src="results\n=1000_out.png" />

* **进程数p=4**

  <img src="results\p=4&n=1000.png" />

* **进程数p=8**

  <img src="results\p=8&n=1000.png" />

## 2. 数据量n=10000

* **output.txt**

  <img src="results\n=10000_out.png" />

* **进程数p=4**

  <img src="results\p=4&n=10000.png" />

* **进程数p=8**

  <img src="results\p=8&n=10000.png" />



