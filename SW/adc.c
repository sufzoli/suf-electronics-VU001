// #include <DrvSYS.h>
#include <DrvADC.h>
#include <DrvGPIO.h>
#include "adc.h"
#include "Bargraph.h"

// uint32_t adc_agnd[2] = {2048,2048};	// Calculated virtual analog ground ADC value
// uint32_t adc_agnd_arr[2][16];	// The previous 16 agnd value (circular buffer!!!), fill at startup with the adc mean value
// unsigned char adc_agnd_arr_pos[2] = {0,0};
unsigned char adc_result[2] = {0,0};	// The final displayable result
const uint32_t adc_sqrt_levels[50] = { // The calculated levels of the display LEDs
     11,      18,      28,      45,      71,     112,     177,     281,     445,    706,
   1118,    1773,    2809,    4453,    7057,   11185,   17727,   22317,   28095,  35369,
  44528,   56057,   70571,   88844,  111848,  140808,  177267,  223166,  280950, 353695,
 396852,  445275,  499607,  560568,  628968,  705714,  791824,  888441,  996847, 1118481,
1254956, 1408084, 1579896, 1772673, 1988972, 2231663, 2503967, 2809497, 3152308, 3536948};

uint64_t adc_aggregate[2] = {0,0}; // Aggregate value of the adc
// uint32_t adc_agnd_aggregate[2] = {0,0}; // Aggregate value of the agnd
uint32_t adc_sample_pos[2] = {0,0}; // current position of the sample taking

/*
 *  The calculation is based on the dBm scale and contains square values
 *  Square values is used because the signal averaging procedure is RMS.
 *  The RMS calculation contains a root at the end. If we put square values
 *  in the comparison table, this root calculation can be eliminated:
 *  Xrms^2 = (X1^2 + X2^2 + X3^2 + X4^2 + .... + Xn^2)/n
 *  The n must be a power of 2. This reduces the division to a simple right shift operation
 */

/*
 * Ground calculation:
 * At the and of the sampling cycle (the number of samples must be calculated - ~0.1s sample ??? power of 2!!!)
 * create an average of the converted signal values.
 * Put this average in the adc_agnd_arr buffer
 * Calculate the average of the whole buffer
 * put the result into the adc_agnd - use this at the next sampling cycle
 */


void adc_init()
{

	DrvGPIO_InitFunction(E_FUNC_ADC0);

	/* Select ADC clock source from 12M */
	// DrvSYS_SelectIPClockSource (E_SYS_ADC_CLKSRC, 0x00);

	DrvADC_Open(ADC_SINGLE_END, ADC_CONTINUOUS_OP, 0x03, EXTERNAL_12MHZ, 5);
	// The ADC conversion rate:
	// Base freq    conversion cycle    num channels  divisor
	// 12 000 000 /        27        /       2      /    5     =  44.444 KHz
	// DrvADC_SetADCChannel(1);
	DrvADC_EnableADCInt(ADC_ConvReady_CallBack, 0);
	DrvADC_StartConvert();
}

void ADC_ConvReady_CallBack(uint32_t dummydata)
{
	adc_addvalue(CH_L,DrvADC_GetConversionData(0));
	adc_addvalue(CH_R,DrvADC_GetConversionData(1));
	// Bargraph_Set(0,DrvADC_GetConversionData(0) / 80);
	Bargraph_Set(adc_result[CH_L],adc_result[CH_R]);
}

/*
 * Search the level in the array (the array is sorted)
 * using b-tree algorithm
 */
unsigned char level_search(uint32_t level)
{
	unsigned char lbound = 0;
	unsigned char ubound = 49;
	unsigned char testpos;
	while(ubound-lbound > 1)
	{
		testpos = lbound + ((ubound - lbound) >> 1);
		if(adc_sqrt_levels[testpos] == level) // we accidentally found the correct value, don't iterate further
		{
			return testpos;
		}
		if(adc_sqrt_levels[testpos] > level)
		{
			ubound = testpos;
		}
		else
		{
			lbound = testpos;
		}
	}
	return lbound;
}

void adc_addvalue(unsigned char ch, uint32_t value)
{
	int32_t tmp_value;
//	unsigned char i;
//	tmp_value = value - adc_agnd[ch]; // subtract virtual ground
	tmp_value = value;
	tmp_value *= tmp_value; // power
	adc_aggregate[ch] += tmp_value; // add to the aggregate
//	adc_agnd_aggregate[ch] += value;
	adc_sample_pos[ch]++;
	if(adc_sample_pos[ch] == (1 << NUM_SAMPLE_POWER))
	{
		// calculate the gnd avg and add gnd avg to the circular buffer
//		adc_agnd_arr[ch][adc_agnd_arr_pos[ch]] = adc_agnd_aggregate[ch] >> NUM_SAMPLE_POWER;
		// advance the circular buffer position or reset if it is at the end
//		adc_agnd_arr_pos[ch] = (adc_agnd_arr_pos[ch] == 15) ? 0 : adc_agnd_arr_pos[ch] + 1;
		// calculate the buffer avg
/*
		tmp_value = 0;
		for(i=0;i<16;i++)
		{
			tmp_value += adc_agnd_arr[ch][i];
		}
		// add the buffer avg to the adc_agnd
		adc_agnd[ch] = tmp_value >> 4;
*/
		// calculate the value avg and find the level in the adc_sqrt_levels (btree)
		adc_result[ch] = level_search(adc_aggregate[ch] >> NUM_SAMPLE_POWER);
		// reset the adc_sample_pos
		adc_sample_pos[ch] = 0;
//		adc_agnd_aggregate[ch] = 0;
		adc_aggregate[ch] = 0;
	}
}




