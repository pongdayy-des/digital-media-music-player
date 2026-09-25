#include "FileManager.h"
#include "Song.h"
#include "Podcast.h"
#include "Audiobook.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace {

// Splits a line by '|' into fields. A trailing empty field caused by a
// trailing '|' is preserved (important for optional/empty description).
std::vector<std::string> splitFields(const std::string& line, char delim) {
    std::vector<std::string> fields;
    std::string current;
    std::istringstream stream(line);
    while (std::getline(stream, current, delim)) {
        fields.push_back(current);
    }
    // std::getline with a delimiter drops a truly-empty trailing field
    // (e.g. "a|b|" -> ["a","b"] instead of ["a","b",""]); restore it so
    // field counts stay predictable for optional trailing text fields.
    if (!line.empty() && line.back() == delim) {
        fields.emplace_back("");
    }
    return fields;
}

bool parseInt(const std::string& text, int& out) {
    if (text.empty()) return false;
    try {
        std::size_t consumed = 0;
        int value = std::stoi(text, &consumed);
        if (consumed != text.size()) return false;
        out = value;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace

bool FileManager::saveMedia(const MediaLibrary& library, const std::string& path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) {
        std::cout << "Error: could not open '" << path << "' for writing.\n";
        return false;
    }
    for (AbstractMediaContent* media : library.getAll()) {
        out << media->getType() << "|" << media->getId() << "|" << media->getTitle() << "|"
            << media->getDuration() << "|" << media->getPlayCount() << "|"
            << media->serializeExtra() << "\n";
    }
    return true;
}

bool FileManager::loadMedia(MediaLibrary& library, const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cout << "Note: '" << path << "' not found. Starting with an empty library.\n";
        return false;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    int errorCount = 0;

    while (std::getline(in, line)) {
        ++lineNumber;
        if (line.empty()) continue; // tolerate blank lines

        std::vector<std::string> f = splitFields(line, '|');
        if (f.size() < 5) {
            std::cout << "Warning: malformed record at line " << lineNumber
                       << " (too few fields), skipped.\n";
            ++errorCount;
            continue;
        }

        const std::string& type = f[0];
        const std::string& id = f[1];
        const std::string& title = f[2];
        int duration = 0, playCount = 0;
        if (!parseInt(f[3], duration) || duration < 0) {
            std::cout << "Warning: invalid duration at line " << lineNumber << ", skipped.\n";
            ++errorCount;
            continue;
        }
        if (!parseInt(f[4], playCount) || playCount < 0) {
            std::cout << "Warning: invalid play count at line " << lineNumber << ", skipped.\n";
            ++errorCount;
            continue;
        }
        if (id.empty() || title.empty()) {
            std::cout << "Warning: empty id/title at line " << lineNumber << ", skipped.\n";
            ++errorCount;
            continue;
        }
        if (library.exists(id)) {
            std::cout << "Warning: duplicate id '" << id << "' at line " << lineNumber
                       << ", skipped.\n";
            ++errorCount;
            continue;
        }

        std::unique_ptr<AbstractMediaContent> media;
        if (type == "Song") {
            if (f.size() < 8) { ++errorCount; std::cout << "Warning: malformed Song at line " << lineNumber << ", skipped.\n"; continue; }
            media = std::make_unique<Song>(id, title, duration, f[5], f[6], f[7]);
        } else if (type == "Podcast") {
            if (f.size() < 8) { ++errorCount; std::cout << "Warning: malformed Podcast at line " << lineNumber << ", skipped.\n"; continue; }
            int episode = 0;
            if (!parseInt(f[6], episode)) { ++errorCount; std::cout << "Warning: invalid episode number at line " << lineNumber << ", skipped.\n"; continue; }
            media = std::make_unique<Podcast>(id, title, duration, f[5], episode, f[7]);
        } else if (type == "Audiobook") {
            if (f.size() < 8) { ++errorCount; std::cout << "Warning: malformed Audiobook at line " << lineNumber << ", skipped.\n"; continue; }
            int chapter = 0;
            if (!parseInt(f[7], chapter)) { ++errorCount; std::cout << "Warning: invalid chapter at line " << lineNumber << ", skipped.\n"; continue; }
            media = std::make_unique<Audiobook>(id, title, duration, f[5], f[6], chapter);
        } else {
            std::cout << "Warning: unknown media type '" << type << "' at line " << lineNumber
                       << ", skipped.\n";
            ++errorCount;
            continue;
        }

        // Restore the persisted play count (constructor initializes to 0).
        for (int i = 0; i < playCount; ++i) media->incrementPlayCount();

        if (library.addMedia(std::move(media))) {
            ++loadedCount;
        } else {
            ++errorCount;
        }
    }

    std::cout << "Loaded " << loadedCount << " media item(s) from '" << path << "'";
    if (errorCount > 0) std::cout << " (" << errorCount << " record(s) skipped due to errors)";
    std::cout << ".\n";
    return true;
}

bool FileManager::savePlaylists(const PlaylistManager& manager, const std::string& path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) {
        std::cout << "Error: could not open '" << path << "' for writing.\n";
        return false;
    }
    for (Playlist* playlist : manager.getAll()) {
        out << playlist->getId() << "|" << playlist->getName() << "|";
        const auto& tracks = playlist->getTracks();
        for (std::size_t i = 0; i < tracks.size(); ++i) {
            out << tracks[i]->getId();
            if (i + 1 < tracks.size()) out << ",";
        }
        out << "\n";
    }
    return true;
}

bool FileManager::loadPlaylists(PlaylistManager& manager, const MediaLibrary& library,
                                 const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cout << "Note: '" << path << "' not found. No playlists loaded.\n";
        return false;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    int errorCount = 0;

    while (std::getline(in, line)) {
        ++lineNumber;
        if (line.empty()) continue;

        std::vector<std::string> f = splitFields(line, '|');
        if (f.size() < 2) {
            std::cout << "Warning: malformed playlist record at line " << lineNumber
                       << ", skipped.\n";
            ++errorCount;
            continue;
        }
        const std::string& id = f[0];
        const std::string& name = f[1];
        if (id.empty()) {
            std::cout << "Warning: empty playlist id at line " << lineNumber << ", skipped.\n";
            ++errorCount;
            continue;
        }
        if (manager.exists(id)) {
            std::cout << "Warning: duplicate playlist id '" << id << "' at line " << lineNumber
                       << ", skipped.\n";
            ++errorCount;
            continue;
        }

        Playlist* playlist = manager.createPlaylist(id, name);
        if (playlist == nullptr) {
            ++errorCount;
            continue;
        }

        if (f.size() >= 3 && !f[2].empty()) {
            std::istringstream idsStream(f[2]);
            std::string mediaId;
            while (std::getline(idsStream, mediaId, ',')) {
                if (mediaId.empty()) continue;
                AbstractMediaContent* media = library.findById(mediaId);
                if (media == nullptr) {
                    std::cout << "Warning: playlist '" << id << "' references unknown media id '"
                               << mediaId << "', reference skipped.\n";
                    continue;
                }
                playlist->addTrack(media);
            }
        }
        ++loadedCount;
    }

    std::cout << "Loaded " << loadedCount << " playlist(s) from '" << path << "'";
    if (errorCount > 0) std::cout << " (" << errorCount << " record(s) skipped due to errors)";
    std::cout << ".\n";
    return true;
}
