#ifndef LY_SCREEN_HPP
#define LY_SCREEN_HPP
#include "LY_Color.hpp"
#include "LY_Types.hpp"

/**
 * @brief The side of the screen, true is background, false is foreground
 * */
typedef enum LY_SIDE{
		LY_SIDE_BACKGROUND,
		LY_SIDE_FOREGROUND
}LY_SIDE;

namespace LY{
	namespace Screen{
			LY_Color GetColor(LY_SIDE side);
			void SetColor(LY_SIDE side, LY_Color colour);
			void PutChar(char c);
			void PutString(const char* str);
			void Clear(LY_Color colour);
			void SetCursor(uint8_t x, uint8_t y);
			void itoa(unsigned long long i);
			void BlockEnable(void);
			void BlockDisable(void);
		}
	}
#endif
