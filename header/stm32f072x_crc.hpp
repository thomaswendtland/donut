
#pragma once

#include <cstdint>
#include "Bitfield.hpp"

using namespace donut;

namespace stm32f072x {
	template <std::uint32_t BaseAddress, std::uint16_t Irq, typename ... Pins>
	struct CrcController {
		struct Dr {
			using WidthType = std::uint32_t;
			static constexpr WidthType Address = BaseAddress + 0x0;
			using Value = Bitfield<Dr, std::uint32_t, 0, 32, AccessType::ReadWrite>;
		};
		struct Idr {
			using WidthType = std::uint32_t;
			static constexpr WidthType Address = BaseAddress + 0x4;
			using Value = Bitfield<Idr, std::uint8_t, 0, 8, AccessType::ReadWrite>;
		};
		struct Cr {
			using WidthType = std::uint32_t;
			static constexpr WidthType Address = BaseAddress + 0x8;
			using Reset = Bitfield<Cr, std::uint8_t, 0, 1, AccessType::ReadWrite>;
			using Rev_In = Bitfield<Cr, std::uint8_t, 5, 2, AccessType::ReadWrite>;
			using Rev_Out = Bitfield<Cr, std::uint8_t, 7, 1, AccessType::ReadWrite>;
		};
		struct Init {
			using WidthType = std::uint32_t;
			static constexpr WidthType Address = BaseAddress + 0xC;
			using Value = Bitfield<Init, std::uint32_t, 0, 32, AccessType::ReadWrite>;
		};
	};
	using CRC1 = CrcController<0x40023000, 0xFF>;
	using CRC2 = CrcController<0x40004000, 0xFF>;
	using CRC3 = CrcController<0x40004400, 0xFF>;

} // end of namespace stm32f072x