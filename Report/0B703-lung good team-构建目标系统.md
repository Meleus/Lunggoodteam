# 构建目标系统
### 小组名称：Lung good team
### 小组成员：杭锦泉 滕飞 朱君楷
***
# 一、实验目的
* 掌握裁剪Linux内核的方法，理解内核选项的意义

* 熟悉编译内核并加载内核到目标系统的方法与过程

* 了解模块与内核的关系，掌握内核模块配置编译、安装与卸载流程，为进一步编程，如驱动编程打下基础

* 掌握创建、安装(卸载)并使用文件系统的方法
***
# 二、实验内容
* 首先用默认配置重新编译一遍已安装到开发板的内核，将新的内核替换现有内核，检查是否通过
* 在原始版本基础上，重新配置Linux内核，构建一个嵌入式的Linux内核
* 编译安装重新配置后的内核、模块及dtbs设备树
* 针对默认配置中不少于10个kernel feature进行重新配置（裁剪为主、偶有增加），并解释理由；(Y=>N，N=>Y)
* 保留必要的模块安装，剩余(占多数)取消；(M=>N)
* make后将新的内核安装到开发板运行测试
* 选择至少二个模块加载与卸载，检查是否加载、卸载成功
***
# 三、实验过程和结果
## A、用默认配置重新编译一遍已安装到开发板的内核，将新的内核替换现有内核，检查是否通过
### 1、先远程登录树莓派，记录下树莓派当前内核版本：
![1](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/1.png)

### 2、然后安装所需要的软件：
1、下载安装git:    Sudo apt-get install git bc  
2、下载linux内核： git clone git://github.com/raspberrypi/linux.git  
3、将编译器路径加入系统变量中：打开 .bashrc文件，例如加入，  
    export  PATH=$PATH:$HOME/rasppi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin

### 3、再根据树莓派的型号，选择默认的系统配置选项，效果如下：
![2](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/2.png)

### 4、再编译内核，并配置模块
执行make -j4 zImage  modules  dtbs：  
![3](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/3.png)  
执行sudo make modules_install：  
![4](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/4.png)
再执行以下命令：  
sudo cp arch/arm/boot/dts/*.dtb /boot/  
sudo cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/  
sudo cp arch/arm/boot/dts/overlays/REAOME /boot/overlays/   
sudo cp arch/arm/boot/zImage /boot/$KERNEL.img  

### 5、重启树莓派，查看效果  
![5](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/5.png)

## B、在原始版本基础上，重新配置Linux内核，构建一个嵌入式的Linux内核  
### 1、先远程登录树莓派，记录下启动文件boot的大小：  
![6](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/6.png)  

### 2、开始重新配置内核：  
![7](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/7.png)  

### 3、移出部分内核模块：  
因为本机中没有无线电，也没有RF切换设备，先移出Amateur和RF switch这两个设备：  
![7](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/8.png)  
因为本机中不需要交换分区及openMPI快速进程通信或调试程序，去除Support for paging of anonymous memory (swap)和Enable process_vm_readv/writev syscalls：  
![8](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/10.png)  
去除Automatic process group scheduling、support initial ramdisks compressed using LZMA等管理内存盘压缩模式的模块：  
![9](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/11.png)  
启动设备没有2TB大，移出Support for large (2TB+) block devices and files模块：  
![10](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/12.png)  
 取消对模拟电视信号及AM/FM无线电接收机信号的支持：  
 ![12](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/14.png)  
 
### 4、添加部分内核模块：  
增加对IDT ICS932S40系列时钟频率控制芯片的支持，增加Integrated Circuits ICS932S401：  
![11](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/13.png)  
增加Platform Support For Chrome Hardware模块及Hardware Spinlock Drivers模块：  
![13](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/15.png)  
保存配置：  
![14](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/16.png)  

### 5、重新编译内核：  
![15](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/17.png)  
![16](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/18.png)  

### 6、查看效果：  
查看启动文件boot大小：  
![19](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/19.png)  
比原来的小了40k左右的大小
 
## C、选择至少二个模块加载与卸载，检查是否加载、卸载成功  
### 1、查看总共有哪些模块：  
![18](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/20.png)  
### 2、先尝试卸载hci_uart及bnep，但都被占用了：  
![19](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/21.png)  
### 3、根据lsmod显示的结果，卸载未被占用的模块fixed：  
![20](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/22.png)  
查看结果，模块确实被卸载了：
![21](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/23.png)  
### 4、再尝试先卸载i2c模块再重新加载：  
![21](https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/24.png)  

***
# 四、实验总结
* 掌握了裁剪Linux内核的方法，理解了内核选项的意义
* 熟悉了编译内核并加载内核到目标系统的方法与过程
* 了解了模块与内核的关系，掌握了内核模块配置编译、安装与卸载流程，为进一步编程，如驱动编程打下了基础
* 掌握了创建、安装(卸载)并使用文件系统的方法
