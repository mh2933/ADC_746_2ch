#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void setADC1voltage (float val);
    virtual void setADC2current (float val);
    virtual void setAh (float val);
//    virtual void setImageProgress (int val);
    virtual void setTextProgress (int val);
protected:
};

#endif // SCREEN1VIEW_HPP
