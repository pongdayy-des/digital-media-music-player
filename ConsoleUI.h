#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "MediaLibrary.h"
#include "PlaylistManager.h"
#include "MusicPlayer.h"
#include "StatisticsManager.h"

// ConsoleUI
// ---------------------------------------------------------------------------
// Presentation layer only: displays menus, reads/validates raw input, and
// forwards user intent to the appropriate manager (MediaLibrary,
// PlaylistManager, MusicPlayer, StatisticsManager, FileManager). It holds
// no business rules of its own - e.g. it never decides whether an ID is a
// duplicate; it calls MediaLibrary::addMedia() and reports the boolean
// result.
class ConsoleUI {
private:
    MediaLibrary library;
    PlaylistManager playlistManager;
    MusicPlayer player;
    StatisticsManager stats;

    void mediaLibraryMenu();
    void playlistMenu();
    void playerMenu();
    void statisticsMenu();

    void addMediaFlow();
    void removeMediaFlow();
    void searchMediaFlow();
    void filterMediaFlow();

    void createPlaylistFlow();
    void renamePlaylistFlow();
    void addMediaToPlaylistFlow();
    void removeMediaFromPlaylistFlow();
    void reorderPlaylistFlow();
    void deletePlaylistFlow();

    void saveAll();
    void loadAll();

public:
    ConsoleUI();
    void run();
};

#endif // CONSOLE_UI_H
