#ifndef LIBQUACKLE_QUACKER_HINT_H_
#define LIBQUACKLE_QUACKER_HINT_H_

#include <vector>

#include "alphabetparameters.h"

class Hint {
public:
  Hint() {}
  ~Hint() {}

  Quackle::LongLetterString hint() const;

  void addMsg(Quackle::LongLetterString msg);
  void addMessage(Quackle::LongLetterString message);

  Quackle::LongLetterString toString();

private:
  Quackle::LongLetterString m_hint;
  vector<Quackle::LongLetterString> m_messages;
};

inline void Hint::addMsg(Quackle::LongLetterString msg) { m_messages.push_back(msg); }

inline void Hint::addMessage(Quackle::LongLetterString message) { addMsg(message); }

#endif // LIBQUACKLE_QUACKER_HINT_H_
