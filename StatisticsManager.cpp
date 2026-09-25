#include "StatisticsManager.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

StatisticsManager::StatisticsManager(const MediaLibrary& libraryIn) : library(libraryIn) {}

std::vector<AbstractMediaContent*> StatisticsManager::rankedByPlayCount() const {
    std::vector<AbstractMediaContent*> all = library.getAll();
    std::sort(all.begin(), all.end(), [](AbstractMediaContent* a, AbstractMediaContent* b) {
        if (a->getPlayCount() != b->getPlayCount()) {
            return a->getPlayCount() > b->getPlayCount(); // descending by plays
        }
        if (a->getTitle() != b->getTitle()) {
            return a->getTitle() < b->getTitle(); // deterministic tie-break #1
        }
        return a->getId() < b->getId(); // deterministic tie-break #2
    });
    return all;
}

void StatisticsManager::displayAllPlayCounts() const {
    auto ranked = rankedByPlayCount();
    if (ranked.empty()) {
        std::cout << "(No media in library)\n";
        return;
    }
    for (auto* media : ranked) {
        std::cout << std::left << std::setw(24) << media->getTitle()
                   << " " << media->getPlayCount() << " plays\n";
    }
}

void StatisticsManager::displayMostListened() const {
    auto ranked = rankedByPlayCount();
    std::cout << "========== MOST LISTENED ==========\n";
    if (ranked.empty()) {
        std::cout << "(No media in library)\n";
        return;
    }
    for (std::size_t i = 0; i < ranked.size(); ++i) {
        std::cout << (i + 1) << ". " << std::left << std::setw(20) << ranked[i]->getTitle()
                   << " " << ranked[i]->getPlayCount() << " plays\n";
    }
}

void StatisticsManager::displayTopN(std::size_t n) const {
    auto ranked = rankedByPlayCount();
    std::size_t limit = std::min(n, ranked.size());
    if (limit == 0) {
        std::cout << "(No media in library)\n";
        return;
    }
    for (std::size_t i = 0; i < limit; ++i) {
        std::cout << (i + 1) << ". " << std::left << std::setw(20) << ranked[i]->getTitle()
                   << " " << ranked[i]->getPlayCount() << " plays\n";
    }
}
