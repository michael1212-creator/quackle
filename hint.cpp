#include "hint.h"

Quackle::LongLetterString Hint::hint() const {
  //TODO mm (high): will need to collect all messages into this one big message
  if (!m_hint.empty()) {
    return m_hint;
  }

}

Quackle::LongLetterString Hint::toString() {
  return hint();
}
