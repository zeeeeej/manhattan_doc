1、SDK目录层级：
app_sdk_vx.x.x
    -------- bin 存放编译后的程序目录
    -------- build cmake后存放的makefile及一些中间文件
    -------- CMakeLists.txt cmake文件
    -------- include 头文件目录
    -------- libs 引用的库文件
    -------- prebuilt 所使用的编译链
    -------- src 程序源文件

2、使用方法：
    1）进入build目录执行cmake ..
    2) 然后执行make
    3）生成的程序会在build目录下
