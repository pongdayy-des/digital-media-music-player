#include "MusicPlayer.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

MusicPlayer::MusicPlayer()
    : rng(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())) {}

void MusicPlayer::loadPlaylist(Playlist* playlist) {
    currentPlaylist = playlist;
    currentIndex = 0;
    currentPosition = 0;
    state = PlaybackState::STOPPED;
}

void MusicPlayer::startTrack(std::size_t index) {
    if (currentPlaylist == nullptr || currentPlaylist->empty()) return;
    currentIndex = index;
    currentPosition = 0;
    state = PlaybackState::PLAYING;
    AbstractMediaContent* media = currentPlaylist->trackAt(currentIndex);
    if (media != nullptr) {
        media->incrementPlayCount(); // exactly once per new playback start
    }
}

void MusicPlayer::play() {
    if (currentPlaylist == nullptr || currentPlaylist->empty()) {
        std::cout << "Cannot play: no playlist loaded or playlist is empty.\n";
        return;
    }
    switch (state) {
        case PlaybackState::STOPPED:
            startTrack(currentIndex); // begin a fresh playback -> playCount++
            break;
        case PlaybackState::PAUSED:
            state = PlaybackState::PLAYING; // resume, no playCount change
            break;
        case PlaybackState::PLAYING:
            // Already playing: no-op, must NOT increment playCount again.
            break;
    }
}

void MusicPlayer::pause() {
    if (state == PlaybackState::PLAYING) {
        state = PlaybackState::PAUSED; // position/current media are preserved
    }
    // Pausing while STOPPED/PAUSED is a harmless no-op, never crashes.
}

void MusicPlayer::stop() {
    state = PlaybackState::STOPPED;
    currentPosition = 0;
}

void MusicPlayer::next() {
    if (currentPlaylist == nullptr || currentPlaylist->empty()) return;
    const std::size_t n = currentPlaylist->size();

    if (n == 1) {
        // Single-track playlist: every mode simply restarts the same track.
        startTrack(0);
        return;
    }

    switch (mode) {
        case PlaybackMode::SEQUENTIAL: {
            std::size_t nextIndex = (currentIndex + 1) % n; // wrap at the end
            startTrack(nextIndex);
            break;
        }
        case PlaybackMode::SHUFFLE: {
            std::uniform_int_distribution<std::size_t> dist(0, n - 1);
            std::size_t candidate;
            do {
                candidate = dist(rng);
            } while (candidate == currentIndex); // avoid repeating the current track
            startTrack(candidate);
            break;
        }
        case PlaybackMode::REPEAT:
            startTrack(currentIndex); // Repeat: Next still replays current track
            break;
    }
}

void MusicPlayer::previous() {
    if (currentPlaylist == nullptr || currentPlaylist->empty()) return;
    const std::size_t n = currentPlaylist->size();

    if (n == 1) {
        startTrack(0);
        return;
    }

    switch (mode) {
        case PlaybackMode::SEQUENTIAL: {
            std::size_t prevIndex = (currentIndex == 0) ? (n - 1) : (currentIndex - 1);
            startTrack(prevIndex);
            break;
        }
        case PlaybackMode::SHUFFLE: {
            std::uniform_int_distribution<std::size_t> dist(0, n - 1);
            std::size_t candidate;
            do {
                candidate = dist(rng);
            } while (candidate == currentIndex);
            startTrack(candidate);
            break;
        }
        case PlaybackMode::REPEAT:
            startTrack(currentIndex);
            break;
    }
}

void MusicPlayer::setMode(PlaybackMode newMode) { mode = newMode; }
PlaybackMode MusicPlayer::getMode() const { return mode; }
PlaybackState MusicPlayer::getState() const { return state; }

void MusicPlayer::tick(int seconds) {
    if (state != PlaybackState::PLAYING || currentPlaylist == nullptr) return;
    AbstractMediaContent* media = currentPlaylist->trackAt(currentIndex);
    if (media == nullptr) return;
    currentPosition = std::min(currentPosition + seconds, media->getDuration());
    if (currentPosition >= media->getDuration()) {
        // Track finished: behave like an automatic Next.
        next();
    }
}

AbstractMediaContent* MusicPlayer::getCurrentMedia() const {
    if (currentPlaylist == nullptr) return nullptr;
    return currentPlaylist->trackAt(currentIndex);
}

int MusicPlayer::getCurrentPosition() const { return currentPosition; }
Playlist* MusicPlayer::getCurrentPlaylist() const { return currentPlaylist; }

namespace {
std::string formatTime(int totalSeconds) {
    int m = totalSeconds / 60;
    int s = totalSeconds % 60;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << m << ":" << std::setw(2) << s;
    return oss.str();
}
} // namespace

void MusicPlayer::displayNowPlaying() const {
    AbstractMediaContent* media = getCurrentMedia();
    if (media == nullptr) {
        std::cout << "Nothing is loaded.\n";
        return;
    }
    std::cout << "Now Playing: " << media->getTitle() << " [" << media->getType() << "]\n";

    const int duration = std::max(media->getDuration(), 1);
    const int barWidth = 20;
    int filled = (currentPosition * barWidth) / duration;
    filled = std::clamp(filled, 0, barWidth);

    std::cout << "[" << std::string(static_cast<std::size_t>(filled), '#')
               << std::string(static_cast<std::size_t>(barWidth - filled), '-') << "] "
               << formatTime(currentPosition) << " / " << formatTime(media->getDuration()) << "\n";

    std::cout << "State: " << (state == PlaybackState::PLAYING ? "PLAYING" :
                                state == PlaybackState::PAUSED ? "PAUSED" : "STOPPED")
               << "   Mode: " << (mode == PlaybackMode::SEQUENTIAL ? "SEQUENTIAL" :
                                    mode == PlaybackMode::SHUFFLE ? "SHUFFLE" : "REPEAT")
               << "\n";
}
