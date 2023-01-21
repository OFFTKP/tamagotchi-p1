#ifndef OPCODES_HXX
#define OPCODES_HXX
#include <string>

enum class OpcodeType {
    #define XMACRO(name, unused, ...) name,
    #include "opcodes.def"
    #undef XMACRO
    };

[[maybe_unused]] static std::string serialize(OpcodeType type) {
    switch (type) {
        #define XMACRO(name, unused, ...) case OpcodeType::name: return #name;
        #include "opcodes.def"
        #undef XMACRO
    }
    return "Error";
}
#endif