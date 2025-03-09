#pragma once

#include <fstream>
#include <limits>
#include <optional>

#include "varjus_api/internal/globalDefinitions.hpp"


struct IOItem
{
    enum EncodingType { UNKNOWN, UTF8, UTF16_LE, UTF16_BE };

    IOItem(const VarjusString& filename, bool in_binary_mode) 
        : m_bBinary(in_binary_mode), m_sFileName(filename) {}
    virtual ~IOItem() = default;

    [[nodiscard]] constexpr VarjusString GetFilePath() const noexcept { return m_sFileName; }
    [[nodiscard]] constexpr EncodingType GetEncoding() const noexcept { return m_eEncodingType; }
protected:
    bool m_bBinary = false;
    bool m_bErrorOccurred = false;
    VarjusString m_sFileName;
    mutable EncodingType m_eEncodingType{};
};

struct IOWriter : public IOItem
{
    IOWriter(const VarjusString& filename, bool in_binary_mode) : IOItem(filename, in_binary_mode)
    {
        m_bErrorOccurred = !CreateMissingDirectoriesFromPath(filename);
    }

    [[nodiscard]] virtual bool IO_Write(const VarjusString& content) const;
    [[nodiscard]] virtual bool IO_Append(const VarjusString& content) const;

private:
    [[nodiscard]] bool CreateMissingDirectoriesFromPath(VarjusString path) const;
    void IO_WriteStream(STD_OFSTREAM& stream, const VarjusString& content) const {
        stream.write(content.data(), std::streamsize(content.size()));
    }
};

struct IOReader : public IOItem
{
    IOReader(const VarjusString& filename, bool in_binary_mode) : IOItem(filename, in_binary_mode) {}

    [[nodiscard]] virtual std::optional<VarjusString> IO_Read(/*size_t num_bytes = std::numeric_limits<size_t>::max()*/) const;

private:
    [[nodiscard]] VarjusString IO_ReadStream(STD_IFSTREAM& stream) const;

};

#ifdef __linux__
#define DIRECTORY_SEPARATOR VSL("/")
#define DIRECTORY_SEPARATOR_CHAR '/'
#else
#define DIRECTORY_SEPARATOR VSL("\\")
#define DIRECTORY_SEPARATOR_CHAR VarjusChar('\\')

#endif

//these don't belong here but whatever
#define VARJUS_DIRECTORY() (fs::exe_path())

struct VarjusIOWriter : public IOWriter
{
    VarjusIOWriter(const VarjusString& relative_path, bool binary = false);
};
struct VarjusIOReader : public IOReader
{
    VarjusIOReader(const VarjusString& relative_path, bool binary = false);
};