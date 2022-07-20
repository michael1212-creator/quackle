#ifndef LIBQUACKLE_QUACKER_HINT_H_
#define LIBQUACKLE_QUACKER_HINT_H_

#include "alphabetparameters.h"

class StaticHint;
class GreedyHint;
class ChampHint;

class Hint {
public:
  Hint(Quackle::LongLetterString message);
  virtual ~Hint() {}

  virtual StaticHint *toStatic();
  virtual GreedyHint *toGreedy();
  virtual ChampHint *toChamp();

  Quackle::LongLetterString message();

private:
  Quackle::LongLetterString m_message;
};

inline Quackle::LongLetterString Hint::message() { return m_message; }

#endif // LIBQUACKLE_QUACKER_HINT_H_
