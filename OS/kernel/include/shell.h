#ifndef FISHOS_SHELL_H_
#define FISHOS_SHELL_H_

#include "types.h"

void parse_command();
// 添加'->' 标记
void addShellIndent();
// 执行shell输入
void executeInput(char *input, char *arguements);
// 关掉Indent 输入
void turnShellIndentOnOff(int onOrOff);
// 关掉Shell
void turnShellInputOnOff(int onOrOff);
// 支持过任务
uint8_t is_enable_multitask(char *arguements);
// 获取输入的字符
void getTypedText(int charCount, int startingYPos, int cursor_y, char *c);
// 获取shell中的输入,同时运行程序
void runShellFunction(uint32_t runFunction, char *arguements, uint32_t priority, uint32_t burst_time, uint8_t multitask);

// 程序列表
void program_hello(char *arguements);	// 0, 打印字符
void program_help(char *arguements);	// 1, 打印help列表
void program_song(char *arguements);	// 2, 打印声音
void program_now(char *arguements);	// 3, 当前时间
void program_echo(char *arguements);	// 4, 输出
void program_about(char *arguements);	// 5, 关于信息


#endif //FISHOS_SHELL_H_
