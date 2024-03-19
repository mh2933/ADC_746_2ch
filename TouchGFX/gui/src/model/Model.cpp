#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <cstdio>
#include <chrono>


#ifndef SIMULATOR
#include "main.h"


extern "C"
{

    extern ADC_HandleTypeDef hadc3;

    int _write(int file, char *ptr, int len);

    long map(long x, long in_min, long in_max, long out_min, long out_max)
    {
      return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
    }
}
#endif

Model::Model() : modelListener(0), Voltage (10.5), Current (6.5), mAh (10000), tickCounter (100), milli_seconds (0), seconds (0.0), tickCounterNow (100), count_milli_seconds(0)
{
	//mAh = tickCounter * 100; // Initialize Ah here after tickCounter is set
	//startTime = std::chrono::system_clock::now();

}


void Model::tick()
{
#ifndef SIMULATOR

	//milli_seconds = 0;
    seconds = 0;




//    if (tickCounter > 0)
//    {
//        tickCounter--;
////        HAL_Delay(100);  // Delay for 100 ms
////        milli_seconds++; // Increment seconds
//    }
//    else
//    {
//        tickCounter = 100;
//    }


    ADC_ChannelConfTypeDef sConfig = {0};
    uint32_t adc_sum_1 = 0;
    uint32_t adc_sum_2 = 0;
    uint16_t adc_count = 0;

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
        uint16_t value_1 = HAL_ADC_GetValue(&hadc3);
        HAL_ADC_Stop(&hadc3);
        adc_sum_1 += value_1;
        adc_count++;

        //Voltage = map(value_1, 0, 4095, 0, 100);

        // Configure for channel 8
        sConfig.Channel = ADC_CHANNEL_8;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }

        HAL_ADC_Start(&hadc3);
        HAL_ADC_PollForConversion(&hadc3, 10);
        uint16_t value_2 = HAL_ADC_GetValue(&hadc3);
        HAL_ADC_Stop(&hadc3);
        adc_sum_2 += value_2;

        //Current = map(value_2, 0, 4095, 0, 100);
    }

//    if (HAL_Delay(100))
//    {
//         milli_seconds++;
//    }

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
            Voltage = map(static_cast<float>(adc_sum_1) / adc_count, 0, 4095, 0.0, 100.0);
            Current = map(static_cast<float>(adc_sum_2) / adc_count, 0, 2046, -20.0, 5.0);

            // Calculate mAh consumed per second
            float mAhConsumedPerSecond = (Current * seconds) / 3.6;
            printf("mAhConsumedPerSecond: %f\n", mAhConsumedPerSecond);

            // Subtract the mAh consumed in the last second from the total mAh
            mAh += mAhConsumedPerSecond; // divide by 10 to make the LCD and printf function to match
            printf("Current: %.2f\n", Current);

            printf("mAh: %.3f\n", mAh);


            seconds = 0;
            milli_seconds = 0;

            // Ensure mAh does not go below 0
            if (mAh < 0)
            {
                mAh = 0;
            }
        }
    }


    tickCounter = map(mAh, 10100, 0, 100, 0);

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
