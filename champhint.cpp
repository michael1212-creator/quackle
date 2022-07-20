#include "champhint.h"

ChampHint::ChampHint(Quackle::LongLetterString message, double equity,
                     double win)
    : StaticHint(message, equity) {
  m_win = win;
}

ChampHint *ChampHint::toChamp() { return this; }
