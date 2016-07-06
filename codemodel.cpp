#include "codemodel.h"


void ScopeNode::addData(const std::string &data)
{
    m_data.push_back(data);
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


void ScopeNode::addChildScope(const ScopeNode &scopeNode)
{
    m_children.insert(std::make_pair(scopeNode.getName(), &scopeNode));
}

ScopeType ScopeNode::getType()
{
    return m_type;
}

void ScopeNode::print(std::string &output) const
{
    output += "name: ";
    output += m_name;
    output += "\n";
    output += "type: ";
    output += ScopeTypeNames[m_type];
    output += "\n";

    if (!m_children.empty())
    {
        output += "children:\n";
        output += "{\n";
        for (auto& child : m_children)
        {
            child.second->print(output);
        }
        output += "}\n";
    }

    if (!m_data.empty())
    {
        output += "data:";
        output += "{";
        for (auto& data : m_data)
        {
            output += data + " ";
        }
        output += "}\n";
    }
}
