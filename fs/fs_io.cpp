#include "fs_io.hpp"
#include "fs_globals.hpp"


#include "varjus_api/internal/globalDefinitions.hpp"
#include "linter/error.hpp"

#include <algorithm>
#include <sstream>
#include <filesystem>
#include <cassert>

/***********************************************************************
 >                             IOReader
***********************************************************************/


std::optional<VarjusString> IOReader::IO_Read(EncodingType type) const {

    if (m_bErrorOccurred)
        return {};

    std::filesystem::path path(m_sFileName);

    std::ifstream file(path, std::ios::in | (m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
    if (!file.is_open()) {
        return {};
    }
     
    auto content = IO_ReadStream(file, type);

    file.close();
    return content.length() ? std::make_optional(content) : std::nullopt;
}

VarjusString IOReader::IO_ReadStream(std::ifstream& stream, EncodingType type) const {

    using LC = LocaleConverter;
    std::istreambuf_iterator<char> begin(stream), end;
    std::string contents(begin, end);

    if ((std::uint8_t)contents[0] == 0xEF && (std::uint8_t)contents[1] == 0xBB && (std::uint8_t)contents[2] == 0xBF) {
        m_eEncodingType = e_utf8;
        contents = contents.substr(3);
#ifdef UNICODE
        return LC::utf8_to_u16string(contents);
#else
        return contents;
#endif
    }
    else if ((std::uint8_t)contents[0] == 0xFF && (std::uint8_t)contents[1] == 0xFE) {
        m_eEncodingType = e_utf16le;
        contents = contents.substr(2);
#ifdef UNICODE
        return LC::utf16le_to_u16string(contents);
#else
        return LC::u16string_to_ansi(LC::utf16le_to_u16string(contents));
#endif
    }
    else if ((std::uint8_t)contents[0] == 0xFE && (std::uint8_t)contents[1] == 0xFF) {
        m_eEncodingType = e_utf16be;
        contents = contents.substr(2);
#ifdef UNICODE
        return LC::utf16be_to_u16string(contents);
#else
        return LC::u16string_to_ansi(LC::utf16be_to_u16string(contents));
#endif

    }

    //no idea what the encoding is, just decide that it's ansi
    if ((m_eEncodingType == e_unknown) && (type == e_utf8 || type == e_ansi || type == e_auto || type == e_unknown)) {
#ifdef UNICODE
        return LC::ansi_to_u16string(contents);
#else
        return contents;
#endif
    }

    switch (type) {
    case e_utf8:
#ifdef UNICODE
        return LC::utf8_to_u16string(contents);
#else
        return LC::u16string_to_ansi(LC::utf8_to_u16string(contents));
#endif
    case e_utf16le:
#ifdef UNICODE
        return LC::utf16le_to_u16string(contents);
#else
        return LC::u16string_to_ansi(LC::utf16le_to_u16string(contents));
#endif
    case e_utf16be:
#ifdef UNICODE
        return LC::utf16be_to_u16string(contents);
#else
        return LC::u16string_to_ansi(LC::utf16be_to_u16string(contents));
#endif
    case e_ansi:
    case e_unknown:
    case e_auto:
    default:
        break;
    }

    return VSL("");
}

VarjusIOReader::VarjusIOReader(const VarjusString& relative_path, bool binary)
    : IOReader(VARJUS_DIRECTORY() + relative_path, binary) {}


