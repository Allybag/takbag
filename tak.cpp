#include "tak.h"

#include <iostream>

int main()
{
    std::size_t ply = 0;
    Position position(6);
    position.play(PtnTurn("a1", ++ply));
    position.play(PtnTurn("a6", ++ply));
    position.play(PtnTurn("c3", ++ply));
    position.play(PtnTurn("c4", ++ply));
    std::cout << position.print();
    return 0;

    std::cout << position.print();
    position.move(Move(14, 1, 1, Direction::Right));
    position.move(Move(5, 1, 1, Direction::Down));

    position.place(Place(14, Stone::WhiteCap));
    position.place(Place(17, Stone::BlackCap));
    position.move(Move(14, 1, 1, Direction::Right));
    position.place(Place(5, Stone::BlackWall));
    std::cout << position.print();
    position.move(Move(15, 3, 0x111, Direction::Left));

    std::cout << position.print();
    return 0;
}
