#include "hintsgenerator.h"
#include "alphabetparameters.h"
#include "hint.h"

using namespace Quackle;

HintsGenerator::HintsGenerator() {}

HintsGenerator::~HintsGenerator() {}

void HintsGenerator::createAITitle(ComputerPlayer *ai) {
  m_hints += "According to " + ai->name() + "\n";
  m_hints += "-----------------------------\n";
}

void HintsGenerator::addAI(ComputerPlayer *ai) {
  m_ais.insert(m_ais.end(), ai);
}

void HintsGenerator::addAIs(vector<ComputerPlayer *> ais) {
  m_ais.insert(m_ais.end(), ais.begin(), ais.end());
}

void HintsGenerator::positionChanged(const Quackle::GamePosition &position) {
  for (auto ai : m_ais) {
    ai->setPosition(position);
  }
}

struct AIArgs {
  LongLetterString *m_hints;
  ComputerPlayer *ai;
  void (*preLoop)(struct AIArgs *);
  void (*loopBody)(struct AIArgs *, Move, int);
  void (*postLoop)(struct AIArgs *);
  int moves;
  void *customArgs;
};

void collectHints(struct AIArgs *args) {
  int i = 0;
  args->preLoop(args);
  for (Move move : args->ai->moves(args->moves)) {
    args->loopBody(args, move, i);
    i++;
  }
  args->postLoop(args);
}

struct StaticArgs {
  union {
    double lowestValuation = -40;
    double lowestEquity;
  };
  union {
    double highValuation;
    double highEquity;
  };
};

void staticPreLoop(struct AIArgs *args) {
  struct StaticArgs *customArgs = (struct StaticArgs *)args->customArgs;
  *(args->m_hints) += "The top " + to_string(args->moves) + " moves are:\n";
}

void staticLoopBody(struct AIArgs *args, Move move, int i) {
  // TODO mm (high): do the thing
  struct StaticArgs *customArgs = (struct StaticArgs *)args->customArgs;
  Hint *hint = move.hint();
  *(args->m_hints) += "Static Hint " + to_string(i + 1) + "\n";
}

void staticPostLoop(struct AIArgs *args) {
  struct StaticArgs *customArgs = (struct StaticArgs *)args->customArgs;
  *(args->m_hints) += "Really bad moves can have a valuation of " +
                      to_string(customArgs->lowestValuation) +
                      ", whereas really good moves can have a valuation of " +
                      to_string(customArgs->highValuation) + "\n";
}

struct GreedyArgs {
  const int highestScore = 365;
  const LongLetterString highestScoringWord = "QUIXOTRY";

  const LongLetterString highestScoringTheoreticalWord = "OXYPHENBUTAZONE";
  const int theoreticalMaxScore = 1778;

  const LongLetterString source =
      "https://bestlifeonline.com/highest-scoring-scrabble-move/ 2022/06/28";
};

void greedyPreLoop(struct AIArgs *args) {
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  *(args->m_hints) +=
      "The top " + to_string(args->moves) + " highest scoring moves are:\n";
}

void greedyLoopBody(struct AIArgs *args, Move move, int i) {
  // TODO mm (high): do the thing
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  Hint *hint = move.hint();
  *(args->m_hints) += "Greedy Hint " + to_string(i + 1) + "\n";
}

void greedyPostLoop(struct AIArgs *args) {
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  *(args->m_hints) += "The highest scoring word ever recorded in Scrabble is " +
                      customArgs->highestScoringWord + " for " +
                      to_string(customArgs->highestScore) + " points.\n" +
                      "The theoretical maximum is " +
                      to_string(customArgs->theoreticalMaxScore) +
                      " points for " +
                      customArgs->highestScoringTheoreticalWord + " (" +
                      customArgs->source + ").\n";
}

struct ChampArgs {};

void champPreLoop(struct AIArgs *args) {
  struct ChampArgs *customArgs = (struct ChampArgs *)args->customArgs;
  *(args->m_hints) += "Champ\n";
}

void champLoopBody(struct AIArgs *args, Move move, int i) {
  // TODO mm (high): do the thing
  struct ChampArgs *customArgs = (struct ChampArgs *)args->customArgs;
  Hint *hint = move.hint();
  *(args->m_hints) += "Champ Hint " + to_string(i + 1) + "\n";
}

void champPostLoop(struct AIArgs *args) {
  *(args->m_hints) += "Higher win% (starting at 0%, up to 100%) means that the "
                      "AI is more confident of a win using this move.\n";
}

LongLetterString HintsGenerator::generateHints() {
  for (ComputerPlayer *ai : m_ais) {
    createAITitle(ai);

    if (ai->isStatic()) {
      struct StaticArgs StaticArgs;
      struct AIArgs args = {&m_hints,       ai, staticPreLoop, staticLoopBody,
                            staticPostLoop, 3,  &StaticArgs};
      collectHints(&args);
    } else if (ai->isGreedy()) {
      struct GreedyArgs GreedyArgs;
      struct AIArgs args = {&m_hints,       ai, greedyPreLoop, greedyLoopBody,
                            greedyPostLoop, 5,  &GreedyArgs};
      collectHints(&args);
    } else if (ai->isChamp()) {
      struct ChampArgs ChampArgs;
      struct AIArgs args = {&m_hints,      ai, champPreLoop, champLoopBody,
                            champPostLoop, 3,  &ChampArgs};
      collectHints(&args);
    } else {
      m_hints +=
          "Hints for " + ai->name() + " have not been implemented yet.\n\n";
    }
  }
  return m_hints;
}

LongLetterString HintsGenerator::getHints() { return m_hints; }

void HintsGenerator::clearHints() { m_hints = ""; }
