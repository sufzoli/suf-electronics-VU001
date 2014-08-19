#include <DrvSys.h>
#include "M051Series.h"

#include "Bargraph.h"
#include "adc.h"
int main(void)
{
	UNLOCKREG(x);
	DrvSYS_SetOscCtrl(E_SYS_XTL12M, 1);
	while (DrvSYS_GetChipClockSourceStatus(E_SYS_XTL12M) != 1);

	DrvSYS_SelectSysTickSource(0); // Set SysTick source to 12MHz XTAL
	DrvSYS_Open(50000000);

	// DrvSYS_SelectHCLKSource(0);
	// DrvSYS_SetClockDivider(E_SYS_HCLK_DIV, 0);

	SysTick_Config(10000);

	Bargraph_Init();
	adc_init();

    while(1)
    {
    }
}

void SysTick_Handler(void)
{
	Bargraph_Display();
}
