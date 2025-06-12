import random
import argparse
import sys

def generate_input_file(filename, count, min_val, max_val):
    """
    生成随机整数序列并写入文件
    
    参数:
        filename (str): 输出文件名
        count (int): 生成的整数数量
        min_val (int): 随机数最小值 (包含)
        max_val (int): 随机数最大值 (包含)
    """
    # 验证参数有效性
    if min_val > max_val:
        print("错误：最小值不能大于最大值")
        sys.exit(1)
    
    if count <= 0:
        print("错误：数量必须大于0")
        sys.exit(1)
    
    # 生成随机整数序列
    random_numbers = [random.randint(min_val, max_val) for _ in range(count)]
    
    # 写入文件（覆盖模式）
    with open(filename, 'w') as f:
        for num in random_numbers:
            f.write(f"{num}\n")
    
    print(f"已生成 {count} 个随机整数并写入 {filename}")
    print(f"取值范围: {min_val} 到 {max_val}")

if __name__ == "__main__":
    # 设置32位有符号整数的范围
    INT32_MIN = -2147483648
    INT32_MAX = 2147483647
    
    # 创建命令行参数解析器
    parser = argparse.ArgumentParser(
        description='生成随机整数测试文件，用于MPI排序程序测试',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    
    # 添加命令行参数
    parser.add_argument('-f', '--file', 
                        default='input.txt', 
                        help='输出文件名')
    
    parser.add_argument('-c', '--count', 
                        type=int, 
                        default=1000, 
                        help='生成的整数数量')
    
    parser.add_argument('--min', 
                        type=int, 
                        default=INT32_MIN, 
                        help='随机数最小值')
    
    parser.add_argument('--max', 
                        type=int, 
                        default=INT32_MAX, 
                        help='随机数最大值')
    
    parser.add_argument('--range', 
                        nargs=2, 
                        metavar=('MIN', 'MAX'), 
                        type=int,
                        help='同时指定最小值和最大值（覆盖单独的min/max参数）')
    
    # 解析命令行参数
    args = parser.parse_args()
    
    # 处理范围参数
    if args.range:
        min_val, max_val = args.range
    else:
        min_val = args.min
        max_val = args.max
    
    # 调用生成函数
    generate_input_file(args.file, args.count, min_val, max_val)