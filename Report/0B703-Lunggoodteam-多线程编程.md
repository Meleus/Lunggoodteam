#多线程编程
###小组名称：Lung good team
###小组成员：杭锦泉 滕飞 朱君楷
***
#一、实验目的
* 掌握基于pthread线程库的多线程编程技术。

* 掌握基本的线程间同步技术(sem,mutex)。

* 理解共享资源并掌握其操作方法。
***
#二、实验内容
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
#三、实验过程和结果
## A、读者优先
读的操作结束前，有新的读加进来，继续读
读的操作结束前，没有新的读加进来，那么就写
写的时候，有读的进程排队，那么阻塞后面排队的写（写与排在它后面的任意线程互斥）

#### 1. 先考虑一种基本的输入：3个写&3个读同时进来（第一个是写）

![3_a_1](screencut/HW3/3_a_1.png)

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

![3_a_4](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW3/3_a_4.png)

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




































