# ADC_746_2ch
reading two adc channels on an stm32f746 and converts it to voltage, current, Ah. 
The latest functionality is as of 2024-04-20:
model.cpp implements readings from two adc channels and converts the data to voltage, current, Ah. The built in RTC
is used to calculate seconds for use with the Ah. 
Class functions has been implmented inside model.cpp for voltage and current to make it more readable and a tidier look, 
one more function should be implemented for the conversion of adc values to make Model::tick look better.
Have started to implement one thread inside main.c to be able to utilize more functionality like data storage, 
I want to be able to save the Ah to the SD card but the display graphics chrashes when implemented so the FreeRTOS threads
is looked into. 
