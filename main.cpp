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
        } catch (IncorrectOperator& e){
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}