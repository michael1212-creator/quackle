#include "statichint.h"

StaticHint::StaticHint(Quackle::LongLetterString message, double equity)
    : Hint(message) {
  m_equity = equity;
}

StaticHint::~StaticHint() {}
