#ifndef STATISTICS_MANAGER_H
#define STATISTICS_MANAGER_H

#include <vector>
#include "AbstractMediaContent.h"
#include "MediaLibrary.h"

// StatisticsManager
// ---------------------------------------------------------------------------
// Stateless analytics over a MediaLibrary (non-owning reference). Computes
// play-count rankings. Ties are broken deterministically by title
// (ascending), then by id, so output is always reproducible.
class StatisticsManager {
private:
    const MediaLibrary& library; // non-owning reference to the library

public:
    explicit StatisticsManager(const MediaLibrary& library);

    void displayAllPlayCounts() const;
    void displayMostListened() const; // full ranking
    void displayTopN(std::size_t n) const;

private:
    std::vector<AbstractMediaContent*> rankedByPlayCount() const;
};

#endif // STATISTICS_MANAGER_H
