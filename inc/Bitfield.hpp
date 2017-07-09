
#include <cstdint>
#include <limits>

namespace donut {

    enum class AccessType {
        Read,
        Write,
        ReadWrite
    };

    template<typename DataType, typename Register, std::uint32_t offset, std::uint32_t width, AccessType Access>
    struct Bitfield {
        static_assert((offset+width) < std::numeric_limits<typename Register::WidthType>::digits, "Invalid offset/width for Bitfield.");
    };
}
