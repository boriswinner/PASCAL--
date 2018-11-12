#include <iostream>
#include "lexer.h"

using namespace std;


int main() {
    Lexer lexer("input.txt");
    std::ofstream out("out.txt");
    out << std::setw(5) << "line" << std::setw(5) << "col" << std::setw(16) << "type" << std::setw(23) << "value" << std::setw(16)
        << "text" << std::endl;
    while (true){
        try{
            Token t = lexer.get_next();
            out << t.print();
            if (t.type_ == token_types::ENDOFFILE){
                break;
            }
        } catch (LexerException& e){
            std::cout << e.what() << std::endl;
        } catch(std::exception& e){
            std::cout << "Unknown exception" << std::endl;;
        }
    }

    return 0;
}