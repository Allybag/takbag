#include "tak.h"

#include <iostream>

int main()
{
    Position position(6);
    position.place(Place(0, Stone::WhiteFlat));
    position.place(Place(5, Stone::BlackFlat));
    position.place(Place(14, Stone::WhiteFlat));
    position.place(Place(15, Stone::BlackFlat));

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
