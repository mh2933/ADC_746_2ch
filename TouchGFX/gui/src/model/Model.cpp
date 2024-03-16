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

Model::Model() : modelListener(0), Voltage (300), Current (60), mAh (10000), tickCounter (100), seconds (100)
{
	//mAh = tickCounter * 100; // Initialize Ah here after tickCounter is set
	startTime = std::chrono::high_resolution_clock::now();
}

void Model::someFunction() {
    int someVariable = 123;
    printf("Debug: someVariable = %d\n", someVariable);
}

void Model::tick()
{
#ifndef SIMULATOR
//	static int elapsedSeconds = 0;
//
//    elapsedSeconds++;

	someFunction();

    // Calculate the elapsed time since startTime
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);

    // Update seconds based on elapsed time
    seconds = elapsed.count()/1000;

	printf("Current: ");

    if (tickCounter > 0)
    {
        tickCounter--;
        HAL_Delay(1000); // Delay for 1 second
        seconds++; // Increment seconds
    }
    else
    {
        tickCounter = 100;
    }


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

    if (adc_count > 0)
    {
        Voltage = map(static_cast<float>(adc_sum_1) / adc_count, 0, 4095, 0, 100);
        Current = map(static_cast<float>(adc_sum_2) / adc_count, 0, 2046, -20, 5);

        // Calculate mAh consumed per second
        float mAhConsumedPerSecond = (Current * seconds) / 3600;

        // Subtract the mAh consumed in the last second from the total mAh
        mAh += mAhConsumedPerSecond;
        printf("Current: %d\n", Current);
        printf("seconds: %d\n", seconds);
        printf("mAh: %f\n", mAh);

        // Ensure mAh does not go below 0
        if (mAh < 0)
        {
            mAh = 0;
        }
    }
//    mAh = tickCounter * 100;

#endif

    modelListener->setADC1(Voltage);
    modelListener->setADC2(Current);
    modelListener->setAh(mAh);
    modelListener->tickCounterUpdated(tickCounter);
}
