#include "message.h"

Message::Message(double equityAdd, Quackle::LongLetterString textMessage,
                 Quackle::LongLetterString indent)
    : m_specifiesEquity(true), m_textMessage(textMessage),
      m_equityAdd(equityAdd), m_indent(indent) {}

Message::Message(Quackle::LongLetterString textMessage,
                 Quackle::LongLetterString indent)
    : m_specifiesEquity(false), m_textMessage(textMessage), m_indent(indent) {}
