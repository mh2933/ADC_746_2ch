#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::setADC1voltage (float val)
{
	 Unicode::snprintfFloat(textArea1voltageBuffer, TEXTAREA1VOLTAGE_SIZE, "%.2f", val);
	 textArea1voltage.invalidate();
}

void Screen1View::setADC2current (float val)
{
	 Unicode::snprintfFloat(textArea2currentBuffer, TEXTAREA2CURRENT_SIZE, "%.2f", val);
	 textArea2current.invalidate();
}

void Screen1View::setAh (float val)
{
	 Unicode::snprintfFloat(textArea3Buffer, TEXTAREA3_SIZE, "%.2f", val);
	 textArea3.invalidate();
}

void Screen1View::setTextProgress (int val)
{
	 textProgress1.setValue(val);
}

//void Screen1View::setImageProgress (int val)
//{
//	 imageProgress1.setValue(val);
//}
