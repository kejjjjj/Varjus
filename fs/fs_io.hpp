#pragma once

#include <fstream>
#include <limits>
#include <optional>

#include "varjus_api/internal/globalDefinitions.hpp"


struct IOItem
{

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

struct IOReader : public IOItem
{
    IOReader(const VarjusString& filename, bool in_binary_mode) : IOItem(filename, in_binary_mode) {}

    [[nodiscard]] virtual std::optional<VarjusString> IO_Read(EncodingType type = e_unknown) const;

private:
    [[nodiscard]] VarjusString IO_ReadStream(std::ifstream& stream, EncodingType type=e_unknown) const;

};

#ifdef _WIN32
#define DIRECTORY_SEPARATOR VSL("\\")
#define DIRECTORY_SEPARATOR_CHAR VarjusChar('\\')
#else
#define DIRECTORY_SEPARATOR VSL("/")
#define DIRECTORY_SEPARATOR_CHAR VarjusChar('/')
#endif

//these don't belong here but whatever
#define VARJUS_DIRECTORY() (fs::exe_path())

struct VarjusIOReader : public IOReader
{
    VarjusIOReader(const VarjusString& relative_path, bool binary = false);
};