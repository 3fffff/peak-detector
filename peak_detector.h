#ifndef __PEAK_DETECTOR_H__
#define __PEAK_DETECTOR_H__

// suppress types error
#include <stdint.h>

#define PD_DEFAULT_LAG 100
#define PD_DEFAULT_THRESHOLD 15.5F
#define PD_DEFAULT_INFLUENCE 0.05F

struct peak_detector
{
	float prevData;
	float prevAvg;
	float prevStddev;
	float Var;

	uint32_t lag, idx;
	float threshold;
	float influence;
	float influenceAvg;
	int8_t peak;
	int8_t prev_peak;
	uint8_t plus;
	uint8_t minus;
	float avgSum;
	float sum;
	float buffer[PD_DEFAULT_LAG];
	/*
	peak positive = 1
	no peak = 0
	peak negative = -1
	*/
};

void peak_det_init(struct peak_detector *pd,
                   // uint32_t lag,
                   float threshold_eps_mul,
                   float influence, float influenceAvg, uint8_t use_plus, uint8_t use_minus);
void peak_det_push(struct peak_detector *pd, float next_sample);
int8_t peak_det_get_peak(struct peak_detector *pd);

#endif /*__PEAK_DETECTOR_H__*/
