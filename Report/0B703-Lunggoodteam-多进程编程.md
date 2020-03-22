# 多进程编程
### 小组名称：Lung good team
### 小组成员：杭锦泉 滕飞 朱君楷
***
# 一、实验目的
1.学会创建多进程程序,理解fork(),vfork() 创建的子进程的特点。

2.学会使用有名管道和无名管道实现进程间通信。

3.学会使用信号机制实现进程间通信。
***
# 二、实验内容
1.编写fork(),vfork()多进程程序,通过输出结果比较所创建的子进程的不同之处。

2.编写fork()多进程程序,进程间模拟信号通信,包括信号发送、捕捉与处理。

3.编写fork()多进程程序,进程间实现有名管道和无名管道通信。具体要求如下:

* 编写1个服务器程序server.c和2个客户端程序client1.c,client2.c ,服务器和客户端之间采用FIFO管道通信;
* 服务器和客户端各自创建自己的子进程,父进程负责
完成FIFO管道通信,子进程负责输出接收到的信息,
父子进程间采用无名管道通信。
***
# 三、实验过程和结果
## A、编写fork(),vfork()多进程程序,通过输出结果比较所创建的子进程的不同之处。

### 1、fork()&vfork()
fork()调用执行一次会返回2个值(考虑错误信息实际为3个值),如果
返回父进程,fork()返回子进程的进程号,如果返回子进程,fork()
则返回0,错误则返回-1

两者被调用一次，但是返回两次。两次返回的唯一区别是子进程的返回值是0，而父进程的返回值则是新子进程的进程ID。

### 2、分别用fork和vfork函数创建子进程
#### 1)pid_t fork(void)
`pid_t vfork(void)`

![2_1](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_1.png)
#### 2)先编译并运行fork()函数
![2_2](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_2.png)
#### 3)再编译并运行vfork()函数
![2_3](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_3.png)
### 3、总结
* fork()用于创建一个新进程。由fork()创建的子进程是父进程的副本。即子进程获取父进程数据空间，堆和 栈的副本。父子进程之间不共享这些存储空间的部分。而vfork()创建的进程并不将父进程的地址空间完全复制到子进程中，因为子进程会立即调用exec (或exit)于是也就不会存放该地址空间。相反，在子进程调用exec或exit之前，它在父进程的空间进行。
* vfork()与fork()另一个区别就是：vfork保证子进程先运行，在调用exec或exit之前与父进程数据是共享的,在它调用exec或exit之后父进程才可能被调度运行。
* vfork和fork之间的还有一个区别是： vfork保证子进程先运行，在她调用exec或exit之后父进程才可能被调度运行。如果在调用这两个函数之前子进程依赖于父进程的进一步动作，则会导致死锁。
***
## B、编写fork()多进程程序，进行间模拟信号通信，包括信号发送、捕捉与处理。

### 1、信号
信号是Linux系统中用于进程之间通信或操作的一种机制，信号可以在任何时候发送给某一进程，而无须知道该进程的状态。如果该进程并未处于执行状态，则该信号就由内核保存起来，知道该进程恢复执行并传递给他为止。如果一个信号被进程设置为阻塞，则该信号的传递被延迟，直到其阻塞被取消时才被传递给进程。 
Linux提供了几十种信号，分别代表着不同的意义。信号之间依靠他们的值来区分，但是通常在程序中使用信号的名字来表示一个信号。在Linux系统中，这些信号和以他们的名称命名的常量被定义在/usr/includebitssignum.h文件中。通常程序中直接包含<signal.h>就好。
### 2、信号的来源
信号是在软件层次上对中断机制的一种模拟，是一种异步通信方式，信号可以在用户空间进程和内核之间直接交互。内核也可以利用信号来通知用户空间的进程来通知用户空间发生了哪些系统事件。信号事件有两个来源：
* 硬件来源，例如按下了cltr+C，通常产生中断信号sigint
* 软件来源，例如使用系统调用或者命令发出信号。最常用的发送信号的系统函数是kill,raise,setitimer,sigation,sigqueue函数。软件来源还包括一些非法运算等操作。
 
### 3、信号的处理
一旦有信号产生，用户进程对信号产生的相应有三种处理方式：
* 执行默认操作，linux对每种信号都规定了默认操作。
* 捕捉信号，定义信号处理函数，当信号发生时，执行相应的处理函数。
* 忽略信号，当不希望接收到的信号对进程的执行产生影响，而让进程继续执行时，可以忽略该信号，即不对信号进程作任何处理。
有两个信号是应用进程无法捕捉和忽略的，即SIGKILL和SEGSTOP，这是为了使系统管理员能在任何时候中断或结束某一特定的进程。

### 4、编写相应处理程序
#### 1)主程序
![2_4](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_4.png)

![2_5](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2.5.png)
#### 2)运行结果
![2_6](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_6.png)

## C、编写fork()多进程程序,进程间实现有名管道和无名管道通信。

### 1、管道
管道是Linux中进程间通信的一种常用方式，经常被用于命令行不同进程间通信，也可以在应用程序中建立进程间通信。管道分为无名管道和有名管道两种：无名管道可用于具有亲缘关系进程间的通信，如父子进程、兄弟进程。有名管道克服了管道没有名字的限制，允许无亲缘关系进程间的通信。管道是单向的字节流，它可以把一个进程的标准输出与另一个进程的标准输入连接起来，shell负责建立进程间的这些临时性的管道，而进程根本不知道这些重定向操作，仍然按照通常的方式进行操作。

### 2、实现程序
#### 1)server.c
##### 1)创建4个有名管道，分别用于服务器向两个客户端发信息和两个客户端向服务器发信息
![2_7](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_7.png)

##### 2)服务器向两个客户端分别发送一组信息
![2_8](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_8.png)

##### 3)服务器接收两个客户端返回的信息
![2_9](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_9.png)

##### 4)服务器通过无名管道将返回的信息发送到子进程
![2_10](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_10.png)

##### 5)子进程输出返回的信息
![2_11](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_11.png)

#### 2)client1.c
##### 1)从有名管道中读取数据
![2_12](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_12.png)

##### 2)利用有名管道向服务器端发送数据
![2_13](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_13.png)

##### 3)将从有名管道读取的数据发送到子进程
![2_14](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_14.png)

##### 4)子进程输出收到的数据
![2_15](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_15.png)

#### 3)client2.c
流程同client1.c:
![2_16](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_16.png)
![2_17](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_17.png)
![2_18](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_18.png)
![2_19](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_19.png)

### 3、运行结果
下面是三个进程各自的运行结果，可以看出：服务器端显示了两个客户端发回的数据，客户端也显示出了服务器端发送来的数据。
![2_20](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_20.png)
![2_21](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_21.png)
![2_22](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW2/2_22.png)

# 四、附实验源码
## server.c
```
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<memory.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define BUFFSIZE 100

int main()
{
  int flag,fifo_fd1_s,fifo_fd2_s,fifo_fd1_c,fifo_fd2_c;
  flag = mkfifo("./MyFIFO1_s", 0777);
  if(flag<0){
    perror("MyFIFO1 error");
    exit(1);
  }
  fifo_fd1_s = open("./MyFIFO1_s", O_RDWR|O_NONBLOCK);
  if(fifo_fd1_s<0){
    perror("MyFIFO1 open error");
    exit(1);
  }
  flag = mkfifo("./MyFIFO2_s", 0777);
  if(flag<0){
    perror("MyFIFO2 error");
    exit(1);
  }
  fifo_fd2_s = open("./MyFIFO2_s", O_RDWR|O_NONBLOCK);
  if(fifo_fd2_s<0){
    perror("MyFIFO2 open error");
    exit(1);
  }
  flag = mkfifo("./MyFIFO1_c", 0777);
  if(flag<0){
    perror("MyFIFO1 error");
    exit(1);
  }
  fifo_fd1_c = open("./MyFIFO1_c", O_RDWR|O_NONBLOCK);
  if(fifo_fd1_c<0){
    perror("MyFIFO1 open error");
    exit(1);
  }
  flag = mkfifo("./MyFIFO2_c", 0777);
  if(flag<0){
    perror("MyFIFO2 error");
    exit(1);
  }
  fifo_fd2_c = open("./MyFIFO2_c", O_RDWR|O_NONBLOCK);
  if(fifo_fd2_c<0){
    perror("MyFIFO2 open error");
    exit(1);
  }
  pid_t pid;
  int pfd[2];
  char buff1[BUFFSIZE],buff2[BUFFSIZE];
  int rdnum,wtnum;
  memset(buff1,0,sizeof(buff1));
  memset(buff2,0,sizeof(buff2));
  if(pipe(pfd)<0){
    perror("pipe error");
    exit(1);
  }

  pid=fork();
  if(pid<0){
    perror("fork error");
    exit(1);
  }
  else if(pid==0){
    close(pfd[1]);
    sleep(1);
    rdnum=read(pfd[0],buff1,BUFFSIZE);
    printf("%s\n",buff1);
    close(pfd[0]);
    exit(0);
  }
  else{
    close(pfd[0]);
    flag = write(fifo_fd1_s, "server to client1\n",18);
    if(flag<0){
      perror("MyFIFO1 write error");
      exit(1);
    }
    flag = write(fifo_fd2_s, "server to client2\n",18);
    if(flag<0){
      perror("MyFIFO1 write error");
      exit(1);
    }
    int count=2;
    while(1){
      sleep(1);
      flag = read(fifo_fd1_c, buff1,BUFFSIZE);
      if(flag>0){
        count--;
        if(!count)break;
      }
      sleep(1);
      flag = read(fifo_fd2_c, buff2,BUFFSIZE);
      if(flag>0){
        count--;
        if(!count)break;
      }
    }
    wtnum=write(pfd[1],"client1 return:",15);
    wtnum=write(pfd[1],buff1,strlen(buff1));
    wtnum=write(pfd[1],"client2 return:",15);
    wtnum=write(pfd[1],buff2,strlen(buff2));

    close(pfd[1]);
    waitpid(pid,NULL,0);
    exit(0);
  }
}
```

## client1.c
```
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<memory.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define BUFFSIZE 100

int main()
{
  int flag,fifo_fd1_s,fifo_fd1_c;
  fifo_fd1_s = open("./MyFIFO1_s", O_RDWR|O_NONBLOCK);
  fifo_fd1_c = open("./MyFIFO1_c", O_RDWR|O_NONBLOCK);

  pid_t pid;
  int pfd[2];
  char buff[BUFFSIZE];
  int rdnum,wtnum;
  memset(buff,0,sizeof(buff));
  if(pipe(pfd)<0){
    perror("pipe error");
    exit(1);
  }

  pid=fork();
  if(pid<0){
    perror("fork error");
    exit(1);
  }
  else if(pid==0){
    close(pfd[1]);
    sleep(1);
    rdnum=read(pfd[0],buff,BUFFSIZE);
    printf("%s\n",buff);
    close(pfd[0]);
    exit(0);
  }
  else{
    close(pfd[0]);
    while(1){
      sleep(1);
      flag = read(fifo_fd1_s, buff,BUFFSIZE);
      if(flag>0){
        flag = write(fifo_fd1_c, "client1 to server\n",18);
        if(flag<0){
          perror("MyFIFO1 write error");
          exit(1);
        }
        break;
      }
    }
    wtnum=write(pfd[1],"server message:",15);
    wtnum=write(pfd[1],buff,strlen(buff));
    close(pfd[1]);
    waitpid(pid,NULL,0);
    exit(0);
  }
}
```

## client2.c
```
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<memory.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define BUFFSIZE 100

int main()
{
  int flag,fifo_fd2_s,fifo_fd2_c;
  fifo_fd2_s = open("./MyFIFO2_s", O_RDWR|O_NONBLOCK);
  fifo_fd2_c = open("./MyFIFO2_c", O_RDWR|O_NONBLOCK);

  pid_t pid;
  int pfd[2];
  char buff[BUFFSIZE];
  int rdnum,wtnum;
  memset(buff,0,sizeof(buff));
  if(pipe(pfd)<0){
    perror("pipe error");
    exit(1);
  }

  pid=fork();
  if(pid<0){
    perror("fork error");
    exit(1);
  }
  else if(pid==0){
    close(pfd[1]);
    sleep(1);
    rdnum=read(pfd[0],buff,BUFFSIZE);
    printf("%s\n",buff);
    close(pfd[0]);
    exit(0);
  }
  else{
    close(pfd[0]);
    while(1){
      sleep(1);
      flag = read(fifo_fd2_s, buff,BUFFSIZE);
      if(flag>0){
        flag = write(fifo_fd2_c, "client2 to server\n",18);
        if(flag<0){
          perror("MyFIFO2 write error");
          exit(1);
        }
        break;
      }
    }
    wtnum=write(pfd[1],"server message:",15);
    wtnum=write(pfd[1],buff,strlen(buff));
    close(pfd[1]);
    waitpid(pid,NULL,0);
    exit(0);
  }
}
```
