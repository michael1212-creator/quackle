#ifndef LIBQUACKLE_QUACKER_STATICHINT_H_
#define LIBQUACKLE_QUACKER_STATICHINT_H_

#include "hint.h"
#include "alphabetparameters.h"

class StaticHint : public Hint {
public:
  StaticHint(Quackle::LongLetterString message, double equity);
  ~StaticHint();

  double equity();

private:
  double m_equity;
};

inline double StaticHint::equity() { return m_equity; }

#endif // LIBQUACKLE_QUACKER_STATICHINT_H_
