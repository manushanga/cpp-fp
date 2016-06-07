#pragma once
#include <string>
#include <vector>
#include <regex>

#include "codemodel.h"

class Parser
{
private:
    enum ScopeType
    {
        S_CLASS,
        S_STRUCT,
        S_NAMESPACE,
        S_ENUM,
        S_NONE
    };
    enum TokenType
    {
        T_WORD,
        T_LITERAL_QUOTE,
        T_MUL,
        T_DIV,
        T_PLUS,
        T_SUB,
        T_AMP,
        T_ASSIGN,
        T_GT,
        T_LT,
        T_COLON,
        T_SEPARATOR,
        T_BLOCK_START,
        T_BLOCK_END,
        T_BRACKET_OPEN,
        T_BRACKET_CLOSE,
        T_STATEMENT_END,
        T_SHARP,
        T_NONE
    };
    struct Scope
    {
        ScopeType type;
        std::string text;
        Scope()
        {
            reset();
        }
        void reset()
        {
            type = S_NONE;
            text = "";
        }
        bool isSet()
        {
            return type != S_NONE;
        }
    };
    struct Token
    {
        TokenType type;
        std::string text;

        void reset()
        {
            type = T_NONE;
            text = "";
        }
        bool isSet()
        {
            return type != T_NONE;
        }
    };

    TokenType m_map[256];
    std::regex m_re_basic_type;
    std::regex m_re_variable;
    std::vector<Parser::Token> m_tokens;
    std::vector<int> m_tstack;
    std::vector<Scope> m_sstack;
    ScopeNode *m_scopeRoot;
    std::stack<ScopeNode *> m_scopeNodes;

public:
    Parser();

    int parseVar(int from);
    int parseType(int from, int to);
    int parseFunc(int from, int to);
    int parseFuncDecl(int from, int to);
    int parseArgList(int from, int to);
    int parseEnum(int from, int to);
    int parseClass(int from, int to);
    int parseMember(int from, int to);
    int parseConstructor(int from, int to);
    int parseDestructor(int from, int to);
    void tokenize(const std::string& cpp);
    void parse();
    void print();
    void printScope();

private:
    int parseFuncSignature(int from, int to);
};
