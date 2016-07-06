#pragma once
#include <string>
#include <vector>
#include <regex>
#include <tuple>

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
        int index;
        int ttop;
        Scope()
        {
            reset();
        }
        void reset()
        {
            type = S_NONE;
            index = 0;
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

    ScopeNode *m_scopeRoot;
    std::vector<std::tuple<int, ScopeNode *>> m_scopeNodes;

public:
    Parser();
    int parseBlock(int from, int to);
    int parseVar(int from);
    int parseType(int from, int to);
    int parseFunc(int from, int to);
    int parseArgList(int from, int to, std::vector<std::string> &args);
    int parseEnum(int from, int to);
    int parseClass(int from, int to);
    int parseClassName(int from, int to);
    int parseMemberFunc(int from, int to);
    int parseMember(int from, int to);
    int parseConstructor(int from, int to);
    int parseDestructor(int from, int to);
    int parseClassBootstrap(int from, int to);
    int parseEnumBootstrap(int from, int to, ScopeNode* scopeNode);
    int parseIdentifierName(int from, int to);
    void tokenize(const char* cpp, int len);
    void tokenize(const std::string& cpp);
    void parse();
    void print();
    void printScope();

    std::string toString(int from, int to);

private:
    int parseFuncSignature(int from, int to, std::string& retType, std::string& funcName, std::vector<std::string>& args);

    ScopeNode *getCurrentScope();
    bool isCurrentScopeNull();
    int getCurrentTokenIndex();
};
