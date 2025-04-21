#ifndef _ft2_sample_saver_h_
#define _ft2_sample_saver_h_

#include <stdint.h>
#include <stdbool.h>
#include "ft2_unicode.h"

enum
{
	SAVE_NORMAL = 0,
	SAVE_RANGE = 1
};

void saveSample(UNICHAR *filenameU, bool saveAsRange);

#endif
