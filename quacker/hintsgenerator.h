#ifndef LIBQUACKLE_QUACKER_HINTSGENERATOR_H_
#define LIBQUACKLE_QUACKER_HINTSGENERATOR_H_

#include "computerplayer.h"
#include "quacker.h"
#include <QObject>
#include <vector>

namespace Quackle {

class HintsGenerator : public QObject {
  Q_OBJECT

public:
  HintsGenerator(TopLevel *toplevel);
  ~HintsGenerator();

  void addAI(ComputerPlayer *ai);
  void addAIs(vector<ComputerPlayer *> ais);

  void positionChanged(const Quackle::GamePosition &position);

  void generateHints(bool forceUpdateMoves = false);
  LongLetterString getHints();
  void clearHints();

  void genChampHintsChanged(bool shouldGenChampHints);

  LongLetterString committed(const Quackle::GamePosition &position,
                             Quackle::Move &move);

  void updateAIState(const Quackle::GamePosition &position);

signals:
  void hintsUpdated();
  void kibitzAs(Quackle::ComputerPlayer *computerPlayer, HintsGenerator *,
                bool shouldClone, bool updateGameMoves);

public slots:
  void threadFinishedGeneratingMoves();

private:
  LongLetterString createAITitle(ComputerPlayer *ai);

  bool m_shouldGenChampHints;
  vector<ComputerPlayer *> m_ais;
  LongLetterString m_hints;
//  LongLetterString m_positionTiles;

  // AIs which have been disabled from generating hints
  vector<ComputerPlayer *> blacklistedAIs() const;

  vector<ComputerPlayer *> whitelistedAIs() const;

  void movesAs(ComputerPlayer *ai, bool shouldClone = false,
               bool updateGameMoves = false);
};

inline void HintsGenerator::genChampHintsChanged(bool shouldGenChampHints) {
  m_shouldGenChampHints = shouldGenChampHints;
}

} // namespace Quackle

#endif // LIBQUACKLE_QUACKER_HINTSGENERATOR_H_
