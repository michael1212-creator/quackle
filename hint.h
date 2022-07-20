#ifndef LIBQUACKLE_QUACKER_HINT_H_
#define LIBQUACKLE_QUACKER_HINT_H_

#include <vector>

#include "alphabetparameters.h"
#include "move.h"

class Hint {
public:
  Hint(Quackle::Move move);
  ~Hint() {}

  Quackle::LongLetterString message() const;
  Quackle::Move move() const;

  void addMsg(Quackle::LongLetterString msg);
  void addMessage(Quackle::LongLetterString message);

private:
  Quackle::LongLetterString m_message;
  vector<Quackle::LongLetterString> m_messages;
  Quackle::Move m_move;
};

inline Quackle::Move Hint::move() const { return m_move; }

inline void Hint::addMsg(Quackle::LongLetterString msg) { m_messages.push_back(msg); }

inline void Hint::addMessage(Quackle::LongLetterString message) { addMsg(message); }

#endif // LIBQUACKLE_QUACKER_HINT_H_
