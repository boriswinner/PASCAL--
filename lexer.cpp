#include "lexer.h"

Buffer::Buffer(const std::string &filename) : filename_(filename), line_(1), column_(0) {
    input_file = std::ifstream(filename);
    c_ = input_file.get();
}

int Buffer::next_char() {
    int t = c_;
    column_++;
    if (t == '\n') {
        line_++;
        column_ = 0;
    }
    c_ = input_file.get();
    return t;
}

void Buffer::return_char(char c) {
    input_file.putback(c_);
    c_ = c;
}

std::string Token::print() {
    std::stringstream buffer;
    buffer << std::setw(5) << line_ << std::setw(5) << column_ << std::setw(16) <<
           token_types_names[type_] << std::setw(23) << value_ << std::setw(16) << text_ << std::endl;
    return buffer.str();
}