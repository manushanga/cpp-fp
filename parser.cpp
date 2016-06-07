#include "codemodel.h"
#include "parser.h"

#include <vector>
#include <iostream>
#include <stack>
#include <regex>

#define LEN_CHK(__minlen, __ret) \
    if (m_tokens.size() < __minlen) \
        return __ret;
// data type
// [a-zA-Z_0-9><]+

// func text
// [a-zA-Z_0-9]+

// variable text
// [a-zA-Z_0-9]+

Parser::Parser()
{
    m_map['('] = T_BRACKET_OPEN;
    m_map['"'] = T_LITERAL_QUOTE;
    m_map[')'] = T_BRACKET_CLOSE;
    m_map['*'] = T_MUL;
    m_map['&'] = T_AMP;
    m_map['/'] = T_DIV;
    m_map['+'] = T_PLUS;
    m_map['-'] = T_SUB;
    m_map['='] = T_ASSIGN;
    m_map['{'] = T_BLOCK_START;
    m_map['}'] = T_BLOCK_END;
    m_map[','] = T_SEPARATOR;
    m_map[';'] = T_STATEMENT_END;
    m_map['>'] = T_GT;
    m_map['<'] = T_LT;
    m_map[':'] = T_COLON;
    m_map['#'] = T_SHARP;
    m_re_basic_type = "[a-zA-Z0-9_]*";
    m_re_variable = "[a-zA-Z_0-9]+";
}

int Parser::parseVar(int from)
{
    LEN_CHK(from,-1);

    if (m_tokens[from].type != T_WORD)
    {
        return -1;
    }
    if (std::regex_match(m_tokens[from].text, m_re_variable))
    {
        return from + 1;
    } else
    {
        return -1;
    }
}

int Parser::parseType(int from, int to)
{
    LEN_CHK(from,-1)
    if (m_tokens[from].text == "static" )
    {
        from+=1;
    }
    LEN_CHK(from,-1)
    if (m_tokens[from].text == "const")
    {
        from+=1;
    }

    const char* signedness[] = {"signed","unsigned"};
    for (int i=0;i<sizeof(signedness)/sizeof(void *);i++)
    {
        LEN_CHK(from,-1)
        if (m_tokens[from].text == signedness[i])
        {
            from+=1;
            break;
        }
    }

    LEN_CHK(from,-1)
    if (m_tokens[from].text == "long")
    {
        from+=1;
    }

    if (m_tokens[from].type != T_WORD)
        return -1;

    LEN_CHK(from+1,-1)
    if (m_tokens[from+1].type == T_MUL || m_tokens[from+1].type == T_AMP)
    {
        from+=1;
    }

    LEN_CHK(from+1,-1)
    if (m_tokens[from+1].type != T_LT)
        return from +1;

    int i=from +1;
    int err=0;
    int scope=0;
    while (i<to && err == 0)
    {
        const Token& t = m_tokens[i];
        switch (t.type)
        {
        case T_LT:
            scope++;
            break;
        case T_GT:
            scope--;

            if (scope == 0)
            {
                LEN_CHK(i+1,-1)
                if (m_tokens[i+1].type == T_MUL || m_tokens[i+1].type == T_AMP)
                {
                    i+=1;
                }
                return i + 1;
            }

            break;
        case T_SEPARATOR:
        case T_AMP:
        case T_MUL:
        case T_WORD:
            break;
        default:
            err = 1;
            break;
        }
        i++;
    }

    return -1;
}

int Parser::parseFuncDecl(int from, int to)
{
    int ret = parseFuncSignature(from, to);
    if (ret > 0)
    {
        if (m_tokens[ret].type == T_STATEMENT_END)
        {
            return ret;
        }
    }
    return -1;
}

int Parser::parseFunc(int from, int to)
{
    int ret = parseFuncSignature(from, to);
    if (ret > -1)
    {
        if (m_tokens[ret].type != T_BLOCK_START)
            return -1;

        int i = ret;
        std::stack<int> blocks;
        while (i<to)
        {
            switch (m_tokens[i].type)
            {
            case T_BLOCK_START:
                blocks.push(i);
                break;
            case T_BLOCK_END:
                blocks.pop();
                if (blocks.empty())
                {
                    return i+1;
                }
                break;
            default:
                break;
            }
            i++;
        }
    }
    return -1;
}

int Parser::parseArgList(int from, int to)
{

}

int Parser::parseEnum(int from, int to)
{
    if (m_tokens[from].text != "enum")
        return -1;

    if (m_tokens[from+1].text == "class")
        from++;

    int i=from+3;

    if (m_tokens[from+1].type != T_WORD) // must be enum name
        return -1;

    auto node = new Node(m_tokens[from+1].text, TID_ENUM);

    m_scopeNodes.top()->addNode(*node);

    if (m_tokens[from+2].type == T_COLON) // integral data type specfication
    {
        i = parseType(from+3, to);
        if (i == -1)
            return -1;
        else
            i++;
    }
    int state = 0;
    while (i < to)
    {
        switch (state) {
        case 0:
            if (m_tokens[i].type == T_WORD)
            {
                node->addData(m_tokens[i].text);
                state = 1;
            }
            else
                return -1;
            break;
        case 1:
            if (m_tokens[i].type == T_ASSIGN)
                state = 2;
            else if (m_tokens[i].type == T_SEPARATOR)
                state = 0;
            else
                return -1;
            break;
        case 2:
            if (m_tokens[i].type == T_WORD)
                state = 3;
            break;
        case 3:
            if (m_tokens[i].type == T_SEPARATOR)
                state = 0;
            else
                return -1;
            break;
        default:
            break;
        }
        i++;
    }
    return i;
}

int Parser::parseClass(int from, int to)
{
    if (m_tokens[from].text != "class")
        return -1;

    int i = from + 1;
    i = parseType(i, to);

    if (i == -1)
        return -1;

    if (m_tokens[i].type == T_COLON)
    {
        i++;
        while (i<to)
        {
            if (m_tokens[i].text == "protected" ||
                m_tokens[i].text == "public" ||
                m_tokens[i].text == "private" ||
                m_tokens[i].text == "virtual")
            {
                i = parseType(i+1, to);
                if (i == -1)
                {
                    return -1;
                }
                if (m_tokens[i].type == T_BLOCK_START)
                    break;
                else if (m_tokens[i].type != T_SEPARATOR)
                    return -1;
            }
            i++;
        }
    }
    i+=1;
    while(i<to)
    {
        if (m_tokens[i].type == T_WORD)
        {
            if (m_tokens[i].text == "protected:" ||
                m_tokens[i].text == "public:" ||
                m_tokens[i].text == "private:")
            {
                i+=2;
            } else if (m_tokens[i].text == "class" ||
                       m_tokens[i].text == "enum" ||
                       m_tokens[i].text == "struct")
            {
                while (i<to && m_tokens[i].type != T_BLOCK_START)
                {
                    i++;
                }
                std::stack<int> blocks;
                bool stop =false;
                while (i<to && !stop)
                {
                    switch (m_tokens[i].type)
                    {
                    case T_BLOCK_START:
                        blocks.push(i);
                        break;
                    case T_BLOCK_END:
                        blocks.pop();
                        if (blocks.empty())
                        {
                            stop = true;
                        }
                        break;
                    default:
                        break;
                    }
                    i++;
                }
            }
        }
        int ret = parseFuncDecl(i,to);
        if (ret > 0)
        {
            std::cout<<"memx"<<m_tokens[i].text<<std::endl;
            printScope();
            i = ret;
        }
        else if ((ret = parseMember(i, to)) > 0)
        {
            std::cout<<"memf"<<m_tokens[i].text<<std::endl;
            printScope();
            i = ret;
        }
        else if ((ret = parseFunc(i,to)) > 0)
        {
            std::cout<<"memfxx"<<m_tokens[i].text<<std::endl;
            printScope();
            i = ret;
        }
        else
        {
            i++;
        }
    }
    return to;
}

int Parser::parseMember(int from, int to)
{
    int x = parseType(from, to);
    if (x >= 0)
    {
        LEN_CHK(x+1,-1)
        if (m_tokens[x].type == T_WORD && m_tokens[x+1].type == T_STATEMENT_END)
            return x+2;
    }
    return -1;
}

void Parser::tokenize(const std::string& cpp)
{
    int i=0;
    TokenType state = T_NONE;
    Token current;
    while (i<cpp.size())
    {
        char c = cpp[i];
        switch (c)
        {
        case '\r':
        case '\t':
        case '\n':
        case ' ':
            if (current.isSet())
                m_tokens.push_back(current);
            current.reset();
            break;
        case ',':
        case '(':
        case ')':
        case '{':
        case '}':
        case '*':
        case ':':
        case ';':
        case '&':
        case '=':
        case '<':
        case '>':
        case '#':
            if (current.isSet())
                m_tokens.push_back(current);
            current.text = c;
            current.type = m_map[c];
            m_tokens.push_back(current);
            current.reset();
            break;
        default:
            current.text += c;
            current.type = T_WORD;
            break;

        }
        i++;
    }
}

void Parser::parse()
{
    m_scopeRoot = new ScopeNode("", SID_FILE);
    m_scopeNodes.push(m_scopeRoot);

    const char* blocks[] = {"class","namespace", "enum", "struct"};

    int i=0;
    while (i<m_tokens.size())
    {
        const Token& t = m_tokens[i];
        switch (t.type)
        {
        case T_SHARP:
        {
            std::cout<<"ss"<<std::endl;
            break;
        }
        case T_BLOCK_START:
            break;
        case T_BRACKET_OPEN:
            break;
        case T_BRACKET_CLOSE:
            break;
        case T_BLOCK_END:
        {
            if (!m_sstack.empty())
            {
                int from = m_tstack.back();
                Scope scope = m_sstack.back();
                switch (scope.type)
                {
                case S_ENUM:
                {
                    int ret = parseEnum(from, i+1);
                    if (ret > 0)
                    {
                        i = ret;
                    }
                    break;
                }
                case S_CLASS:
                {
                    int ret = parseClass(from, i+1);
                    if (ret > 0)
                    {
                        i = ret;
                    }
                    break;
                }
                default:
                    break;
                }

                m_tstack.pop_back();
                m_sstack.pop_back();
                if (m_scopeNodes.size()>1)
                    m_scopeNodes.pop();

            }
            break;
        }
        default:
        {
            bool found=false;
            if (m_tokens[i].type == T_WORD )
            {
                Scope scope;
                if (m_tokens[i].text == "namespace" && m_tokens[i+1].type == T_WORD) // namespace with a name
                {
                    scope.type = S_NAMESPACE;
                    scope.text = m_tokens[i+1].text;
                    m_sstack.push_back(scope);
                    m_tstack.push_back(i);

                    auto nodePtr = new ScopeNode(m_tokens[i+1].text, SID_NAMESPACE);
                    m_scopeNodes.top()->addChildScope(*nodePtr);
                    m_scopeNodes.push(nodePtr);
                    found = true;
                }
                else if (m_tokens[i].text == "namespace" && m_tokens[i+1].type == T_BLOCK_START) // namspace without a name
                {
                    scope.type = S_NAMESPACE;
                    scope.text = "$ANONYMOUS";
                    m_sstack.push_back(scope);
                    m_tstack.push_back(i);

                    auto nodePtr = new ScopeNode(scope.text, SID_NAMESPACE);
                    m_scopeNodes.top()->addChildScope(*nodePtr);
                    m_scopeNodes.push(nodePtr);
                    found = true;
                }
                else if (m_tokens[i].text == "class" && m_tokens[i+1].type == T_WORD)
                {
                    scope.type = S_CLASS;
                    scope.text = m_tokens[i+1].text;
                    m_sstack.push_back(scope);
                    m_tstack.push_back(i);

                    auto nodePtr = new ScopeNode(m_tokens[i+1].text, SID_CLASS);
                    m_scopeNodes.top()->addChildScope(*nodePtr);
                    m_scopeNodes.push(nodePtr);
                    found = true;

                }
                else if (m_tokens[i].text == "struct" && m_tokens[i+1].type == T_WORD)
                {
                    scope.type = S_STRUCT;
                    scope.text = m_tokens[i+1].text;
                    m_sstack.push_back(scope);
                    m_tstack.push_back(i);

                    auto nodePtr = new ScopeNode(m_tokens[i+1].text, SID_STRUCT);
                    m_scopeNodes.top()->addChildScope(*nodePtr);
                    m_scopeNodes.push(nodePtr);
                    found = true;
                }
                else if (m_tokens[i].text == "enum" && m_tokens[i+1].type == T_WORD)
                {
                    scope.type = S_ENUM;
                    scope.text = m_tokens[i+1].text;
                    m_sstack.push_back(scope);

                    m_tstack.push_back(i);
                    found = true;
                }
            }


            if (m_sstack.empty() && found == false)
            {
                int ret;
                if ((ret = parseFuncDecl(i, m_tokens.size())) > 0)
                {
                    i = ret -1;
                } else if ((ret = parseFunc(i, m_tokens.size())) > 0 )
                {
                    i = ret -1;
                }

            }
            break;
        }
        }
        i++;
    }
}

void Parser::print()
{
    std::string str;
    m_scopeRoot->print(str);
    std::cout<<str<<std::endl;
}

void Parser::printScope()
{
    for (Scope& s: m_sstack)
    {
        std::cout << s.text << "::" ;
    }
    std::cout << std::endl;

}

int Parser::parseFuncSignature(int from, int to)
{
    int ret = parseType(from, to);

    if (ret == -1)
        return -1;
    if (m_tokens[ret].type != T_WORD)
        return -1;

    if (m_tokens[ret + 1].type != T_BRACKET_OPEN)
        return -1;

    int i=ret + 2;

    // 0 - arg type
    // 1 - arg name

    int state=0;
    while (i < to)
    {
        int x;
        const Token& t = m_tokens[i] ;
        switch(state)
        {
        case 0:
            LEN_CHK(i+1,-1)
            if (t.type == T_BRACKET_CLOSE )
            {
                return i+1;
            }
            x = parseType(i, to);
            if (x == -1)
            {
                return -1;
            } else
            {
                i = x;
                state = 1;
            }
            break;
        case 1:
            x = parseVar(i);
            if (x==-1)
            {
                return -1;
            } else
            {
                i = x;
                state = 2;
            }
            break;
        case 2:
            LEN_CHK(i+1,-1)
            if (m_tokens[i].type == T_BRACKET_CLOSE)
            {
                return i+1;
            } else if (m_tokens[i].type == T_SEPARATOR)
            {
                state = 0;
                i++;
            } else
            {
                return -1;
            }
            break;
        default:
            break;
        }
    }
    return -1;
}
