#ifndef PODCAST_H
#define PODCAST_H

#include "AbstractMediaContent.h"

// Podcast : public AbstractMediaContent
class Podcast : public AbstractMediaContent {
private:
    std::string host;
    int episodeNumber;
    std::string description;

public:
    Podcast(std::string id, std::string title, int duration,
            std::string host, int episodeNumber, std::string description);

    const std::string& getHost() const;
    int getEpisodeNumber() const;
    const std::string& getDescription() const;

    void displayInfo() const override;
    std::string getType() const override;
    std::string serializeExtra() const override;
};

#endif // PODCAST_H
