#include "fs_io.hpp"
#include "fs_globals.hpp"

#include <algorithm>
/***********************************************************************
 >                             IOWriter
***********************************************************************/
bool IOWriter::IO_Write(const std::string& content) const 
{

    if (m_bErrorOccurred)
        return false;

    std::ofstream file(m_sFileName, std::ios::out | std::ios_base::openmode(m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
    if (!file.is_open()) {
        return false;
    }

    IO_WriteStream(file, content);
    file.close();
    return true;
}
bool IOWriter::IO_Append(const std::string& content) const 
{
    if (m_bErrorOccurred)
        return false;

    std::ofstream file(m_sFileName, std::ios::app | (m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
    if (!file.is_open()) {
        return false;
    }
    IO_WriteStream(file, content);
    file.close();
    return true;
}
bool IOWriter::CreateMissingDirectoriesFromPath(std::string path) const
{
    size_t pos = 0;
    std::string progress;

    do {

        pos = path.find_first_of('\\');

        if (pos == std::string::npos)
            break;

        progress += path.substr(0, pos + 1);
        path = path.erase(0, pos + 1);

        progress.pop_back();
        if (!fs::directory_exists(progress)) {
            if (!fs::create_directory(progress)) {
                return false;
            }
        }
        progress += '\\';

    } while (true);

    return true;

}

/***********************************************************************
 >                             IOReader
***********************************************************************/
std::optional<std::string> IOReader::IO_Read(/*size_t num_bytes*/) const {

    if (m_bErrorOccurred)
        return {};

    std::ifstream file(m_sFileName, std::ios::in | (m_bBinary ? std::ios::binary : std::ios_base::openmode(0)));
    if (!file.is_open()) {
        return {};
    }

    auto content = IO_ReadStream(file);
    file.close();
    return content.length() ? std::make_optional(content) : std::nullopt;
}


std::string IOReader::IO_ReadStream(std::ifstream& stream) const {
    std::string buffer;

    try {
        char buf[4096];
        while (stream.read(buf, sizeof(buf))) {
            buffer.append(buf, size_t(stream.gcount()));

            if (!m_bContainsUnicode) {
                m_bContainsUnicode = std::any_of(buf, buf + stream.gcount(), [](unsigned char c) { return c > 127; });
            }
        }
        buffer.append(buf, size_t(stream.gcount()));

        if (!m_bContainsUnicode) {
            m_bContainsUnicode = std::any_of(buf, buf + stream.gcount(), [](unsigned char c) { return c > 127; });
        }
    }
    catch ([[maybe_unused]] std::ios_base::failure& ex) {
    }

    return buffer;
}


/***********************************************************************
 >                              AgentIO
***********************************************************************/

VarjusIOWriter::VarjusIOWriter(const std::string& relative_path, bool binary)
    : IOWriter(VARJUS_DIRECTORY() + "\\" + relative_path, binary) {}


VarjusIOReader::VarjusIOReader(const std::string& relative_path, bool binary)
    : IOReader(VARJUS_DIRECTORY() + "\\" + relative_path, binary) {}


