#include "LY_Types.hpp"
#include "LY_Color.hpp"
#include "LY_Screen.hpp"
#include "LY_Ports.hpp"
#include "LY_Math.cpp"

uint16_t *vidmem= (uint16_t *)0xB8000;
LY_Color backColour = LY_COLOR_BLACK;
LY_Color foreColour = LY_COLOR_WHITE;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

uint8_t block_print = 0;
uint8_t block_nl = 0;

static void UpdateCursor()
{
   // The screen is 80 characters wide...
   uint16_t cursorLocation = cursor_y * 80 + cursor_x;
   LY::Ports::OutputB(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
   LY::Ports::OutputB(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
   LY::Ports::OutputB(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
   LY::Ports::OutputB(0x3D5, cursorLocation);      // Send the low cursor byte.
}

/**
 * @brief Gets the current color
 * @param side The side to get the color
 * */
LY_Color LY::Screen::GetColor(LY_SIDE side)
{
	if(side==LY_SIDE_BACKGROUND){
		return backColour;
	}else{
		return foreColour;
	}
}
/**
 * @brief Sets the color to a screen side
 * @param side The side to set colour
 * @param colour The new colour
 * @see GetColor
 * */
void LY::Screen::SetColor(LY_SIDE side, LY_Color colour)
{
	if(side==LY_SIDE_BACKGROUND)
	{
		backColour=colour;
	}else{
		foreColour=colour;
	}
}
/**
 * @brief Puts the char on screen
 * @param c The character to write
 * */
void LY::Screen::PutChar(char c)
{
	uint8_t  attributeByte = (backColour << 4) | (foreColour & 0x0F);
	uint16_t attribute = attributeByte << 8;
	uint16_t *location;
	if (c == 0x08 && cursor_x)
	{
		cursor_x--;
	}else if(c == '\r')
	{
		cursor_x=0;
	}else if(c == '\n')
	{
		if(block_nl == 0)
		{
			cursor_x=0;
			cursor_y++;
		}
	}
	if(c >= ' ') /* Printable character */
	{
		location = vidmem + (cursor_y*80 + cursor_x);
		*location = c | attribute;
		cursor_x++;
	}
	if(cursor_x >= 80) /* New line, please*/
	{
		cursor_x = 0;
		cursor_y++;
	}

	
	/* Scroll if needed*/
	uint8_t attributeByte2 = (1 /*black*/ << 4) | (15 /*white*/ & 0x0F);
	uint16_t blank = 0x20 /* space */ | (attributeByte2 << 8);
	if(cursor_y >= 25)
	{
       int i;
       for (i = 0*80; i < 24*80; i++)
       {
           vidmem[i] = vidmem[i+80];
       }

       // The last line should now be blank. Do this by writing
       // 80 spaces to it.
       for (i = 24*80; i < 25*80; i++)
       {
           vidmem[i] = blank;
       }
       // The cursor should now be on the last line.
       cursor_y = 24;
   }
	/* Update cursor */
	UpdateCursor();
}
/**
 * @brief Puts a complete string to screen
 * @param str The string to write
 * */
void LY::Screen::BlockEnable(void)
{
	block_print = 1;
}

void LY::Screen::BlockDisable(void)
{
	block_print = 0;
}

void LY::Screen::PutString(const char* str)
{
	int i=0;
	if(block_print == 0)
	{
		while(str[i]) 
		{
			LY::Screen::PutChar(str[i++]);
		}
	}
}
/**
 * @brief Cleans the screen with a color
 * @param colour The colour to fill the screen
 * */
 void LY::Screen::Clear(LY_Color colour)
{
   // Make an attribute byte for the default colours
   uint8_t attributeByte = (colour /*background*/ << 4) | (15 /*white - foreground*/ & 0x0F);
   uint16_t blank = 0x20 /* space */ | (attributeByte << 8);

   int i;
   for (i = 0; i < 80*25; i++)
   {
       vidmem[i] = blank;
   }

   // Move the hardware cursor back to the start.
   cursor_x = 0;
   cursor_y = 0;
   
   UpdateCursor();
}
/**
 * @brief Sets the cursor via software
 * @param x The position of X
 * @param y The position of y
 * */
void LY::Screen::SetCursor(uint8_t x, uint8_t y)
{
	cursor_x=x;
	cursor_y=y;
	
	UpdateCursor();
}

void LY::Screen::itoa(unsigned long long i){
	unsigned long long numero_cifras=0;
	int primera_cifra=1;
	unsigned long long cifra_almacenada_factor=0;
	unsigned long long numero_a_imprimir=i;
	unsigned long long num_imp=0;
	do
	{
		i=i/10;
		numero_cifras++;	
	}while(i!=0);
	for(int l=(numero_cifras-1);l>=0;l--)
	{
		num_imp=numero_a_imprimir/LY::Math::Pow(10,l);

		if(primera_cifra==0)
		{
			LY::Screen::PutChar((char)(num_imp-cifra_almacenada_factor*10+48));
			cifra_almacenada_factor=num_imp;			
		}
		else
		{
			cifra_almacenada_factor=num_imp;
			LY::Screen::PutChar((char)(num_imp+48));
			primera_cifra=0;
		}
	}
}
