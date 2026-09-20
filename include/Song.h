#ifndef SONG_H
#define SONG_H

#include "AbstractMediaContent.h"

// Song : public AbstractMediaContent
// Concrete media type representing a music track.
class Song : public AbstractMediaContent {
private:
    std::string artist;
    std::string album;
    std::string genre;

public:
    Song(std::string id, std::string title, int duration,
         std::string artist, std::string album, std::string genre);

    const std::string& getArtist() const;
    const std::string& getAlbum() const;
    const std::string& getGenre() const;

    void displayInfo() const override;
    std::string getType() const override;
    std::string serializeExtra() const override;
};

#endif // SONG_H
