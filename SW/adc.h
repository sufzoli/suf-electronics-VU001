#define CH_L	0
#define CH_R	1
#define NUM_SAMPLE_POWER	12 // 4096 samples

void adc_init();
void ADC_ConvReady_CallBack(uint32_t dummydata);
void adc_addvalue(unsigned char ch, uint32_t value);


