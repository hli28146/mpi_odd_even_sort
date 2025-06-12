#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <limits.h>

int cmp(const void *a, const void *b);// 比较函数
int compute_partner(int phase, int rank, int size);// 判断配对通信进程
void read_file(const char *filename,int size, int *actual_n, int **padded_global_data, int *padded_total);// 读取文件
int count_integers(const char *filename);// 统计文件中的整数数量
void merge_low(int my_keys[], int recv_keys[], int temp_keys[], int local_n);// 合并两个已排序数组，保留较小的一半
void merge_high(int my_keys[], int recv_keys[], int temp_keys[], int local_n);// 合并两个已排序数组，保留较大的一半


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int actual_n = 0;               // 实际读取的数据量
    int *padded_global_data = NULL; // 填充后的全局数据
    int padded_total = 0;           // 填充后的总数据量
    int local_n = 0;                // 每个进程处理的数据量
    
    // 0号进程读取文件并填充数据
    if(rank==0){
        read_file("input.txt", size, &actual_n, &padded_global_data, &padded_total);
    }
    
    // 广播实际数据量和填充后数据量
    MPI_Bcast(&actual_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&padded_total, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // 计算每个进程的数据量
    local_n = padded_total / size;
    
    // 为每个进程分配本地数据内存
    int *local_data = (int*)malloc(local_n * sizeof(int));
    int *temp_keys = (int*)malloc(local_n * sizeof(int)); // 归并排序临时数组
    
    // 0号进程散播待排序的序列
    MPI_Scatter(padded_global_data, local_n, MPI_INT, 
                local_data, local_n, MPI_INT, 
                0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        free(padded_global_data);  // 0号进程释放读入数据
    }
    
    // 局部排序
    qsort(local_data, local_n, sizeof(int), cmp);
    
    // 各进程通信，进行奇偶排序
    for (int phase = 0; phase < size; phase++) {
        // 获取配对进程
        int partner = compute_partner(phase, rank, size);
        
        // 使用MPI_Sendrecv进行安全通信，避免死锁
        int *recv_buf = (int*)malloc(local_n * sizeof(int));
        MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0,
                     recv_buf, local_n, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // 只有当partner有效时才处理数据
        if (partner != MPI_PROC_NULL) {
            if (rank < partner) {
                // 保留较小的一半
                merge_low(local_data, recv_buf, temp_keys, local_n);
            } else {
                // 保留较大的一半
                merge_high(local_data, recv_buf, temp_keys, local_n);
            }
        }
        
        free(recv_buf);// 释放资源
    }
    
    free(temp_keys); // 释放临时数组
    
    // 收集数据
    int *gathered_data = NULL;
    if (rank == 0) {
        gathered_data = (int*)malloc(padded_total * sizeof(int));
    }

    // 收集所有进程的数据到0号进程
    // 按标识rank排序拼接
    MPI_Gather(local_data, local_n, MPI_INT,
               gathered_data, local_n, MPI_INT,
               0, MPI_COMM_WORLD);
    
    free(local_data);
    
    // 0号进程输出结果
    if (rank == 0) {
        // 输出到控制台
        printf("排序后的序列 (%d 个元素):\n", actual_n);
        for (int i = 0; i < actual_n; i++) {
            printf("%d ", gathered_data[i]);
            if ((i + 1) % 20 == 0) printf("\n"); 
        }
        printf("\n");
        
        // 输出到文件
        FILE *out_file = fopen("output.txt", "w");
        if (out_file == NULL) {
            fprintf(stderr, "错误：无法打开输出文件\n");
            free(gathered_data);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        for (int i = 0; i < actual_n; i++) {
            fprintf(out_file, "%d ", gathered_data[i]);
            if ((i + 1) % 20 == 0) fprintf(out_file,"\n"); 
        }
        fclose(out_file);
        
        free(gathered_data);// 释放资源
    }
    
    MPI_Finalize();
    return 0;
}

// 比较函数
int cmp(const void *a, const void *b) {
    //使用比较运算符判断，防止溢出
    return *(int *)a < *(int *)b ? -1 : *(int *)a > *(int *)b;
}

// 判断配对通信进程
int compute_partner(int phase, int rank, int size) {
    int partner;
    if (phase % 2 == 0) {  // 偶阶段
        if (rank % 2 == 0) { //进程号为偶
            partner = rank + 1;
        } else { //进程号为奇
            partner = rank - 1;
        }
    } else {  // 奇阶段
        if (rank % 2 == 0) { //进程号为偶
            partner = rank - 1;
        } else { //进程号为奇
            partner = rank + 1;
        }
    }
    
    // 无效配对
    if (partner < 0 || partner >= size) {
        partner = MPI_PROC_NULL;
    }
    return partner;
}

// 读取文件,填充数据保证各进程数据量一致
void read_file(const char *filename,int size, int *actual_n, int **padded_global_data, int *padded_total) {
    // 统计文件中的数据量
    *actual_n = count_integers(filename);
    
    // 根据文件数据动态分配内存
    int *global_data = (int*)malloc(*actual_n * sizeof(int));
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "错误：无法打开输入文件\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    int count = 0;
    int num;
    while (fscanf(file, "%d", &num) == 1) {
        if(count < *actual_n)global_data[count++] = num;
        else count++;//异常情况，读取的数据量大于实际统计数据
    }
    
    // 验证读取的数据量是否与实际统计数据一致
    if (count != *actual_n) {
        fprintf(stderr, "错误：文件内容可能被修改，预期 %d 个整数，实际读取 %d 个\n", *actual_n, count);
        fclose(file);
        free(global_data);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    fclose(file);
    
    // 计算填充后的总数据量，使其可被进程数整除
    if (*actual_n % size != 0) {
        *padded_total = ((*actual_n + size - 1) / size) * size;
    } else {
        *padded_total = *actual_n;
    }
    
    // 创建填充数组，保证各进程具有相同的数据量
    // 指针指向分配的内存空间
    *padded_global_data = (int*)malloc(*padded_total * sizeof(int));
    memcpy(*padded_global_data, global_data, *actual_n * sizeof(int));
    for (int i = *actual_n; i < *padded_total; i++) {
        (*padded_global_data)[i] = INT_MAX;  // 用最大值填充
    }
    
    free(global_data);  // 释放原始数据
    
    printf("实际读取数据量: %d \n", *actual_n);
}

// 统计文件中的整数数量
int count_integers(const char *filename) {
    FILE *file = fopen(filename, "r");
    //文件打开失败
    if (file == NULL) {
        fprintf(stderr, "错误：无法打开文件以统计整数数量\n");
        return -1;
    }
    
    int count = 0;
    int num;
    //统计待排序序列大小
    while (fscanf(file, "%d", &num) == 1) {
        count++;
    }
    
    fclose(file);
    return count;
}

// 合并两个已排序数组，保留较小的一半
void merge_low(int my_keys[], int recv_keys[], int temp_keys[], int local_n) {
    int m_i = 0, r_i = 0, t_i = 0;
    while (t_i < local_n) {
        if (my_keys[m_i] <= recv_keys[r_i]) {//取my_keys中较小的部分
            temp_keys[t_i] = my_keys[m_i];
            t_i++;
            m_i++;
        } else {//取recv_keys中较小的部分
            temp_keys[t_i] = recv_keys[r_i];
            t_i++;
            r_i++;
        }
    }
    
    // 将结果复制回原数组，保留较小的一半
    memcpy(my_keys, temp_keys, local_n * sizeof(int));
}

// 合并两个已排序数组，保留较大的一半
void merge_high(int my_keys[], int recv_keys[], int temp_keys[], int local_n) {
    int m_i = local_n - 1;  // 指向my_keys末尾
    int r_i = local_n - 1;  // 指向recv_keys末尾
    int t_i = local_n - 1;  // 指向临时数组末尾
    
    // 从后向前合并，保留较大的元素
    while (t_i >= 0) {
        if (my_keys[m_i] >= recv_keys[r_i]) {
            temp_keys[t_i] = my_keys[m_i];
            t_i--;
            m_i--;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            t_i--;
            r_i--;
        }
    }
    
    // 将结果复制回原数组，保留较大的一半
    memcpy(my_keys, temp_keys, local_n * sizeof(int));
}
