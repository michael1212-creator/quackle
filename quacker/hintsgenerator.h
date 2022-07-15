//
// Created by mihej on 14/07/22.
//

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

private:
  void createAITitle(ComputerPlayer *ai);

  bool m_genHints;
  vector<ComputerPlayer *> m_ais;
  LongLetterString m_hints;
};

#endif // LIBQUACKLE_QUACKER_HINTSGENERATOR_H_
} // namespace Quackle
