#ifndef LIBQUACKLE__MESSAGE_H_
#define LIBQUACKLE__MESSAGE_H_

#include "alphabetparameters.h"

#define TWO_DP(num) std::snprintf(buf, 16, "%.2f", num)

class Message {
public:
  Message(double equityAdd, Quackle::LongLetterString textMessage,
          Quackle::LongLetterString indent = "");
  Message(Quackle::LongLetterString textMessage,
          Quackle::LongLetterString indent = "");

  friend Quackle::LongLetterString
  operator+(Message lhs, const Quackle::LongLetterString rhs) {
    if (!lhs.m_specifiesEquity) {
      return lhs.m_indent + lhs.m_textMessage + rhs;
    }
    char buf[16];
    TWO_DP(lhs.m_equityAdd);
    return lhs.m_indent + Quackle::LongLetterString(buf) + lhs.m_textMessage +
           rhs;
  }

  friend Quackle::LongLetterString
  operator+(const Quackle::LongLetterString lhs, Message rhs) {
    if (!rhs.m_specifiesEquity) {
      return lhs + rhs.m_indent + rhs.m_textMessage;
    }
    char buf[16];
    TWO_DP(rhs.m_equityAdd);
    return lhs + rhs.m_indent + Quackle::LongLetterString(buf) +
           rhs.m_textMessage;
  }

private:
  bool m_specifiesEquity;
  double m_equityAdd;
  Quackle::LongLetterString m_textMessage;
  Quackle::LongLetterString m_indent = "";
};

#endif // LIBQUACKLE__MESSAGE_H_
