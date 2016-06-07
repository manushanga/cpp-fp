#include "codemodel.h"

Node::Node(const std::string& name, NodeType type) :
    m_name(name),
    m_type(type)
{

}

const std::string &Node::getName() const
{
    return m_name;
}

void Node::addData(const std::string &data)
{
    m_data.push_back(data);
}

void Node::print(std::string &output) const
{
    output += "name: ";
    output += m_name;
    output += "\n";
    output += "type: ";
    output += NodeTypeNames[m_type];
    output += "\n";
    output += "data: [";
    for (auto& data : m_data)
    {
        output += data;
        output += " ";
    }
    output += "]\n";
}

ScopeNode::ScopeNode(const std::string &name, ScopeType type) :
    m_name(name),
    m_type(type)
{

}

const std::string &ScopeNode::getName() const
{
    return m_name;
}

void ScopeNode::addNode(const Node &node)
{
    m_nodes.insert(std::make_pair(node.getName(), &node));
}

void ScopeNode::addChildScope(const ScopeNode &scopeNode)
{
    m_children.insert(std::make_pair(scopeNode.getName(), &scopeNode));
}

void ScopeNode::print(std::string &output) const
{
    output += "name: ";
    output += m_name;
    output += "\n";
    output += "type: ";
    output += ScopeTypeNames[m_type];
    output += "\n";
    output += "nodes: \n";
    output += "[\n";
    for (auto& node : m_nodes)
    {
        node.second->print(output);
    }
    output += "]\n";
    output += "children:\n";
    output += "{\n";
    for (auto& child : m_children)
    {
        child.second->print(output);
    }
    output += "}\n";

}
