//
// Created by Jacob Abraham on 3/8/21.
//

#include "parser.h"


void Parser::parse() {

    parse_file();
}

Token Parser::expect(TokenType expected_type) {
    Token token = lexer.getToken();
    if(token.token_type != expected_type)
        throw SyntaxError("Unexpected token of type '" +
                              Token::getTokenType(expected_type) +
                              "' expected '" +
                              Token::getTokenType(token) +
                              "'",
                          token.line_no);
    else
        return token;
}

#define PARSER_FUNC(rule, rettype, ...) \
    rettype Parser::parse_##rule(__VA_ARGS__)


//file -> newlines_0 statement_list newlines_0
PARSER_FUNC(file, void) {
    parse_newlines_0();
    parse_statement_list();
    parse_newlines_0();
}
//statement_list -> statement
//statement_list -> statement newlines_1 statement_list
PARSER_FUNC(statement_list, void) {
    parse_statement();
    Token t = lexer.peek();
    if(t.token_type == TokenType::NEWLINE) {
        parse_newlines_1();
        parse_statement_list();
    }
}
//statement -> directive
//statement -> labeled_statement
//statement -> instruction
PARSER_FUNC(statement, void) {
    Token t1 = lexer.getToken();
    Token t2 = lexer.peek();
    lexer.ungetToken(t1);

    if(t1.token_type == TokenType::DOT) {
        parse_directive();
    }
    else if(t1.token_type == TokenType::ID || t1.token_type == TokenType::NUM) {
        if(t2.token_type == TokenType::COLON) {
            parse_labeled_statement();
        }
        else {
            parse_instruction();
        }
    }
    else if(t2.token_type == TokenType::END_OF_FILE) {
        //do nothing, no more statements
    }
    else {
        throw SyntaxError("Unexpected start to statement: " +
                              Token::getTokenType(t1.token_type) +
                              ",'" +
                              t1.lexeme +
                              "'",
                          t1.line_no);
    }
}
//directive -> DOT ID atom_list
PARSER_FUNC(directive, void) {
    expect(TokenType::DOT);
    expect(TokenType::ID);
    parse_atom_list();
}
//labeled_statement -> label_list newlines_0 statement
PARSER_FUNC(labeled_statement, void) {
    parse_label();
    parse_newlines_0();
    parse_statement();
}
//label_list -> label
//label_list -> label newlines_0 label_list
PARSER_FUNC(label_list, void) {
}
//label -> ID COLON
PARSER_FUNC(label, void) {
    expect(TokenType::ID);
    expect(TokenType::COLON);
}
//instruction -> atom_list
PARSER_FUNC(instruction, void) {
    parse_atom_list();
}
//atom -> ID | number
PARSER_FUNC(atom, void) {
    Token t = lexer.peek();
    if(t.token_type == TokenType::ID) {
        expect(TokenType::ID);
    }
    else {
        parse_number();
    }
}
//atom_list -> atom
//atom_list -> atom atom_list
PARSER_FUNC(atom_list, void) {
    Token t = lexer.peek();
    if(t.token_type == TokenType::ID ||
       t.token_type == TokenType::NUM ||
       t.token_type == TokenType::HEXNUM ||
       t.token_type == TokenType::BINNUM) {
        parse_atom();
        t = lexer.peek();
        if(t.token_type == TokenType::ID ||
           t.token_type == TokenType::NUM ||
           t.token_type == TokenType::HEXNUM ||
           t.token_type == TokenType::BINNUM) {
            parse_atom_list();
        }
    }
}
//number -> NUM | HEXNUM | BINNUM
PARSER_FUNC(number, void) {
    Token t = lexer.peek();
    if(t.token_type == TokenType::NUM) {
        expect(TokenType::NUM);
    }
    else if(t.token_type == TokenType::HEXNUM) {
        expect(TokenType::HEXNUM);
    }
    else if(t.token_type == TokenType::BINNUM) {
        expect(TokenType::BINNUM);
    }
    else {
        throw SyntaxError("Unexpected token of type '" +
                              Token::getTokenType(t) +
                              "' expected 'NUM', 'HEXNUM', or 'BINNUM'",
                          t.line_no);
    }
}
//newlines_0 ->
//newlines_0 -> NEWLINE
//newlines_0 -> NEWLINE newlines_0
PARSER_FUNC(newlines_0, void) {
    Token t = lexer.peek();
    if(t.token_type == TokenType::NEWLINE) {
        expect(TokenType::NEWLINE);
        t = lexer.peek();
        if(t.token_type == TokenType::NEWLINE) {
            parse_newlines_0();
        }
    }
}
//newlines_1 -> NEWLINE
//newlines_1 -> NEWLINE newlines_1
PARSER_FUNC(newlines_1, void) {
    expect(TokenType::NEWLINE);
    Token t = lexer.peek();
    if(t.token_type == TokenType::NEWLINE) {
        parse_newlines_1();
    }
}


#undef PARSER_FUNC