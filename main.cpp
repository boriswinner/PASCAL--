#include <utility>
#include <iostream>
#include <string>
#include <ctype.h>
#include <iomanip>
#include <map>
#include <fstream>

using namespace std;

enum class token_types : int {
    KEYWORD,
    IDENTIFICATOR,
    OPERATOR,
    NUMBER,
    LITERAL,
};

enum class token_subtypes : int {
    /* KEYWORDS*/
    IF,
    ELSE,
    AND,
    OR,

    INTEGER,
    STRING,

    /* OPERATORS*/
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    EQUAL,
    NOT_EQUAL,
};

static std::map<std::string, token_subtypes> keywords{
        {"and",     token_subtypes::AND},
        {"or",      token_subtypes::OR},
        {"if",      token_subtypes::IF},
        {"else",    token_subtypes::ELSE},
        {"string",  token_subtypes::STRING},
        {"integer", token_subtypes::INTEGER},
};

static std::map<std::string, token_subtypes> operators{
        {">",  token_subtypes::GREATER},
        {">=", token_subtypes::GREATER_EQUAL},
        {"<",  token_subtypes::LESS},
        {"<=", token_subtypes::LESS_EQUAL},
        {"=",  token_subtypes::EQUAL},
        {"<>", token_subtypes::NOT_EQUAL},
};


class Buffer {
private:
    string filename_;
    ifstream input_file;
    int c_;
public:
    explicit Buffer(const string &filename) : filename_(filename) {
        input_file = ifstream(filename);
        c_ = input_file.get();
    }

    int get_current_char() {
        int t = c_;
        c_ = input_file.get();
        return t;
    }

    int look_current_char() {
        return c_;
    }

    void return_char(char c) {
        c_ = c;
    }
};

class Token {
public:
    Token(int line, int column, token_types type, token_subtypes subtype) : line_(line), column_(column), type_(type),
                                                                            subtype_(subtype) {}

    Token(int line, int column, token_types type, string text) : line_(line), column_(column), type_(type),
                                                                 text_(std::move(text)) {}

    Token(bool is_null): is_null_(true){}

    void print() {
        cout << setw(10) << "line" << setw(10) << "column" << setw(10) << "type" << setw(12) << "subtype" << setw(12)
             << "text" << endl;
        cout << setw(10) << line_ << setw(10) << column_ << setw(10) << static_cast<int>(type_) << setw(12)
             << static_cast<int>(subtype_) << setw(12) << text_ << endl;
    }

    bool isNull(){
        return is_null_;
    }

private:
    int line_;
    int column_;
    token_types type_;
    token_subtypes subtype_;
    string text_;
    bool is_null_ = false;
};

class Lexer {
public:
    Lexer(const string &filename) : buffer(filename) {}

    Token get_next() {
        int state = 0;
        int c = 0;
        while (true){
            switch (state) {
                case 0: {
                    c = buffer.look_current_char();
                    if (c == char_traits<int>::eof()){
                        return Token(true);
                    }else if ((c == ' ') || (c == '\n')) {
                        c = buffer.get_current_char();
                        state = 0;
                    } else if (is_letter(c)) {
                        state = 2;
                    } else if (c == '>' || c == '<' || c == '='){
                        state = 3;
                    }
                    break;
                }
                case 2: {
                    return read_identificator();
                    break;
                }
                case 3: {
                    return read_relop();
                    break;
                }
            }
        }
    }

private:
    int line_;
    Buffer buffer;

    bool is_letter(int c) {
        return isalpha(c);
    }

    bool is_digit(int c) {
        return isdigit(c);
    }

    Token read_relop() {
        int state = 0;
        char c;
        while (true){
            switch (state) {
                case 0: {
                    int c = buffer.get_current_char();
                    if (c == '<') {
                        state = 1;
                    } else if (c == '=') {
                        return {0, 0, token_types::OPERATOR, token_subtypes::EQUAL};
                    } else if (c == '>') {
                        state = 6;
                    }
                    break;
                }
                case 1: {
                    int c = buffer.look_current_char();
                    if (c == '=') {
                        c = buffer.get_current_char();
                        return {0, 0, token_types::OPERATOR, token_subtypes::LESS_EQUAL};
                    } else if (c == '>') {
                        c = buffer.get_current_char();
                        return {0, 0, token_types::OPERATOR, token_subtypes::NOT_EQUAL};
                    } else {
                        //buffer.return_char(c);
                        return {0, 0, token_types::OPERATOR, token_subtypes::LESS};
                    }
                    break;
                }
                case 6: {
                    int c = buffer.look_current_char();
                    if (c == '=') {
                        c = buffer.get_current_char();
                        return {0, 0, token_types::OPERATOR, token_subtypes::GREATER_EQUAL};
                    } else{
                        //buffer.return_char(c);
                        return {0, 0, token_types::OPERATOR, token_subtypes::GREATER};
                    }
                    break;
                }
            }
        }
    }

    Token read_identificator() {
        string s;
        int c = buffer.look_current_char();
        while (is_letter(c) || is_digit(c)) {
            c = buffer.get_current_char();
            s.push_back(char(c));
            c = buffer.look_current_char();
        }
        //buffer.return_char(c);
        if (keywords.find(s) == keywords.end()) {
            return {0, 0, token_types::IDENTIFICATOR, s};
        } else {
            return {0, 0, token_types::KEYWORD, keywords[s]};
        }
    }

};

int main() {
    Lexer lexer("input.txt");
    while (true){
        Token t = lexer.get_next();
        if (t.isNull()){
            break;
        }
        t.print();
    }

    return 0;
}