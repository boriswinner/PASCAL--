#include <iostream>
#include "lexer.h"

using namespace std;


int main() {
    Lexer lexer("input.txt");
    std::ofstream out("out.txt");
    while (true){
        try{
            Token t = lexer.get_next();
            if (t.type_ == token_types::ENDOFFILE){
                break;
            }
            out << t.print();
        } catch (LexerException& e){
            std::cout << e.what() << std::endl;
        } catch(std::exception& e){
            std::cout << "Unknown exception" << std::endl;;
        }
    }

    return 0;
}