#pragma once

class LexerException : public std::exception {
protected:
    std::string m_msg;
public:

    LexerException(int line, int column, std::string msg = "") {
        m_msg = "LexerError: " + msg +  " at line " + std::to_string(line) + " column " + std::to_string(column);
    }

    virtual const char *what() const throw() {
        return m_msg.c_str();
    }
};

class IncorrectOperatorException : public LexerException {
public:
    IncorrectOperatorException(int line, int column) : LexerException(line, column, "Incorrect operator") {}
};

class IncorrectCharacterException : public LexerException {
public:
    IncorrectCharacterException(int line, int column) : LexerException(line, column,"Incorrect character") {}
};

class UnterminatedStringException : public LexerException {
public:
    UnterminatedStringException(int line, int column) : LexerException(line, column,"Unterminated string") {}
};

class UnterminatedCommentException : public LexerException {
public:
    UnterminatedCommentException(int line, int column) : LexerException(line, column,"Unterminated comment") {}
};

class OutOfRangeNumberException : public LexerException {
public:
    OutOfRangeNumberException(int line, int column) : LexerException(line, column,"Out of range") {}
};

class IncorrectCommentException : public LexerException {
public:
    IncorrectCommentException(int line, int column) : LexerException(line, column,"Incorrect Comment") {}
};
