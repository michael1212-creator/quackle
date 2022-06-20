#include <iostream>

#include "greedyplayer.h"

using namespace Quackle;

Greedy::Greedy() {
  m_name = MARK_UV("Greedy");
  m_id = 3;
}

Greedy::~Greedy() {}

Move Greedy::move() { return m_simulator.currentPosition().greedyBestMove(); }

MoveList Greedy::moves(int nmoves) {
  m_simulator.currentPosition().kibitz(nmoves);
  return m_simulator.currentPosition().moves();
}
