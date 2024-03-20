#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <cstdio>
#include <chrono>
#include <cstdint>


#ifndef SIMULATOR
#include "main.h"


extern "C"
{

    extern ADC_HandleTypeDef hadc3;

    int _write(int file, char *ptr, int len);

    float map(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
    }
}
#endif

Model::Model() : modelListener(0), Voltage (10.5), Current (6.5), mAh (10000), tickCounter (100), milli_seconds (0), seconds (0.0), tickCounterNow (100), count_milli_seconds(0)
{

}


void Model::tick()
{
#ifndef SIMULATOR

    //seconds = 0;

    ADC_ChannelConfTypeDef sConfig = {0};
    float adc_sum_1 = 0;
    float adc_sum_2 = 0;
    float adc_count = 0;

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
        HAL_ADC_PollForConversion(&hadc3, 5);
        float value_1 = HAL_ADC_GetValue(&hadc3);
        HAL_ADC_Stop(&hadc3);
        adc_sum_1 += value_1;
        adc_count++;

//        printf("printing adc_sum_1: %.2f\n", adc_sum_1);
//        printf("printing value_1:   %.2f\n", value_1);
//        printf("printing adc_count: %.2f\n\n", adc_count);
//        HAL_Delay(1000);

        //printf("value_1: %.2d\n", value_1);

        // Configure for channel 8
        sConfig.Channel = ADC_CHANNEL_8;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }

        HAL_ADC_Start(&hadc3);
        HAL_ADC_PollForConversion(&hadc3, 5);
        uint16_t value_2 = HAL_ADC_GetValue(&hadc3);
        HAL_ADC_Stop(&hadc3);
        adc_sum_2 += value_2;

    }


	HAL_Delay(10);
	milli_seconds++; // Increment seconds
	//count_milli_seconds += milli_seconds;

    printf("before if statement milli_seconds: %ld\n ", milli_seconds);

    if (milli_seconds >= 1)  // 1 = 10 milliseconds
    {

    	printf("start of if statement milli_seconds: %ld\n ", milli_seconds);
    	//counting_milliseconds = 0;
    	printf("start of if statement\n");
    	seconds = 0.1;

    	printf("seconds: %f\n", seconds);
    	//printf("counting_milliseconds %ld\n", counting_milliseconds);

        if (adc_count > 0)
        {
//        	float r1 = 18;
//        	float r2 = 1.2;
            float voltage_divider = 16.0; // 1/(r2/(r1+r2))
            float adc1_average = adc_sum_1 / adc_count;
            printf("voltage_divider: %.2f\n", voltage_divider);

            float v_per_ampere = 0.025;
            float Vcc = 3.3;
            float adc2_average = adc_sum_2 / adc_count;
            float midpoint_val = 1.6639;

            float calculated_volt = adc2_average * (Vcc / 4095.0);
            printf("calculated_volt: %.5f\n\n", calculated_volt);
            float calculated_voltage_to_current = (calculated_volt - midpoint_val) / v_per_ampere;
            printf("calculated_voltage_to_current: %.5f\n\n", calculated_voltage_to_current);

            Voltage = map(adc1_average, 0, 4095, 0.0, 50.0);
            Current = map(calculated_voltage_to_current, -50.0, 50.0, -50.0, 50.0);
            printf("printing Voltage after mapfunction2: %.2f\n", Voltage);


            // Calculate mAh consumed per second
            float mAhConsumedPerSecond = (Current * seconds) / 3.6;
            printf("mAhConsumedPerSecond: %f\n", mAhConsumedPerSecond);

            // Subtract the mAh consumed in the last second from the total mAh
            mAh += mAhConsumedPerSecond; // divide by 10 to make the LCD and printf function to match
            printf("Current: %.2f\n", Current);

            printf("mAh: %.3f\n", mAh);

            printf("adc_count: %.3f\n", adc_count);

            seconds = 0;
            milli_seconds = 0;

            // Ensure mAh does not go below 0 and abow 10000
            if (mAh < 0) mAh = 0;
            if (mAh > 10000) mAh = 10000;

            adc_count = 0;
        }
    }
    printf("adc_count: %.3f\n", adc_count);

    tickCounter = map(mAh, 10005, 0, 100, 0);

    printf("after if statement\n");
    printf("seconds: %.2f \n", seconds);
    printf("size of Voltage: %zu\n", sizeof(Voltage));
    printf("Voltage: %.2f\n", Voltage);


#endif

    modelListener->setADC1voltage(Voltage);
    modelListener->setADC2current(Current);
    modelListener->setAh(mAh);
    modelListener->tickCounterUpdated(tickCounter);
}
