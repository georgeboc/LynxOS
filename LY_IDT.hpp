#include "LY_Types.hpp"

#ifndef LY_IDT_HPP
#define LY_IDT_HPP

namespace LY{
	namespace IDT{
			void Flush();
			void SetGate(uint8_t, uint32_t, uint16_t, uint8_t);
			void Install();
			struct Entry{
				uint16_t base_low;
				uint16_t sel;
				uint8_t always0;
				uint8_t flags;
				uint16_t base_high;
			} __attribute__((packed));
			struct Ptr{
				uint16_t limit;
				uint32_t base;
			} __attribute__((packed));
		}
	}

#endif
