#include "hint.h"

Hint::Hint(Quackle::Move move) {
  m_move = move;
}

Quackle::LongLetterString Hint::message() const {
  //TODO mm (high): will need to collect all messages into this one big message
  return m_message;
}
