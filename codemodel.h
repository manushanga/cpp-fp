#pragma once

#include <string>
#include <vector>

// typenames without default args for templates or spaces
// int, F<A,G>
class Type
{
public:
    Type(const std::string& typeName);
    const std::string& getTypeName();
private:
    std::string m_typename;
};


class TranslationUnit
{
public:
private:
    std::vector<std::string> m_includes;


};

class CodeModel
{
public:
    std::map<std::string, TranslationUnit*> m_units;
};
