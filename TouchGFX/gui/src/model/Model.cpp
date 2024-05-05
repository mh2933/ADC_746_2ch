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

extern "C"
{
    uint32_t HAL_GetTick(void);
    void StartDefaultTask(void * argument);

    extern ADC_HandleTypeDef hadc3;
    extern RTC_HandleTypeDef hrtc;
    extern RTC_TimeTypeDef RTC_Time;
    extern RTC_DateTypeDef RTC_Date;
    extern RTC_TimeTypeDef sTime;

    xQueueHandle messageQ;

    // Function to start the default task with the mAh value


//    extern SDRAM_HandleTypeDef hsdram1;
//    extern SD_HandleTypeDef hsd1;
//    extern DMA_HandleTypeDef hdma_sdmmc1_rx;
//    extern DMA_HandleTypeDef hdma_sdmmc1_tx;

    //SD Card variables FATFS
    //uint8_t retSD;    /* Return value for SD */
//    extern char SDPath[4];   /* SD logical drive path */
//    extern FATFS SDFatFS;    /* File system object for SD logical drive */
//    extern FIL SDFile;       /* File object for SD */
//    extern float mAh;
//
//
//    //FILE I/O variables
//    extern FRESULT res;													/* FastFs function common result code */
//    extern uint32_t byteswritten, bytesread;							/* File write/read counts */
//    uint8_t wtext[] = "Hello from mathias :), SDIO from RTOS";  /* File write buffer */
//    extern uint8_t rtext[100];											/* File read buffer */

    /* USER CODE END Header_StartDefaultTask */
//    void StartDefaultTask(void * argument)
//    {
//      /* init code for FATFS */
//      MX_FATFS_Init();
//
//      /* USER CODE BEGIN 5 */
//    	//1. Mount - 0
//    	f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
//
//    	//TEST Write operation
//    	//2. Open file for Writing
//    	if(f_open(&SDFile, "F7FILE1.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
//    	{
//    		printf("Failed to open Write file\r\n");
//    	}
//    	else
//    	{
//    		printf("Opened Write file successfully\r\n");
//
//    		// Write mAh value to the file
////    		        char wtext[100];
////    		        snprintf(wtext, sizeof(wtext), "mAh: %.2f", mAh);
//
//    		//Write data to text file
//    		res = f_write(&SDFile, wtext, strlen((char *)wtext), (UINT*)&byteswritten);
//    		if((byteswritten == 0) || (res != FR_OK))
//    		{
//    			printf("Failed to write file!\r\n");
//    		}
//    		else
//    		{
//    			printf("File written successfully\r\n");
//    			printf("Write Content: %s\r\n", wtext);
//    		}
//    		f_close(&SDFile);
//    	}
//
//    	//Test read file
//    	f_open(&SDFile, "F7FILE1.TXT",  FA_READ);
//    		memset(rtext,0,sizeof(rtext));
//    		res = f_read(&SDFile, rtext, sizeof(rtext), (UINT*)&bytesread);
//    		if((bytesread == 0) || (res != FR_OK))
//    		{
//    			printf("Failed to read file!\r\n");
//    		}
//    		else
//    		{
//    			printf("File read successfully\r\n");
//    			printf("File content: %s\r\n", (char *)rtext);
//    		}
//    		f_close(&SDFile);
//
//
//      /* Infinite loop */
//      for(;;)
//      {
//        osDelay(1);
//      }
//      /* USER CODE END 5 */
//    }
//
//
    float map(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
    }
}
#endif

Model::Model() : modelListener(0), Voltage (10.5), Current (6.5), mAh (8800), tickCounter (100), milli_seconds (0), seconds (0.0), tickCounterNow (100), count_milli_seconds(0), previous_second(60)
{
	messageQ = xQueueGenericCreate(1, 1, 0);
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

    //printf("adc_count %.2f\n", adc_count);

    adc2_average = adc_sum_2 / adc_count;

	float v_per_ampere = 0.0239;
	float Vcc = 3.320;
	//printf("adc2_average: %f\n\n", adc2_average);
	float midpoint_val = 1.5425;

	float calculated_volt = adc2_average * (Vcc / 4095.0);
	//printf("calculated_volt: %.5f\n\n", calculated_volt);
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
			if (mAh > 8800) mAh = 8800;

			if ((Current > -0.15) & (Current < 0.15))
			{
				Current = 0;
			}
	        previous_second = real_second;
	    }

    return mAh;
}

 //Function to format the mAh value as a string
void formatMahValue(float mahValue, char* buffer, size_t bufferSize) {
    snprintf(buffer, bufferSize, "%.2f mAh", mahValue);
    return;
}

void Model::tick()
{

#ifndef SIMULATOR

    uint32_t currentMilliseconds = HAL_GetTick();
    printf("currentMilleSeconds %ld\n", currentMilliseconds);
    cout << "Hello" << endl;


//	if (xQueueReceive(messageQ, &counter, 0) == pdTRUE)
//	{
//
//	}
	float mahValue = 0;

    Voltage = adcReadVoltage();
    if (Voltage < 0.2) Voltage = 0;

    mAh = calcMilliAh();
    mahValue = mAh;
    char wtext[15];  // Buffer to hold the formatted mAh value
    formatMahValue(mahValue, wtext, sizeof(wtext));

    // Pass the address of wtext to StartDefaultTask
    StartDefaultTask(static_cast<void*>(wtext));


    // tickCounter is related to percentage bargraph on the UI
    tickCounter = map(mAh, 8805, 0, 100, 0);


    //printf("real_second %d\n", real_second);
    //printf("mAh %f\n", mAh);



#endif

    modelListener->setADC1voltage(Voltage);
    modelListener->setADC2current(Current);
    modelListener->setAh(mAh);
    modelListener->tickCounterUpdated(tickCounter);
    //modelListener->updateTime(RTC_Time.Hours, RTC_Time.Minutes, RTC_Time.Seconds);

}
