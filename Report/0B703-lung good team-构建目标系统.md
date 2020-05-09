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
![(1)](https://https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/1.png)

### 2、然后安装所需要的软件：
1、下载安装git:    Sudo apt-get install git bc
2、下载linux内核： git clone git://github.com/raspberrypi/linux.git
3、将编译器路径加入系统变量中：打开 .bashrc文件，例如加入，
    export  PATH=$PATH:$HOME/rasppi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin

### 3、再根据树莓派的型号，选择默认的系统配置选项，效果如下：
![(2)](https://https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/2.png)

### 4、再编译内核，并配置模块
执行make -j4 zImage  modules  dtbs：
![(3)](https://https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/3.png)
执行sudo make modules_install：
![(4)](https://https://github.com/Meleus/Lunggoodteam/blob/master/screencut/HW5/4.png)

## B、


#
***
# 四、实验总结
* 掌握了裁剪Linux内核的方法，理解了内核选项的意义

* 熟悉了编译内核并加载内核到目标系统的方法与过程

* 了解了模块与内核的关系，掌握了内核模块配置编译、安装与卸载流程，为进一步编程，如驱动编程打下了基础

* 掌握了创建、安装(卸载)并使用文件系统的方法
