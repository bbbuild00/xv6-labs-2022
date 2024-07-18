#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
// 实现素数筛选的功能
void prime_sieve(int p[2]) {
    int prime;
    int n;
    close(p[1]);// 关闭当前管道的写入端
    
    // 从管道读取第一个数，即素数
    if(read(p[0], &prime, sizeof(prime)) == 0) {
        // 如果没有读取到数字，关闭读取端并退出
        //递归的结束部分
        close(p[0]);
        exit(0);
    }
    printf("prime %d\n", prime);
   
    // 创建一个新的管道
    int next_pipe[2];
    pipe(next_pipe);

    // 创建一个新的进程
    if(fork() == 0) {
        // 子进程
        close(p[0]);
        prime_sieve(next_pipe);//递归
    } 
    else {
        // 父进程
        close(next_pipe[0]);
        
        //从当前管道读取数字
        //过滤掉素数的倍数
        //将剩余的写入到下一个管道
        while(read(p[0], &n, sizeof(n)) > 0) {
            if(n % prime != 0) {
                write(next_pipe[1], &n, sizeof(n));
            }
        }
        
        close(p[0]);
        close(next_pipe[1]);
        wait(0);//等待子进程执行完毕
    }
}

int main(int argc, char *argv[]) {
    // 创建初始管道
    int p[2];
    pipe(p);
    
    // 创建第一个筛选进程
    if(fork() == 0) {
        //子进程: 从初始管道开始筛选
        prime_sieve(p);
    } else {
        //父进程
        close(p[0]);//关闭初始管道的读取端
        
        //向管道写入数据
        for(int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));
        }

        close(p[1]);//关闭初始管道的写入端
        wait(0);//等待筛选进程完成
        exit(0);
    }
    return 0;
}

