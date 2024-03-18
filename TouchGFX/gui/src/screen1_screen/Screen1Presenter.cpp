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

void Screen1Presenter::setADC1 (float val)
{
	view.setADC1 (val);
}

void Screen1Presenter::setADC2 (float val)
{
	view.setADC2 (val);
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
