#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

// 启用或禁用调试信息
#define DEBUG 0

// 用于处理调试打印的宏
#define debug(codes) if(DEBUG) {codes}

// 函数：创建子进程并执行给定参数的命令
void xargs_exec(char* program, char** paraments);

// 函数：读取输入行并执行带参数的命令
// first_arg: 包含命令行传递的初始参数列表的字符串数组
// size: 'first_arg' 数组的大小
// program_name: 要执行的程序名称
// n: 'xargs' 每次应传递给命令的参数数量
void xargs(char** first_arg, int size, char* program_name, int n)
{
    char buf[1024]; // 用于存储输入行的缓冲区
    debug(
        for (int i = 0; i < size; ++i) {
            printf("first_arg[%d] = %s\n", i, first_arg[i]);
        }
    )
    char *arg[MAXARG]; // 命令的参数列表
    int m = 0; // 缓冲区中的位置索引

    // 从标准输入逐个字符地读取
    while (read(0, buf + m, 1) == 1) {
        if (m >= 1024) {
            fprintf(2, "xargs: arguments too long.\n");
            exit(1);
        }
        // 如果遇到换行符，则处理缓冲区
        if (buf[m] == '\n') {
            buf[m] = '\0'; // 将字符串以空字符终止
            debug(printf("this line is %s\n", buf);)
            memmove(arg, first_arg, sizeof(char*) * size); // 将初始参数复制到 arg 数组中

            // 在初始参数之后设置参数索引
            int argIndex = size;
            if (argIndex == 0) {
                arg[argIndex] = program_name;
                argIndex++;
            }

            // 为新参数分配内存并复制它
            arg[argIndex] = buf;
            arg[argIndex + 1] = 0; // 确保参数列表以空字符终止
            debug(
                for (int j = 0; j <= argIndex; ++j)
                    printf("arg[%d] = *%s*\n", j, arg[j]);
            )
            
            xargs_exec(program_name, arg); // 执行带参数的命令
            m = 0; // 重置缓冲区索引以便读取下一输入行
        } else {
            m++; // 移动到缓冲区中的下一个字符位置
        }
    }
}

// 函数：创建子进程并执行命令
void xargs_exec(char* program, char** paraments)
{
    if (fork() > 0) {
        // 父进程等待子进程完成
        wait(0);
    } else {
        // 子进程执行命令
        debug(
            printf("child process\n");
            printf("    program = %s\n", program);
            for (int i = 0; paraments[i] != 0; ++i) {
                printf("    paraments[%d] = %s\n", i, paraments[i]);
            }
        )
        if (exec(program, paraments) == -1) {
            // 如果 exec 失败，打印错误信息
            fprintf(2, "xargs: Error exec %s\n", program);
        }
        debug(printf("child exit");)
    }
}

// 主函数：处理命令行参数并调用 xargs
int main(int argc, char* argv[])
{
    debug(printf("main func\n");)
    if (argc < 2) {
        // 如果提供的参数不足，打印用法信息
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit(1);
    }

    int n = 1; // 传递给命令的参数数量的默认值
    char *name = argv[1]; // 默认执行的命令
    int first_arg_index = 1; // argv 中第一个参数的索引

    // 检查是否提供了 -n 选项
    if (argc >= 4 && strcmp(argv[1], "-n") == 0) {
        n = atoi(argv[2]); // 获取 n 的值
        name = argv[3]; // 要执行的命令
        first_arg_index = 3; // 调整第一个参数索引
    }

    debug(
        printf("command = %s\n", name);
    )

    // 调用 xargs 函数，传递命令和参数
    xargs(argv + first_arg_index, argc - first_arg_index, name, n);
    exit(0);
}

