# 构建目标系统
### 小组名称：Lung good team
### 小组成员：杭锦泉 滕飞 朱君楷
***
# 一、实验目的
## 能够综合应用课程所学的技术与工具，包括：
* Socket通信

* 多进程、多线程编程

* 交叉调试目标端程序

* 磁盘分区与文件系统创建

* 模块与驱动编程
***
# 二、实验内容
* 将树莓派设为智能家居Linux服务器，可用来采集并维护环境数据，如PM2.5、温度、湿度、气味、电器状态数据等。在实际环境中数据来自相应的传感器，本次试验中用scull设备模拟。有条件的小组鼓励使用真实设备采集数据。

* 要求创建2个以上的scull设备，设备驱动可选择从内核源码树外(Kbuild)编译安装，或加入到内核源码树内。驱动函数要求包括： open, release, read, write, llseek, ioctl。

* 实验中的环境数据存储在特定文件系统中。该文件系统要求具备属性：在线写入、持久性、断电可靠性。

* PC机、移动设备或另外一个树莓派用作远程客户端，随时请求获取环境数据，客户端和服务器之间采用Socket通信。

* APP编译采用交叉编译，用gdb-gdbserver交叉调试APP。
***
# 三、实验过程和结果
## A、构建和配置内核树
* 首先查看自己的linux内核的版本，在终端中输入： `uname -r`
* 选择同自己内核相应的linux-source版本安装。在终端中输入：
　　`sudo apt-get install linux-source-4.15.0`
* 下载完毕后在/usr/src/下解压linux-source-4.15.0.tar.bz2
解压方法,在终端中输入: sudo -i 切换到根用户下,定位到/usr/src/目录下，终端输入：`tar jxvf linux-source-4.15.0.tar.bz2`

* 开始配置内核，选择最快的原版的配置（默认）方式：在终端中输入：`make oldconfig`

* 安装一些必要的组件`sudo apt-get install libncurses5-dev libssl-dev`

* 然后编译模块，定位到源代码文件夹下，在终端中输入： `make modules`

* make(这两步要保证/home有足够的空间，它不会一开始跟你说，跑了个把钟头再中断就哭了)

* 完成之后，安装模块，在终端中输入：`make modules_install`

* 一般这儿不报错就说明你安装成功了，你也可以按照PPT68～69，用个简单的模块尝试

随意位置下，创建modemo.c、Makefile
依次执行：`make`、`ls`、`insmod modemo.ko`、`lsmod`

![mod_ep](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/Final/mod_ep.png)

这属于模块放在内核源码树外

我insmod出了问题，[解决办法](https://blog.csdn.net/m0_38066161/article/details/81812816)

## B、创建scull设备及相关设备驱动
* 代码文件

`scull.c`、`scull.h`、`Makefile`如文末所示

在3.3版本之后的内核编译中，头文件的名称有所[改变](https://blog.csdn.net/qq_40421682/article/details/97261197)

函数名也有些许改动
> copy_to_user()改为raw_copy_to_user();

> copy_from_user()改为raw_copy_from_user();

> init_MUTEX((&scull_device->sem);改为sema_init(&scull_device->sem, 1);

上述代码已经改过了

`#define MEMDEV_NR_DEVS 2 /*设备数*/`

* 创建完成后`make`

* 依次执行`ls`、`insmod memdev.ko`、`cat /proc/devices`

![devices](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/Final/devices.png)

可以看到scull驱动程序被正确的插入到内核当中，主设备号为241

## C、测试驱动程序
* 首先在/dev/目录下创建与该驱动程序相对应的文件节点

`mknod scull0 c 241 0`

* 使用ls查看创建好的驱动程序节点文件

`ls -al memdev0`

* 编写`test.c`，来对驱动程序进行测试

* 编译并执行该程序

`gcc -o test test.c`、`./test`

* 结果

![scull_result](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/Final/scull_result.png)

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
