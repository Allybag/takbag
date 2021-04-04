#include "tak.h"

#include <iostream>

int main()
{
    std::size_t ply = 0;
    Position position(6);
    position.play(PtnTurn("Fa1", ++ply));
    position.play(PtnTurn("a6", ++ply));
    position.play(PtnTurn("c3", ++ply));
    position.play(PtnTurn("fc4", ++ply));

    position.play(PtnTurn("1c3>1", ++ply));
    position.play(PtnTurn("1a6-f", ++ply));

    position.play(PtnTurn("Cc3", ++ply));
    position.play(PtnTurn("Cc6", ++ply));
    position.play(PtnTurn("c3>1", ++ply));
    position.play(PtnTurn("Sa6", ++ply));
    position.play(PtnTurn("3c4<111C", ++ply));


    std::cout << position.print();
    return 0;
}
