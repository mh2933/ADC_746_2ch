#ifndef MODEL_HPP
#define MODEL_HPP

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
    int Voltage;
    float Current;
    float mAh;
    volatile int tickCounter;
    int seconds;
    //volatile int seconds;
    //volatile int minutes;
    //volatile float elapsedSeconds;

};

#endif // MODEL_HPP
