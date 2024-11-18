#include <string>
#include <variant>
#include <optional>
#include <vector>

enum class eIRcmd{
    ERROR,
    SYMBOL,
    FUNCTION,
    STRUCT,
    LABEL,
    BEGIN,
    END,
    RET,
    JMP,
    JNE,
    JE,
    CALL,
    FRAME,
    MOV,
    ADD,
    SUB,
    DIV,
    MUL,
    MOD,
    OR,
    NOT,
    NEG,
    EQUAL,
    NOTEQUAL,
    GREATER,
    LESS,
    FLOOR,
    MIN,
    MAX,
    INSERT,
    COMMENT,
    DEREF,
    ALLOC,
    USING,
    DEBUG,
};

enum class eIRparam{
    ERROR,
    VAR,
    STRING,
    LABEL,
    FUNC,
    STRUCT,
    ARRAY,
    ENTER,
    LEAVE,
    GENERAL,
    SOURCE,
    LINE,
};

class IR_val{
    public:
    std::string val;
    bool ref = false;
    bool deref = false;
};

using IR_lit_val = typename std::optional<std::variant<int, std::string>>;

class IR_param{
    public:
    eIRparam type;
    IR_lit_val val;
};

using IR_arg = typename std::variant<IR_val, IR_param>;

class IR_Cmd{
    public:
    eIRcmd type;
    std::vector<IR_arg> args;
};

std::istream& operator>>(std::istream& stream, eIRcmd& ecmd);
std::istream& operator>>(std::istream& stream, eIRparam& eparam);
std::istream& operator>>(std::istream& stream, IR_val& val);
std::istream& operator>>(std::istream& stream, IR_lit_val& litval);
std::istream& operator>>(std::istream& stream, IR_param& param);
std::istream& operator>>(std::istream& stream, IR_arg& arg);
std::istream& operator>>(std::istream& stream, IR_Cmd& cmd);