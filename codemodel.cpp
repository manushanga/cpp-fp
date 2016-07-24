#include <stack>
#include <iostream>
#include "codemodel.h"

void Node::addData(const std::string &data)
{
    m_data.push_back(data);
}

Node::Node(const std::string &name, NodeType type) :
    m_parent(nullptr),
    m_name(name),
    m_type(type)
{

}

const std::string &Node::getName() const
{
    return m_name;
}


void Node::addChildScope(const Node &scopeNode)
{
    m_children.insert(std::make_pair(scopeNode.getName(), &scopeNode));
}

NodeType Node::getType()
{
    return m_type;
}

NodeList Node::find(const std::string &name, int limit)
{
    NodeList nlist;
    std::stack<const Node*> st;
    st.push(this);
    while (!st.empty())
    {
        const Node* n = st.top();
        st.pop();

        for (auto& ch : n->m_children)
        {
            if (limit < nlist.size())
            {
                break;
            }
            else
            {
                st.push(ch.second);
            }
            if (ch.second->m_name.find(name) != std::string::npos)
            {
                nlist.push_back(ch.second);
            }

        }
        if (limit < nlist.size())
        {
            break;
        }
    }
    return nlist;
}

void Node::print(std::string &output) const
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

std::string Node::printToStr() const
{
    std::string str;
    print(str);
    return str;
}

Node::~Node()
{
    for (auto& ch : m_children)
    {
        delete ch.second;
    }
}
