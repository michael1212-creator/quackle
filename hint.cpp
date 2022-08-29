#include "hint.h"

Hint::Hint() {}

Hint::~Hint() {}

Quackle::LongLetterString Hint::hint(Quackle::LongLetterString indentation,
                                     Quackle::LongLetterString eol) {
  if (m_hint.empty() || m_indentation != indentation || m_eol != eol) {
    m_hint.clear();
    m_eol = eol;
    m_indentation = indentation;
    for (auto it : m_messages) {
      m_hint += indentation + it + eol;
    }
  }
  return m_hint;
}

Quackle::LongLetterString Hint::toString() { return hint(); }
