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
  void *customArgs;
};

struct GenericArgs {
  void (*preLoop)(struct AIArgs *);
  void (*loopBody)(struct AIArgs *, Move, int);
  void (*postLoop)(struct AIArgs *);
  int numMoves;
};

void collectHints(struct AIArgs *args) {
  int i = 0;
  struct GenericArgs customArgs = *((GenericArgs *) (args->customArgs));
  customArgs.preLoop(args);
  for (Move move : args->ai->moves(customArgs.numMoves)) {
    customArgs.loopBody(args, move, i);
    i++;
  }
  customArgs.postLoop(args);
}

void staticPreLoop(struct AIArgs *args);
void staticLoopBody(struct AIArgs *args, Move move, int i);
void staticPostLoop(struct AIArgs *args);

struct StaticArgs {
  struct GenericArgs genericArgs = {staticPreLoop, staticLoopBody,
                                    staticPostLoop, 3};

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
  *(args->m_hints) +=
      "The top " + to_string(customArgs->genericArgs.numMoves) + " moves are:\n";
}

void staticLoopBody(struct AIArgs *args, Move move, int i) {
  // TODO mm (high): do the thing
  struct StaticArgs *customArgs = (struct StaticArgs *)args->customArgs;
  Hint *hint = move.hint();
  *(args->m_hints) += "Static Hint " + to_string(i + 1) + "\n";
}

void staticPostLoop(struct AIArgs *args) {
  struct StaticArgs *customArgs = (struct StaticArgs *)args->customArgs;
  *(args->m_hints) += "\nReally bad moves can have a valuation of " +
                      to_string(customArgs->lowestValuation) +
                      ", whereas really good moves can have a valuation of " +
                      to_string(customArgs->highValuation) + "\n";
}

void greedyPreLoop(struct AIArgs *args);
void greedyLoopBody(struct AIArgs *args, Move move, int i);
void greedyPostLoop(struct AIArgs *args);

struct GreedyArgs {
  struct GenericArgs genericArgs = {greedyPreLoop, greedyLoopBody,
                                    greedyPostLoop, 5};

  const int highestScore = 365;
  const LongLetterString highestScoringWord = "QUIXOTRY";

  const LongLetterString highestScoringTheoreticalWord = "OXYPHENBUTAZONE";
  const int theoreticalMaxScore = 1778;

  const LongLetterString source =
      "https://bestlifeonline.com/highest-scoring-scrabble-move/ 2022/06/28";
};

void greedyPreLoop(struct AIArgs *args) {
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  *(args->m_hints) += "The top " + to_string(customArgs->genericArgs.numMoves) +
                      " highest scoring moves are:\n";
}

void greedyLoopBody(struct AIArgs *args, Move move, int i) {
  // TODO mm (high): do the thing
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  Hint *hint = move.hint();
  *(args->m_hints) += "Greedy Hint " + to_string(i + 1) + "\n";
}

void greedyPostLoop(struct AIArgs *args) {
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  *(args->m_hints) +=
      "\nThe highest scoring word ever recorded in Scrabble is " +
      customArgs->highestScoringWord + " for " +
      to_string(customArgs->highestScore) + " points.\n" +
      "The theoretical maximum is " +
      to_string(customArgs->theoreticalMaxScore) + " points for " +
      customArgs->highestScoringTheoreticalWord + " (" + customArgs->source +
      ").\n";
}

void champPreLoop(struct AIArgs *args);
void champLoopBody(struct AIArgs *args, Move move, int i);
void champPostLoop(struct AIArgs *args);

struct ChampArgs {
  struct GenericArgs genericArgs = {champPreLoop, champLoopBody, champPostLoop,
                                    3};
};

void champPreLoop(struct AIArgs *args) {
  struct ChampArgs *customArgs = (struct ChampArgs *)args->customArgs;
  *(args->m_hints) += "The top " + to_string(customArgs->genericArgs.numMoves) +
                      " moves with highest win% are:\n";
}

void champLoopBody(struct AIArgs *args, Move move, int i) {
  // TODO mm (high): do the thing
  struct ChampArgs *customArgs = (struct ChampArgs *)args->customArgs;
  Hint *hint = move.hint();
  *(args->m_hints) += "Champ Hint " + to_string(i + 1) + "\n";
}

void champPostLoop(struct AIArgs *args) {
  *(args->m_hints) +=
      "\nHigher win% (starting at 0%, up to 100%) means that the "
      "AI is more confident of a win using this move.\n";
}

LongLetterString HintsGenerator::generateHints() {
  for (ComputerPlayer *ai : m_ais) {
    createAITitle(ai);

    struct AIArgs args = {&m_hints, ai, 0};
    if (ai->isStatic()) {
      struct StaticArgs StaticArgs;
      args.customArgs = &StaticArgs;
      collectHints(&args);
    } else if (ai->isGreedy()) {
      struct GreedyArgs GreedyArgs;
      args.customArgs = &GreedyArgs;
      collectHints(&args);
    } else if (ai->isChamp() && m_shouldGenChampHints) {
      struct ChampArgs ChampArgs;
      args.customArgs = &ChampArgs;
      collectHints(&args);
    } else {
      m_hints += "Hints for " + ai->name() + " have been disabled.\n";
    }
    m_hints += "\n\n";
  }
  return m_hints;
}

LongLetterString HintsGenerator::getHints() { return m_hints; }

void HintsGenerator::clearHints() { m_hints = ""; }
