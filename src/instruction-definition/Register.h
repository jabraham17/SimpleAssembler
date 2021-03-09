//
// Created by Jacob Abraham on 3/7/21.
//

#ifndef __SIMPLEASSEMBLER_REGISTER_H__
#define __SIMPLEASSEMBLER_REGISTER_H__

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

//register name, hex code, size in bytes
#define REGISTER_LIST(V) \
    V(R0, 0x00, 2)       \
    V(R1, 0x01, 2)       \
    V(R2, 0x02, 2)       \
    V(R3, 0x03, 2)       \
    V(R4, 0x04, 2)       \
    V(R5, 0x05, 2)       \
    V(R6, 0x06, 2)       \
    V(R7, 0x07, 2)

namespace Register {
    //enum of registers
    enum class RegisterMapping : uint8_t {
#define MAKE_ENUM(NAME, HEX, SIZE) NAME,
        REGISTER_LIST(MAKE_ENUM)
#undef MAKE_ENUM
            UNKNOWN
    };

    struct RegisterData {
        const RegisterMapping reg;
        const std::string name;
        const uint8_t hex;
        const uint8_t size;
    };

#define REG_COUNT(NAME, HEX, SIZE) +1
    constexpr size_t NumRegisters = REGISTER_LIST(REG_COUNT) + 1;
#undef REG_COUNT

#define REG_STRUCT(NAME, HEX, SIZE) RegisterData{RegisterMapping::NAME, #NAME, HEX, SIZE},
    const std::array<const struct RegisterData, NumRegisters> Registers = {REGISTER_LIST(REG_STRUCT) RegisterData{RegisterMapping::UNKNOWN, "UNKNOWN", 0, 0}};
#undef REG_STRUCT


    //get the register based on a string
    RegisterMapping matchRegister(const std::string& lexeme) {
        //make string upper, only works with UTF-8
        //use lambda as this will get inlined, function pointer will not
        std::string upperLexeme(lexeme);
        std::transform(upperLexeme.begin(), upperLexeme.end(), upperLexeme.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        //perform compare
        auto it = std::find_if(Registers.begin(), Registers.end(), [&upperLexeme](const RegisterData& rd) { return rd.name == upperLexeme; });

        //return the register or none
        return it != Registers.end() ? it->reg : RegisterMapping::UNKNOWN;
    }

#define MAKE_GETTER(VALUE, VALUE_TYPE, KEY, KEY_TYPE, UNKNOWN)                            \
    VALUE_TYPE get_##VALUE(KEY_TYPE KEY) {                                                \
        auto it = std::find_if(Registers.begin(),                                         \
                               Registers.end(),                                           \
                               [&KEY](const RegisterData& rd) { return rd.KEY == KEY; }); \
        return it != Registers.end() ? it->VALUE : UNKNOWN;                               \
    }

    MAKE_GETTER(reg, RegisterMapping, hex, const uint16_t&, RegisterMapping::UNKNOWN)
    MAKE_GETTER(hex, uint16_t, reg, const RegisterMapping&, 0)
    MAKE_GETTER(name, std::string, hex, const uint16_t&, "UNKNOWN")
    MAKE_GETTER(name, std::string, reg, const RegisterMapping&, "UNKNOWN")

#undef MAKE_GETTER

}// namespace Register

#endif//__SIMPLEASSEMBLER_REGISTER_H__
