#include "Engine.h"
#include "tak/Position.h"
#include "MonteCarlo.h"

static constexpr int winValue = 100;
static constexpr int infinity = 10001; // Not really infinity, but pretty high

// Returns a score from the point of view of the player to move
int Engine::evaluate(const Position& position)
{
    auto result = position.checkResult();
    auto player = position.getPlayer();
    int colour = player == Player::White ? 1 : -1;

    if (result != Result::None)
    {
        if (result == Result::Draw)
            return 0;

        if ((player == Player::Black) == (result & StoneBits::Black))
            return winValue;

        return -winValue;
    }

    auto flatCounts = position.checkFlatCount();
    int score = 0;
    score += static_cast<int>(flatCounts[Player::White]) * 2; // A top flat on the board is worth two points
    score -= static_cast<int>(flatCounts[Player::Black]) * 2;

    auto reserveCounts = position.getReserveCount();
    score -= reserveCounts[Player::White]; // A flat on the board is worth one point
    score += reserveCounts[Player::Black];

    return score * colour;
}

std::string Engine::chooseMove(const Position& position)
{
    return chooseMoveRandom(position);
}

std::string Engine::chooseMoveFirst(const Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto move = moves.front();
    return moveToPtn(move, position.size());
}

std::string Engine::chooseMoveRandom(const Position& position)
{
    assert(position.checkResult() == Result::None);
    auto moves = position.generateMoves();
    auto randomMove = chooseRandomElement(moves);
    return moveToPtn(*randomMove, position.size());
}

std::string Engine::chooseMoveNegamax(const Position& position, int depth)
{
    auto moves = position.generateMoves();

    int bestScore = -infinity;
    Move* bestMove = nullptr;
    for (auto& move : moves)
    {
        auto nextPosition = Position(position);
        nextPosition.play(move);

        int score = -negamax(nextPosition, depth, -infinity, infinity, 1);
        if (score > bestScore)
        {
            bestScore = score;
            bestMove = &move;
        }
    }

    return moveToPtn(*bestMove, position.size());
}

int Engine::negamax(const Position& position, int depth, int alpha, int beta, int colour)
{
    if (depth == 0 || position.checkResult() != Result::None)
        // depth !=0 => game is over, so we multiply to make early wins more positive, and early losses more negative
        return evaluate(position) * (depth + 1);

    auto moves = position.generateMoves();
    // auto orderedMoves = orderMoves(moves);

    int score = -infinity;
    for (auto& move : moves)
    {
        Position nextPosition(position);
        nextPosition.play(move);

        score = std::max(score, -negamax(nextPosition, depth - 1, beta * -1, alpha * -1, colour * -1));
        alpha = std::max(alpha, score);
        if (alpha >= beta)
           break; // Alpha Beta Cutoff Kapow!
    }

    return score;
}
