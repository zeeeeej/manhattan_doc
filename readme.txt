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

3、sdk接口：
    1）陀螺仪
    头文件：mpu6887p.h
    接口函数：int get_angle();
    参数：无
    返回值：返回开门时陀螺仪累计的角度值，关门后置0；

    2）拍照
    头文件：photo.h
    接口函数：int qjy_take_photo(uint8_t type, uint8_t *id, uint8_t *img_name, uint8_t *path);
    参数：type 拍照类型  0 -- 陀螺仪触发   1 -- 上位机主动下发拍照指令    2 -- 第三方调用
    --    id 照片ID 第三方调用不需要设置，传入一个id便可
    --    img_name 照片名称 第三方调用设置
    --    path 照片存储路径 第三方调用设置
