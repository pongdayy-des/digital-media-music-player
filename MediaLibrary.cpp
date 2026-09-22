#include "MediaLibrary.h"
#include "Song.h"
#include "Podcast.h"
#include "Audiobook.h"
#include <algorithm>
#include <iostream>
#include <cctype>

namespace {
std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}

bool containsIgnoreCase(const std::string& haystack, const std::string& needleLower) {
    return toLower(haystack).find(needleLower) != std::string::npos;
}
} // namespace

void MediaLibrary::setPreRemoveCallback(PreRemoveCallback callback) {
    preRemoveCallback = std::move(callback);
}

bool MediaLibrary::addMedia(std::unique_ptr<AbstractMediaContent> media) {
    if (!media) return false;
    if (exists(media->getId())) return false; // reject duplicate ID
    items.push_back(std::move(media));
    return true;
}

bool MediaLibrary::removeMedia(const std::string& id) {
    auto it = std::find_if(items.begin(), items.end(),
        [&id](const std::unique_ptr<AbstractMediaContent>& m) { return m->getId() == id; });
    if (it == items.end()) return false;

    // Notify listeners BEFORE destroying the object so no dangling
    // reference can ever be observed by another module.
    if (preRemoveCallback) {
        preRemoveCallback(id);
    }
    items.erase(it); // unique_ptr destructor frees the media (RAII)
    return true;
}

bool MediaLibrary::exists(const std::string& id) const {
    return findById(id) != nullptr;
}

bool MediaLibrary::empty() const { return items.empty(); }
std::size_t MediaLibrary::size() const { return items.size(); }

AbstractMediaContent* MediaLibrary::findById(const std::string& id) const {
    for (const auto& item : items) {
        if (item->getId() == id) return item.get();
    }
    return nullptr;
}

std::vector<AbstractMediaContent*> MediaLibrary::search(const std::string& keyword) const {
    std::vector<AbstractMediaContent*> result;
    const std::string needle = toLower(keyword);
    for (const auto& item : items) {
        bool matched = containsIgnoreCase(item->getId(), needle) ||
                       containsIgnoreCase(item->getTitle(), needle);
        if (!matched) {
            if (const auto* song = dynamic_cast<Song*>(item.get())) {
                matched = containsIgnoreCase(song->getArtist(), needle);
            } else if (const auto* podcast = dynamic_cast<Podcast*>(item.get())) {
                matched = containsIgnoreCase(podcast->getHost(), needle);
            } else if (const auto* book = dynamic_cast<Audiobook*>(item.get())) {
                matched = containsIgnoreCase(book->getAuthor(), needle);
            }
        }
        if (matched) result.push_back(item.get());
    }
    return result;
}

std::vector<AbstractMediaContent*> MediaLibrary::filterByType(const std::string& type) const {
    std::vector<AbstractMediaContent*> result;
    for (const auto& item : items) {
        if (item->getType() == type) result.push_back(item.get());
    }
    return result;
}

std::vector<AbstractMediaContent*> MediaLibrary::getAll() const {
    std::vector<AbstractMediaContent*> result;
    result.reserve(items.size());
    for (const auto& item : items) result.push_back(item.get());
    return result;
}

void MediaLibrary::displayAll() const {
    if (items.empty()) {
        std::cout << "(Media library is empty)\n";
        return;
    }
    for (const auto& item : items) {
        item->displayInfo(); // polymorphic dispatch to Song/Podcast/Audiobook
    }
}
