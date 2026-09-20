#ifndef AUDIOBOOK_H
#define AUDIOBOOK_H

#include "AbstractMediaContent.h"

// Audiobook : public AbstractMediaContent
class Audiobook : public AbstractMediaContent {
private:
    std::string author;
    std::string narrator;
    int chapter;

public:
    Audiobook(std::string id, std::string title, int duration,
              std::string author, std::string narrator, int chapter);

    const std::string& getAuthor() const;
    const std::string& getNarrator() const;
    int getChapter() const;

    void displayInfo() const override;
    std::string getType() const override;
    std::string serializeExtra() const override;
};

#endif // AUDIOBOOK_H
