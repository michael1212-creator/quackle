#ifndef LIBQUACKLE__GREEDYHINT_H_
#define LIBQUACKLE__GREEDYHINT_H_

#include "hint.h"
#include "alphabetparameters.h"

class GreedyHint : public Hint  {
public:
  GreedyHint(Quackle::LongLetterString message, int score);
  ~GreedyHint() {}

  GreedyHint *toGreedy();

  double m_score;
};

#endif // LIBQUACKLE__GREEDYHINT_H_
