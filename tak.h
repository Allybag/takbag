#include <cstddef>

#include <vector>
#include <string>
#include <unordered_map>

enum class Player
{
    White,
    Black
};

enum class Stone : uint8_t
{
    Blank = 0,
    WhiteFlat = 1,
    WhiteWall = 2,
    WhiteCap  = 3,
    BlackFlat = 5,
    BlackWall = 6,
    BlackCap  = 7,
};

template <typename Value>
struct PlayerPair
{
    Value White;
    Value Black;

    explicit PlayerPair(Value value) : PlayerPair(value, value) { }
    PlayerPair(Value white, Value black) : White(white), Black(black) { }

    PlayerPair() = delete;
    PlayerPair(const PlayerPair&) = default;
    PlayerPair(PlayerPair&&) = default;
    PlayerPair& operator=(const PlayerPair&) = default;
    PlayerPair& operator=(PlayerPair&&) = default;
    ~PlayerPair() = default;


    Value& operator[](Player player) { return player == Player::White ? White : Black; }
    const Value& operator[](Player player) const { return player == Player::White ? White : Black; }

};

struct Square
{
    Stone topStone;
    uint8_t reserveCount;
    uint32_t stack; // A reserveCount long bitSet, 0 is white and black is flat

    std::size_t count() const
    {
        return topStone == Stone::Blank ? 0 : reserveCount + 1;
    }

    Square() : topStone(Stone::Blank), reserveCount(0), stack(0) { }

};

class Position
{
    const std::size_t mSize;
    std::vector<Square> mBoard;
    Player mToPlay;

    // On playtak.com these are now customisable, but the rules say:
    // 4x4 -> 15 flats and no caps, 5x5 -> 21 flats and 1 cap, 6x6 -> 30 flats and 1 cap, 8x8 -> 50 flats and 2 caps
    PlayerPair<std::size_t> mFlatReserves {21, 21};
    PlayerPair<std::size_t> mCapReserves {1, 1};

    explicit Position(std::size_t size) : mSize(size), mToPlay(Player::White) { }
};
