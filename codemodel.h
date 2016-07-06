#pragma once

#include <string>
#include <vector>
#include <map>

enum ScopeType
{
    SID_FILE=0, SID_NAMESPACE, SID_CLASS, SID_STRUCT, SID_FUNCTION, SID_MEMBER,
    SID_CONSTRUCTOR, SID_DESTRUCTOR, SID_OPERATOR, SID_ENUM, SID_UNKNOWN
};

static const char *ScopeTypeNames[] =
{
    "File", "Namespace", "Class", "Struct", "Function", "Member",
    "Constructor", "Destructor", "Operator", "Enum", "Unknown"
};

class ScopeNode
{
public:
    ScopeNode(const std::string& name, ScopeType type);
    const std::string &getName() const;
    void addData(const std::string& data);
    void addChildScope(const ScopeNode& scopeNode);
    ScopeType getType();
    void print(std::string& output) const;
private:
    std::map<std::string, const ScopeNode*> m_children;
    std::string m_name;
    std::vector<std::string> m_data;
    ScopeType m_type;
};
