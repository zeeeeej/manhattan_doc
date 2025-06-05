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
    备注：调用前须调用gsensor_init初始化，与gsensor_capture_init不可同时用。

    接口函数：int gsensor_capture_init();
    参数：无
    返回值：0为初始化成功，其他为初始化失败；
    备注：调用前须调用gsensor_init初始化，与get_angle不可同时用。

    接口函数：int gsensor_capture_deinit();
    参数：无
    返回值：0为成功，其他为失败；

    2）拍照
    头文件：photo.h
    接口函数：int qjy_take_photo(uint8_t type, uint8_t *id, uint8_t *img_name, uint8_t *path);
    参数：type 拍照类型  0 -- 陀螺仪触发   1 -- 上位机主动下发拍照指令    2 -- 第三方调用
    --    id 照片ID 第三方调用不需要设置，传入一个id便可
    --    img_name 照片名称 第三方调用设置
    --    path 照片存储路径 第三方调用设置

    3)串口发送
    头文件：uart.h
    接口函数：void rk_uart_sendbyte(uint8_t byte);
    参数：byte 发送的字符
    备注：调用发送前需要调用rs485_pwr_on打开485发送的标志，发送完之后需要调用rs485_pwr_off关闭，中间需要加延时，保证数据完整，main.c有参考test_485_send。

    4）串口数据解析
    头文件：uart.h
    接口函数：int qjy_uart_init(void* param, int8_t id);
    参数：param  uart数据解析回调函数，结构体recv_callback_func第一个参数uart_recv_callback为海大原来的协议解析，实现函数为qjy_uart_parser，cus_recv_callback为增加的第三方解析，实现方式可参考main.c
    --    id 初始化485的id地址

    5）外部接口
    头文件：api.h
    接口函数：int api_get_pic_compress();
    参数：无
    返回值：返回照片清晰度，100为最高

    接口函数：int api_get_gs_angle();
    参数：无
    返回值：返回陀螺仪触发角度

    接口函数：int api_get_gs_rotation();
    参数：无
    返回值：返回陀螺仪触发角度的方向，具体查看串口通信协议；

    接口函数：int api_get_max_pic_num();
    参数：无
    返回值：获取保存照片的最大张数
