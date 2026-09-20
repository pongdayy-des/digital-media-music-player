#include "AbstractMediaContent.h"

AbstractMediaContent::AbstractMediaContent(std::string idIn, std::string titleIn, int durationIn)
    : id(std::move(idIn)), title(std::move(titleIn)), duration(durationIn), playCount(0) {}

const std::string& AbstractMediaContent::getId() const { return id; }
const std::string& AbstractMediaContent::getTitle() const { return title; }
int AbstractMediaContent::getDuration() const { return duration; }
int AbstractMediaContent::getPlayCount() const { return playCount; }

void AbstractMediaContent::setTitle(const std::string& newTitle) { title = newTitle; }

void AbstractMediaContent::setDuration(int newDuration) {
    if (newDuration >= 0) {
        duration = newDuration;
    }
}

void AbstractMediaContent::incrementPlayCount() { ++playCount; }
