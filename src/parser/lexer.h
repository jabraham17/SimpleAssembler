//
// Created by Jacob Abraham on 3/8/21.
//

#ifndef __SIMPLEASSEMBLER_LEXER_H__
#define __SIMPLEASSEMBLER_LEXER_H__


#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>


#define LEXER_TOKENS(F) \
    F(NUM)              \
    F(HEXNUM)              \
    F(BINNUM)           \
    F(ID)               \
    F(DOT)              \
    F(COLON)            \
    F(LBRACK)           \
    F(RBRACK)           \
    F(NEWLINE)

enum class TokenType: uint {

    END_OF_FILE,
#define TOKEN(t) t,
    LEXER_TOKENS(TOKEN)
#undef TOKEN
        NONE,
        ERROR
};

class Token {
public:
    std::string lexeme;
    TokenType token_type;
    int line_no;

    Token(std::string lexeme, TokenType token_type, int line_no) : lexeme(std::move(lexeme)), token_type(token_type),
                                                                   line_no(line_no) {}

    Token() : lexeme(), token_type(TokenType::ERROR), line_no(-1) {}

    static const std::string types[];

    friend std::ostream& operator<<(std::ostream& out, const Token& t) {
        out << t.getString();
        return out;
    }

    [[nodiscard]] std::string getString() const {
        std::stringstream str;
        str << "{" << Token::types[(uint)token_type] << ",";
        if(!lexeme.empty()) str << lexeme << ",";
        str << line_no << "}";
        return str.str();
    }
    std::string getString() {
        return const_cast<const Token*>(this)->getString();
    }

    static std::string getTokenType(const Token& t) { return Token::types[(uint)t.token_type]; }

    static std::string getTokenType(const TokenType& tt) { return Token::types[(uint)tt]; }
};

class Lexer {
public:
    Token getToken();

    Token peek();

    TokenType ungetToken(const Token&);

    explicit Lexer(std::istream& input) : input(input), line_number(1), tokens(), input_buffer() {}

    Lexer(const Lexer& old) = default;

private:
    std::istream& input;
    int line_number;
    std::vector<Token> tokens;
    std::vector<char> input_buffer;


    //get ID
    Token getID();

    bool isIDStart();

    bool isIDBody();

    //returns NUM
    Token getNUM();
    bool isNUM();
    bool isDigit();
    bool isHexDigit();
    bool isBinaryDigit();
    bool isHexStart();
    bool isBinaryStart();

    //get symbols
    Token getSymbol();

    bool isSymbol();


    //skip stuff
    bool isComment();

    //both of these return the newlines they find
    Token ignoreComment();
    std::vector<Token> skipWhiteSpace();

    //input buffer code
    bool endOfInput();

    char peekChar();

    void ungetChar(char);

    char getChar();
};

#endif//__SIMPLEASSEMBLER_LEXER_H__
