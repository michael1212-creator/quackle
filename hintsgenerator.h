#ifndef LIBQUACKLE_QUACKER_HINTSGENERATOR_H_
#define LIBQUACKLE_QUACKER_HINTSGENERATOR_H_

#include <computerplayer.h>
#include <vector>

namespace Quackle {

class HintsGenerator {
public:
  HintsGenerator();
  ~HintsGenerator();

  void addAI(ComputerPlayer *ai);
  void addAIs(vector<ComputerPlayer *> ais);

  void positionChanged(const Quackle::GamePosition &position);

  LongLetterString generateHints();
  LongLetterString getHints();
  void clearHints();

  void genChampHintsChanged(bool shouldGenChampHints);

private:
  void createAITitle(ComputerPlayer *ai);

  bool m_shouldGenChampHints;
  vector<ComputerPlayer *> m_ais;
  LongLetterString m_hints;
};

inline void HintsGenerator::genChampHintsChanged(bool shouldGenChampHints) {
  m_shouldGenChampHints = shouldGenChampHints;
}

} // namespace Quackle

#endif // LIBQUACKLE_QUACKER_HINTSGENERATOR_H_
