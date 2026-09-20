#include "Song.h"
#include <iostream>
#include <iomanip>

Song::Song(std::string idIn, std::string titleIn, int durationIn,
           std::string artistIn, std::string albumIn, std::string genreIn)
    : AbstractMediaContent(std::move(idIn), std::move(titleIn), durationIn),
      artist(std::move(artistIn)), album(std::move(albumIn)), genre(std::move(genreIn)) {}

const std::string& Song::getArtist() const { return artist; }
const std::string& Song::getAlbum() const { return album; }
const std::string& Song::getGenre() const { return genre; }

void Song::displayInfo() const {
    std::cout << "[Song]     ID: " << std::left << std::setw(6) << id
              << " Title: " << std::setw(24) << title
              << " Artist: " << std::setw(18) << artist
              << " Album: " << std::setw(14) << album
              << " Genre: " << std::setw(10) << genre
              << " Duration: " << duration << "s"
              << " Plays: " << playCount << '\n';
}

std::string Song::getType() const { return "Song"; }

std::string Song::serializeExtra() const {
    return artist + "|" + album + "|" + genre;
}
