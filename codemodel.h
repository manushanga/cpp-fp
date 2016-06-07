#pragma once

#include <string>
#include <vector>
#include <map>

enum NodeType { TID_FUNCTION=0, TID_MEMBER, TID_ENUM };
enum ScopeType { SID_FILE=0, SID_NAMESPACE, SID_CLASS, SID_STRUCT, SID_UNKNOWN };

static const char *ScopeTypeNames[] = {"File", "Namespace", "Class", "Struct", "Unknown"};
static const char *NodeTypeNames[] = {"Function", "Member", "Enum"};

class Node
{
public:
    Node(const std::string& name, NodeType type);
    const std::string& getName() const;
    void addData(const std::string& data);
    void print(std::string& output) const;
private:
    NodeType m_type;
    std::string m_name;
    std::vector<std::string> m_data;
};


class ScopeNode
{
public:
    ScopeNode(const std::string& name, ScopeType type);
    const std::string &getName() const;
    void addNode(const Node& node);
    void addChildScope(const ScopeNode& scopeNode);
    void print(std::string& output) const;
private:
    std::map<std::string, const ScopeNode*> m_children;
    std::map<std::string, const Node*> m_nodes;
    std::string m_name;
    ScopeType m_type;
};
