#pragma once

class LexerException : public std::exception {
protected:
    std::string m_msg;
public:

    LexerException(int line, int column) {
        m_msg = ("LexerError at line "+std::to_string(line)+" column "+std::to_string(column));
    }

    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }
};

class IncorrectOperatorException : public LexerException {
public:
    IncorrectOperatorException(int line, int column) : LexerException(line, column) {
        m_msg = ("LexerError: Incorrect operator at line "+std::to_string(line)+" column "+std::to_string(column));
    }
};

class IncorrectCharacterException : public LexerException {
public:
    IncorrectCharacterException(int line, int column) : LexerException(line, column) {
        m_msg = "LexerError: Incorrect character at line "+std::to_string(line)+" column "+std::to_string(column);
    }
};

class UnterminatedStringException : public LexerException {
public:
    UnterminatedStringException(int line, int column) : LexerException(line, column) {
        m_msg = ("LexerError: Unterminated string at line "+std::to_string(line)+" column "+std::to_string(column));
    }
};

class UnterminatedCommentException : public LexerException {
public:
    UnterminatedCommentException(int line, int column) : LexerException(line, column) {
        m_msg = ("LexerError: Unterminated comment at line "+std::to_string(line)+" column "+std::to_string(column));
    }
};

class OutOfRangeNumberException : public LexerException {
public:
    OutOfRangeNumberException(int line, int column) : LexerException(line, column) {
        m_msg = ("LexerError: Out of range at line "+std::to_string(line)+" column "+std::to_string(column));
    }
};
