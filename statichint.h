#ifndef LIBQUACKLE_QUACKER_STATICHINT_H_
#define LIBQUACKLE_QUACKER_STATICHINT_H_

#include "hint.h"
#include "alphabetparameters.h"

class StaticHint : public Hint {
public:
  StaticHint(Quackle::LongLetterString message, double equity);
  ~StaticHint() {}

  StaticHint *toStatic();

  double m_equity;
};

#endif // LIBQUACKLE_QUACKER_STATICHINT_H_
