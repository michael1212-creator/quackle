#ifndef LIBQUACKLE_QUACKER_HINT_H_
#define LIBQUACKLE_QUACKER_HINT_H_

#include "alphabetparameters.h"
#include "move.h"

class Hint {
public:
  Hint(Quackle::LongLetterString message, Quackle::Move move);
  ~Hint() {}

  Quackle::LongLetterString message();
  Quackle::Move move();

private:
  Quackle::LongLetterString m_message;
  Quackle::Move m_move;
};

inline Quackle::LongLetterString Hint::message() { return m_message; }

inline Quackle::Move Hint::move() { return m_move; }

#endif // LIBQUACKLE_QUACKER_HINT_H_
