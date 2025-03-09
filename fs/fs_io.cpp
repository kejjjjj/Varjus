#include "fs_io.hpp"
#include "fs_globals.hpp"

#include "varjus_api/internal/globalDefinitions.hpp"
#include "linter/error.hpp"

#include <algorithm>
#include <sstream>
#include <iostream>
/***********************************************************************
 >                             IOWriter
***********************************************************************/
bool IOWriter::IO_Write(const VarjusString& content) const 
{

    if (m_bErrorOccurred)
        return false;

    STD_OFSTREAM file(m_sFileName, std::ios::out | std::ios_base::openmode(m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
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

    STD_OFSTREAM file(m_sFileName, std::ios::app | (m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
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
#ifdef UNICODE
static std::wstring FixLittleEndianness(const std::wstring& src)
{
    if (src.length() % 2 != 0)
        throw std::runtime_error("corrupted eof byte in file");

    std::wstring fixed;

    for (auto it = src.begin(); it != src.end(); it += 2) {

        if (*std::next(it) == '\0') {
            fixed.push_back(*it);
            continue;
        }
        auto combined = static_cast<VarjusChar>((*it << 8) | static_cast<VarjusChar>(*std::next(it)));
        fixed.push_back(combined);
    }

    return fixed;
}
#endif

std::optional<VarjusString> IOReader::IO_Read(/*size_t num_bytes*/) const {

    if (m_bErrorOccurred)
        return {};

    STD_IFSTREAM file(m_sFileName, std::ios::in | (m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
    if (!file.is_open()) {
        return {};
    }

    auto content = IO_ReadStream(file);

#ifdef UNICODE
    if (m_eEncodingType == UTF16_LE) {
        content = FixLittleEndianness(content);
    }else if (m_eEncodingType == UTF16_BE) {
        content = FixLittleEndianness(content.substr(1) + L'\0');
    }
#endif

    file.close();
    return content.length() ? std::make_optional(content) : std::nullopt;
}


VarjusString IOReader::IO_ReadStream(STD_IFSTREAM& stream) const {


#if UNICODE
    unsigned char bom[4] = { 0 };
    stream.read((wchar_t*)bom, 2);

    if (bom[0] == 0xEF && bom[2] == 0xBB) {

        stream.read((wchar_t*)bom, 1);

        if(bom[0] == 0xBF)
            m_eEncodingType = UTF8;
    }
    else if (bom[0] == 0xFF && bom[2] == 0xFE) m_eEncodingType = UTF16_LE;
    else if (bom[0] == 0xFE && bom[2] == 0xFF) m_eEncodingType = UTF16_BE;

    if (m_eEncodingType == UNKNOWN) {
        STD_CERR << "the input file must include the byte order mark!\n";
        throw std::runtime_error("the input file must include the byte order mark!");
    }

#else
    unsigned char bom[3] = { 0 };
    stream.read(reinterpret_cast<char*>(bom), 2);
    if (bom[0] == 0xEF && bom[1] == 0xBB) {

        stream.read(reinterpret_cast<char*>(bom), 1);
        if(bom[0] == 0xBF)
            m_eEncodingType = UTF8;
    }
    else if (bom[0] == 0xFF && bom[1] == 0xFE) m_eEncodingType = UTF16_LE;
    else if (bom[0] == 0xFE && bom[1] == 0xFF) m_eEncodingType = UTF16_BE;

    if (m_eEncodingType == UNKNOWN) {
        stream.clear();
        stream.seekg(0, std::ios::beg);
    }
#endif

    STD_STRINGSTREAM ss;
    ss << stream.rdbuf();
    return ss.str();
}

VarjusIOWriter::VarjusIOWriter(const VarjusString& relative_path, bool binary)
    : IOWriter(VARJUS_DIRECTORY() + relative_path, binary) {}


VarjusIOReader::VarjusIOReader(const VarjusString& relative_path, bool binary)
    : IOReader(VARJUS_DIRECTORY() + relative_path, binary) {}


