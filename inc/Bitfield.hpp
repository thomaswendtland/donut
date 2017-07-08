
#include <cstdint>

namespace donut {

    enum class AccessType {
        Read,
        Write,
        ReadWrite
    };

    template<typename DataType, typename Register, std::uint32_t offset, std::uint32_t width, AccessType Access>
    struct Bitfield {

    };
}
