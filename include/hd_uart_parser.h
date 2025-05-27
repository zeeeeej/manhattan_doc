#ifndef __HD_UART_PARSER__
#define __HD_UART_PARSER__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 收到3.17 广播门开事件（0x1E）
 * @param action_id_str action_id（时间戳（4字节）+序号（1字节））转成的字符串格式：时间戳（单位：秒）+序号（3位的10进制，范围0～255）
 */
typedef void (*hd_on_action_id_changed)(const char *action_id_str);

/**
 * 初始化
 *
 * 初始化例子：
 * uint8_t addr = 0x01;
 * 1.初始化qjy_uart_init
 *      recv_callback_func func = {qjy_uart_parser, hd_uart_recv};
 *      qjy_uart_init(&func, addr);
 * 2.初始化hd_uart_init
 *      hd_uart_init("/userdata/jpeg",addr,on_action_id_changed);
 *
 * @param pic_dir_path  图片保存的地址
 * @param addr          从机地址。01:静态摄像头；02：动态摄像头。
 * @param callback      action_id回调
 * @return 0：成功；1：失败。
 */
int hd_uart_init(uint8_t addr, const char *pic_dir_path, hd_on_action_id_changed callback);

/**
 * 解析函数
 * @param byte
 */
void hd_uart_recv(uint8_t byte);

/**
 * 反初始化
 */
void hd_uart_deinit();

#ifdef __cplusplus
}
#endif

#endif // __HD_UART_PARSER__