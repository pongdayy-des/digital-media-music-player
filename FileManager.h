#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include "MediaLibrary.h"
#include "PlaylistManager.h"

// FileManager
// ---------------------------------------------------------------------------
// Responsible ONLY for reading/writing the plain-text persistence format.
// Never stores raw pointers or memory addresses - only IDs and primitive
// values, which is the only thing that can safely survive a program
// restart.
//
// Load order (required for reference resolution):
//   1. Load media.txt into MediaLibrary.
//   2. Load playlists.txt: for each media id referenced, look it up in the
//      already-populated MediaLibrary via findById() and store the
//      resulting non-owning pointer in the Playlist.
//
// Robustness: a malformed line/record is reported and skipped; it never
// aborts the whole load. Unknown media ids referenced by a playlist are
// skipped with a warning, since MediaLibrary is the single source of truth
// for what media actually exists.
class FileManager {
public:
    static bool saveMedia(const MediaLibrary& library, const std::string& path);
    static bool loadMedia(MediaLibrary& library, const std::string& path);

    static bool savePlaylists(const PlaylistManager& manager, const std::string& path);
    static bool loadPlaylists(PlaylistManager& manager, const MediaLibrary& library,
                               const std::string& path);
};

#endif // FILE_MANAGER_H
