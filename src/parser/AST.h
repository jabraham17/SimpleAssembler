//
// Created by Jacob Abraham on 3/8/21.
//

#ifndef __SIMPLEASSEMBLER_AST_H__
#define __SIMPLEASSEMBLER_AST_H__

#include <memory>
#include <vector>



namespace AST {

    template <class T>
    using ptr_list = std::vector<std::unique_ptr<T>>;

    struct Label;
    struct Instruction;

    struct Statement {
        ptr_list<Label> labels;
        //instruction is either assembly code, or for the assembler (directive)
        std::unique_ptr<Instruction> inst;
    };

    struct Atom;
    struct Instruction {
        std::unique_ptr<Atom> name;
        ptr_list<Atom> args;
        bool isDirective;
    };

    struct Label {
        std::string label;
    };

    enum class AtomType {
        ID,
        NUM,
        HEXNUM,
        BINNUM
    };
    struct Atom {
        std::string str;
        AtomType type;
    };
}// namespace AST


#endif//__SIMPLEASSEMBLER_AST_H__
