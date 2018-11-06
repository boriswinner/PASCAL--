#include <map>
#include <string>
#include "string.h"

struct case_insensitive_string_cmp {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return stricmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

#define TOKENTYPES \
    X(ENDOFFILE) \
    X(KEYWORD) \
    X(IDENTIFICATOR) \
    X(OPERATOR) \
    X(NUMBER) \
    X(LITERAL) \
    X(SPECIALSYMBOL) \

#define KEYWORDS \
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

#define OPERATORS \
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

#define SPECIALSYMBOLS \
    X(Plus, "+") \
    X(Minus, "-") \
    X(Asterisk, "*") \
    X(Slash, "/") \
    X(DoubleAsterisk, "**") \
    X(At, "@") \
    X(Caret, "^") \
    X(ShiftLeft, "<<") \
    X(ShiftRight, ">>") \
    X(SymmetricDiff, "><") \
    X(Colon, ":") \
    X(Comma, ",") \
    X(Dot, ".") \
    X(DoubleDot, "..") \
    X(Semicolon, ";") \
    X(OpenParenthesis, "(") \
    X(CloseParenthesis, ")") \
    X(OpenSquareBracket, "[") \
    X(CloseSquareBracket, "]") \

enum token_types: int {
#define X(name) name,
    TOKENTYPES
#undef X
};

char const* token_types_names[]={
    #define X(name) #name,
        TOKENTYPES
    #undef X
};

enum token_subtypes: int{
    UNKNOWN,
#define X(name) name,
    KEYWORDS
#undef X
#define X(name, str) name,
    OPERATORS
#undef X
};

char const* token_subtypes_names[]={
        "UNKNOWN",
#define X(name) #name,
        KEYWORDS
#undef X
#define X(name, str) #name,
        OPERATORS
#undef X
};

static std::map<std::string, token_subtypes, case_insensitive_string_cmp> keywords{
#define X(name) {#name, name},
        KEYWORDS
#undef X
};

static std::map<std::string, token_subtypes, case_insensitive_string_cmp> operators{
#define X(name, str) {str, name},
        OPERATORS
#undef X
};