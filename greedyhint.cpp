#include "greedyhint.h"

GreedyHint::GreedyHint(Quackle::LongLetterString message, int score)
    : Hint(message) {
  m_score = score;
}

GreedyHint *GreedyHint::toGreedy() {
  return this;
}

