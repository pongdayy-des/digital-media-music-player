#ifndef MEDIA_LIBRARY_H
#define MEDIA_LIBRARY_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "AbstractMediaContent.h"

// MediaLibrary
// ---------------------------------------------------------------------------
// The single OWNER of every AbstractMediaContent object in the system.
// Media is stored as std::vector<std::unique_ptr<AbstractMediaContent>>:
// each unique_ptr expresses exclusive ownership, and destroying the vector
// (or removing an element) automatically frees the object - this is RAII in
// action, no manual delete anywhere in the codebase.
//
// Every other component (Playlist, MusicPlayer, StatisticsManager, ...)
// that needs to reference a piece of media receives a *non-owning*
// AbstractMediaContent* obtained through find()/getAll(). They must never
// delete it.
class MediaLibrary {
public:
    // Callback invoked with the id of a media item right before it is
    // physically removed from the library, so interested parties (namely
    // PlaylistManager) can purge dangling references from every playlist
    // BEFORE the object disappears. This keeps MediaLibrary decoupled from
    // Playlist: it does not need to know playlists exist at all.
    using PreRemoveCallback = std::function<void(const std::string& removedId)>;

private:
    std::vector<std::unique_ptr<AbstractMediaContent>> items;
    PreRemoveCallback preRemoveCallback;

public:
    MediaLibrary() = default;

    // Non-copyable: a library owns unique resources, copying it would
    // require deep-cloning every media object which is unnecessary here.
    MediaLibrary(const MediaLibrary&) = delete;
    MediaLibrary& operator=(const MediaLibrary&) = delete;

    void setPreRemoveCallback(PreRemoveCallback callback);

    // Takes ownership of a newly-created media object.
    // Returns false (and does not insert) if the id already exists.
    bool addMedia(std::unique_ptr<AbstractMediaContent> media);

    // Removes and destroys the media with the given id.
    // Returns false if the id does not exist.
    bool removeMedia(const std::string& id);

    bool exists(const std::string& id) const;
    bool empty() const;
    std::size_t size() const;

    // Non-owning lookup. Returns nullptr if not found.
    AbstractMediaContent* findById(const std::string& id) const;

    // Case-insensitive substring search across id/title and type-specific
    // fields (artist/author/host).
    std::vector<AbstractMediaContent*> search(const std::string& keyword) const;

    // Filter by exact type name ("Song", "Podcast", "Audiobook").
    std::vector<AbstractMediaContent*> filterByType(const std::string& type) const;

    // Non-owning view of every item currently in the library.
    std::vector<AbstractMediaContent*> getAll() const;

    void displayAll() const;
};

#endif // MEDIA_LIBRARY_H
