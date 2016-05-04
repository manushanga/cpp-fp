#include <parser.h>
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
    int state = 0;
    while (i < to)
    {
        switch (state) {
        case 0:
            if (m_tokens[i].type == T_WORD)
                state = 1;
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
                        std::cout<<"enum"<<std::endl;
                    }
                    break;
                }
                case S_CLASS:
                {
                    int ret = parseClass(from, i+1);
                    if (ret > 0)
                    {}
                    break;
                }
                default:
                    break;
                }

                m_tstack.pop_back();
                m_sstack.pop_back();

            }
            break;
        }
        default:
        {
            bool found=false;
            if (m_tokens[i].type == T_WORD &&
                m_tokens[i+1].type == T_WORD)
            {
                const Token& tc = m_tokens[i];
                const Token& tp = m_tokens[i+1];
                for (int x=0;x<sizeof(blocks)/sizeof(void*);x++)
                {
                    if (tc.text == blocks[x])
                    {
                        found = true;
                        Scope scope;
                        switch (x)
                        {
                        case 0:
                            scope.type = S_CLASS;
                            scope.text = tp.text;
                            m_sstack.push_back(scope);

                            m_tstack.push_back(i);
                            break;
                        case 1:
                            scope.type = S_NAMESPACE;
                            scope.text = tp.text;
                            m_sstack.push_back(scope);

                            m_tstack.push_back(i);
                            break;
                        case 2:
                            scope.type = S_ENUM;
                            scope.text = tp.text;
                            m_sstack.push_back(scope);

                            m_tstack.push_back(i);
                            break;
                        case 3:
                            scope.type = S_STRUCT;
                            scope.text = tp.text;
                            m_sstack.push_back(scope);

                            m_tstack.push_back(i);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }

            if (m_sstack.empty() && found == false)
            {
                int ret;
                if ((ret = parseFuncDecl(i, m_tokens.size())) > 0)
                {
                    i = ret -1;
                    std::cout<<"funcdecl"<<std::endl;
                } else if ((ret = parseFunc(i, m_tokens.size())) > 0 )
                {
                    i = ret -1;
                    std::cout<<"func"<<std::endl;
                }

            }
            break;
        }
        }
        i++;
    }
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
