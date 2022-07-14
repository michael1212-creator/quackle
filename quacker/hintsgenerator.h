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

private:
  bool m_genHints;
  vector<ComputerPlayer *> m_ais;
};

#endif // LIBQUACKLE_QUACKER_HINTSGENERATOR_H_
} // namespace Quackle
