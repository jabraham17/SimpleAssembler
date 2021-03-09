//
// Created by Jacob Abraham on 3/7/21.
//

#ifndef __SIMPLEASSEMBLER_INSTRUCTION_H__
#define __SIMPLEASSEMBLER_INSTRUCTION_H__

#include "helpers/macro-magic.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

//MAX 6 bytes
//name, hex prefix, hex, args: first is dest, then sources
#define INSTRUCTION_LIST(V)                           \
    V(NOP, 0x0000, NONE)                              \
    V(ADD, 0x0001, REGISTER, REGISTER, REGISTER_IMM)  \
    V(SUB, 0x0002, REGISTER, REGISTER, REGISTER_IMM)  \
    V(MULT, 0x0003, REGISTER, REGISTER, REGISTER_IMM) \
    V(DIV, 0x0003, REGISTER, REGISTER, REGISTER_IMM)  \
    V(SHL, 0x0005, REGISTER, IMM)                     \
    V(SHR, 0x0006, REGISTER, IMM)                     \
    V(CMP, 0x0007, REGISTER, REGISTER)                \
    V(JGTE, 0x0008, REGISTER_IMM)                     \
    V(JGT, 0x0009, REGISTER_IMM)                      \
    V(JEQ, 0x000A, REGISTER_IMM)                      \
    V(JNEQ, 0x000B, REGISTER_IMM)                     \
    V(JLT, 0x000C, REGISTER_IMM)                      \
    V(JLTE, 0x000D, REGISTER_IMM)                     \
    V(JMP, 0x000E, REGISTER_IMM)                      \
    V(AND, 0x000F, REGISTER, REGISTER, REGISTER_IMM)  \
    V(OR, 0x0010, REGISTER, REGISTER, REGISTER_IMM)   \
    V(NOT, 0x0011, REGISTER, REGISTER, REGISTER_IMM)  \
    V(XOR, 0x0012, REGISTER, REGISTER, REGISTER_IMM)


namespace Instruction {
    //enum of instructions
    enum class InstructionMapping : uint16_t {
#define MAKE_ENUM(NAME, HEX, ...) NAME,
        INSTRUCTION_LIST(MAKE_ENUM)
#undef MAKE_ENUM
            UNKNOWN
    };

    //argument types for instructions
    enum class ArgumentType {
        NONE,
        REGISTER,
        IMM,
        REGISTER_IMM
    };

    struct InstructionData {
        const InstructionMapping inst;
        const std::string name;
        const uint16_t hex;
        //max number of args is total instruction length minus length of operand
        //6-2=4
        const size_t nOperands;
        const ArgumentType operands[4];
    };

#define INST_COUNT(NAME, HEX, ...) +1
    constexpr size_t NumInstructions = INSTRUCTION_LIST(INST_COUNT) + 1;
#undef INST_COUNT

#define PROCESS_ARG(arg) ArgumentType::arg,

#define INST_STRUCT(NAME, HEX, ...) InstructionData{InstructionMapping::NAME, #NAME, HEX, COUNT_ARGUMENTS(__VA_ARGS__), {FOR_EACH(PROCESS_ARG, __VA_ARGS__)}},
    const std::array<const struct InstructionData, NumInstructions> Instructions = {INSTRUCTION_LIST(INST_STRUCT) InstructionData{InstructionMapping::UNKNOWN, "UNKNOWN", 0, 0, {}}};
#undef INST_STRUCT

    //get the instruction based on a string
    InstructionMapping matchInstruction(const std::string& lexeme) {
        //make string upper, only works with UTF-8
        //use lambda as this will get inlined, function pointer will not
        std::string upperLexeme(lexeme);
        std::transform(upperLexeme.begin(), upperLexeme.end(), upperLexeme.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        //perform compare
        auto it = std::find_if(Instructions.begin(), Instructions.end(), [&upperLexeme](const InstructionData& id) { return id.name == upperLexeme; });

        //return the register or none
        return it != Instructions.end() ? it->inst : InstructionMapping::UNKNOWN;
    }

#define MAKE_GETTER(VALUE, VALUE_TYPE, KEY, KEY_TYPE, UNKNOWN)                               \
    VALUE_TYPE get_##VALUE(KEY_TYPE KEY) {                                                   \
        auto it = std::find_if(Instructions.begin(),                                         \
                               Instructions.end(),                                           \
                               [&KEY](const InstructionData& rd) { return rd.KEY == KEY; }); \
        return it != Instructions.end() ? it->VALUE : UNKNOWN;                               \
    }

    MAKE_GETTER(inst, InstructionMapping, hex, const uint16_t&, InstructionMapping::UNKNOWN)
    MAKE_GETTER(hex, uint16_t, inst, const InstructionMapping&, 0)
    MAKE_GETTER(name, std::string, hex, const uint16_t&, "UNKNOWN")
    MAKE_GETTER(name, std::string, inst, const InstructionMapping&, "UNKNOWN")

#undef MAKE_GETTER

}// namespace Instruction

#endif//__SIMPLEASSEMBLER_INSTRUCTION_H__
