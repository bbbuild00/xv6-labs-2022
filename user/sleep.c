#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//实现了让当前进行睡眠指定秒数
int main(int argc, char* argv[])
{
    if(argc != 2){
        fprintf(2, "Usage: sleep times\n");
        exit(1);
    }
    //将第二个参数（即 argv[1]）转换为整数，表示 sleep 的时间。
    int time = atoi(argv[1]);
    
    if(time <= 0){
        fprintf(2, "Error: time must be a positive integer.\n");
        exit(1);
    }
    //sleep(time) 是一个 xv6 提供的系统调用，用于让当前进程睡眠指定的秒数。
    if(sleep(time) != 0){
        fprintf(2, "Error in sleep sys_call!\n");
        exit(1);
    }
    exit(0);
}

