#pragma once

#include <fstream>
#include <limits>
#include <optional>

struct IOItem
{

    IOItem(const std::string& filename, bool in_binary_mode) : m_bBinary(in_binary_mode), m_sFileName(filename)
    {
    }
    virtual ~IOItem() = default;

    constexpr std::string get_target() { return m_sFileName; }

protected:
    bool m_bBinary = false;
    bool m_bErrorOccurred = false;
    std::string m_sFileName;
};

struct IOWriter : public IOItem
{
    IOWriter(const std::string& filename, bool in_binary_mode) : IOItem(filename, in_binary_mode)
    {
        m_bErrorOccurred = !CreateMissingDirectoriesFromPath(filename);
    }

    virtual bool IO_Write(const std::string& content) const;
    virtual bool IO_Append(const std::string& content) const;

private:
    bool CreateMissingDirectoriesFromPath(std::string path) const;
    void IO_WriteStream(std::ofstream& stream, const std::string& content) const {
        stream.write(content.data(), std::streamsize(content.size()));
    }
};

struct IOReader : public IOItem
{
    IOReader(const std::string& filename, bool in_binary_mode) : IOItem(filename, in_binary_mode) {}

    virtual std::optional<std::string> IO_Read(/*size_t num_bytes = std::numeric_limits<size_t>::max()*/) const;

private:
    std::string IO_ReadStream(std::ifstream& stream) const;

};

//these don't belong here but whatever
#define VARJUS_DIRECTORY() (fs::exe_path())

struct VarjusIOWriter : public IOWriter
{
    VarjusIOWriter(const std::string& relative_path, bool binary = false);
};
struct VarjusIOReader : public IOReader
{
    VarjusIOReader(const std::string& relative_path, bool binary = false);
};