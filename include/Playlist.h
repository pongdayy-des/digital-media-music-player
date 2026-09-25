#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>
#include <vector>
#include "AbstractMediaContent.h"

// Playlist
// ---------------------------------------------------------------------------
// OWNERSHIP MODEL (important for the defense):
//   MediaLibrary OWNS every AbstractMediaContent via std::unique_ptr.
//   Playlist NEVER owns media - it only stores non-owning
//   AbstractMediaContent* pointers that reference objects that live inside
//   MediaLibrary. A Playlist is therefore conceptually an ordered list of
//   REFERENCES, similar to a playlist app that stores song IDs rather than
//   copies of the audio file.
//
//   Consequences:
//     - Playlist::~Playlist() does NOT delete anything it points to.
//     - Removing a media item from a Playlist never affects MediaLibrary.
//     - If a media item is removed from MediaLibrary, MediaLibrary invokes
//       a callback (see MediaLibrary::PreRemoveCallback) that
//       PlaylistManager uses to call Playlist::removeReference() on every
//       playlist that referenced it BEFORE the object is destroyed. This is
//       how the system guarantees Playlist never holds a dangling pointer.
class Playlist {
private:
    std::string playlistId;
    std::string name;
    std::vector<AbstractMediaContent*> tracks; // non-owning references

public:
    Playlist(std::string playlistId, std::string name);

    const std::string& getId() const;
    const std::string& getName() const;
    void rename(const std::string& newName);

    // Adds a reference to media owned elsewhere. Does not take ownership.
    // Rejects nullptr and duplicate entries.
    bool addTrack(AbstractMediaContent* media);

    // Removes the reference at logical position (0-based). Does not delete
    // the underlying media (it isn't ours to delete).
    bool removeTrackAt(std::size_t index);

    // Removes every reference to the media with the given id. Used by
    // PlaylistManager when a media item is deleted from the library, and
    // also usable directly by the user ("remove from playlist").
    bool removeReference(const std::string& mediaId);

    // Swaps track order: moves item at 'from' to position 'to'.
    bool reorder(std::size_t from, std::size_t to);

    void clear();

    bool empty() const;
    std::size_t size() const;

    AbstractMediaContent* trackAt(std::size_t index) const;
    const std::vector<AbstractMediaContent*>& getTracks() const;

    void display() const;
};

#endif // PLAYLIST_H
