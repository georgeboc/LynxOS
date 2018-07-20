#ifndef LY_PORTS_HPP
#define LY_PORTS_HPP

#include "LY_Types.hpp"

namespace LY{
	namespace Ports{
			uint8_t InputB(uint16_t _port);
			uint16_t InputW(uint16_t port);
			uint32_t InputL(uint16_t port);

			void InputSL(uint16_t port,unsigned int buffer, int count);
			void InputSW(uint16_t port,unsigned int buffer, int count);
			unsigned int OutputSL(uint16_t port, int count);
			unsigned int OutputSW(uint16_t port, int count);

			void OutputB(uint16_t port, uint8_t value); 
			void OutputW(uint16_t port, uint16_t value);
			void OutputL(uint16_t port, uint32_t value);			
		}
	}
#endif
