#ifndef LIBQUACKLE_QUACKER_HINTSGENERATOR_H_
#define LIBQUACKLE_QUACKER_HINTSGENERATOR_H_

#include "computerplayer.h"
#include <vector>

namespace Quackle {

class HintsGenerator {

public:
  HintsGenerator();
  ~HintsGenerator();

  void addAI(ComputerPlayer *ai);
  void addAIs(vector<ComputerPlayer *> ais);

  void positionChanged(const Quackle::GamePosition &position);

  LongLetterString generateHints(bool forceUpdateMoves = false);
  LongLetterString getHints();
  void clearHints();

  void genChampHintsChanged(bool shouldGenChampHints);

private:
  void createAITitle(ComputerPlayer *ai, LongLetterString *appendTo = NULL);

  bool m_shouldGenChampHints;
  vector<ComputerPlayer *> m_ais;
  LongLetterString m_hints;

  //AIs which have been disabled from generating hints
  vector<ComputerPlayer *> blacklistedAIs() const;

  vector<ComputerPlayer *> whitelistedAIs() const;
};

inline void HintsGenerator::genChampHintsChanged(bool shouldGenChampHints) {
  m_shouldGenChampHints = shouldGenChampHints;
}

} // namespace Quackle

#endif // LIBQUACKLE_QUACKER_HINTSGENERATOR_H_
