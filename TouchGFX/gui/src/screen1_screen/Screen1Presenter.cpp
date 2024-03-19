#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}

void Screen1Presenter::activate()
{

}

void Screen1Presenter::deactivate()
{

}

void Screen1Presenter::setADC1voltage (float val)
{
	view.setADC1voltage (val);
}

void Screen1Presenter::setADC2current (float val)
{
	view.setADC2current (val);
}

void Screen1Presenter::setAh (float val)
{
	view.setAh (val);
}

void Screen1Presenter::tickCounterUpdated (int val)
{
    //view.setImageProgress (val);
    view.setTextProgress (val);
}
