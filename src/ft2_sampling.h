#ifndef _ft2_sampling_h_
#define _ft2_sampling_h_

enum
{
	INPUT_FREQ_44KHZ = 2,
	INPUT_FREQ_48KHZ = 4,
	INPUT_FREQ_96KHZ = 8
};

void startSampling(void);
void stopSampling(void);
void handleSamplingUpdates(void);

#endif
