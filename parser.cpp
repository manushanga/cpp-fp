#include "codemodel.h"
#include "parser.h"

#include <vector>
#include <iostream>
#include <stack>
#include <regex>
#include <cassert>

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

int Parser::parseBlock(int from, int to)
{
    int i = from;
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
    return -1;
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


int Parser::parseClassName(int from, int to)
{
    if (m_tokens[from].type != T_WORD)
        return -1;

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

    int ret = parseIdentifierName(from, to);

    if (ret == -1)
        return -1;


    from = ret;
    LEN_CHK(from,-1)

    if (m_tokens[from].type == T_MUL || m_tokens[from].type == T_AMP)
    {
        from+=1;
    }

    LEN_CHK(from,-1)
    if (m_tokens[from].type != T_LT)
        return from ;

    int i=from ;
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
        case T_COLON:
            break;
        default:
            err = 1;
            break;
        }
        i++;
    }

    return -1;
}

int Parser::parseFunc(int from, int to)
{
    std::string retType, funcName;
    std::vector<std::string> args;
    int ret = parseFuncSignature(from, to, retType, funcName, args);
    if (ret > -1)
    {
        // function decl
        if (m_tokens[ret].type == T_STATEMENT_END)
        {
            ScopeNode *node = new ScopeNode(funcName, SID_FUNCTION);
            node->addData(retType);
            node->addData(funcName);
            for (auto& arg : args)
            {
                node->addData(arg);
            }
            std::cout<<"xx"<<getCurrentScope()->getName()<<std::endl;
            getCurrentScope()->addChildScope(*node);

            return ret + 1;
        }
        else if (m_tokens[ret].type == T_BLOCK_START)
        {
            ScopeNode *node = new ScopeNode(funcName, SID_FUNCTION);
            node->addData(retType);
            node->addData(funcName);
            for (auto& arg : args)
            {
                node->addData(arg);
            }
            getCurrentScope()->addChildScope(*node);

            return parseBlock(ret, to);
        }

    }
    return -1;
}

int Parser::parseArgList(int from, int to, std::vector<std::string>& args)
{
    // 0 - arg type
    // 1 - arg name

    int state=0;
    int i=from;
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
                args.push_back(toString(i, x));
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
                args.push_back(toString(i, x));
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

int Parser::parseEnum(int from, int to)
{
    int i=from;
    int state = 0;
    while (i < to)
    {
        switch (state) {
        case 0:
            if (m_tokens[i].type == T_WORD)
            {
                getCurrentScope()->addData(m_tokens[i].text);
                state = 1;
            }
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
    int i = from;
    while(i<to)
    {
        if (m_tokens[i].type == T_WORD)
        {
            if (m_tokens[i].text == "protected" ||
                m_tokens[i].text == "public" ||
                m_tokens[i].text == "private")
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
        int ret = -1;
        std::cout<<"m---"<<m_tokens[i].text<<std::endl;
        if ((ret = parseMember(i, to)) > -1)
        {
            std::cout<<"memf"<<m_tokens[i].text<<std::endl;
            printScope();
            i = ret ;
        }
        else if ((ret = parseMemberFunc(i,to)) > -1)
        {
            std::cout<<"memfxx"<<m_tokens[i].text<<std::endl;
            printScope();
            i = ret ;
        }
        i++;
    }
    return -1;
}

int Parser::parseMemberFunc(int from, int to)
{
    int ret=-1;
    if ((ret = parseDestructor(from, to)) > -1)
    {
        return ret;
    }
    else if ((ret = parseConstructor(from, to)) > -1)
    {
        return ret;
    }
    else if ((ret = parseFunc(from, to)) > -1)
    {
        return ret;
    }
    return -1;
}

int Parser::parseMember(int from, int to)
{
    int x = parseType(from, to);
    if (x >= 0)
    {
        if (m_tokens[x].type == T_WORD && m_tokens[x+1].type == T_STATEMENT_END)
            return x+1;
    }
    return -1;
}

int Parser::parseConstructor(int from, int to)
{
    std::vector<std::string> args;

    if (m_tokens[from].type != T_WORD || m_tokens[from+1].type != T_BRACKET_OPEN)
        return -1;

    int ret = parseArgList(from + 2,to, args);

    if (ret == -1)
        return -1;

    if (m_tokens[ret].type == T_STATEMENT_END) // decl
    {
        ScopeNode *node = new ScopeNode(m_tokens[from].text, SID_CONSTRUCTOR);
        for (auto& arg : args)
        {
            node->addData(arg);
        }
        getCurrentScope()->addChildScope(*node);
        return ret;
    }
    else if (m_tokens[ret].type == T_BLOCK_START)
    {
        ScopeNode *node = new ScopeNode(m_tokens[from].text, SID_CONSTRUCTOR);
        for (auto& arg : args)
        {
            node->addData(arg);
        }
        getCurrentScope()->addChildScope(*node);
        int jj=parseBlock(ret,to);
        return jj;
    }
    else
    {
        return -1;
    }

}

int Parser::parseDestructor(int from, int to)
{
    std::vector<std::string> args;

    if (m_tokens[from].type != T_WORD || m_tokens[from+1].type != T_BRACKET_OPEN)
        return -1;

    if (m_tokens[from].text[0] != '~')
        return -1;

    int ret = parseArgList(from + 2,to, args);

    if (ret == -1)
        return -1;

    if (m_tokens[ret].type == T_STATEMENT_END) // decl
    {
        ScopeNode *node = new ScopeNode(m_tokens[from].text, SID_DESTRUCTOR);
        getCurrentScope()->addChildScope(*node);
        return ret + 1;
    }
    else if (m_tokens[ret].type == T_BLOCK_START)
    {
        ScopeNode *node = new ScopeNode(m_tokens[from].text, SID_DESTRUCTOR);
        getCurrentScope()->addChildScope(*node);
        return parseBlock(ret,to);
    }
    else
    {
        return -1;
    }
}

int Parser::parseClassBootstrap(int from, int to)
{
    if (m_tokens[from].type != T_WORD || m_tokens[from].text != "class" || m_tokens[from+1].type != T_WORD)
        return -1;

    int i = from + 1;
    i = parseClassName(i, to);

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
                std::cout<<"ooo "<<toString(i, to)<<std::endl;
                i = parseType(i+1, to);


                if (i == -1)
                {
                    return -1;
                }
                std::cout<<"ooo "<<toString(i, to)<<std::endl;
                if (m_tokens[i].type == T_BLOCK_START)
                    return i;
                else if (m_tokens[i].type != T_SEPARATOR)
                    return -1;

            }

            i++;
        }
    }
    return m_tokens[i].type == T_BLOCK_START ? i +1 : -1 ;
}

int Parser::parseEnumBootstrap(int from, int to, ScopeNode *scopeNode)
{
    if (m_tokens[from].text != "enum")
        return -1;

    if (m_tokens[from+1].text == "class")
        from++;

    int i=from+3;

    if (m_tokens[from+1].type != T_WORD) // must be enum name
        return -1;

    if (m_tokens[from+2].type == T_COLON) // integral data type specfication
    {
        i = parseType(from+3, to);
        if (i == -1)
            return -1;
        else
            return i+1;
    }

    return (m_tokens[from+2].type == T_BLOCK_START) ? from + 2 : -1;
}

int Parser::parseIdentifierName(int from, int to)
{
    int i =  from;
    int state =0;
    while (i<to)
    {
        switch (state)
        {
        case 0:
            if (m_tokens[i].type == T_WORD)
            {
                state = 1;
            } else if (m_tokens[i].type == T_COLON)
            {
                state = 2;
            } else
            {
                return -1;
            }
            break;
        case 1:
            if (m_tokens[i].type == T_COLON)
            {
                state = 2;
            }else
            {
                return i;
            }
            break;
        case 2:
            if (m_tokens[i].type == T_COLON)
            {
                state = 3;
            }else
            {
                return -1;
            }
            break;
        case 3:
            if (m_tokens[i].type == T_WORD)
            {
                state = 1;
            }else
            {
                return -1;
            }
            break;
        default:
            return -1;
        }

        i++;
    }

    return i;
}

void Parser::tokenize(const char *cpp, int len)
{
    int i=0;
    TokenType state = T_NONE;
    Token current;
    while (i<len)
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
        case '"':
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

void Parser::tokenize(const std::string& cpp)
{
    tokenize(cpp.c_str(), cpp.size());
}

void Parser::parse()
{
    m_scopeRoot = new ScopeNode("", SID_FILE);
    m_scopeNodes.push_back(std::make_tuple(0,m_scopeRoot));

    ScopeNode* currentNamedScope=nullptr;

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
            if (currentNamedScope)
            {
                std::cout<<currentNamedScope<<currentNamedScope->getName()<<std::endl;
                for (int i=m_scopeNodes.size()-1;i>=0;i--)
                {
                    if (!isCurrentScopeNull())
                    {
                        std::get<1>(m_scopeNodes[i])->addChildScope(*currentNamedScope);
                        break;
                    }
                }
            }

            std::cout<<"-------"<<toString(i, i+10)<< " "<< currentNamedScope<<std::endl;
            m_scopeNodes.push_back(std::make_tuple(i, currentNamedScope));
            currentNamedScope = nullptr;
            break;
        case T_BRACKET_OPEN:
            break;
        case T_BRACKET_CLOSE:
            break;
        case T_BLOCK_END:
        {
            int from = getCurrentTokenIndex();
            if (!isCurrentScopeNull())
            {
                std::cout<<"vv"<<getCurrentScope()->getName()<<std::endl;
                switch (getCurrentScope()->getType())
                {
                case SID_ENUM:
                {
                    int ret = parseEnum(from, i);
                    if (ret > 0)
                    {
                        //i = ret;
                    }
                    break;
                }
                case SID_CLASS:
                {
                    int ret = parseClass(from, i);
                    if (ret > 0)
                    {
                        //i = ret;
                    }
                    break;
                }
                default:
                    break;
                }
            }
            m_scopeNodes.pop_back();

            break;
        }
        default:
        {
            bool found=false;

            if (m_tokens[i].type == T_WORD )
            {
                if (m_tokens[i].text == "namespace" && m_tokens[i+1].type == T_WORD) // namespace with a name
                {
                    auto nodePtr = new ScopeNode(m_tokens[i+1].text, SID_NAMESPACE);
                    currentNamedScope = nodePtr;
                    found = true;
                }
                else if (m_tokens[i].text == "namespace" && m_tokens[i+1].type == T_BLOCK_START) // namspace without a name
                {

                    auto nodePtr = new ScopeNode("$ANONYMOUS", SID_NAMESPACE);
                    currentNamedScope = nodePtr;
                    found = true;
                }
                else if (m_tokens[i].text == "class" && m_tokens[i+1].type == T_WORD)
                {

                    auto nodePtr = new ScopeNode(m_tokens[i+1].text, SID_CLASS);
                    int ret = parseClassBootstrap(i, m_tokens.size());

                    if (ret != -1)
                    {
                        currentNamedScope = nodePtr;
                        found = true;
                    }

                }
                else if (m_tokens[i].text == "struct" && m_tokens[i+1].type == T_WORD)
                {
                    auto nodePtr = new ScopeNode(m_tokens[i+1].text, SID_STRUCT);
                    currentNamedScope = nodePtr;
                    found = true;
                }
                else if (m_tokens[i].text == "enum" && m_tokens[i+1].type == T_WORD)
                {
                    auto nodePtr = new ScopeNode(m_tokens[i+1].text, SID_ENUM);

                    int ret = parseEnumBootstrap(i, m_tokens.size(), nodePtr);

                    if (ret != -1)
                    {
                        currentNamedScope = nodePtr;
                        found = true;
                    }
                }
            }

/*
            if (m_sstack.empty() && found == false)
            {
                int ret;
                if ((ret = parseFunc(i, m_tokens.size())) > 0 )
                {
                    i = ret -1;
                }

            }*/
            break;
        }
        }
        i++;
    }

    assert(m_scopeNodes.size() == 1);
}

void Parser::print()
{
    std::string str;
    m_scopeRoot->print(str);
    std::cout<<str<<std::endl;
}

void Parser::printScope()
{


}

std::string Parser::toString(int from, int to)
{
    std::string ret;
    for (int i=from;i<to;i++)
    {
        ret += m_tokens[i].text;
    }
    return ret;
}

int Parser::parseFuncSignature(int from, int to, std::string& retType, std::string& funcName, std::vector<std::string>& args)
{
    int ret = parseType(from, to);

    if (ret == -1)
        return -1;
    if (m_tokens[ret].type != T_WORD)
        return -1;

    if (m_tokens[ret + 1].type != T_BRACKET_OPEN)
        return -1;

    retType = toString(from, ret);
    funcName = toString(ret, ret + 1);

    int i=ret + 2;

    return parseArgList(i, to, args);
}

ScopeNode *Parser::getCurrentScope()
{
    auto node = std::get<1>(m_scopeNodes.back());
    assert(node != nullptr);
    return node;
}

bool Parser::isCurrentScopeNull()
{
    return std::get<1>(m_scopeNodes.back()) == nullptr;
}

int Parser::getCurrentTokenIndex()
{
    return std::get<0>(m_scopeNodes.back());
}
