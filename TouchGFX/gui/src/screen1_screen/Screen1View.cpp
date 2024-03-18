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

void Screen1View::setADC1 (float val)
{
	 Unicode::snprintfFloat(textArea1Buffer, TEXTAREA1_SIZE, "%.2f", val);
	 textArea1.invalidate();
}

void Screen1View::setADC2 (float val)
{
	 Unicode::snprintfFloat(textArea2Buffer, TEXTAREA2_SIZE, "%.2f", val);
	 textArea2.invalidate();
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
