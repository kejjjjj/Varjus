#include "fs_io.hpp"
#include "fs_globals.hpp"


#include "varjus_api/internal/globalDefinitions.hpp"
#include "linter/error.hpp"

#include <algorithm>
#include <sstream>
#include <filesystem>

/***********************************************************************
 >                             IOReader
***********************************************************************/


std::optional<VarjusString> IOReader::IO_Read(/*size_t num_bytes*/) const {

    if (m_bErrorOccurred)
        return {};

    std::filesystem::path path(m_sFileName);

    STD_IFSTREAM file(path, std::ios::in | (m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
    if (!file.is_open()) {
        return {};
    }
     
    auto content = IO_ReadStream(file);

    file.close();
    return content.length() ? std::make_optional(content) : std::nullopt;
}

VarjusString IOReader::IO_ReadStream(STD_IFSTREAM& stream) const {

    using LC = LocaleConverter;
    std::istreambuf_iterator<char> begin(stream), end;
    std::string contents(begin, end);

    if ((std::uint8_t)contents[0] == 0xEF && (std::uint8_t)contents[1] == 0xBB && (std::uint8_t)contents[2] == 0xBF) {
        m_eEncodingType = e_utf8;
        contents = contents.substr(3);
    }
    else if ((std::uint8_t)contents[0] == 0xFF && (std::uint8_t)contents[1] == 0xFE) {
        m_eEncodingType = e_utf16le;
        contents = contents.substr(2);
#ifdef UNICODE
        return LC::utf16le_to_u16string(contents);
#else
        return LC::u16string_to_utf8(LC::utf16le_to_u16string(contents));
#endif
    }
    else if ((std::uint8_t)contents[0] == 0xFE && (std::uint8_t)contents[1] == 0xFF) {
        m_eEncodingType = e_utf16be;
        contents = contents.substr(2);
#ifdef UNICODE
        return LocaleConverter::utf16be_to_u16string(contents);
#else
        return LC::u16string_to_utf8(LocaleConverter::utf16be_to_u16string(contents));
#endif

    }
#ifdef UNICODE
    return LocaleConverter::utf8_to_u16string(contents);
#else
    return contents;
#endif
}

VarjusIOReader::VarjusIOReader(const VarjusString& relative_path, bool binary)
    : IOReader(VARJUS_DIRECTORY() + relative_path, binary) {}


