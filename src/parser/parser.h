//
// Created by Jacob Abraham on 3/8/21.
//

#ifndef __SIMPLEASSEMBLER_PARSER_H__
#define __SIMPLEASSEMBLER_PARSER_H__

#include "AST.h"
#include "lexer.h"
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>


class SyntaxError : public std::exception {
public:
    std::string message;
    int line_no;

    SyntaxError(std::string msg, int ln) : message(std::move(msg)), line_no(ln) {}

    SyntaxError() : message("Unknown"), line_no(-1) {}

    [[nodiscard]] const char* what() const noexcept override {
        //const ref trick to allow for copy so string isn't local stack memory
        const std::string& ret = getString();
        return ret.c_str();
    }

    friend std::ostream& operator<<(std::ostream& out, const SyntaxError& se) {
        out << se.getString();
        return out;
    }
    [[nodiscard]] std::string getString() const {
        std::string str("Syntax Error: " + this->message + " on line " + std::to_string(this->line_no) + "");
        return str;
    }
};


class Parser {

    //public interface
public:
    explicit Parser(std::istream& in) : lexer(in) {}

    explicit Parser(Lexer& lexer) : lexer(lexer) {}

    void parse();


private:
    Lexer lexer;

    Token expect(TokenType);


#define PARSER_RULES(F)        \
    F(file, AST::ptr_list<AST::Statement>)              \
    F(statement_list, AST::ptr_list<AST::Statement>)    \
    F(statement, std::unique_ptr<AST::Statement>)         \
    F(directive, std::unique_ptr<AST::Instruction>)         \
    F(labeled_statement, std::unique_ptr<AST::Statement>) \
    F(label_list, void, AST::ptr_list<AST::Label>&)        \
    F(label, std::unique_ptr<AST::Label>)             \
    F(instruction, std::unique_ptr<AST::Instruction>)       \
    F(atom, std::unique_ptr<AST::Atom>)         \
    F(atom_list, void, AST::ptr_list<AST::Atom>&)         \
    F(number, std::unique_ptr<AST::Atom>)            \
    F(newlines_0, void)        \
    F(newlines_1, void)

#define HEADER(rule, rettype, ...) rettype parse_##rule(__VA_ARGS__);
    PARSER_RULES(HEADER)
#undef PASRER_HEADER
};


#endif//__SIMPLEASSEMBLER_PARSER_H__
