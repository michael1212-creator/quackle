#ifndef LIBQUACKLE_QUACKER_HINT_H_
#define LIBQUACKLE_QUACKER_HINT_H_

#include <vector>

#include "alphabetparameters.h"
#include "message.h"

class Hint {
public:
  Hint();
  ~Hint();

  Quackle::LongLetterString hint(Quackle::LongLetterString indentation = "",
                                 Quackle::LongLetterString eol = "\n");

  void addMsg(Quackle::LongLetterString msg,
              Quackle::LongLetterString indent = "");
  void addMessage(Quackle::LongLetterString message,
                  Quackle::LongLetterString indent = "");
  void addMsg(double equityAdd, Quackle::LongLetterString msg,
              Quackle::LongLetterString indent = "");
  void addMessage(double equityAdd, Quackle::LongLetterString message,
                  Quackle::LongLetterString indent = "");
  void addMsg(Message msg);
  void addMessage(Message message);

  Quackle::LongLetterString toString();

private:
  Quackle::LongLetterString m_hint = "";
  vector<Message> m_messages;
  Quackle::LongLetterString m_eol = "\n";
  Quackle::LongLetterString m_indentation = "";

  //in case we want to extend the hints, the way Championship Player does
  // for Static
  void messagesChanged();
};


inline void Hint::messagesChanged() { m_hint.clear(); }

inline void Hint::addMsg(Quackle::LongLetterString msg,
                         Quackle::LongLetterString indent) {
  addMsg(Message(msg, indent));
}

inline void Hint::addMessage(Quackle::LongLetterString message,
                             Quackle::LongLetterString indent) {
  addMsg(Message(message, indent));
}

inline void Hint::addMsg(double equityAdd, Quackle::LongLetterString msg,
                         Quackle::LongLetterString indent) {
  addMsg(Message(equityAdd, msg, indent));
}

inline void Hint::addMessage(double equityAdd,
                             Quackle::LongLetterString message,
                             Quackle::LongLetterString indent) {
  addMsg(Message(equityAdd, message, indent));
}

inline void Hint::addMsg(Message msg) {
  messagesChanged();
  m_messages.push_back(msg);
}

inline void Hint::addMessage(Message message) { addMsg(message); }

#endif // LIBQUACKLE_QUACKER_HINT_H_
