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
* 首先查看自己的linux内核的版本，在终端中输入： uname -r
* 选择同自己内核相应的linux-source版本安装。在终端中输入：
　　sudo apt-get install linux-source-4.15.0
* 下载完毕后在/usr/src/下解压linux-source-2.6.35.tar.bz2
解压方法,在终端中输入: sudo -i 切换到根用户下,定位到/usr/src/目录下，终端输入：tar jxvf linux-source-4.15.0.tar.bz2
* 开始配置内核，选择最快的原版的配置（默认）方式：在终端中输入：make oldconfig
* 安装一些必要的组件sudo apt-get install libncurses5-dev libssl-dev
* 然后编译模块，定位到源代码文件夹下，在终端中输入： make modules
* make(这两步要保证/home有足够的空间，它不会一开始跟你说，跑了个把钟头再中断就哭了)
* 完成之后，安装模块，在终端中输入：make modules_install
* 一般这儿不报错就说明你安装成功了，你也可以按照PPT68～69，用个简单的模块尝试
随意位置下，创建modemo.c、Makefile
依次执行：make、ls、insmod modemo.ko、lsmod
![mod_ep](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/Final/mod_ep.png)
这属于模块放在内核源码树外
我insmod出了问题，解决办法：https://blog.csdn.net/m0_38066161/article/details/81812816

## B、创建scull设备及相关设备驱动
* 代码文件
'memdev.c'
'memdev.h'
'Makefile'
在3.3版本之后的内核编译中，头文件的名称有所改变
https://blog.csdn.net/qq_40421682/article/details/97261197
上述代码已经改过了
* 创建完成后make
![scull_make](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/Final/scull_make.png)
* 依次ls、insmod memdev.ko、cat /proc/devices
![devices](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/Final/devices.png)
可以看到memdev驱动程序被正确的插入到内核当中，主设备号为260，该设备号为memdev.h中定义的#define MEMDEV_MAJOR 260。

## C、测试驱动程序
* 首先在/dev/目录下创建与该驱动程序相对应的文件节点
mknod memdev0 c 260 0
* 使用ls查看创建好的驱动程序节点文件
ls -al memdev0
* 编写'memtest.c'，来对驱动程序进行测试
* 编译并执行该程序
gcc -o memtest memtest.c
./memtest
* 结果
![scull_result](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/Final/scull_result.png)
# 四、代码部分
* memdev.c

```
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/switch_to.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "memdev.h"

static int mem_major = MEMDEV_MAJOR;

module_param(mem_major, int, S_IRUGO);

struct mem_dev *mem_devp; /*设备结构体指针*/

struct cdev cdev;

/*文件打开函数*/
int mem_open(struct inode *inode, struct file *filp)
{
    struct mem_dev *dev;
    
    /*获取次设备号*/
    int num = MINOR(inode->i_rdev);

    if (num >= MEMDEV_NR_DEVS)
            return -ENODEV;
    dev = &mem_devp[num];
    
    /*将设备描述结构指针赋值给文件私有数据指针*/
    filp->private_data = dev;
    
    return 0;
}

/*文件释放函数*/
int mem_release(struct inode *inode, struct file *filp)
{
  return 0;
}

/*读函数*/
static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
  unsigned long p = *ppos;
  unsigned int count = size;
  int ret = 0;
  struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/

  /*判断读位置是否有效*/
  if (p >= MEMDEV_SIZE)
    return 0;
  if (count > MEMDEV_SIZE - p)
    count = MEMDEV_SIZE - p;

  /*读数据到用户空间*/
  if (copy_to_user(buf, (void*)(dev->data + p), count))
  {
    ret = - EFAULT;
  }
  else
  {
    *ppos += count;
    ret = count;
    
    printk(KERN_INFO "read %d bytes(s) from %d\n", count, p);
  }

  return ret;
}

/*写函数*/
static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
  unsigned long p = *ppos;
  unsigned int count = size;
  int ret = 0;
  struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/
  
  /*分析和获取有效的写长度*/
  if (p >= MEMDEV_SIZE)
    return 0;
  if (count > MEMDEV_SIZE - p)
    count = MEMDEV_SIZE - p;
    
  /*从用户空间写入数据*/
  if (copy_from_user(dev->data + p, buf, count))
    ret = - EFAULT;
  else
  {
    *ppos += count;
    ret = count;
    
    printk(KERN_INFO "written %d bytes(s) from %d\n", count, p);
  }

  return ret;
}

/* seek文件定位函数 */
static loff_t mem_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t newpos;

    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = offset;
        break;

      case 1: /* SEEK_CUR */
        newpos = filp->f_pos + offset;
        break;

      case 2: /* SEEK_END */
        newpos = MEMDEV_SIZE -1 + offset;
        break;

      default: /* can't happen */
        return -EINVAL;
    }
    if ((newpos<0) || (newpos>MEMDEV_SIZE))
     return -EINVAL;
     
    filp->f_pos = newpos;
    return newpos;

}

/*文件操作结构体*/
static const struct file_operations mem_fops =
{
  .owner = THIS_MODULE,
  .llseek = mem_llseek,
  .read = mem_read,
  .write = mem_write,
  .open = mem_open,
  .release = mem_release,
};

/*设备驱动模块加载函数*/
static int memdev_init(void)
{
  int result;
  int i;

  dev_t devno = MKDEV(mem_major, 0);

  /* 静态申请设备号*/
  if (mem_major)
    result = register_chrdev_region(devno, 2, "memdev");
  else /* 动态分配设备号 */
  {
    result = alloc_chrdev_region(&devno, 0, 2, "memdev");
    mem_major = MAJOR(devno);
  }
  
  if (result < 0)
    return result;

  /*初始化cdev结构*/
  cdev_init(&cdev, &mem_fops);
  cdev.owner = THIS_MODULE;
  cdev.ops = &mem_fops;
  
  /* 注册字符设备 */
  cdev_add(&cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);
   
  /* 为设备描述结构分配内存*/
  mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);
  if (!mem_devp) /*申请失败*/
  {
    result = - ENOMEM;
    goto fail_malloc;
  }
  memset(mem_devp, 0, sizeof(struct mem_dev));
  
  /*为设备分配内存*/
  for (i=0; i < MEMDEV_NR_DEVS; i++)
  {
        mem_devp[i].size = MEMDEV_SIZE;
        mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
        memset(mem_devp[i].data, 0, MEMDEV_SIZE);
  }
    
  return 0;

  fail_malloc:
  unregister_chrdev_region(devno, 1);
  
  return result;
}

/*模块卸载函数*/
static void memdev_exit(void)
{
  cdev_del(&cdev); /*注销设备*/
  kfree(mem_devp); /*释放设备结构体内存*/
  unregister_chrdev_region(MKDEV(mem_major, 0), 2); /*释放设备号*/
}

MODULE_AUTHOR("David Xie");
MODULE_LICENSE("GPL");

module_init(memdev_init);
module_exit(memdev_exit);
```

* memdev.h

```
#ifndef _MEMDEV_H_
#define _MEMDEV_H_

#ifndef MEMDEV_MAJOR
#define MEMDEV_MAJOR 260 /*预设的mem的主设备号*/
#endif

#ifndef MEMDEV_NR_DEVS
#define MEMDEV_NR_DEVS 2 /*设备数*/
#endif

#ifndef MEMDEV_SIZE
#define MEMDEV_SIZE 4096
#endif

/*mem设备描述结构体*/
struct mem_dev
{
  char *data;
  unsigned long size;
};

#endif /* _MEMDEV_H_ */
```

* Makefile

```
ifneq ($(KERNELRELEASE),)
	obj-m:=memdev.o
else
	KERNELDIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.mod.c *.mod.o *.ko
endif
```

* memtest.c

```
#include <stdio.h>

int main()
{
    FILE *fp0 = NULL;
    char Buf[4096];   

    fp0 = fopen("/dev/memdev0", "r+");
    if (fp0 == NULL)
    {
        printf("Open Memdev0 Error!\n");
        return -1;
    }
    else
    {
    printf("打开Memdev0成功啦!\n");
    }

    strcpy(Buf, "Mem is char device!");
    printf("写入内容BUF: %s \n", Buf);

    fwrite(Buf, sizeof(Buf), 1, fp0);

    fseek(fp0, 0, SEEK_SET);

    strcpy(Buf, "Buf is NULL!");
    printf("现在清空BUF: %s \n", Buf);

    fread(Buf, sizeof(Buf), 1, fp0);

    printf("读回内容BUF: %s \n", Buf);

    return 0;


}
```
