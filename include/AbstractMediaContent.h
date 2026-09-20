#ifndef ABSTRACT_MEDIA_CONTENT_H
#define ABSTRACT_MEDIA_CONTENT_H

#include <string>

// AbstractMediaContent
// ---------------------------------------------------------------------------
// Abstract base class for every playable item in the library (Song, Podcast,
// Audiobook, ...). It owns the attributes that are common to ALL media types
// (id, title, duration, playCount) and defines the polymorphic interface
// (displayInfo, getType, serialize) that every concrete subclass must
// implement. Because it declares pure virtual functions, it can never be
// instantiated directly - only through a concrete derived class.
//
// Ownership: instances of concrete subclasses are always owned through
// std::unique_ptr<AbstractMediaContent> inside MediaLibrary. Any other part
// of the system (e.g. Playlist) only ever holds a non-owning raw pointer to
// an object that is owned elsewhere. See Playlist.h for details.
class AbstractMediaContent {
protected:
    std::string id;
    std::string title;
    int duration;   // duration in seconds
    int playCount;

public:
    AbstractMediaContent(std::string id, std::string title, int duration);

    // Non-copyable would be too strict for value members inside vectors of
    // unique_ptr (we never copy AbstractMediaContent itself), but we make
    // the intent explicit: copies of the polymorphic base are disallowed to
    // avoid slicing. Derived classes still get compiler-generated special
    // members for their own data, but a Song can only be duplicated via a
    // dedicated clone-like operation if one is ever needed.
    AbstractMediaContent(const AbstractMediaContent&) = delete;
    AbstractMediaContent& operator=(const AbstractMediaContent&) = delete;

    virtual ~AbstractMediaContent() = default;

    // ---- Encapsulated accessors -------------------------------------------------
    const std::string& getId() const;
    const std::string& getTitle() const;
    int getDuration() const;
    int getPlayCount() const;

    void setTitle(const std::string& newTitle);
    void setDuration(int newDuration);

    // Called exactly once per "new playback started" event by MusicPlayer.
    void incrementPlayCount();

    // ---- Polymorphic interface ---------------------------------------------------
    virtual void displayInfo() const = 0;
    virtual std::string getType() const = 0;

    // Serializes the type-specific fields (NOT id/title/duration/playCount,
    // which FileManager already writes generically) into a single line of
    // '|'-separated fields, ready to be appended to the shared record line.
    virtual std::string serializeExtra() const = 0;
};

#endif // ABSTRACT_MEDIA_CONTENT_H
