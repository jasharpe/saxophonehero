#include "game.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "time.hpp"
#include <cmath>
#include "soundmanager.hpp"

// there is a window of +-WINDOW milliseconds in which a
// note can be hit.
const long WINDOW = 100;
const int MAX_SCORE = 20;
const int MIN_SCORE = 0;

Game::Game() : m_start_time(0), m_score(MAX_SCORE) {
  load_notes();
}

void Game::change_score(int score_delta) {
  m_score += score_delta;
  if (m_score < MIN_SCORE) {
    m_score = MIN_SCORE;
  } else if (m_score > MAX_SCORE) {
    m_score = MAX_SCORE;
  }
}

void Game::load_notes() {
  m_notes = std::vector<Note>();
  
  // load music file
  const char* file = "data/giantsteps.notes";
  std::ifstream notes(file);
  if (notes.is_open()) {
    std::string line;
    std::getline(notes, line);
    m_length = atol(line.c_str());
    //std::cerr << "length: " << length << std::endl;
    while (notes) {
      if (!std::getline(notes, line)) {
        break;
      }

      std::istringstream line_stream(line);
      std::string element;
      getline(line_stream, element, ',');
      long time = atol(element.c_str());
      getline(line_stream, element, ',');
      int note = atoi(element.c_str());
      //std::cerr << "Note(" << time << ", " << note << ")" << std::endl;
      m_notes.push_back(Note(time, note));
    }
    //std::cerr << "finished reading file" << std::endl;
  } else {
    std::cerr << "Unable to open file " << file << std::endl;
  }
}

void Game::reload() {
  stop();
  load_notes();
  start();
}

void Game::start() {
  // adjust this so that it's in sync with the music if necessary
  m_start_time = get_time();// - 23000;
  SM.PlayMusic(0);
}

void Game::stop() {
  m_start_time = 0;
  SM.StopMusic(0);
}

void Game::restart() {
  stop();
  start();
}

int Game::update() {
  unsigned long time = (get_time() - m_start_time) % m_length;
  int missed = 0;
  for (unsigned int i = 0; i < m_notes.size(); i++) {
    Note* note_i = &m_notes[i];
    if (!note_i->hit && std::abs((long) (note_i->time - time)) < WINDOW) {
      note_i->should_hit = true;
    }
    
    if (std::abs((long) (note_i->time - time)) > WINDOW) {
      if (note_i->should_hit) {
        missed++;
        note_i->should_hit = false;
      }
    }

    if (std::abs((long) (note_i->time - time)) > 2 * WINDOW) {
      note_i->hit = 0;
    }
  }
  change_score(-missed);
  return missed;
}

Note* Game::play_note(int note) {
  unsigned long cur_time = get_time();
  unsigned long time = (cur_time - m_start_time) % m_length;
  for (unsigned int i = 0; i < m_notes.size(); i++) {
    Note* note_i = &m_notes[i];
    if (!note_i->hit && note_i->note == note && std::abs((long) (note_i->time - time)) < WINDOW) {
      note_i->should_hit = false;
      note_i->hit = cur_time;
      change_score(1);
      return note_i;
    }
  }
  change_score(-1);
  return NULL;
}
