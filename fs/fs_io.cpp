#include "fs_io.hpp"
#include "fs_globals.hpp"


#include "varjus_api/internal/globalDefinitions.hpp"
#include "linter/error.hpp"

#include <algorithm>
#include <sstream>
#include <filesystem>

#include <locale>
std::string LocaleConverter::ToNarrow(const std::wstring& wide_str) {
    std::locale loc;

    const auto str = wide_str.data();

    auto len = wide_str.length();

    std::string narrow_str;
    narrow_str.resize(len);

    std::use_facet<std::ctype<wchar_t>>(loc).narrow(str, str + len, '?', &narrow_str[0]);

    return narrow_str;
}

std::wstring LocaleConverter::ToWide(const std::string& string) {

    std::locale loc;

    const auto str = string.data();

    auto len = string.length();

    std::wstring wide_str;
    wide_str.resize(len);

    std::use_facet<std::ctype<wchar_t>>(loc).widen(str, str + len, wide_str.data());

    return wide_str;
}

/***********************************************************************
 >                             IOWriter
***********************************************************************/
bool IOWriter::IO_Write(const VarjusString& content) const 
{

    if (m_bErrorOccurred)
        return false;

    std::filesystem::path path(m_sFileName);
    STD_OFSTREAM file(path, std::ios::out | std::ios_base::openmode(m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
    if (!file.is_open()) {
        return false;
    }

    IO_WriteStream(file, content);
    file.close();
    return true;
}
bool IOWriter::IO_Append(const VarjusString& content) const 
{
    if (m_bErrorOccurred)
        return false;

    std::filesystem::path path(m_sFileName);
    STD_OFSTREAM file(path, std::ios::app | (m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
    if (!file.is_open()) {
        return false;
    }
    IO_WriteStream(file, content);
    file.close();
    return true;
}
bool IOWriter::CreateMissingDirectoriesFromPath(VarjusString path) const
{
    size_t pos = 0;
    VarjusString progress;

    do {

        pos = path.find_first_of(DIRECTORY_SEPARATOR_CHAR);

        if (pos == VarjusString::npos)
            break;

        progress += path.substr(0, pos + 1);
        path = path.erase(0, pos + 1);

        progress.pop_back();
        if (!fs::directory_exists(progress)) {
            if (!fs::create_directory(progress)) {
                return false;
            }
        }
        progress += DIRECTORY_SEPARATOR_CHAR;

    } while (true);

    return true;

}

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

    std::istreambuf_iterator<char> begin(stream), end;
    std::string contents(begin, end);

    if ((std::uint8_t)contents[0] == 0xEF && (std::uint8_t)contents[1] == 0xBB && (std::uint8_t)contents[2] == 0xBF) {
        m_eEncodingType = e_utf8;
        contents = contents.substr(3);
    }
    else if ((std::uint8_t)contents[0] == 0xFF && (std::uint8_t)contents[1] == 0xFE) {
        m_eEncodingType = e_utf16le;
        contents = contents.substr(2);
    }
    else if ((std::uint8_t)contents[0] == 0xFE && (std::uint8_t)contents[1] == 0xFF) {
        m_eEncodingType = e_utf16be;
        contents = contents.substr(2);
    }

#ifdef UNICODE
    return LocaleConverter::ToWide(contents);
#else
    return contents;
#endif
}

VarjusIOWriter::VarjusIOWriter(const VarjusString& relative_path, bool binary)
    : IOWriter(VARJUS_DIRECTORY() + relative_path, binary) {}


VarjusIOReader::VarjusIOReader(const VarjusString& relative_path, bool binary)
    : IOReader(VARJUS_DIRECTORY() + relative_path, binary) {}


