#include <utility>

#include <utility>

#include <iostream>
#include <string>
#include <ctype.h>
#include <iomanip>
#include <map>
#include <fstream>

using namespace std;

enum class token_types: int {
    TYPE_STRING,
    TYPE_INTEGER,

    IF,
    ELSE,
    AND,
    OR,

    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    IDENTIFICATOR,
    NUMBER,
    LITERAL
};

static std::map<std::string, token_types> keywords{
        {"and", token_types::AND},
        {"or", token_types::OR},
        {"if", token_types::IF},
        {"else", token_types::ELSE},
        {"string", token_types::TYPE_STRING},
        {"integer", token_types::TYPE_INTEGER},
};

static std::map<std::string, token_types> operators{
        {">", token_types::GREATER},
        {">=", token_types::GREATER_EQUAL},
        {"<", token_types::LESS},
        {"<=", token_types::LESS_EQUAL},
};


class Buffer{
private:
    string filename_;
    ifstream input_file;
    int c_;
public:
    explicit Buffer(const string &filename): filename_(filename){
        input_file = ifstream(filename);
        c_ = input_file.get();
    }

    int get_current_char(){
        int t = c_;
        c_ = input_file.get();
        return t;
    }

    int look_current_char(){
        return c_;
    }

    void return_char(char c){
        c_ = c;
    }
};

class Token{
public:
    Token(int line, int column, token_types type, int value, string text): line_(line), column_(column), type_(type), value_(value), text_(
            std::move(text)){}

    void print(){
        cout << setw(10) << "line" << setw(10)<< "column" << setw(10) << "type" << setw(12) << "value" << setw(12) << "text" << endl;
        cout << setw(10) << line_ << setw(10)<< column_ << setw(10) << static_cast<int>(type_) << setw(12) << value_ << setw(12) << text_ << endl;
    }

private:
    int line_;
    int column_;
    token_types type_;
    int value_;
    string text_;
};

class Lexer{
public:
    Lexer(const string &filename): buffer(filename){}

    Token get_next(){
        int state = 0;
        int c = 0;
        switch (state){
            case 0:{
                c = buffer.get_current_char();
                if ((c == ' ') || (c == '\n')){
                    state = 1;
                }
                if (is_letter(c)){
                    state = 2;
                }
            }
            case 1:{

            }
            case 2:{
                if (is_letter(buffer.look_current_char()) || is_digit(buffer.look_current_char())){
                    return read_identificator(c);
                }
            }
        }
    }

private:
    int line_;
    Buffer buffer;

    bool is_letter(int c){
        return isalpha(c);
    }

    bool is_digit(int c){
        return isdigit(c);
    }

    Token try_relop(){
        int state = 0;
        switch (state){
            case 0:{
                int c = getchar();
                if (c == '<'){
                    state = 1;
                } else if (c == '='){
                    //
                } else if (c == '>'){
                    state = 6;
                }
            }
            case 1:{
                int c = getchar();
                if (c == '='){
                    //return LE
                } else if (c == '>'){
                    //
                }else{
                    //
                }
            }
            case 6:{
                int c = getchar();
                if (c == '='){
                    //
                }
            }
        }
    }

    Token read_identificator(int c){
        string s;
        while(is_letter(c) || is_digit(c)){
            s.push_back(char(c));
            c = buffer.get_current_char();
//            if (c == std::char_traits<int>::eof()){
//                break;
//            }
        }
        buffer.return_char(c);
        if ( keywords.find(s) == keywords.end() ) {
            return {0,0,token_types::IDENTIFICATOR,0,s};
        } else {
            return {0,0,keywords[s],0,s};

        }
    }

};

int main() {
    Lexer lexer("input.txt");
    Token t = lexer.get_next();
    t.print();
    return 0;
}