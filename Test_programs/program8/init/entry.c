// 添加三个不同的应用程序
// 

typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;
void kern_entry()
{
	uint8_t *input = (uint8_t *)0xB8000;
	uint8_t color = (0 << 4) | (15 & 0x09);
	uint32_t i;	
	while(i < 80*2)
	{
		*input++ = ' '; *input++ = color;
		i++;
	}
	*input++ = 'H'; *input++ = color;
	*input++ = 'e'; *input++ = color;
	*input++ = 'l'; *input++ = color;
	*input++ = 'l'; *input++ = color;
	*input++ = 'o'; *input++ = color;
	*input++ = ','; *input++ = color;
	*input++ = ' '; *input++ = color;

	

	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = '!'; *input++ = color;
	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = '!'; *input++ = color;
	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = 'P'; *input++ = color;
	*input++ = '8'; *input++ = color;
	*input++ = '!'; *input++ = color;
	//asm volatile("hlt");
	asm volatile("sti");
	//asm volatile("iret");//
}

