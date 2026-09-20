#include "Podcast.h"
#include <iostream>
#include <iomanip>

Podcast::Podcast(std::string idIn, std::string titleIn, int durationIn,
                  std::string hostIn, int episodeNumberIn, std::string descriptionIn)
    : AbstractMediaContent(std::move(idIn), std::move(titleIn), durationIn),
      host(std::move(hostIn)), episodeNumber(episodeNumberIn), description(std::move(descriptionIn)) {}

const std::string& Podcast::getHost() const { return host; }
int Podcast::getEpisodeNumber() const { return episodeNumber; }
const std::string& Podcast::getDescription() const { return description; }

void Podcast::displayInfo() const {
    std::cout << "[Podcast]  ID: " << std::left << std::setw(6) << id
              << " Title: " << std::setw(24) << title
              << " Host: " << std::setw(18) << host
              << " Episode: " << std::setw(4) << episodeNumber
              << " Duration: " << duration << "s"
              << " Plays: " << playCount << '\n';
}

std::string Podcast::getType() const { return "Podcast"; }

std::string Podcast::serializeExtra() const {
    return host + "|" + std::to_string(episodeNumber) + "|" + description;
}
