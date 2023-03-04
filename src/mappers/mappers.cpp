#include <fstream>
#include <vector>

#include "defines.hpp"
#include "mappers.hpp"

namespace RNES::Mappers {

    class BinaryParser {
    public:
        BinaryParser(const char* t_filePath) : m_data(), m_index(0) {
            std::ifstream fileStream(t_filePath, std::ios::in | std::ios::binary | std::ios::ate);
            ASSERT(fileStream.is_open(), "Failed to open file");

            const size_t fileSize = fileStream.tellg();
            fileStream.seekg(std::ios::begin);

            m_data.resize(fileSize);
            fileStream.read(reinterpret_cast<char*>(m_data.data()), fileSize);
        }

        uint8_t read8() {
            const uint8_t result = m_data[m_index];

            m_index += 1;
            return result;
        }

        uint16_t read16() {
            const uint16_t result =
                (m_data[m_index + 1] << 8) |
                (m_data[m_index + 0] << 0);

            m_index += 2;
            return result;
        }

        uint32_t read32() {
            const uint32_t result =
                (m_data[m_index + 3] << 24) |
                (m_data[m_index + 2] << 16) |
                (m_data[m_index + 1] <<  8) |
                (m_data[m_index + 0] <<  0);

            m_index += 4;
            return result;
        }

        uint64_t read64() {
            const uint64_t result =
                (m_data[m_index + 7] << 56) |
                (m_data[m_index + 6] << 48) |
                (m_data[m_index + 5] << 40) |
                (m_data[m_index + 4] << 32) |
                (m_data[m_index + 3] << 24) |
                (m_data[m_index + 2] << 16) |
                (m_data[m_index + 1] <<  8) |
                (m_data[m_index + 0] <<  0);

            m_index += 8;
            return result;
        }

        void skip(size_t t_amount) {
            m_index += t_amount;
        }

    private:
        std::vector<uint8_t> m_data;
        size_t m_index;
    };

    Mapper parseMapperFromINES(const char* t_filePath) {
        BinaryParser parser(t_filePath);

        const uint32_t magicBytes = parser.read32();
        ASSERT(magicBytes == 0x1A53454E, "Invalid magic bytes");

        const size_t PRGROMSize = 16384 * parser.read8();
        const size_t CHRROMSize = 8192  * parser.read8();

        const size_t flags6  = parser.read8();
        const size_t flags7  = parser.read8();
        const size_t flags8  = parser.read8();
        const size_t flags9  = parser.read8();
        const size_t flags10 = parser.read8();

        parser.skip(5); // unused bytes
    }

}
