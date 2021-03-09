#include "instruction-definition/Instruction.h"
#include "instruction-definition/Register.h"
#include "parser/lexer.h"
#include "parser/parser.h"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto r = Register::matchRegister("R3");
    std::cout << "R3: " << (int)Register::get_hex(r) << std::endl;
    std::cout << Register::get_name(Register::RegisterMapping::R3) << std::endl;
    std::cout << Register::get_name(3) << std::endl;

    auto a = Instruction::matchInstruction("ADD");
    std::cout << "ADD: " << (int)Instruction::get_hex(a) << std::endl;
    std::cout << Instruction::get_name(Instruction::InstructionMapping::ADD) << std::endl;
    std::cout << Instruction::get_name(0x13) << std::endl;


    std::fstream in("test.txt");
    Parser parser(in);
    try {
        parser.parse();
    }
    catch(SyntaxError se) {
        std::cout << "Error" << se << std::endl;
    }
    in.close();

    /*while(lexer.peek().token_type != TokenType::END_OF_FILE) {
        std::cout << lexer.getToken() << std::endl;
    }*/


    return 0;
}
