# 构建主机端开发环境(下)
### 小组名称：Lung good team
### 小组成员：杭锦泉 滕飞 朱君楷
***
# 一、实验目的
* 学会下载安装Linux及相关工具到目标机或目标机仿真环境。

* 学会使用交叉编译器编译应用程序，并能够将可执行程序下载到目标机运行。

* 学会使用交叉调试工具gdb-gdbserver调试应用程序。
***
# 二、实验内容
* Host-Target 方案1：下载安装Linux系统到开发板，其中包括bootloader、内核、模块、库、文件系统等；建立host-target连接 ，常见连接有SSH，VNC，Putty。
* Host-Target 方案2：下载安装目标机仿真环境qemu，安装qemu树莓派系统。
* 构建并测试开发板交叉编译器。
* 建立gdb-gdbserver交叉调试环境并能够调试应用程序。
***
# 三、实验过程和结果
## A、Host-Target 方案
我们先尝试安装了qemu仿真环境及对应的树莓派系统，但因为不知道如何进行host和target端的通信，最后放弃了。
然后我们在买好的树莓派上安装好了系统，并通过ssh进行了连接，为了调试方便，我们后面使用了MobaXterm的rdp方式连接树莓派，效果如下：  
![(1)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/%20(1).png)

## B、构建并测试开发板交叉编译器
### 1、在host端安装交叉编译工具
我们先从github上下载工具：  
![(2)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/%20(2).png)  
将其加入环境变量：  
![(3)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/%20(3).png)  
查看是否安装成功：  
![(4)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/%20(4).png)  

### 2、测试效果
我们编写程序用c语言输出一个全局变量的值：  
![(5)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/%20(5).png)  
利用指令（arm-linux-gnueabihf-gcc main.c -o main -lm）对其进行交叉编译，可以看到生成了main.o文件  
![(6)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(6).png)  
用FileZilla将其传到树莓派中，可以看到，其能正常运行：  
![(7)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(7).png)  

## C、建立gdb-gdbserver交叉调试环境并能够调试应用程序
### 1、在target端安装gdb
从(http://ftp.gnu.org/gnu/gdb/gdb-8.2.tar.gz)下载gdb安装包，解压后进行编译：  
![(8)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(8).png)  
再执行sudo make install：  
![(9)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(9).png)  
再将生成的arm-linux-gdb文件拷贝到系统/usr/bin/目录下，这样便可以在任何地方很方便的调用。

### 2、测试效果
利用指令（arm-linux-gnueabihf-gcc -g main.c -o main_g -lm）生成gdb调试文件，并将其同原文件一同拷贝至树莓派中，进行gdb调试，效果如下：  
![(10)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(10).png)  

### 3、在target端安装gdb-server
进入/home/pi/gdb-8.2/gdb/gdbserver文件夹，执行如下指令：  
cd /home/pi/gdb-8.2/gdb/gdbserver  
export PATH=$PATH:/usr/local/arm-gdb/bin    
./configure --target=arm-linux --host=arm-linux  
make CC=arm-linux-gnueabihf-gcc  
即编译完了gdbserver，最终结果如下：  
![(11)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(11).png)

### 4、进行交叉调试
先将gdbserver复制到host端，然后再在target端启动文件，开始调试：  
![(12)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(12).png)  
再在host端也启动远程调试：  
![(13)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(13).png)  
此时，树莓派中断也显示在的debugging了：  
![(14)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(14).png)  
再在host端进行远程调试，尝试各种操作：  
![(15)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(15).png)  
可以看到，target端也有对应的输出：  
![(16)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(16).png)  
最后，程序运行结束，host端提示运行完毕，target端也输出了全部的内容：  
![(17)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(17).png)  
![(18)](https://github.com/Meleus/Lunggoodteam/raw/master/screencut/HW4/(18).png)  
***
# 四、实验总结
* 学会了下载安装Linux及相关工具到目标机或目标机仿真环境。

* 学会了使用交叉编译器编译应用程序，并能够将可执行程序下载到目标机运行。

* 学会了使用交叉调试工具gdb-gdbserver调试应用程序。
