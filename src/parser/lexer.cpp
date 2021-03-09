//
// Created by Jacob Abraham on 3/8/21.
//

#include "lexer.h"

const std::string Token::types[] = {
    "END_OF_FILE",
#define TOKEN(t) #t,
    LEXER_TOKENS(TOKEN)
#undef TOKEN
        "NONE",
    "ERROR"};


Token Lexer::getToken() {
    Token t;
    std::vector<Token> newlines;
    //if we have tokens, get them,
    if(!tokens.empty()) {
        t = tokens.back();
        tokens.pop_back();
        return t;
    }

    //skip all spaces
    newlines = skipWhiteSpace();
    tokens.insert(tokens.end(), newlines.begin(), newlines.end());
    if(!tokens.empty()) {
        t = tokens.back();
        tokens.pop_back();
        return t;
    }

    //ignore comments
    while(isComment()) {
        t = ignoreComment();
        if(t.token_type != TokenType::NONE) return t;
        //after we ignore a comment, skip all whitespace
        newlines = skipWhiteSpace();
        tokens.insert(tokens.end(), newlines.begin(), newlines.end());
        if(!tokens.empty()) {
            t = tokens.back();
            tokens.pop_back();
            return t;
        }
    }

    //check for each element
    if(isIDStart()) {
        t = getID();
        if(t.token_type != TokenType::NONE) return t;
    }
    else if(isNUM()) {
        t = getNUM();
        if(t.token_type != TokenType::NONE) return t;
    }
    else if(isSymbol()) {
        t = getSymbol();
        if(t.token_type != TokenType::NONE) return t;
    }
    else {
        //if at this point we have not returned, its either eof or error
        t = Token("", TokenType::ERROR, line_number);
        if(endOfInput()) {
            t.token_type = TokenType::END_OF_FILE;
        }
        return t;
    }

    //if we reach this point, its an error
    t = Token("", TokenType::ERROR, line_number);
    return t;
}

Token Lexer::peek() {
    Token t = getToken();
    ungetToken(t);
    return t;
}

TokenType Lexer::ungetToken(const Token& t) {
    tokens.push_back(t);
    return t.token_type;
}

//returns ID
Token Lexer::getID() {
    Token t("", TokenType::ERROR, line_number);

    if(isIDStart()) {
        //get the start token
        t.lexeme += getChar();
        //get the rest of the num
        while(!endOfInput() && isIDBody()) t.lexeme += getChar();
        //now we have a valid token
        t.token_type = TokenType::ID;
    }
    return t;
}

bool Lexer::isIDStart() {
    char c = peekChar();
    return isalpha(c) || c == '_';
}

bool Lexer::isIDBody() {
    char c = peekChar();
    return isalnum(c) || c == '_';
}

//returns NUM
Token Lexer::getNUM() {
    Token t("", TokenType::ERROR, line_number);

    if(isHexStart()) {
        //consume hex start
        getChar();
        getChar();
        while(!endOfInput() && isHexDigit()) t.lexeme += getChar();
        t.token_type = TokenType::HEXNUM;
    }
    else if(isBinaryStart()) {
        //consume hex start
        getChar();
        getChar();
        while(!endOfInput() && isBinaryDigit()) t.lexeme += getChar();
        t.token_type = TokenType::BINNUM;
    }
    else if(isDigit()) {
        while(!endOfInput() && isDigit()) t.lexeme += getChar();
        t.token_type = TokenType::NUM;
    }
    //if no valid num sequence, then error is set

    return t;
}
bool Lexer::isNUM() {
    return isDigit() || isHexDigit() || isBinaryDigit();
}
bool Lexer::isDigit() {
    char c = peekChar();
    return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
           c == '5' || c == '6' || c == '7' || c == '8' || c == '9';
}
bool Lexer::isHexDigit() {
    char c = peekChar();
    return isDigit() ||
           c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
           c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f';
}
bool Lexer::isBinaryDigit() {
    char c = peekChar();
    return c == '0' || c == '1';
}
bool Lexer::isHexStart() {
    char c1 = getChar();
    char c2 = peekChar();
    ungetChar(c1);
    return c1 == '0' && c2 == 'x';
}
bool Lexer::isBinaryStart() {
    char c1 = getChar();
    char c2 = peekChar();
    ungetChar(c1);
    return c1 == '0' && c2 == 'b';
}

Token Lexer::getSymbol() {
    Token t("", TokenType::ERROR, line_number);

    //get the char
    if(isSymbol()) {
        char c = getChar();
        switch(c) {
            case '.': {
                t.token_type = TokenType::DOT;
                break;
            }
            case ':': {
                t.token_type = TokenType::COLON;
                break;
            }
            case '[': {
                t.token_type = TokenType::LBRACK;
                break;
            }
            case ']': {
                t.token_type = TokenType::RBRACK;
                break;
            }
            default: break;
        }
    }
    return t;
}

bool Lexer::isSymbol() {
    char c = peekChar();
    return c == '.' || c == ':' || c == '[' || c == ']';
}


std::vector<Token> Lexer::skipWhiteSpace() {

    std::vector<Token> newlines;

    //get a new char
    char c = getChar();
    //while not eof and c is whitespace
    while(!endOfInput() && isspace(c)) {
        if(c == '\n') newlines.emplace_back("", TokenType::NEWLINE, line_number - 1);
        c = getChar();
    }
    //if we got one too many, put it back
    if(!endOfInput()) ungetChar(c);

    //reverse the newlines
    std::reverse(newlines.begin(), newlines.end());
    return newlines;
}

bool Lexer::isComment() {
    //need two chars for '//'
    char c1 = getChar();
    char c2 = peekChar();
    ungetChar(c1);

    return c1 == '/' && c2 == '/';
}

Token Lexer::ignoreComment() {

    //get a new char
    char c = getChar();
    //while not eof and not end of line
    while(!endOfInput() && c != '\n') c = getChar();

    if(c == '\n') return Token("", TokenType::NEWLINE, line_number - 1);
    else
        return Token("", TokenType::NONE, -1);
}

bool Lexer::endOfInput() {
    if(!input_buffer.empty()) {
        //make sure input buffer isn't just holding eof
        if(input_buffer.front() == '\0') return true;
        else
            return false;
    }
    else
        return input.eof();
}

char Lexer::peekChar() {
    char c = getChar();
    ungetChar(c);
    return c;
}

void Lexer::ungetChar(char c) {
    if(c != EOF) input_buffer.push_back(c);
    if(c == '\n') line_number--;
}

char Lexer::getChar() {

    //check for end of input
    if(endOfInput()) return '\0';

    char c;
    if(!input_buffer.empty()) {
        c = input_buffer.back();
        input_buffer.pop_back();
    }
    else
        input.get(c);

    if(c == '\n') line_number++;

    return c;
}