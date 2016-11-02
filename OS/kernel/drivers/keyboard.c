#include "keyboard.h"
#include "common.h"
#include "idt.h"
#include "debug.h"
#include "gdt.h"

extern int32_t cursor_x;


int	key_input = 0;
int	times	 = 0;
uint8_t	scan_code_first;
uint8_t scan_code_second;
//中断注册
static void keyboard_callback(pt_regs *regs)
{

    if (times % 2 == 0)
    {
        scan_code_first = inb(0x60);
	//printk("%d", scan_code_first);
    }
    else
    {
        scan_code_second = inb(0x60);
	//monitor_write_dec(scan_code_first,rc_black, rc_blue);
    }
    //uint8_t scan_code = inb(0x60);//读出扫描码，一次按键产生两个：Make Code, Break codefish 
                                  //对应产生两次中断
    //打印扫描码
    times++;
    
    if (times %2 == 0)
    {
        key_input = 1;
    }	
}

void init_keyboard()
{
	register_interrupt_handler(IRQ1, keyboard_callback);
}


// 从键盘获取一个输入字符
char get_char_from_keyboard()
{
    while (1)
    {
        if (key_input == 1)
        {
            key_input = 0;
	    //uint8_t scan_code_rel;
	    //scan_code_rel = (scan_code_first & 0xff) ;
	    //scan_code_first = scan_code_rel ;
	    //monitor_write_dec(scan_code_rel,rc_black, rc_green);
            // 解析扫描码，返回输入的字符
            // 'a'-'z'; '0'-'9'; '.'
	
            if (scan_code_first == 0x39)
            {
                return ' ';
            }
            else if (scan_code_first == 0x0E)
            {
                return 0x8;
            }
            else if (scan_code_first == 0x1C)
            {
                return '\n';
            }
            else if (scan_code_first == 0x1E)
            {
		return 'a';
            }
            else if (scan_code_first == 0x30)
            {
                return 'b';
            }
            else if (scan_code_first == 0x2E)
            {
                return 'c';
            }
            else if (scan_code_first == 0x20)
            {
                return 'd';
            }
            else if (scan_code_first == 0x12)
            {
                return 'e';
            }
            else if (scan_code_first == 0x21)
            {
                return 'f';
            }
            else if (scan_code_first == 0x22)
            {
                return 'g';
            }
            else if (scan_code_first == 0x23)
            {
                return 'h';
            }
            else if (scan_code_first == 0x17)
            {
                return 'i';
            }
            else if (scan_code_first == 0x24)
            {
                return 'j';
            }
            else if (scan_code_first == 0x25)
            {
                return 'k';
            }
            else if (scan_code_first == 0x26)
            {
                return 'l';
            }
            else if (scan_code_first == 0x32)
            {
                return 'm';
            }
            else if (scan_code_first == 0x31)
            {
                return 'n';
            }
            else if (scan_code_first == 0x18)
            {
                return 'o';
            }
            else if (scan_code_first == 0x19)
            {
                return 'p';
            }
            else if (scan_code_first == 16)
            {
                return 'q';
            }
            else if (scan_code_first == 0x13)
            {
                return 'r';
            }
            else if (scan_code_first == 31)
            {
                return 's';
            }
            else if (scan_code_first == 0x14)
            {
                return 't';
            }
            else if (scan_code_first == 0x16)
            {
                return 'u';
            }
            else if (scan_code_first == 0x2F)
            {
                return 'v';
            }
            else if (scan_code_first == 17)
            {
                return 'w';
            }
            else if (scan_code_first == 0x2D)
            {
                return 'x';
            }
            else if (scan_code_first == 0x15)
            {
                return 'y';
            }
            else if (scan_code_first == 0x2C)
            {
                return 'z';
            }
            else if (scan_code_first == 0x0B)
            {
                return '0';
            }
            else if (scan_code_first == 0x02)
            {
                return '1';
            }
            else if (scan_code_first == 0x03)
            {
                return '2';
            }
            else if (scan_code_first == 0x04)
            {
                return '3';
            }
            else if (scan_code_first == 0x05)
            {
                return '4';
            }
            else if (scan_code_first == 0x06)
            {
                return '5';
            }
            else if (scan_code_first == 0x07)
            {
                return '6';
            }
            else if (scan_code_first == 0x08)
            {
                return '7';
            }
            else if (scan_code_first == 0x09)
            {
                return '8';
            }
            else if (scan_code_first == 0x0A)
            {
                return '9';
            }
            else if (scan_code_first == 0x34)
            {
                return '.';
            }
            else
            {
                // to do
            }
        }
    }
}
