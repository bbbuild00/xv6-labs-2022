#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    //用pipe函数创建两个管道，‘p2c’用于父进程向子进程发送数据，‘c2p’用于子进程向父进程发送数据。
    int p2c[2], c2p[2]; 
    if (pipe(p2c) == -1 || pipe(c2p) == -1) {
        fprintf(2, "Error: pipe() error.\n");
        exit(1);
    }
    
    //用fork函数创建子进程
    int pid = fork();
    if (pid == -1) {
        fprintf(2, "Error: fork() error.\n");
        exit(1);
    }
    
    //子进程关闭‘p2c’的写端和‘c2p’的读端，从‘p2c’读取父进程发送的数据并打印收到的‘ping’信息，将数据写回‘c2p’并关闭相应端口，退出子进程。
    if (pid == 0) {
        char buffer[1];
        close(p2c[1]); // close the write end of p2c
        close(c2p[0]); // close the read end of c2p

        // read data from parent
        if (read(p2c[0], buffer, 1) == -1) {
            fprintf(2, "Error: read() error in child.\n");
            exit(1);
        }
        
        close(p2c[0]); // close the read end of p2c
        printf("%d: received ping\n", getpid());// print received message
        
        // write data back to parent
        if (write(c2p[1], buffer, 1) == -1) {
            fprintf(2, "Error: write() error in child.\n");
            exit(1);
        }
        close(c2p[1]); // close the write end of c2p

        exit(0);
    } 
    //父进程关闭‘p2c’的读端和‘c2p’的写端，向‘p2c’写入数据，等待子进程的完成。从‘c2p’读取数据并打印收到的‘pong’信息，退出父进程。
    else {
        char buffer[1];
        buffer[0] = 'a'; // data to send

        close(p2c[0]); // close the read end of p2c
        close(c2p[1]); // close the write end of c2p

        // 2rite data to child
        if (write(p2c[1], buffer, 1) == -1) {
            fprintf(2, "Error: write() error in parent.\n");
            exit(1);
        }
        close(p2c[1]); // close the write end of p2c
        wait(0);// wait for the child process to complete

        // read data from the child
        if (read(c2p[0], buffer, 1) == -1) {
            fprintf(2, "Error: read() error in parent.\n");
            exit(1);
        }
        close(c2p[0]); // close the read end of c2p

        // print received message
        printf("%d: received pong\n", getpid());
        
        //调用exit(0)退出程序。
        exit(0);
    }
}


