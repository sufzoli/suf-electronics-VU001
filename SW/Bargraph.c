#include <DrvGPIO.h>

unsigned char bg_l;
unsigned char bg_r;
unsigned char bg_disp_pos;


void Bargraph_Init()
{
	uint32_t i;
	for(i=0;i<8;i++)
	{
		DrvGPIO_Open(0,i,E_IO_OUTPUT);
		DrvGPIO_Open(2,i,E_IO_OUTPUT);
		DrvGPIO_Open(3,i,E_IO_OUTPUT);
	}
	DrvGPIO_SetPortBits(0,0);
	DrvGPIO_SetPortBits(2,0);
	DrvGPIO_SetPortBits(3,0);
	bg_l = 0;
	bg_r = 0;
	bg_disp_pos = 0;
}

void Bargraph_Set(unsigned char L, unsigned char R)
{
	bg_l = L;
	bg_r = R;
}

void Bargraph_Display()
{
	int32_t disp;
	unsigned char lr_sel, digit;

	lr_sel = bg_disp_pos & 1;
	digit = bg_disp_pos >> 1;

	DrvGPIO_SetPortBits(0,0);
	DrvGPIO_SetPortBits(2,0);
	DrvGPIO_SetPortBits(3,0);

	if(lr_sel)
	{
		disp = bg_l - (digit << 3);
		if(disp < 0)
		{
			disp = 0;
		}
		DrvGPIO_SetPortBits(2, (disp > 7) ? 0xFF : ((1 << disp) - 1));
		DrvGPIO_SetPortBits(0, 1 << digit);
	}
	else
	{
		disp = bg_r - (digit << 3);
		if(disp < 0)
		{
			disp = 0;
		}
		DrvGPIO_SetPortBits(3, (disp > 7) ? 0xFF : ((1 << disp) - 1));
		DrvGPIO_SetPortBits(0, 1 << digit);
	}

   	if(bg_disp_pos < 13)
   	{
    	bg_disp_pos++;
	}
	else
	{
		bg_disp_pos = 0;
	}
}
