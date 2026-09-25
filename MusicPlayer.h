#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <random>
#include <vector>
#include "Playlist.h"

enum class PlaybackState { STOPPED, PLAYING, PAUSED };
enum class PlaybackMode { SEQUENTIAL, SHUFFLE, REPEAT };

// MusicPlayer
// ---------------------------------------------------------------------------
// A finite state machine (PlaybackState) that plays a Playlist (non-owning
// pointer - the player never owns the playlist or its media). Tracks the
// current index into the playlist, current simulated position (seconds),
// playback mode, and drives play/pause/next/previous transitions.
//
// playCount increment rule: incremented exactly once per "new playback
// started" event - i.e. when transitioning STOPPED -> PLAYING for a given
// track, or when Next/Previous selects a new track and starts playing it.
// Resuming from PAUSED, or repeatedly refreshing the UI while PLAYING,
// never increments playCount again.
class MusicPlayer {
private:
    Playlist* currentPlaylist = nullptr; // non-owning
    std::size_t currentIndex = 0;
    PlaybackState state = PlaybackState::STOPPED;
    PlaybackMode mode = PlaybackMode::SEQUENTIAL;
    int currentPosition = 0; // seconds

    std::mt19937 rng;

    void startTrack(std::size_t index); // sets index, resets position, ++playCount

public:
    MusicPlayer();

    void loadPlaylist(Playlist* playlist);

    void play();
    void pause();
    void next();
    void previous();
    void stop();
    void setMode(PlaybackMode newMode);
    PlaybackMode getMode() const;
    PlaybackState getState() const;

    // Advances simulated playback position by one "tick" (used by the
    // progress-bar simulation). Does nothing if not PLAYING.
    void tick(int seconds = 15);

    AbstractMediaContent* getCurrentMedia() const;
    int getCurrentPosition() const;
    Playlist* getCurrentPlaylist() const;

    void displayNowPlaying() const;
};

#endif // MUSIC_PLAYER_H
