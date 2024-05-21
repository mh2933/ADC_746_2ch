#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <cstdio>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

#include "stm32f7xx_hal.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

using namespace std;

#ifndef SIMULATOR
#include "main.h"
//#include "ff.h"
#include "fatfs.h"
#include "string.h"
#include "stm32f7xx_hal.h"
#include <cmsis_os2.h>





extern "C"
{
    /* milliseconds */
    uint32_t HAL_GetTick(void);
    void StartDefaultTask(void * argument);

    extern ADC_HandleTypeDef hadc3;
    extern RTC_HandleTypeDef hrtc;
    extern RTC_TimeTypeDef RTC_Time;
    extern RTC_DateTypeDef RTC_Date;
    extern RTC_TimeTypeDef sTime;

    extern osMessageQueueId_t adcCurrentQueueHandle;
    extern osMessageQueueId_t adcVoltageQueueHandle;
    extern osMessageQueueId_t sdQueueHandle;

}
#endif

Model::Model() : modelListener(0), Voltage (10.5), Current (6.5), mAh (8800), tickCounter (100), milli_seconds (0), seconds (0.0), tickCounterNow (100), count_milli_seconds(0), previous_second(60)
{

}

float Model::mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
	  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
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

			// Current = adcReadCurrent();

			// Calculate mAh consumed per second
			float mAhConsumedPerSecond = (Current * seconds) / 3.6;

			// Subtract the mAh consumed in the last second from the total mAh
			mAh += mAhConsumedPerSecond; // divide by 10 to make the LCD and printf function to match

			seconds = 0.0;
			milli_seconds = 0.0;

			// Ensure mAh does not go below 0 and above 10000
			if (mAh < 0) mAh = 0;
			if (mAh > 8800) mAh = 8800;

			if ((Current > -0.15) & (Current < 0.15))
			{
				Current = 0;
			}
	        previous_second = real_second;
	    }

    return mAh;
}

 //Function to format the mAh value as a string and snprintf saves it to the buffer
void formatMahValue(float mahValue, char* buffer, size_t bufferSize) {
    // Clear the buffer
    memset(buffer, 0, bufferSize);

    printf("inside formatMahValue: %.2f\n", mahValue);

	snprintf(buffer, bufferSize, "%.2f mAh", mahValue);
    return;
}

void Model::writeToSDCard(char* data) {

	printf("inside writeToSDCard function\n");
	printf("Data to write: %s\n", data);

	    // Format the mAh value as a string
	    //char mAhString[20]; // Buffer to hold the formatted mAh value
	    //formatMahValue(mAh, mAhString, sizeof(mAhString));
	    //char* localData = this->mAhString;


    SD_Operation_t operation;

    printf("sizeof operation.data %u\n ",sizeof(operation.data));
    printf("sizeof data %u\n ",sizeof(data));

    // Calculate the size of the buffer
    size_t bufferSize = sizeof(operation.data);
    printf("buffersize %u\n", bufferSize);

    // Calculate the target size for the truncated data
    // You can adjust this value to leave more headroom in the buffer
    size_t targetSize = bufferSize / 4; // For example, truncate to half the buffer size

    printf("targetSize %u\n", targetSize);

    // Get the length of the input data
    //size_t dataLength = strlen(data);
    //printf("Length of data: %u\n", dataLength);

    // If the input data is larger than the target size, truncate it
    /*if (dataLength >= targetSize) {
        // Truncate the data to fit within the target size
        dataLength = targetSize - 1; // Leave space for null terminator
        printf("trunkating");
    }*/
    operation.data = data;
    //Copy the truncated data into the buffer
    //strlcpy(operation.data, data, dataLength + 1); // Include space for null terminator
    //operation.data[dataLength] = '\0'; // Null-terminate the string

    operation.command = SD_Operation_t::SD_WRITE;
    //strncpy(operation.data, data, dataLength); // Ensure null-termination
    printf("inside writeToSDCard function after strncpy \n\n");

    operation.data[sizeof(operation.data) - 1] = '\0'; // Force null-termination
    printf("inside writeToSDCard function after operation.data \n\n");

    osMessageQueuePut(sdQueueHandle, &operation, 0, osWaitForever);
    printf("inside writeToSDCard function after osMessageQueuePut \n\n");
    printf("Data: %s \n", operation.data);
    return;
}

void Model::readFromSDCard() {
	SD_Operation_t operation;
    operation.command = SD_Operation_t::SD_READ;
    // Assuming operation.data is already prepared for reading
    osMessageQueuePut(sdQueueHandle, &operation, 0, 0);
}

void Model::tick()
{

#ifndef SIMULATOR
	if (osMessageQueueGetCount(adcCurrentQueueHandle)>0)  // if there is unread messgae in the queue
	{
		if (osMessageQueueGet(adcCurrentQueueHandle, &ADC_Value, 0, 0) == osOK) // if the message is read successfully
		{
		    float adc_sum_2 = 0.0;
		    float adc_count = 0;
		    float adc2_average = 0.0;

			for (uint32_t i = 0; i < 1000; i++)
		    {
		        float ADC_Value_float = static_cast<float>(ADC_Value);
				adc_sum_2 += ADC_Value_float;
		    	adc_count++;
		    	//printf("adc_sum_2 %f\n", adc_sum_2);
			}

			adc2_average = adc_sum_2 / adc_count;

			float v_per_ampere = 0.0239;
			float Vcc = 3.320;
			//printf("adc2_average: %f\n\n", adc2_average);
			float midpoint_val = 1.641;

			float calculated_volt = adc2_average * (Vcc / 4095.0);
			//printf("calculated_volt: %.5f\n\n", calculated_volt);
			float calculated_voltage_to_current = (calculated_volt - midpoint_val) / v_per_ampere;

		    Current = mapFloat(calculated_voltage_to_current, -100.0, 100.0, -100.0, 100.0);

		    if (Current > -0.5 and Current < 0.5) Current = 0;

		    //modelListener->setADC2current(Current);

		    uint32_t milliTick = HAL_GetTick();

            if (milliTick - milliSec >= 500)
            {
			modelListener->setADC2current(Current);
			printf("inside model.cpp currentqueue %d\n", ADC_Value);
			milliSec = milliTick;
			printf("milliTick %ld", milliTick);

//				if (milliSec >= 59)
//				{
//					second = 0;
//				}
            }
		}
	}

	if (osMessageQueueGetCount(adcVoltageQueueHandle)>0)  // if there is unread messgae in the queue
	{
		if (osMessageQueueGet(adcVoltageQueueHandle, &ADC_Value, 0, 0) == osOK) // if the message is read successfully
		{
			float adc_sum_1 = 0.0;
			float adc_count = 0.0;
			float adc_average = 0.0;

			for (uint16_t i = 0; i < 1000; i++)
		    {
		        float ADC_Value_float = static_cast<float>(ADC_Value);
				adc_sum_1 += ADC_Value_float;
		    	adc_count++;
			}

			adc_average = adc_sum_1 / adc_count;

            Voltage = mapFloat(adc_average, 0, 4095, 0.0, 45.0);

			modelListener->setADC1voltage(Voltage);

			//printf("inside model.cpp Voltage queue %d\n", ADC_Value);
		}
	}

//    uint32_t currentMilliseconds = HAL_GetTick();
//    printf("currentMilleSeconds %ld\n", currentMilliseconds);
//    cout << "Hello" << endl;

    if (Voltage < 0.2) Voltage = 0;

    mAh = calcMilliAh();



//    if (real_second - second >= 10)
//    {
//    	printf("last_second %d\n", last_second);
//    	printf("second %d\n", second);
//    	second = real_second;
//    		if (second >= 50)
//    		{
//    			second = 0;
//    		}
//		// Format the mAh value as a string
//		 // Buffer to hold the formatted mAh value
//		formatMahValue(mAh, mAhString, sizeof(mAhString));
//
//		printf("before writeToSDCard call\n");
//		writeToSDCard(mAhString);
//
////		void result = writeToSDCard(mAhString);
////		if (result == 0) {
////			// Log error or handle failure
////			printf("error result < 0\n");
////		}
//    	printf("real_second %d\n", real_second);
//        printf("second %d\n", second);
//        printf("last_second %d\n", last_second);
//
//
//    }


    // tickCounter is related to percentage bargraph on the UI
    tickCounter = mapFloat(mAh, 8800, 0, 100, 0);




#endif

    modelListener->setAh(mAh);
    modelListener->tickCounterUpdated(tickCounter);


}
