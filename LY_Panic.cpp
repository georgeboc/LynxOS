#include "LY_Panic.hpp"
#include "LY_Screen.hpp"

/**
 * @brief Shows a Kernel Panic screen with the selected error message
 * @param error A string containing the name of the error
 * */
void LY::Panic::Show(const char* error)
{
	LY::Screen::Clear(LY_COLOR_RED);
	LY::Screen::SetColor(LY_SIDE_BACKGROUND, LY_COLOR_RED);
	LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
	LY::Screen::SetCursor(29,10); //(80-22)/2
	LY::Screen::PutString("Lynx Kernel Error\n");
	LY::Screen::SetCursor(15,12);
	LY::Screen::PutString(error);
}

void LY::Panic::Show_Set(void)
{
	LY::Screen::Clear(LY_COLOR_RED);
	LY::Screen::SetColor(LY_SIDE_BACKGROUND, LY_COLOR_RED);
	LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
	LY::Screen::SetCursor(29,10); //(80-22)/2
	LY::Screen::PutString("Lynx Kernel Error\n");
	LY::Screen::SetCursor(15,12);
}
