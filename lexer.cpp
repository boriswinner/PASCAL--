#include <stack>
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
    column_--;
    input_file.putback(c_);
    c_ = c;
}

/* print variant type to stream */
template<typename T0, typename ... Ts>
std::ostream &operator<<(std::ostream &s,
                         std::variant<T0, Ts...> const &v) {
    std::visit([&](auto &&arg) { s << arg; }, v);
    return s;
}

std::string Token::print() {
    std::stringstream buffer;
    buffer << std::setw(5) << line_ << std::setw(5) << column_ << std::setw(16) <<
           token_types_names[type_] << std::setw(23) << value_ << std::setw(16) << text_ << std::endl;
    return buffer.str();
}

Token Lexer::get_next() {
    while (true) {
        int c = buffer_.peek();
        if (c == '{' || c == '(' || c == '/') {
            read_comment();
        }
        c = buffer_.peek();
        if ((c == ' ') || c == '\n') {
            c = buffer_.next_char();
        } else if (isalpha(c) || c == '_') {
            return read_identifier();
        } else if (isdigit(c) || (c == '%') || (c == '$')) {
            return read_number();
        } else if (is_operator_symbol(c)) {
            return read_relop();
        } else if (c == std::char_traits<int>::eof()) {
            return {buffer_, token_types::ENDOFFILE};
        } else if ((c == '\'') || (c == '#')) {
            return read_string();
        } else {
            c = buffer_.next_char();
            throw IncorrectCharacterException(buffer_.line(), buffer_.column());
        }
    }
}

Token Lexer::read_relop() {
    std::string s;
    int c = buffer_.peek();
    while (is_operator_symbol(c)) {
        c = do_buffer_step(s, c);
    }
    if (operators.find(s) == operators.end()) {
        throw IncorrectOperatorException(buffer_.line(), buffer_.column());
    }
    return {buffer_, token_types::OPERATOR, token_types_names[operators[s]], s};
}

Token Lexer::read_identifier() {
    std::string s;
    int c = buffer_.peek();
    while (isalpha(c) || isdigit(c) || c == '_') {
        c = do_buffer_step(s, c);
    }
    if (keywords.find(s) == keywords.end()) {
        std::string s_lower(s);
        std::transform(s_lower.begin(), s_lower.end(), s_lower.begin(), ::tolower);
        return {buffer_, token_types::IDENTIFIER, s_lower, s};
    } else {
        return {buffer_, token_types::KEYWORD, token_types_names[keywords[s]], s};
    }
}

Token Lexer::read_number() {
    std::string s;
    int c = buffer_.peek();
    int state = 0;
    enum {
        START, INT, FLOAT, BIN, HEX, SCALEFACTOR
    };
    while (true) {
        switch (state) {
            case START: {
                if (isdigit(c)) {
                    state = INT;
                } else if (c == '$') {
                    state = HEX;
                    c = do_buffer_step(s, c);
                } else if (c == '%') {
                    c = do_buffer_step(s, c);
                    state = BIN;
                }
                break;
            }

            case INT: {
                while (isdigit(c)) {
                    c = do_buffer_step(s, c);
                }
                if (c == '.' || c == 'E' || c == 'e') {
                    state = FLOAT;
                } else {
                    try {
                        int t = std::stoi(s);
                        return {buffer_, token_types::INTEGER, t, s};
                    } catch (const std::out_of_range &oor) {
                        throw OutOfRangeNumberException(buffer_.line(), buffer_.column());
                    }
                }
                break;
            }

            case FLOAT: {
                c = buffer_.peek();
                if (c == '.') {
                    c = do_buffer_step(s, c);
                }
                while (isdigit(c)) {
                    c = do_buffer_step(s, c);
                }
                if (c == 'E' || c == 'e') {
                    state = SCALEFACTOR;
                    c = do_buffer_step(s, c);
                    continue;
                } else {
                    try {
                        float t = std::stof(s);
                        return {buffer_, token_types::FLOAT, t, s};
                    } catch (const std::out_of_range &oor) {
                        throw OutOfRangeNumberException(buffer_.line(), buffer_.column());
                    }
                }
            }


            case HEX: {
                while (isxdigit(c)) {
                    c = do_buffer_step(s, c);
                }
                std::string hex_string = "0x" + s;
                hex_string.erase(2, 1);
                try {
                    int t = int(std::stof(hex_string));
                    return {buffer_, token_types::INTEGER, t, s};
                } catch (const std::out_of_range &oor) {
                    throw OutOfRangeNumberException(buffer_.line(), buffer_.column());
                }
            }

            case BIN: {
                while ((c == '0' || c == '1')) {
                    c = do_buffer_step(s, c);
                }
                std::string bin_string(s);
                bin_string.erase(0, 1);
                try {
                    int t = std::stoi(bin_string, nullptr, 2);
                    return {buffer_, token_types::INTEGER, t, s};
                } catch (const std::out_of_range &oor) {
                    throw OutOfRangeNumberException(buffer_.line(), buffer_.column());
                }
            }

            case SCALEFACTOR: {
                c = buffer_.peek();
                if (c == '+' || c == '-') {
                    c = do_buffer_step(s, c);
                }
                while (isdigit(c)) {
                    c = do_buffer_step(s, c);
                }
                try {
                    float t = std::stof(s);
                    return {buffer_, token_types::FLOAT, t, s};
                } catch (const std::out_of_range &oor) {
                    throw OutOfRangeNumberException(buffer_.line(), buffer_.column());
                }
            }
        }
    }
}

Token Lexer::read_string() {
    std::string s, text;
    int c = buffer_.peek();
    if (c == '\'') {
        text.push_back(c);
        c = buffer_.next_char();
        c = buffer_.peek();
    }
    while (true) {
        if (c == '\'') {
            text.push_back(c);
            c = buffer_.next_char();
            c = buffer_.peek();
            if (c == '\'') {
                text.push_back(c);
                text.push_back(c);
                c = do_buffer_step(s, c);
            } else {
                return {buffer_, token_types::LITERAL, s, text};
            }
        } else if (c == '#') {
            text.push_back(c);
            c = buffer_.next_char();
            Token t = read_number();
            s.push_back(static_cast<char>(stoi(t.text())));
            c = buffer_.peek();
            text += t.text();
            if ((c != '\'') && (c != '#')) {
                text.push_back(c);
                return {buffer_, token_types::LITERAL, s, text};
            } else if (c == '\'') {
                text.push_back(c);
                c = buffer_.next_char();
                c = buffer_.peek();
            }
        } else if (c == std::char_traits<int>::eof()) {
            throw UnterminatedStringException(buffer_.line(), buffer_.column());
        } else {
            text.push_back(c);
            c = do_buffer_step(s, c);
        }
    }
}

void Lexer::read_comment(){
    std::stack<char> st;

    while(true){
        char c = buffer_.next_char();
        if (c == '{'){
            st.push(c);
        } else if (c == '}'){
            if (st.top() == '{'){
                st.pop();
            } else{
                throw IncorrectCommentException(buffer_.line(), buffer_.column());
            }
        } else if (c == '('){
            c = buffer_.peek();
            if (c == '*'){
                c = buffer_.next_char();
                st.push(c);
            } else{
                if (st.empty()){
                    buffer_.return_char('(');
                    break;
                }
                continue;
            }
        } else if (c == '*'){
            c = buffer_.next_char();
            if (c == ')'){
                if (st.top() == '*'){
                    st.pop();
                } else{
                    throw IncorrectCommentException(buffer_.line(), buffer_.column());
                }
            }
        } else if (c == '/'){
            c = buffer_.peek();
            if (c == '/'){
                c = buffer_.next_char();
                while (c != '\n' && c != std::char_traits<int>::eof()){
                    c = buffer_.next_char();
                }
            } else{
                if (st.empty()){
                    buffer_.return_char('/');
                    break;
                }
                continue;
            }
        } else if (c == std::char_traits<int>::eof()){
            if (!st.empty()){
                throw UnterminatedCommentException(buffer_.line(), buffer_.column());
            }
            return;
        }
        if (st.empty()){
            return;
        }
    }
}
