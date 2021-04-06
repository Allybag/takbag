#pragma once

#include <vector>
#include <cstddef>

struct DropCountGenerator
{
    uint8_t mTarget;
    uint8_t mMaxDistance;
    bool mEndsInSmash;
    DropCountGenerator(uint8_t target, uint8_t maxDistance, bool endsInSmash) : mTarget(target), mMaxDistance(maxDistance), mEndsInSmash(endsInSmash) { }

    void generateDropCounts(uint8_t sum, uint8_t distance, uint32_t dropCounts, std::vector<uint32_t>& dropCountVec) const;

};

void DropCountGenerator::generateDropCounts(uint8_t sum, uint8_t distance, uint32_t dropCounts, std::vector<uint32_t> &dropCountVec) const
{
    if (sum == mTarget)
        dropCountVec.push_back(dropCounts);

    if (distance == mMaxDistance)
        return;

    if (mEndsInSmash && distance == (mMaxDistance) && sum != (mTarget - 1))
        return;

    for (uint8_t i = 1; i <= (mTarget - sum); ++i)
        generateDropCounts(sum + i, distance + 1, dropCounts + (i << distance * 4), dropCountVec);
}

