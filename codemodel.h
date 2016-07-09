#pragma once

#include <string>
#include <vector>
#include <map>

enum NodeType
{
    SID_FILE=0, SID_NAMESPACE, SID_CLASS, SID_STRUCT, SID_FUNCTION, SID_MEMBER,
    SID_CONSTRUCTOR, SID_DESTRUCTOR, SID_OPERATOR, SID_ENUM, SID_UNKNOWN
};

static const char *ScopeTypeNames[] =
{
    "File", "Namespace", "Class", "Struct", "Function", "Member",
    "Constructor", "Destructor", "Operator", "Enum", "Unknown"
};

class Node
{
public:
    Node(const std::string& name, NodeType type);
    const std::string &getName() const;
    void addData(const std::string& data);
    void addChildScope(const Node& scopeNode);
    NodeType getType();
    void print(std::string& output) const;
private:
    std::map<std::string, const Node*> m_children;
    std::string m_name;
    std::vector<std::string> m_data;
    NodeType m_type;
};
