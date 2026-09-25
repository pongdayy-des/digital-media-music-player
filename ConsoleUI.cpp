#include "ConsoleUI.h"
#include "Song.h"
#include "Podcast.h"
#include "Audiobook.h"
#include "FileManager.h"
#include <iostream>
#include <limits>
#include <string>

namespace {

// ---- Input helpers ---------------------------------------------------------
// Centralized here so the "cin >> number then getline() skips input" bug
// (Section XXVI of the spec) never happens: every read funnels through
// std::getline on a fresh line, and numeric parsing is done manually.

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::string readNonEmptyLine(const std::string& prompt) {
    while (true) {
        std::string line = readLine(prompt);
        if (!line.empty()) return line;
        std::cout << "Input cannot be empty. Please try again.\n";
    }
}

int readInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::string line = readLine(prompt);
        try {
            std::size_t consumed = 0;
            int value = std::stoi(line, &consumed);
            if (consumed != line.size()) throw std::invalid_argument("trailing chars");
            if (value < minValue || value > maxValue) {
                std::cout << "Please enter a number between " << minValue << " and "
                           << maxValue << ".\n";
                continue;
            }
            return value;
        } catch (const std::exception&) {
            std::cout << "Invalid input. Please enter a valid number.\n";
        }
    }
}

int readNonNegativeInt(const std::string& prompt) {
    while (true) {
        std::string line = readLine(prompt);
        try {
            std::size_t consumed = 0;
            int value = std::stoi(line, &consumed);
            if (consumed != line.size() || value < 0) throw std::invalid_argument("bad");
            return value;
        } catch (const std::exception&) {
            std::cout << "Invalid input. Please enter a non-negative number.\n";
        }
    }
}

char readMenuChar(const std::string& prompt) {
    std::string line = readLine(prompt);
    if (line.empty()) return '\0';
    char c = line[0];
    if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
    return c;
}

} // namespace

ConsoleUI::ConsoleUI() : stats(library) {
    // Wire MediaLibrary -> PlaylistManager so that deleting a media item
    // always purges dangling references from every playlist BEFORE the
    // object is destroyed (see MediaLibrary.h / Playlist.h for rationale).
    library.setPreRemoveCallback([this](const std::string& removedId) {
        playlistManager.onMediaRemoved(removedId);
    });
}

void ConsoleUI::run() {
    loadAll();
    bool running = true;
    while (running) {
        std::cout << "\n========================================\n";
        std::cout << "       DIGITAL MEDIA & MUSIC PLAYER\n";
        std::cout << "========================================\n";
        std::cout << "1. Media Library\n";
        std::cout << "2. Playlist Management\n";
        std::cout << "3. Music Player\n";
        std::cout << "4. Statistics\n";
        std::cout << "5. Save Data\n";
        std::cout << "0. Exit\n";
        int choice = readInt("Choose an option: ", 0, 5);
        switch (choice) {
            case 1: mediaLibraryMenu(); break;
            case 2: playlistMenu(); break;
            case 3: playerMenu(); break;
            case 4: statisticsMenu(); break;
            case 5: saveAll(); break;
            case 0:
                std::cout << "Save data before exiting? (y/n): ";
                {
                    std::string ans;
                    std::getline(std::cin, ans);
                    if (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y')) saveAll();
                }
                running = false;
                std::cout << "Goodbye!\n";
                break;
            default: break;
        }
    }
}

// ---------------------------------------------------------------- MEDIA ----
void ConsoleUI::mediaLibraryMenu() {
    bool back = false;
    while (!back) {
        std::cout << "\n--- MEDIA LIBRARY ---\n";
        std::cout << "1. Show all media\n2. Add media\n3. Remove media\n"
                     "4. Search media\n5. Filter media\n0. Back\n";
        int choice = readInt("Choose an option: ", 0, 5);
        switch (choice) {
            case 1: library.displayAll(); break;
            case 2: addMediaFlow(); break;
            case 3: removeMediaFlow(); break;
            case 4: searchMediaFlow(); break;
            case 5: filterMediaFlow(); break;
            case 0: back = true; break;
            default: break;
        }
    }
}

void ConsoleUI::addMediaFlow() {
    std::cout << "Type: 1) Song  2) Podcast  3) Audiobook\n";
    int type = readInt("Choose type: ", 1, 3);

    std::string id = readNonEmptyLine("ID: ");
    if (library.exists(id)) {
        std::cout << "Error: ID '" << id << "' already exists.\n";
        return;
    }
    std::string title = readNonEmptyLine("Title: ");
    int duration = readNonNegativeInt("Duration (seconds): ");

    std::unique_ptr<AbstractMediaContent> media;
    if (type == 1) {
        std::string artist = readNonEmptyLine("Artist: ");
        std::string album = readLine("Album: ");
        std::string genre = readLine("Genre: ");
        media = std::make_unique<Song>(id, title, duration, artist, album, genre);
    } else if (type == 2) {
        std::string host = readNonEmptyLine("Host: ");
        int episode = readNonNegativeInt("Episode number: ");
        std::string description = readLine("Description: ");
        media = std::make_unique<Podcast>(id, title, duration, host, episode, description);
    } else {
        std::string author = readNonEmptyLine("Author: ");
        std::string narrator = readLine("Narrator: ");
        int chapter = readNonNegativeInt("Chapter: ");
        media = std::make_unique<Audiobook>(id, title, duration, author, narrator, chapter);
    }

    if (library.addMedia(std::move(media))) {
        std::cout << "Media added successfully.\n";
    } else {
        std::cout << "Error: failed to add media (duplicate ID).\n";
    }
}

void ConsoleUI::removeMediaFlow() {
    if (library.empty()) {
        std::cout << "Library is empty.\n";
        return;
    }
    std::string id = readNonEmptyLine("Enter ID to remove: ");
    if (library.removeMedia(id)) {
        std::cout << "Media removed. Any playlist references to it were also cleaned up.\n";
    } else {
        std::cout << "Error: ID not found.\n";
    }
}

void ConsoleUI::searchMediaFlow() {
    std::string keyword = readNonEmptyLine("Search keyword: ");
    auto results = library.search(keyword);
    if (results.empty()) {
        std::cout << "No matches found.\n";
        return;
    }
    for (auto* m : results) m->displayInfo();
}

void ConsoleUI::filterMediaFlow() {
    std::cout << "Type: 1) Song  2) Podcast  3) Audiobook\n";
    int type = readInt("Choose type: ", 1, 3);
    std::string typeName = (type == 1) ? "Song" : (type == 2) ? "Podcast" : "Audiobook";
    auto results = library.filterByType(typeName);
    if (results.empty()) {
        std::cout << "No " << typeName << " found.\n";
        return;
    }
    for (auto* m : results) m->displayInfo();
}

// ------------------------------------------------------------- PLAYLIST ----
void ConsoleUI::playlistMenu() {
    bool back = false;
    while (!back) {
        std::cout << "\n--- PLAYLIST ---\n";
        std::cout << "1. Show playlists\n2. Create playlist\n3. Rename playlist\n"
                     "4. Add media\n5. Remove media\n6. Reorder media\n"
                     "7. Delete playlist\n0. Back\n";
        int choice = readInt("Choose an option: ", 0, 7);
        switch (choice) {
            case 1: playlistManager.listPlaylists(); break;
            case 2: createPlaylistFlow(); break;
            case 3: renamePlaylistFlow(); break;
            case 4: addMediaToPlaylistFlow(); break;
            case 5: removeMediaFromPlaylistFlow(); break;
            case 6: reorderPlaylistFlow(); break;
            case 7: deletePlaylistFlow(); break;
            case 0: back = true; break;
            default: break;
        }
    }
}

void ConsoleUI::createPlaylistFlow() {
    std::string id = readNonEmptyLine("Playlist ID: ");
    if (playlistManager.exists(id)) {
        std::cout << "Error: playlist ID already exists.\n";
        return;
    }
    std::string name = readNonEmptyLine("Playlist name: ");
    if (playlistManager.createPlaylist(id, name) != nullptr) {
        std::cout << "Playlist created.\n";
    } else {
        std::cout << "Error: failed to create playlist.\n";
    }
}

void ConsoleUI::renamePlaylistFlow() {
    std::string id = readNonEmptyLine("Playlist ID: ");
    Playlist* p = playlistManager.findById(id);
    if (p == nullptr) { std::cout << "Error: playlist not found.\n"; return; }
    std::string newName = readNonEmptyLine("New name: ");
    p->rename(newName);
    std::cout << "Playlist renamed.\n";
}

void ConsoleUI::addMediaToPlaylistFlow() {
    std::string pid = readNonEmptyLine("Playlist ID: ");
    Playlist* p = playlistManager.findById(pid);
    if (p == nullptr) { std::cout << "Error: playlist not found.\n"; return; }

    std::string mid = readNonEmptyLine("Media ID to add: ");
    AbstractMediaContent* media = library.findById(mid);
    if (media == nullptr) { std::cout << "Error: media not found in library.\n"; return; }

    if (p->addTrack(media)) {
        std::cout << "Media added to playlist.\n";
    } else {
        std::cout << "Error: media already in playlist.\n";
    }
}

void ConsoleUI::removeMediaFromPlaylistFlow() {
    std::string pid = readNonEmptyLine("Playlist ID: ");
    Playlist* p = playlistManager.findById(pid);
    if (p == nullptr) { std::cout << "Error: playlist not found.\n"; return; }
    if (p->empty()) { std::cout << "Playlist is empty.\n"; return; }

    p->display();
    int index = readInt("Enter track number to remove: ", 1, static_cast<int>(p->size()));
    if (p->removeTrackAt(static_cast<std::size_t>(index - 1))) {
        std::cout << "Track removed from playlist.\n";
    } else {
        std::cout << "Error: invalid track number.\n";
    }
}

void ConsoleUI::reorderPlaylistFlow() {
    std::string pid = readNonEmptyLine("Playlist ID: ");
    Playlist* p = playlistManager.findById(pid);
    if (p == nullptr) { std::cout << "Error: playlist not found.\n"; return; }
    if (p->size() < 2) { std::cout << "Need at least 2 tracks to reorder.\n"; return; }

    p->display();
    int from = readInt("Move track number: ", 1, static_cast<int>(p->size()));
    int to = readInt("To position: ", 1, static_cast<int>(p->size()));
    if (p->reorder(static_cast<std::size_t>(from - 1), static_cast<std::size_t>(to - 1))) {
        std::cout << "Reordered.\n";
    } else {
        std::cout << "Error: could not reorder.\n";
    }
}

void ConsoleUI::deletePlaylistFlow() {
    std::string id = readNonEmptyLine("Playlist ID to delete: ");
    if (playlistManager.deletePlaylist(id)) {
        std::cout << "Playlist deleted.\n";
    } else {
        std::cout << "Error: playlist not found.\n";
    }
}

// --------------------------------------------------------------- PLAYER ----
void ConsoleUI::playerMenu() {
    std::string pid = readNonEmptyLine("Enter playlist ID to load: ");
    Playlist* p = playlistManager.findById(pid);
    if (p == nullptr) { std::cout << "Error: playlist not found.\n"; return; }
    if (p->empty()) { std::cout << "Playlist is empty; nothing to play.\n"; return; }

    player.loadPlaylist(p);
    player.play();

    bool back = false;
    while (!back) {
        std::cout << "\n";
        player.displayNowPlaying();
        std::cout << "P - Play/Pause   N - Next   B - Previous   M - Change Mode   Q - Back\n";
        char c = readMenuChar("Command: ");
        switch (c) {
            case 'P':
                if (player.getState() == PlaybackState::PLAYING) player.pause();
                else player.play();
                break;
            case 'N':
                player.next();
                break;
            case 'B':
                player.previous();
                break;
            case 'M': {
                std::cout << "Mode: 1) Sequential  2) Shuffle  3) Repeat\n";
                int m = readInt("Choose mode: ", 1, 3);
                player.setMode(m == 1 ? PlaybackMode::SEQUENTIAL :
                                m == 2 ? PlaybackMode::SHUFFLE : PlaybackMode::REPEAT);
                break;
            }
            case 'Q':
                back = true;
                break;
            default:
                std::cout << "Unknown command.\n";
                break;
        }
        if (!back && player.getState() == PlaybackState::PLAYING) {
            player.tick(); // simulate time passing while browsing the menu
        }
    }
}

// ----------------------------------------------------------- STATISTICS ----
void ConsoleUI::statisticsMenu() {
    bool back = false;
    while (!back) {
        std::cout << "\n--- STATISTICS ---\n";
        std::cout << "1. Show play counts\n2. Most listened\n3. Top 5\n0. Back\n";
        int choice = readInt("Choose an option: ", 0, 3);
        switch (choice) {
            case 1: stats.displayAllPlayCounts(); break;
            case 2: stats.displayMostListened(); break;
            case 3: stats.displayTopN(5); break;
            case 0: back = true; break;
            default: break;
        }
    }
}

// ------------------------------------------------------------- PERSIST -----
void ConsoleUI::saveAll() {
    bool okMedia = FileManager::saveMedia(library, "data/media.txt");
    bool okPlaylists = FileManager::savePlaylists(playlistManager, "data/playlists.txt");
    if (okMedia && okPlaylists) std::cout << "Data saved successfully.\n";
}

void ConsoleUI::loadAll() {
    FileManager::loadMedia(library, "data/media.txt");
    FileManager::loadPlaylists(playlistManager, library, "data/playlists.txt");
}
