#ifndef LIBQUACKLE__GREEDYPLAYER_H_
#define LIBQUACKLE__GREEDYPLAYER_H_

#include "computerplayer.h"
#include "alphabetparameters.h"

namespace Quackle {

class Greedy : public ComputerPlayer {
public:
  Greedy();
  ~Greedy();

  Move move();
  MoveList moves(int nmoves);
  ComputerPlayer *clone() { return new Greedy; }

  bool isUserVisible() const;
};

inline bool Greedy::isUserVisible() const { return true; }

} // namespace Quackle

#endif // LIBQUACKLE__GREEDYPLAYER_H_
