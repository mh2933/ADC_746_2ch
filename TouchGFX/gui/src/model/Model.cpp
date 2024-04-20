#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <cstdio>
#include <chrono>
#include <cstdint>
#include "stm32f7xx_hal.h"


#ifndef SIMULATOR
#include "main.h"


extern "C"
{

    extern ADC_HandleTypeDef hadc3;
    extern RTC_HandleTypeDef hrtc;
    extern RTC_TimeTypeDef RTC_Time;
    extern RTC_DateTypeDef RTC_Date;
    extern RTC_TimeTypeDef sTime;


//    int _write(int file, char *ptr, int len);

    float map(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
    }
}
#endif

Model::Model() : modelListener(0), Voltage (10.5), Current (6.5), mAh (10000), tickCounter (100), milli_seconds (0), seconds (0.0), tickCounterNow (100), count_milli_seconds(0), previous_second(60)
{

}

float Model::adcReadVoltage()
{
	// VOLTAGE CALCULATION
	//  float r1 = 18;
	//  float r2 = 1.2;
    //  float voltage_divider = 17.26; // 1/(r2/(r1+r2))

	ADC_ChannelConfTypeDef sConfig = {0};
	float adc_sum_1 = 0.0;
	float adc_count = 0.0;
	float adc_average = 0.0;

    for (uint16_t i = 0; i < 1000; i++)
    {
        // Configure for channel 0
        sConfig.Channel = ADC_CHANNEL_0;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_112CYCLES;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }

        HAL_ADC_Start(&hadc3);
        HAL_ADC_PollForConversion(&hadc3, 10);
        float value_1 = HAL_ADC_GetValue(&hadc3);
        HAL_ADC_Stop(&hadc3);
        adc_sum_1 += value_1;
        adc_count++;
    }

    adc_average = adc_sum_1 / adc_count;

    Voltage = map(adc_average, 0, 4095, 0.0, 55.59);

    return Voltage;
}

float Model::adcReadCurrent()
{
	ADC_ChannelConfTypeDef sConfig = {0};
    float adc_sum_2 = 0.0;
    float adc_count = 0;
    float adc2_average = 0.0;

    for (uint16_t i = 0; i < 1000; i++)
    {
    	sConfig.Channel = ADC_CHANNEL_8;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_112CYCLES;
    	if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    	{
    	    Error_Handler();
    	}

        HAL_ADC_Start(&hadc3);
    	HAL_ADC_PollForConversion(&hadc3, 10);
    	float value_2 = HAL_ADC_GetValue(&hadc3);
    	HAL_ADC_Stop(&hadc3);
    	adc_sum_2 += value_2;
    	adc_count++;
    }

    printf("adc_count %.2f\n", adc_count);

    adc2_average = adc_sum_2 / adc_count;

	float v_per_ampere = 0.0239;
	float Vcc = 3.320;
	printf("adc2_average: %f\n\n", adc2_average);
	float midpoint_val = 1.5425;

	float calculated_volt = adc2_average * (Vcc / 4095.0);
	printf("calculated_volt: %.5f\n\n", calculated_volt);
	float calculated_voltage_to_current = (calculated_volt - midpoint_val) / v_per_ampere;

    Current = map(calculated_voltage_to_current, -100.0, 100.0, -100.0, 100.0);

    return Current;
}

uint8_t Model::rtcSeconds()
{
	HAL_RTC_GetTime(&hrtc, &RTC_Time, FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &RTC_Date, FORMAT_BIN);

    real_second = RTC_Time.Seconds;

    return real_second;
}

float Model::calcMilliAh()
{
	real_second = rtcSeconds();

	if (real_second != previous_second)
	    {

	    	seconds = 1.0;

			Current = adcReadCurrent();

			// Calculate mAh consumed per second
			float mAhConsumedPerSecond = (Current * seconds) / 3.6;

			// Subtract the mAh consumed in the last second from the total mAh
			mAh += mAhConsumedPerSecond; // divide by 10 to make the LCD and printf function to match

			seconds = 0.0;
			milli_seconds = 0.0;

			// Ensure mAh does not go below 0 and above 10000
			if (mAh < 0) mAh = 0;
			if (mAh > 10000) mAh = 10000;

			if ((Current > -0.15) & (Current < 0.15))
			{
				Current = 0;
			}
	        previous_second = real_second;
	    }

    return mAh;
}

void Model::tick()
{

#ifndef SIMULATOR

    Voltage = adcReadVoltage();
    if (Voltage < 0.2) Voltage = 0;

    mAh = calcMilliAh();

    // tickCounter is related to percentage bargraph on the UI
    tickCounter = map(mAh, 10005, 0, 100, 0);

    printf("real_second %d\n", real_second);
    printf("mAh %f\n", mAh);

#endif

    modelListener->setADC1voltage(Voltage);
    modelListener->setADC2current(Current);
    modelListener->setAh(mAh);
    modelListener->tickCounterUpdated(tickCounter);
    //modelListener->updateTime(RTC_Time.Hours, RTC_Time.Minutes, RTC_Time.Seconds);

}
