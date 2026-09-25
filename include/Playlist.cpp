#include "Playlist.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

Playlist::Playlist(std::string playlistIdIn, std::string nameIn)
    : playlistId(std::move(playlistIdIn)), name(std::move(nameIn)) {}

const std::string& Playlist::getId() const { return playlistId; }
const std::string& Playlist::getName() const { return name; }

void Playlist::rename(const std::string& newName) { name = newName; }

bool Playlist::addTrack(AbstractMediaContent* media) {
    if (media == nullptr) return false;
    bool alreadyPresent = std::any_of(tracks.begin(), tracks.end(),
        [media](AbstractMediaContent* m) { return m == media; });
    if (alreadyPresent) return false;
    tracks.push_back(media);
    return true;
}

bool Playlist::removeTrackAt(std::size_t index) {
    if (index >= tracks.size()) return false;
    tracks.erase(tracks.begin() + static_cast<long>(index));
    return true;
}

bool Playlist::removeReference(const std::string& mediaId) {
    auto originalSize = tracks.size();
    tracks.erase(std::remove_if(tracks.begin(), tracks.end(),
        [&mediaId](AbstractMediaContent* m) { return m->getId() == mediaId; }),
        tracks.end());
    return tracks.size() != originalSize;
}

bool Playlist::reorder(std::size_t from, std::size_t to) {
    if (from >= tracks.size() || to >= tracks.size() || from == to) return false;
    AbstractMediaContent* moved = tracks[from];
    tracks.erase(tracks.begin() + static_cast<long>(from));
    tracks.insert(tracks.begin() + static_cast<long>(to), moved);
    return true;
}

void Playlist::clear() { tracks.clear(); }

bool Playlist::empty() const { return tracks.empty(); }
std::size_t Playlist::size() const { return tracks.size(); }

AbstractMediaContent* Playlist::trackAt(std::size_t index) const {
    if (index >= tracks.size()) return nullptr;
    return tracks[index];
}

const std::vector<AbstractMediaContent*>& Playlist::getTracks() const { return tracks; }

void Playlist::display() const {
    std::cout << "Playlist [" << playlistId << "] " << name
              << " (" << tracks.size() << " tracks)\n";
    if (tracks.empty()) {
        std::cout << "  (empty)\n";
        return;
    }
    for (std::size_t i = 0; i < tracks.size(); ++i) {
        std::cout << "  " << std::setw(2) << (i + 1) << ". "
                  << tracks[i]->getType() << " - " << tracks[i]->getTitle()
                  << " [" << tracks[i]->getId() << "]\n";
    }
}
