#include "Audiobook.h"
#include <iostream>
#include <iomanip>

Audiobook::Audiobook(std::string idIn, std::string titleIn, int durationIn,
                      std::string authorIn, std::string narratorIn, int chapterIn)
    : AbstractMediaContent(std::move(idIn), std::move(titleIn), durationIn),
      author(std::move(authorIn)), narrator(std::move(narratorIn)), chapter(chapterIn) {}

const std::string& Audiobook::getAuthor() const { return author; }
const std::string& Audiobook::getNarrator() const { return narrator; }
int Audiobook::getChapter() const { return chapter; }

void Audiobook::displayInfo() const {
    std::cout << "[Audiobook]ID: " << std::left << std::setw(6) << id
              << " Title: " << std::setw(24) << title
              << " Author: " << std::setw(18) << author
              << " Narrator: " << std::setw(14) << narrator
              << " Chapter: " << std::setw(4) << chapter
              << " Duration: " << duration << "s"
              << " Plays: " << playCount << '\n';
}

std::string Audiobook::getType() const { return "Audiobook"; }

std::string Audiobook::serializeExtra() const {
    return author + "|" + narrator + "|" + std::to_string(chapter);
}
