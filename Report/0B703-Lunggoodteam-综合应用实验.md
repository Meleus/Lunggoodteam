# 构建目标系统
### 小组名称：Lung good team
### 小组成员：杭锦泉 滕飞 朱君楷
***
# 一、实验目的
* 综合应用之前所学的各种linux功能

***
# 二、实验内容
* 1.将树莓派设为智能家居Linux服务器，可用来采集并维护环境数据，如PM2.5、温度、湿度、气味、电器状态数据等。在实际环境中数据来自相应的传感器，本次试验中用scull设备模拟。有条件的小组鼓励使用真实设备采集数据。
* 2.要求创建2个以上的scull设备，设备驱动可选择从内核源码树外(Kbuild)编译安装，或加入到内核源码树内。驱动函数要求包括： open, release, read, write, llseek, ioctl。
* 3.实验中的环境数据存储在特定文件系统中。该文件系统要求具备属性：在线写入、持久性、断电可靠性。
* 4.PC机、移动设备或另外一个树莓派用作远程客户端，随时请求获取环境数据，客户端和服务器之间采用Socket通信。
* 5.APP编译采用交叉编译，用gdb-gdbserver交叉调试APP。
***
# 三、实验过程和结果
## A、创建scull设备及相关设备驱动 
### 1、编写scull设备的代码文件：
编写scull.c、scull.h、Makefile及build文件如文末所示，实现了open, release, read, write, llseek, ioctl这些功能。

### 2、创建完成之后，编译代码 
执行make指令，效果如下：  
![1](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(1).png) 

### 3、再将该设备安装到内核源码树外
执行insmod scull.ko指令： 
![2](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(2).png)
再执行cat /proc/devices： 
![3](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(3).png)
可以看到scull驱动程序被正确的插入到内核当中，主设备号为240

### 4、测试驱动程序 
首先执行mknod scull0 c 240 0指令，在/dev/目录下创建与该驱动程序相对应的文件节点
![4](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(4).png)  
再执行ls -al scull0，使用ls查看创建好的驱动程序节点文件：  
![5](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(5).png)  
编写test.c文件如文末所示，来对驱动程序进行测试，然后编译并执行该程序，效果如下：  
![6](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(6).png)  

## B、建立文件系统  
### 1、将SD卡插入读卡器后，插入到电脑上，打开虚拟机，用GParted软件调节大小： 
![7](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(7).png)  
![8](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(8).png)  

### 2、把SD卡插回树莓派，在树莓派中查看已有的分区的大小和位置：  
![9](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(9).png)  

### 3、利用刚刚空出来的空间建立新的分区：  
执行sudo fdisk指令，建立新的分区：  
![10](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(10).png)    
![11](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(11).png)  
执行完毕后查看新建立的分区： 
![12](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(12).png)  

### 4、格式化新分区并挂载：
将新分区格式化为ext3格式： 
![13](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(13).png)  
再用sudo mount -t ext3 /dev/mmcblk0p3 /mnt/ext3fs 指令将新分区挂载到操作系统中： 
![14](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(14).png)  
 
## C、建立虚拟机和树莓派之间的socket连接：  
### 1、编写客户端和服务器端的测试程序client.c和server.c如文末所示，分别放到虚拟机和树莓派上后，进行编译：  
![15](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(15).png)  
![16](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(16).png)  
### 2、先执行服务器端的程序，再执行客户端程序，实现两个设备之间的socket通信：  
![17](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(17).png)  
![18](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(18).png)  

## D、综合上述功能，实现一个完整的项目。 
### 1、先在ext3文件系统下创建两个scull设备:  
执行如下指令:  
![19](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(19).png)  

### 2、向两个scull设备中写入一些随机数: 
写入的数字如下所示：  
![20](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(20).png)  

### 3、实现一个完整的项目: 
编写main.c文件如文末所示。主要实现的功能为：从两个scull设备中分别读取数据，做好设备的序号的标记后，再通过socket通信发送到虚拟机上，虚拟机显示收到的数据，并返回给树莓派，树莓派再显示收到的消息。不断执行，直到数据被读取完。 
最终虚拟机端显示的效果为：  
![21](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(21).png)  
树莓派端显示的效果为： 
![22](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/%20(22).png)  

## D、用gdb-gdbserver交叉调试APP：
### 1、在虚拟机上编译main.c文件: 
![23](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/(23).png)  

### 2、将编译得到的main_g文件拷贝到树莓派上，并启动调试: 
![24](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/(24).png)  

### 3、在虚拟机上也启动调试，并连接到树莓派:  
![25](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/(25).png)  

### 4、此时，树莓派中断也显示在的debugging了： 
![26](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/(26).png)  

### 5、再在host端进行远程调试，尝试各种操作： 
![27](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/(27).png)  

### 6、可以看到，树莓派端也有对应的输出： 
![28](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW6/(28).png)  
一直运行到最后，程序运行结束，host端提示运行完毕，target端也输出了全部的内容。

***
# 四、实验总结
* 学习了scull设备的创建与使用，了解了模块与驱动编程
* 再次学习了磁盘分区与文件系统的创建
* 学会了socket通信的使用
* 再次练习了交叉调试目标端程序
* 练习了多进程、多线程编程

***
# 四、代码部分
* scull.c

```
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include "scull.h"
#define SCULL_CMD1 1
#define SCULL_CMD2 2
#define SCULL_CMD3 3
int scull_major = SCULL_MAJOR;
int scull_minor = 0;
 
module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
 
struct scull_dev *scull_device;
 
int scull_trim(struct scull_dev *dev)
{
    if (dev)
    {
        if (dev->data)
        {
            kfree(dev->data);
        }
        dev->data = NULL;
        dev->size = 0;
    }
    return 0;
}
 
int scull_open(struct inode *inode, struct file *filp)
{
    struct scull_dev *dev;
 
    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev;
 
    if ((filp->f_flags & O_ACCMODE) == O_WRONLY)
    {
        if (down_interruptible(&dev->sem))
        {
            return -ERESTARTSYS;
        }
        scull_trim(dev);
        up(&dev->sem);
    }
 
    return 0;
}
 
int scull_release(struct inode *inode, struct file *filp)
{
    return 0;
}
 
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct scull_dev *dev = filp->private_data;
    ssize_t retval = 0;
 
    if (down_interruptible(&dev->sem))
    {
        return -ERESTARTSYS;
    }
    if (*f_pos >= dev->size)
    {
        goto out;
    }
    if (*f_pos + count > dev->size)
    {
        count = dev->size - *f_pos;
    }
 
    if (!dev->data)
    {
        goto out;
    }
 
    if (raw_copy_to_user(buf, dev->data + *f_pos, count))
    {
        retval = -EFAULT;
        goto out;
    }
 
    *f_pos += count;
    retval = count;
 
    out:
        up(&dev->sem);
        return retval;
}
 
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    struct scull_dev *dev = filp->private_data;
    ssize_t retval = -ENOMEM;
 
    if (down_interruptible(&dev->sem))   
    {
        return -ERESTARTSYS;
    }
 
    if (!dev->data)   
    {
        dev->data = kmalloc(SCULL_BUFFER_SIZE, GFP_KERNEL);
        if (!dev->data)
        {
            goto out;
        }
        memset(dev->data, 0, SCULL_BUFFER_SIZE);
     }
 
    if (count > SCULL_BUFFER_SIZE - dev->size)
    {
        count = SCULL_BUFFER_SIZE - dev->size;
    }
 
    if (raw_copy_from_user(dev->data + dev->size, buf, count))
    {
        retval = -EFAULT;
        goto out;
    }
     
    dev->size += count;
    retval = count;
 
    out:
        up(&dev->sem);
        return retval;
}
 
loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
    struct scull_dev *dev = filp->private_data;
    loff_t newpos;
 
    switch(whence)
    {
        case 0:
            newpos = off;
            break;
        case 1:
            newpos = filp->f_pos + off;
            break;
        case 2:
            newpos = dev->size + off;
            break;
        default:
            return -EINVAL;
    }
    if (newpos < 0)
    {
        return -EINVAL;
    }
    filp->f_pos = newpos;
    return newpos;
}

int scull_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
if (cmd == SCULL_CMD1) {
printk("running SCULL_CMD1 \n");
return 0;
}
if (cmd == SCULL_CMD2) {
printk(" running SCULL_CMD2 \n");
return 0;
}
if (cmd == SCULL_CMD3) {
printk(" running SCULL_CMD3 \n");
return 0;
}
printk("cmd error! \n");
return -EFAULT;
}
 
struct file_operations scull_fops = {
    .owner = THIS_MODULE,
    .llseek = scull_llseek,
    .read = scull_read,
    .write = scull_write,
    .open = scull_open,
    .release = scull_release,   
};
 
void  scull_cleanup_module(void)
{
    dev_t devno = MKDEV(scull_major, scull_minor);
 
    if (scull_device)
    {
        scull_trim(scull_device);
        cdev_del(&scull_device->cdev);
        kfree(scull_device);   
    }
    unregister_chrdev_region(devno, 1);
}
 
static void scull_setup_cdev(struct scull_dev *dev)
{
    int err, devno = MKDEV(scull_major, scull_minor);
 
    cdev_init(&dev->cdev, &scull_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &scull_fops;
    err = cdev_add(&dev->cdev, devno, 1);
 
    if (err)
    {
        printk(KERN_NOTICE "Error %d adding scull", err);
    }
}
 
static int __init scull_init_module(void)
{
    int result;
    dev_t dev = 0;
 
    if (scull_major)   
    {
        dev = MKDEV(scull_major, scull_minor);
        result = register_chrdev_region(dev, 1, "scull");
    }
    else
    {
        result = alloc_chrdev_region(&dev, scull_minor, 1, "scull");
        scull_major = MAJOR(dev);
    }
    if (result < 0)
    {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }
 
    scull_device = kmalloc(sizeof(struct scull_dev), GFP_KERNEL);       
    if (!scull_device)
    {
        result = -ENOMEM;
        goto fail;
    }
    memset(scull_device, 0, sizeof(struct scull_dev));
 
    sema_init(&scull_device->sem, 1);
 
    scull_setup_cdev(scull_device);
 
    return 0;
 
    fail:
        scull_cleanup_module();
        return result;
}
 
module_init(scull_init_module);
module_exit(scull_cleanup_module);
 
MODULE_LICENSE("GPL");
```

* scull.h

```
#ifndef _SCULL_H
#define _SCULL_H
 
#define SCULL_MAJOR 0
#define SCULL_BUFFER_SIZE PAGE_SIZE
 
struct scull_dev {
    char *data;
    unsigned long size;
    struct semaphore sem;
    struct cdev cdev;
};
 
#endif
```

* Makefile

```
#sample driver module
    obj-m := scull.o
    KDIR = /root/linux-4.15.0/
    PWD:=$(shell pwd)
    INSTALLDIR=$(PWD)
modules:
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd` modules
clean:
	rm -f *.mod.c *.mod.o *.ko *.o *.tmp_versions
.PHONY:
	modules clean
```

* test.c

```
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>

int main(){
int fd, retval;
char W_buffer[26];
char R_buffer[26];
fd=open("/dev/scull0", O_RDWR);

retval = ioctl(fd, 1, 0);
printf("io control %d\n", retval);
retval = write(fd, W_buffer, 26);
printf("write %d\n", retval);
retval = lseek(fd, 0, 0);
retval = read(fd, R_buffer, 26);
printf("read %d\n", retval);
close(fd);
return 0;
}
```

client.c

```
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc,char *argv[])
{
    int sockfd,numbytes;
    char buf[BUFSIZ];
    struct sockaddr_in their_addr;
    printf("break!");
    while((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1);
    printf("We get the sockfd~\n");
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(8000);
    their_addr.sin_addr.s_addr=inet_addr("192.168.101.50");
    bzero(&(their_addr.sin_zero), 8);
    
    while(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) == -1);
    printf("Get the Server~Cheers!\n");
    numbytes = recv(sockfd, buf, BUFSIZ,0);//接收服务器端信息  
    buf[numbytes]='\0';  
    printf("%s\n",buf);
    while(1)
    {
        printf("Entersome thing:");
        scanf("%s",buf);
        numbytes = send(sockfd, buf, strlen(buf), 0);
        numbytes=recv(sockfd,buf,BUFSIZ,0);  
        buf[numbytes]='\0'; 
        printf("received:%s\n",buf);  
    }
    close(sockfd);
    return 0;
}
```

server.c

```
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd, new_fd, struct_len, numbytes,i;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char buff[BUFSIZ];

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero), 8);
    struct_len = sizeof(struct sockaddr_in);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    while(bind(fd, (struct sockaddr *)&server_addr, struct_len) == -1);
    while(listen(fd, 10) == -1);
	new_fd = accept(fd, (struct sockaddr *)&client_addr, &struct_len);
    printf("Get the Client.\n");
    while((numbytes = recv(new_fd, buff, BUFSIZ, 0)) > 0)
    {
		int scull_num=buff[numbytes-1];
        buff[numbytes-1] = '\0';
        printf("Msg from scull%d:%s\n",scull_num,buff);
            if(send(new_fd,buff,numbytes,0)<0)
            {
                perror("write");
                return 1;
            }
    }
    close(new_fd);
    close(fd);
    return 0;
}
```

main.c

```
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc,char *argv[])
{
	int sockfd,numbytes;
	char buf[BUFSIZ];
	struct sockaddr_in their_addr;
	int fd0,fd1, retval;
	char W_buffer0[26],W_buffer1[26];
	int temp0=10,temp1=90;
	for(int i=0;i<13;i++){
		W_buffer0[2*i]=temp0/10+'0';
		W_buffer0[2*i+1]=temp0%10+'0';
		W_buffer1[2*i]=temp1/10+'0';
		W_buffer1[2*i+1]=temp1%10+'0';
		temp0+=2;
		temp1-=3;
	}
	char R_buffer0[26]="",R_buffer1[26]="";
	fd0=open("/mnt/ext3fs/scull0", O_RDWR);
	fd1=open("/mnt/ext3fs/scull1", O_RDWR);
	while((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1);
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(8000);
	their_addr.sin_addr.s_addr=inet_addr("192.168.101.53");
	bzero(&(their_addr.sin_zero), 8);
    
	while(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) == -1);
	printf("Get the Server\n");

	//retval = write(fd0, W_buffer0, 26);
	//printf("scull0 write \"%s\" \n", W_buffer0);
	//retval = write(fd1, W_buffer1, 26);
	//printf("scull1 write \"%s\" \n", W_buffer1);
	
	lseek(fd0, 0, 0);
	lseek(fd1, 26, 0);

	for(int i=0;i<13;i++)
	{
		//retval = lseek(fd0, 0, 1);
		//printf("%d\n",retval);
		retval = read(fd0, R_buffer0, 2);
		printf("read %d words from scull0\n", retval);

		//retval = lseek(fd1, 0, 1);
		//if(retval==-1) break;
		retval = read(fd1, R_buffer1, 2);
		printf("read %d words from scull1\n", retval);
		R_buffer1[2]='\0';

        	printf("Send to Server:");
        	printf("scull0:%s,scull1:%s\n",R_buffer0,R_buffer1);
		
		R_buffer1[2]=1;
        	numbytes = send(sockfd, R_buffer0, strlen(R_buffer0)+1, 0);
        	numbytes=recv(sockfd,buf,BUFSIZ,0);  
        	buf[numbytes]='\0'; 
        	printf("received:%s\n",buf);

		numbytes = send(sockfd, R_buffer1, strlen(R_buffer1), 0);
        	numbytes=recv(sockfd,buf,BUFSIZ,0);  
        	buf[numbytes]='\0'; 
        	printf("received:%s\n",buf); 
		
		R_buffer1[2]=0;
	}
	close(sockfd);
	close(fd0);
	close(fd1);
	return 0;
}
```
