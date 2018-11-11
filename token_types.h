#include <map>
#include <string>
#include "string.h"

struct case_insensitive_string_cmp {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return stricmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

#define TOKENTYPES_START \
    X(UNKNOWN) \
    X(ENDOFFILE) \
    X(IDENTIFICATOR) \
    X(LITERAL) \
    X(OPERATOR) \
    X(KEYWORD) \

#define TOKENTYPES_KEYWORDS \
    X(IF) \
    X(THEN) \
    X(ELSE) \
    X(AND) \
    X(OR) \
    X(BEGIN) \
    X(END) \
    X(FOR) \
    X(VAR) \
    X(WHILE) \
    X(TO) \
    X(STRING) \
    X(INTEGER) \
    X(FLOAT) \

#define TOKENTYPES_OPERATORS \
    X(Assignment, ":=") \
    X(AssignmentWithPlus, "+=") \
    X(AssignmentWithMinus, "-=") \
    X(AssignmentWithAsterisk, "*=") \
    X(AssignmentWithSlash, "/=") \
    X(Equals, "=") \
    X(StrictLess, "<") \
    X(LessOrEquals, "<=") \
    X(NotEquals, "<>") \
    X(StrictGreater, ">") \
    X(GreaterOrEquals, ">=") \
    X(Plus, "+") \
    X(Minus, "-") \
    X(Multiply, "*") \
    X(Divide, "/") \
    X(OpenParenthesis, "(") \
    X(CloseParenthesis, ")") \
    X(OpenSquareBracket, "[") \
    X(CloseSquareBracket, "]") \
    X(Colon, ":") \
    X(Endofexpr, ";") \
    X(Comma, ",") \
    X(Dot, ".") \

enum token_types: int {
    #define X(name) name,
        TOKENTYPES_START
    #undef X
    #define X(name) name,
        TOKENTYPES_KEYWORDS
    #undef X
    #define X(name, str) name,
        TOKENTYPES_OPERATORS
    #undef X
};

char const* token_types_names[]={
    #define X(name) #name,
        TOKENTYPES_START
    #undef X
    #define X(name) #name,
        TOKENTYPES_KEYWORDS
    #undef X
    #define X(name, str) #name,
        TOKENTYPES_OPERATORS
    #undef X
};

static std::map<std::string, token_types, case_insensitive_string_cmp> keywords{
    #define X(name) {#name, name},
        TOKENTYPES_KEYWORDS
    #undef X
};

static std::map<std::string, token_types, case_insensitive_string_cmp> operators{
    #define X(name, str) {str, name},
        TOKENTYPES_OPERATORS
    #undef X
};
