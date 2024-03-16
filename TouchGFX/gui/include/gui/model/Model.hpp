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

    //virtual void tickCounterUpdated(int value){};
    void tick();
protected:
    ModelListener* modelListener;
	void someFunction();
    int Voltage;
    float Current;
    float mAh;
    volatile int tickCounter;
    int seconds;
    //volatile int seconds;
    //volatile int minutes;
    //volatile float elapsedSeconds;
    std::chrono::high_resolution_clock::time_point startTime;

};

#endif // MODEL_HPP
