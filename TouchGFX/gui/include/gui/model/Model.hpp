#ifndef MODEL_HPP
#define MODEL_HPP

#include <chrono>

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();
    uint8_t rtcSeconds();
    float calcMilliAh();
    float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);



protected:
    ModelListener* modelListener;

    float Voltage;
    float Current;
    float mAh;
    volatile int tickCounter;
    volatile long int milli_seconds;
    float seconds;
    volatile int tickCounterNow;
    long count_milli_seconds;
    uint8_t previous_second;
    uint8_t real_second;
    int ADC_Value;



    //volatile long int counting_milliseconds;


    //std::chrono::high_resolution_clock::time_point startTime;

};



#endif // MODEL_HPP
