# 多线程编程
### 小组名称：Lung good team
### 小组成员：杭锦泉 滕飞 朱君楷
***
# 一、实验目的
* 掌握基于pthread线程库的多线程编程技术。

* 掌握基本的线程间同步技术(sem,mutex)。

* 理解共享资源并掌握其操作方法。
***
# 二、实验内容
### 读者-写者问题多线程实现
* 一个数据集(如数据、文件等)被N个线程读写;
* 一些线程只要求读数据集内容,称为读者 (Reader),实验读者数
不少于6个;
* 另些线程要求修改数据集内容,称为写者 (Writer),实验写者数
不少于3个;
* 多个读者可以同时读数据集内容,不需要互斥操作;
* 一个写者不能和其他写者或读者同时访问数据集,换句话说,写
者和其他写者或读者之间必须互斥操作!
* 读者优先:如果有读者,写者需要等待!用于更新不频繁或更新
影响不显著、读者较多的场合;
* 写者优先:如果有写者,读者需要等待!用于更新频繁或更新影
响显著的场合。
***
# 三、实验过程和结果
## A、读者优先
读的操作结束前，有新的读加进来，继续读
读的操作结束前，没有新的读加进来，那么就写
写的时候，有读的进程排队，那么阻塞后面排队的写（写与排在它后面的任意线程互斥）

#### 1. 先考虑一种基本的输入：3个写&3个读同时进来（第一个是写）

![3_a_8](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_a_8.png)

这种情况下的策略：

![3_a_2](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_a_2.png)

运行结果：

![3_a_output1](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_a_output1.png)

说明：
这儿reader_reading有2个身份：当前在writing，则表示后面排队等的reader数目；当前在reading,则表示还没读完的reader数目。
```
    	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 1);
```
信号量sem1用来控制write，
```
	reader_reading++;
	if(reader_reading == 1){
		sem_wait(&sem1);
	}
```
在reader()函数内，reader_reading++后等于1，也就是说这是第一个reader,队列里有了reader，根据读者优先，要将writer阻塞
```
	reader_reading--;
	if(reader_reading == 0){
		sem_post(&sem1);
	}
```
reader_reading--后等于0，也就是说这是队伍里最后一个reader,需要唤醒其他write

#### 2. 可以看出上面整个流程大概4S，那么使情况更复杂一些

![3_a_7](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_a_7.png)

就是在3.5s时再加入3个读,预想的结果应该是这新加的3个读插在wid[2]前面

*结果：*

![3_a_output2](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_a_output2.png)

流程：
sem1=1, sem2=1

wid[0]-----sem1=0 "write 83" sem1 = 1

rid[0]-----reader_reading=1 sem2=0 sem1=0 sem2=1 "read 83"

于此同时，rid[1]&[2]也并行，reader_reading=3

rid[1]-----sem2=0 sem2=1 "read 83"

rid[2]-----sem2=0 sem2=1 "read 83" reader_reading=0 sem2=0 sem1=1 sem2=1

wid[1]-----sem1=0 这儿是3S "write 86" 这儿是4S

由于在3.5s加入了rid[3]&[4]&[5],reader_reading由0依次变为1、2、3，经过1时将sem1置0，于是wid[1]被阻塞

rid[3]、rid[4]、rid[5]类似的，"read 86"3次，reader_reading依次变为2、1、0，到0时将sem1置1，write被唤醒

wid[2]------"write 77"
***
## B、写者优先  
读的时候，如果有写的线程还在执行，那么阻塞后面的读线程  

#### 1. 写者思路  
写者开头处先输出一段字符串，表明线程开始执行：  
```
	printf("pthread id %ld:writer start\n",pthread_self());
```
因为同一时刻只能有一个写者，先定义一个互斥变量writeLock，当有写线程在执行时，先锁住该变量；同时，为了保证有写者访问数据集时，阻塞读者，我们又定义了一个互斥变量readLock，当有写者在执行操作时，锁住该变量：  
```
	writerNum++;
	if(writerNum==1){
	    pthread_mutex_lock(&readLock);
	}
	pthread_mutex_lock(&writeLock);
```
然后随机生成一个数据，写入到变量中，并显示：  
```
	int rd=rand()%1024;
	printf("pthread id %ld:write %d\n",pthread_self(),rd);
	data=rd;
	sleep(W_SLEEP);
```
再释放两个互斥变量：  
```
	pthread_mutex_unlock(&writeLock);
	writerNum--;
	if(writerNum==0){
	    pthread_mutex_unlock(&readLock);
	}
```
流程图如下所示：  
![3_b_1](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_b_1.png)

#### 2. 读者思路  
读者开始时先用字符串表明读者开始执行，并查询readLock互斥变量，如果变量没被锁住就继续执行，否则等待：  
```
	printf("pthread id %ld:reader start\n",pthread_self());
	pthread_mutex_lock(&readLock);
	pthread_mutex_unlock(&readLock);
```
然后读出数据，并显示：  
```
	printf("pthread id %ld:read %d\n",pthread_self(),data);
	sleep(R_SLEEP);
```
流程图如下所示：  
![3_b_2](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_b_2.png)

#### 3. 实验结果  
我们先创建一个写者线程和一个读者线程，再创建剩余的两个写者线程和五个读者线程：  
```
int main(){
    int i=0;
    pthread_create(&wid[0],NULL,writer,NULL);
    pthread_create(&rid[0],NULL,reader,NULL);
    for(i=1;i<Num_WRITER;i++){
        pthread_create(&wid[i],NULL,writer,NULL);
    }
    for(i=1;i<Num_READER;i++){
        pthread_create(&rid[i],NULL,reader,NULL);
    }
    sleep(10);
    return 0;
}
```
结果如下所示：  
![3_b_3](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_b_3.png)
可以看出，当读者和写者的线程都被创建后，优先执行了写者的操作。
***
# 四、实验总结
* 掌握了基于pthread线程库的多线程编程技术。
* 掌握了基本的线程间同步技术(sem,mutex)。
* 理解了共享资源并掌握其操作方法。
***
# 五、实验源码
```
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

#define Num_WRITER 3
#define Num_READER 6
#define W_SLEEP 1
#define R_SLEEP 3

pthread_t wid[Num_WRITER],rid[Num_READER];
pthread_mutex_t writeLock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readLock=PTHREAD_MUTEX_INITIALIZER;
int data=0;
int readerNum=0,writerNum=0;

void * writer(void* in){
    printf("pthread id %ld:writer start\n",pthread_self());
    writerNum++;
    if(writerNum==1){
        pthread_mutex_lock(&readLock);
    }
    pthread_mutex_lock(&writeLock);
    int rd=rand()%1024;
    printf("pthread id %ld:write %d\n",pthread_self(),rd);
    data=rd;
    sleep(W_SLEEP);
    pthread_mutex_unlock(&writeLock);
    writerNum--;
    if(writerNum==0){
        pthread_mutex_unlock(&readLock);
    }
    pthread_exit((void *) 0);
}

void * reader(void* in){
    printf("pthread id %ld:reader start\n",pthread_self());
    pthread_mutex_lock(&readLock);
    pthread_mutex_unlock(&readLock);
    printf("pthread id %ld:read %d\n",pthread_self(),data);
    sleep(R_SLEEP);
    pthread_exit((void *) 0);
}

int main(){
    int i=0;
    pthread_create(&wid[0],NULL,writer,NULL);
    pthread_create(&rid[0],NULL,reader,NULL);
    for(i=1;i<Num_WRITER;i++){
        pthread_create(&wid[i],NULL,writer,NULL);
    }
    for(i=1;i<Num_READER;i++){
        pthread_create(&rid[i],NULL,reader,NULL);
    }
    sleep(10);
    return 0;
}
```
***




















