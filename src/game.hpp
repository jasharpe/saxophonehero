#ifndef _GAME__H
#define _GAME__H

#include <vector>

// the total number of notes in the song
struct Note {
  Note(long time, int note)
    : time(time), note(note), hit(false), should_hit(false) {
  }

  long time;
  int note;
  unsigned long hit;
  bool should_hit;
};

class Game {
public:
  Game();

  void reload();
  void start();
  void stop();
  void restart();
  int update();
  // attempt to play a note
  // returns a pointer to the hit note if successful, and
  // NULL if not successful
  Note* play_note(int note);

  void load_notes();
  unsigned long m_start_time;
  int m_score;
  // length of the song in milliseconds
  long m_length;
  std::vector<Note> m_notes;

private:
  void change_score(int score_delta);
};

#endif
