#ifndef LIBQUACKLE_QUACKER_HINT_H_
#define LIBQUACKLE_QUACKER_HINT_H_

#include "alphabetparameters.h"

class Hint {
public:
  Hint(Quackle::LongLetterString message);
  ~Hint();

  Quackle::LongLetterString message();

private:
  Quackle::LongLetterString m_message;
};

inline Quackle::LongLetterString Hint::message() { return m_message; }

#endif // LIBQUACKLE_QUACKER_HINT_H_
