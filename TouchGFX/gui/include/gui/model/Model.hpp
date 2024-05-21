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
    void writeToSDCard(char* data);
    void readFromSDCard();

//    typedef struct {
//        enum {
//            SD_WRITE,
//            SD_READ
//        } command;
//        char* data; // For SD_WRITE, this could be the data to write. For SD_READ, this could be a buffer to read into.
//    } SD_Operation_t;




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
    uint8_t last_second = 0;
    uint8_t second = 0;
    //char mAhString[20];
    uint32_t milliSec = 0;






};



#endif // MODEL_HPP
