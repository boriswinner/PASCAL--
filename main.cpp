#include <utility>
#include <iostream>
#include <string>
#include <ctype.h>
#include <iomanip>
#include <map>
#include <fstream>
#include <set>

using namespace std;

enum class token_types : int {
    ENDOFFILE,
    KEYWORD,
    IDENTIFICATOR,
    OPERATOR,
    NUMBER,
    LITERAL,
};

enum class token_subtypes : int {
    UNKNOWN,
    /* KEYWORDS*/
    IF,
    ELSE,
    AND,
    OR,

    INTEGER,
    FLOAT,
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

    int next_char() {
        int t = c_;
        c_ = input_file.get();
        return t;
    }

    int peak() {
        return c_;
    }

//    void return_char(char c) {
//        c_ = c;
//    }
};

class Token {
public:
    Token(int line, int column, token_types type, token_subtypes subtype) :
        line_(line), column_(column), type_(type), subtype_(subtype) {
        text_ = "";
    }

    Token(): type_(token_types::ENDOFFILE), subtype_(token_subtypes::UNKNOWN) {}

    Token(int line, int column, token_types type, string text) : line_(line), column_(column), type_(type),
                                                                 text_(std::move(text)) {
        subtype_ = token_subtypes ::UNKNOWN;
    }

    Token(int line, int column, token_types type, token_subtypes subtype, string text) : line_(line), column_(column), type_(type), subtype_(subtype),
                                                                 text_(std::move(text)) {}

    void print() {
        cout << setw(10) << "line" << setw(10) << "column" << setw(10) << "type" << setw(12) << "subtype" << setw(12)
             << "text" << endl;
        cout << setw(10) << line_ << setw(10) << column_ << setw(10) << static_cast<int>(type_) << setw(12)
             << static_cast<int>(subtype_) << setw(12) << text_ << endl;
    }

    int line_;
    int column_;
    token_types type_;
    token_subtypes subtype_;
    string text_;
};

class Lexer {
public:
    Lexer(const string &filename) : buffer_(filename), line_(0), column_(0) {}

    Token get_next() {
        while (true){
            int c = buffer_.peak();
            column_++;
            if (c == '\n'){
                line_++;
                column_ = 0;
                c = buffer_.next_char();
                continue;
            } else if (c == ' '){
                c = buffer_.next_char();
                continue;
            } else if (isalpha(c)){
                return read_identificator();
            } else if ((c == '+') || (c == '-') || isdigit(c) || (c == '%') || (c == '$')){
                return read_number();
            } else if (c == '>' || c == '<' || c == '='){
                return read_relop();
            } else if (c == char_traits<int>::eof()){
                return {};
            } else if ((c == '\'') || (c == '#')){
                return read_string();
            }
        }
    }

private:
    int line_, column_;
    Buffer buffer_;

    set<int> operator_symbols{'>','<','=','!'};
    bool is_operator_symbol(int c){
        return static_cast<bool>(operator_symbols.count(c));
    };

    Token read_relop() {
        string s;
        int c = buffer_.peak();
        while (is_operator_symbol(c)){
            c = do_buffer_step(s, c);
        }
        if (operators.find(s) != operators.end()){
            return {line_, 0, token_types::OPERATOR, s};
        } else{
            //throw exception
        }
    }

    Token read_identificator() {
        string s;
        int c = buffer_.peak();
        while (isalpha(c) || isdigit(c)) {
            c = do_buffer_step(s, c);
        }
        if (keywords.find(s) == keywords.end()) {
            return {line_, column_, token_types::IDENTIFICATOR, s};
        } else {
            return {line_, column_, token_types::KEYWORD, keywords[s]};
        }
    }

    Token read_number(){
        string s;
        int c = buffer_.peak();
        int state = 0;
        enum {START, INT, FLOAT, BIN, HEX, SCALEFACTOR};
        while(true){
            switch (state){
                case START:{
                    if ((c == '+') || (c == '-') || isdigit(c)){
                        state = INT;
                    } else if (c == '$'){
                        state = HEX;
                        c = do_buffer_step(s, c);
                    } else if (c == '%'){
                        c = do_buffer_step(s, c);
                        state = BIN;
                    }
                    break;
                }

                case INT:{
                    if ((c == '+') || (c == '-')){
                        c = do_buffer_step(s, c);
                    }
                    while (isdigit(c)){
                        c = do_buffer_step(s, c);
                    }
                    if (c == '.' || c == 'E' || c == 'e'){
                        state = FLOAT;
                    } else{
                        return {line_,column_, token_types ::NUMBER, token_subtypes:: INTEGER, s};
                    }
                    break;
                }

                case FLOAT:{
                    c = buffer_.peak();
                    if (c == '.'){
                        c = do_buffer_step(s, c);
                    }
                    while(isdigit(c)){
                        c = do_buffer_step(s, c);
                    }
                    if (c == 'E' || c == 'e'){
                        state = SCALEFACTOR;
                        c = do_buffer_step(s, c);
                        while (isdigit(c)){
                            c = do_buffer_step(s, c);
                        }
                        return {line_,column_,token_types ::NUMBER,token_subtypes::FLOAT, s};
                    } else {
                        return {line_,column_, token_types ::NUMBER, token_subtypes::FLOAT, s};
                    }
                }


                case HEX:{
                    while (isxdigit(c)){
                        c = do_buffer_step(s ,c);
                    }
                    return {line_, column_, token_types ::NUMBER, token_subtypes ::INTEGER, s};
                }

                case BIN:{
                    while ((c == '0' || c == '1')){
                        c = do_buffer_step(s ,c);
                    }
                    return {line_, column_, token_types ::NUMBER, token_subtypes ::INTEGER, s};
                }
            }
        }
    }

    int do_buffer_step(string &s, int c) {
        c = buffer_.next_char();
        s.push_back(char(c));
        c = buffer_.peak();
        return c;
    }

    Token read_string(){
        string s;
        int c = buffer_.peak();
        if (c == '\''){
            c = buffer_.next_char();
            c = buffer_.peak();
        } else{
            //throw exception
        }
        while(true){
            if (c == '\''){
                c = buffer_.next_char();
                c = buffer_.peak();
                if (c == '\''){
                    c = do_buffer_step(s, c);
                } else{
                    return {line_, column_, token_types::LITERAL, s};
                }
            } else if (c == '#'){
                c = buffer_.next_char();
                Token t = read_number();
                s.push_back(static_cast<char>(stoi(t.text_)));
                c = buffer_.peak();
                if ((c != '\'') && (c != '#')){
                    return {line_, column_, token_types::LITERAL, s};
                }
            } else{
                c = do_buffer_step(s, c);
            }
        }
    }

};

int main() {
    Lexer lexer("input.txt");
    while (true){
        Token t = lexer.get_next();
        if (t.type_ == token_types::ENDOFFILE){
            break;
        }
        t.print();
    }

    return 0;
}