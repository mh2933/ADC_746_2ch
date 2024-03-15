# ADC_746_2ch
reading two adc channels on an stm32f746 and converts it to voltage, current, Ah. 
The latest functionality is model.cpp has two counters one for tick one for seconds using HAL_Delay(1000) to
count seconds. Two adc channels are displayed voltage and current respectivly, and Ah aare calculated with the 
counter and there is a bargraph counting percent that is not connected yet. 
