#include "hint.h"

Hint::Hint() {}

Hint::~Hint() {}

Quackle::LongLetterString Hint::hint(Quackle::LongLetterString indentation,
                                     Quackle::LongLetterString eol) {
  if (m_hint.empty()) {
    for (auto it : m_messages) {
      m_hint += indentation + it + eol;
    }
  }
  return m_hint;
}

Quackle::LongLetterString Hint::toString() { return hint(); }
