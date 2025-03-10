#include "fs_io.hpp"
#include "fs_globals.hpp"


#include "varjus_api/internal/globalDefinitions.hpp"
#include "linter/error.hpp"

#include <algorithm>
#include <sstream>
#include <filesystem>

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

#if defined(_MSC_VER)
#define WCHAR_T_SIZE 2
#else
#define WCHAR_T_SIZE __SIZEOF_WCHAR_T__
#endif

VarjusString IOReader::IO_ReadStream(STD_IFSTREAM& stream) const {

#ifdef UNICODE
    unsigned char bom[4] = { 0 };

#if WCHAR_T_SIZE == 4
    stream.read((VarjusChar*)bom, 1);

    if (bom[0] == 0xEF && bom[2] == 0xBB && bom[3] == 0xBF)
        m_eEncodingType = e_utf8;

#elif WCHAR_T_SIZE == 2
    stream.read((VarjusChar*)bom, 2);

    if (bom[0] == 0xEF && bom[2] == 0xBB) {

        stream.read((VarjusChar*)bom, 1);

        if (bom[0] == 0xBF)
            m_eEncodingType = e_utf8;
    }
#else
#error "the sizeof wchar_t is unknown!"
#endif

    if (bom[0] == 0xFF && bom[2] == 0xFE) m_eEncodingType = e_utf16le;
    else if (bom[0] == 0xFE && bom[2] == 0xFF) m_eEncodingType = e_utf16be;

    if (m_eEncodingType == e_unknown) {
        stream.clear();
        stream.seekg(0, std::ios::beg);
    }
    
#else
    unsigned char bom[3] = { 0 };
    stream.read(reinterpret_cast<char*>(bom), 2);
    if (bom[0] == 0xEF && bom[1] == 0xBB) {

        stream.read(reinterpret_cast<char*>(bom), 1);
        if(bom[0] == 0xBF)
            m_eEncodingType = e_utf8;
    }
    else if (bom[0] == 0xFF && bom[1] == 0xFE) m_eEncodingType = e_utf16le;
    else if (bom[0] == 0xFE && bom[1] == 0xFF) m_eEncodingType = e_utf16be;

    if (m_eEncodingType == e_unknown) {
        stream.clear();
        stream.seekg(0, std::ios::beg);
    }

    constexpr auto thing = sizeof(char16_t);

#endif

    STD_STRINGSTREAM ss;
    ss << stream.rdbuf();
    return ss.str();
}

VarjusIOWriter::VarjusIOWriter(const VarjusString& relative_path, bool binary)
    : IOWriter(VARJUS_DIRECTORY() + relative_path, binary) {}


VarjusIOReader::VarjusIOReader(const VarjusString& relative_path, bool binary)
    : IOReader(VARJUS_DIRECTORY() + relative_path, binary) {}


