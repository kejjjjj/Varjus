#include "error.hpp"

void CLinterErrors::PushError(const CLinterError& error) {
    errorStack.push_back(error);
    throw errorStack.back();

}
void CLinterErrors::PushError(const std::string& error, const CodePosition& position) {
    errorStack.emplace_back(CLinterError(error, position));
    throw errorStack.back();
}
void CLinterErrors::PushError(const std::string& error) {
    errorStack.emplace_back(CLinterError(error));
    throw errorStack.back();
}
void CLinterErrors::ClearErrorStack() 
{ 
    errorStack.clear(); 
}
const auto& CLinterErrors::GetErrorStack() 
{
    return errorStack; 
}

std::vector<CLinterError> CLinterErrors::errorStack;
