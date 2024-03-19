#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void setADC1voltage (float val) {};
    virtual void setADC2current (float val) {};
    virtual void setAh (float val) {};
    virtual void tickCounterUpdated (int val) {};

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
