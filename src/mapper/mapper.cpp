#include <array>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

#include "defines.hpp"
#include "error_or.hpp"
#include "mapper.hpp"

namespace RNES::Mapper {

    ErrorOr<std::vector<uint8_t>> readFile(const char *t_filePath);

    class BinaryParser {
    public:
        explicit BinaryParser(std::vector<uint8_t> t_data) : m_data(std::move(t_data)), m_index(0) {
            ;
        }

        template<typename T>
        ErrorOr<T> read() {
            if (m_index + sizeof(T) - 1 < m_data.size()) {
                T result = 0;
                for (size_t i = 0; i < sizeof(T); i++) {
                    result |= m_data[m_index + i] << (sizeof(char) * i);
                }

                return result;
            } else {
                return ErrorCode(ERROR_INDEX_OUT_OF_RANGE);
            }
        }

        template<size_t N>
        ErrorOr<std::array<uint8_t, N>> readBytes() {
            if (m_index + N <= m_data.size()) {
                std::array<uint8_t, N> result{};

                for (size_t i = 0; i < N; i++) {
                    result[i] = m_data[m_index + i];
                }

                return result;
            } else {
                return ErrorCode(ERROR_INDEX_OUT_OF_RANGE);
            }
        }

        ErrorOr<std::vector<uint8_t>> readBytes(size_t t_count) {
            if (m_index + t_count <= m_data.size()) {
                std::vector<uint8_t> result(t_count, 0);

                for (size_t i = 0; i < t_count; i++) {
                    result[i] = m_data[m_index + i];
                }

                return result;
            } else {
                return ErrorCode(ERROR_INDEX_OUT_OF_RANGE);
            }
        }

        ErrorOr<std::vector<uint8_t>> readRest() {
            return readBytes(m_data.size() - m_index - 1);
        }

        void skip(size_t t_amount) {
            m_index += t_amount;
        }

    private:
        std::vector<uint8_t> m_data;
        size_t m_index;
    };

    enum class MirroringType : bool {
        HORIZONTAL = false,
        VERTICAL = true
    };

    enum class ConsoleType : uint8_t {
        NES_OR_FAMICOM = 0,
        NINTENDO_VS,
        NINTENDO_PLAYCHOICE,
        EXTENDED_CONSOLE_TYPE
    };

    enum class PPUTimingMode : uint8_t {
        NTSC = 0,
        PAL,
        MULTI_REGION,
        DENDY
    };

    struct INESHeader {
        MirroringType mirroringType;
        bool hasBattery;
        bool hasTrainer;
        bool usesFourScreenMode;

        ConsoleType consoleType;
        bool usesINES2Format;

        PPUTimingMode cpuPpuTimingMode;

        // TODO: Only NES/Famicom supported

        uint16_t mapperNumber;
        uint8_t subMapperNumber;

        size_t prgRomSize;
        size_t chrRomSize;

        size_t prgRamSize;
        size_t eepromSize;

        size_t chrRamSize;
        size_t chrNVRamSize;

        size_t noMiscellaneousRoms;

        size_t defaultExpansionDevice;
    };

    ErrorOr<INESHeader> parseINESHeader(BinaryParser &t_parser) {
        const auto flags = TRY(t_parser.readBytes<16>());
        REQUIRE(
                (flags[0] == 'N') && (flags[1] == 'E') && (flags[2] == 'S') && (flags[3] == 0x1A),
                ERROR_INVALID_FILE
        );

        INESHeader header {};
        header.mirroringType = static_cast<MirroringType>(flags[6] & 0x01);
        header.hasBattery = flags[6] & 0x02;
        header.hasTrainer = flags[6] & 0x04;
        header.usesFourScreenMode = flags[6] & 0x08;

        header.consoleType = static_cast<ConsoleType>(flags[7] & 0x03);
        header.usesINES2Format = (flags[7] & 0x0C) == 0x08;

        header.mapperNumber = ((flags[8] & 0x0F) << 8) | (flags[7] & 0xF0) | ((flags[6] & 0xF0) >> 4);
        header.subMapperNumber = (flags[8] & 0xF0) >> 4;

        header.prgRomSize = ((flags[9] & 0x0F) << 8) | flags[4];
        header.chrRomSize = ((flags[9] & 0xF0) << 4) | flags[5];

        const auto computeShiftedSize = [](uint8_t t_shiftCount) -> size_t {
            return (t_shiftCount != 0) ? (64 << t_shiftCount) : (0);
        };

        header.prgRamSize = computeShiftedSize(flags[10] & 0x0F);
        header.eepromSize = computeShiftedSize((flags[10] & 0xF0) >> 4);

        header.chrRamSize = computeShiftedSize(flags[11] & 0x0F);
        header.chrNVRamSize = computeShiftedSize((flags[11] & 0xF0) >> 4);

        header.cpuPpuTimingMode = static_cast<PPUTimingMode>(flags[12] & 0x03);

        header.noMiscellaneousRoms = flags[14] & 0x03;

        header.defaultExpansionDevice = flags[15] & 0x3F;

        std::cout << "Mirroring Type: " << static_cast<int>(header.mirroringType) << '\n';
        std::cout << "Has Battery: " << header.hasBattery << '\n';
        std::cout << "Has Trainer: " << header.hasTrainer << '\n';
        std::cout << "Uses Four Screen Mode: " << header.usesFourScreenMode << '\n';

        std::cout << "Console Type: " << static_cast<int>(header.consoleType) << '\n';
        std::cout << "Uses INES 2.0 Format: " << header.usesINES2Format << '\n';

        std::cout << "Mapper Number: " << header.mapperNumber << '\n';
        std::cout << "Submapper Number: " << static_cast<int>(header.subMapperNumber) << '\n';

        std::cout << "PRG ROM Size: " << header.prgRomSize << '\n';
        std::cout << "CHR ROM Size: " << header.chrRomSize << '\n';

        std::cout << "PRG RAM Size: " << header.prgRamSize << '\n';
        std::cout << "EEPROM Size: " << header.eepromSize << '\n';

        std::cout << "CHR RAM Size: " << header.chrRamSize << '\n';
        std::cout << "CHR Non-volatile RAM Size: " << header.chrNVRamSize << '\n';

        std::cout << "CPU/PPU Timing Mode: " << static_cast<int>(header.cpuPpuTimingMode) << '\n';

        std::cout << "# Miscellaneous ROMS: " << header.noMiscellaneousRoms << '\n';

        std::cout << "Default Expansion Device: " << header.defaultExpansionDevice << '\n';

        return header;
    }

    ErrorOr<Mapper> parseMapperFromINES(const char *t_filePath) {
        const std::vector<uint8_t> fileData = TRY(readFile(t_filePath));
        BinaryParser parser{fileData};

        INESHeader header = TRY(parseINESHeader(parser));

        std::array<uint8_t, 512> trainerArea{};
        if (header.hasTrainer) {
            trainerArea = TRY(parser.readBytes<512>());
        }

        // TODO: check this works with VS system stuff
        std::vector<uint8_t> prgRom{};
        if ((header.prgRomSize & 0x0F00) == 0x0F00) {
            const size_t multiplier = header.prgRomSize & 0x03;
            const size_t exponent = header.prgRomSize & 0xFC;
            const size_t trueSize = (1 << exponent) * (2 * multiplier + 1);

            prgRom = TRY(parser.readBytes(trueSize));
        } else {
            std::cout << header.prgRomSize * 16384 << "b\n";
            prgRom = TRY(parser.readBytes(header.prgRomSize * 16384));
        }

        std::vector<uint8_t> chrRom{};
        if ((header.chrRomSize & 0x0F00) == 0x0F00) {
            const size_t multiplier = header.chrRomSize & 0x03;
            const size_t exponent = header.chrRomSize & 0xFC;
            const size_t trueSize = (1 << exponent) * (2 * multiplier + 1);

            chrRom = TRY(parser.readBytes(trueSize));
        } else {
            chrRom = TRY(parser.readBytes(header.chrRomSize * 8192));
        }

        std::vector<uint8_t> miscRom = TRY(parser.readRest());

        return ErrorCode(-1);
    }

    ErrorOr<std::vector<uint8_t>> readFile(const char *t_filePath) {
        std::ifstream fileStream(t_filePath, std::ios::in | std::ios::binary | std::ios::ate);
        REQUIRE(fileStream.is_open(), ERROR_FAILED_TO_OPEN_FILE);

        const std::streampos fileSize = fileStream.tellg();
        fileStream.seekg(std::ios::beg);

        std::vector<uint8_t> result;
        result.resize(fileSize);
        fileStream.read(reinterpret_cast<char *>(result.data()), fileSize);

        return result;
    }

}

