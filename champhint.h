#ifndef LIBQUACKLE__CHAMPHINT_H_
#define LIBQUACKLE__CHAMPHINT_H_

#include "statichint.h"
#include "alphabetparameters.h"

class ChampHint : public StaticHint {
public:
  ChampHint(Quackle::LongLetterString message, double equity, double win);
  ~ChampHint() {}

  ChampHint *toChamp();

  double m_win;
};

#endif // LIBQUACKLE__CHAMPHINT_H_
