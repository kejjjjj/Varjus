#pragma once

#include "manager.hpp"

#include <unordered_map>

class CMemory;

struct CLinterFunction final : public CMemoryIdentifier
{
    CLinterFunction() = default;
    CLinterFunction(const std::string& name, std::size_t index)
        : CMemoryIdentifier(name, index) {
    }

    [[nodiscard]] virtual constexpr EMemoryIdentifierType Type() const noexcept override { return mi_function; }
};

class CFunctionManager
{
    NONCOPYABLE(CFunctionManager);
    friend class CMemory;

public:
    CFunctionManager(CMemory* const m_pOwner);

    [[maybe_unused]] CLinterFunction* DeclareFunction(const std::string& var, std::size_t index);
    [[nodiscard]] CLinterFunction* GetFunction(const std::string& var);
    [[nodiscard]] bool ContainsFunction(const std::string& name) const;
    [[nodiscard]] bool ContainsFunctionGlobally(const std::string& name) const;
    [[nodiscard]] std::size_t GetFunctionCount() const noexcept;

private:
    std::unordered_map<std::string, CLinterFunction> m_oFunctions;
    CMemory* const m_pOwner;
};
