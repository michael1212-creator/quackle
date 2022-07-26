#include <quackleio/util.h>

#include "alphabetparameters.h"
#include "hint.h"
#include "hintsgenerator.h"

using namespace Quackle;

HintsGenerator::HintsGenerator() {}

HintsGenerator::~HintsGenerator() {
  for (int i = 0; i < m_ais.size(); i++) {
    delete m_ais.back();
    m_ais.pop_back();
  }
}

void HintsGenerator::createAITitle(ComputerPlayer *ai,
                                   LongLetterString *appendTo) {
  if (!appendTo) {
    appendTo = &m_hints;
  }
  (*appendTo) += "According to " + ai->name() + "\n";
  (*appendTo) += "-----------------------------\n";
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
  vector<ComputerPlayer *> whitelistedAIs;
};

#define NUM_MOVES_TO_GEN 20

struct GenericArgs {
  void (*preLoop)(struct AIArgs *);
  void (*loopBody)(struct AIArgs *, Move &, int);
  void (*postLoop)(struct AIArgs *);
  int numMovesToShow;
};

void otherAIsRankingsOfMove(struct AIArgs *args, Move &move,
                            LongLetterString indent = " - ") {
  // TODO mm (high): do the thing
  for (auto it : args->whitelistedAIs) {
    int moveRank = it->rankMove(move);
    *(args->m_hints) += indent + it->name() + " ";
    if (moveRank < 0) {
      *(args->m_hints) += "only has top " + to_string(-moveRank) +
                          " moves, and this move is not among them";
    } else if (moveRank > 0) {
      *(args->m_hints) += "ranks this as number " + to_string(moveRank);
    } else {
      *(args->m_hints) += "moves have not been generated yet";
    }
    *(args->m_hints) += ".\n";
  }
}

void collectHints(struct AIArgs *args) {
  struct GenericArgs customArgs = *((GenericArgs *)(args->customArgs));

  // Remove this AI from the whitelist - it should only contain other
  //  non-blacklisted AIs
  int j = 0;
  for (auto it : args->whitelistedAIs) {
    if (it == args->ai) {
      break;
    }
    j++;
  }
  args->whitelistedAIs.erase(args->whitelistedAIs.begin() + j);

  customArgs.preLoop(args);

  int i = 0;
  for (Move &move : args->ai->cachedMoves()) {
    customArgs.loopBody(args, move, i);
    otherAIsRankingsOfMove(args, move);
    i++;
    if (i >= customArgs.numMovesToShow) {
      break;
    }
  }
  customArgs.postLoop(args);
}

void staticPreLoop(struct AIArgs *args);
void staticLoopBody(struct AIArgs *args, Move &move, int i);
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
  *(args->m_hints) += "The top " +
                      to_string(customArgs->genericArgs.numMovesToShow) +
                      " moves are:\n";
}

void staticLoopBody(struct AIArgs *args, Move &move, int i) {
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
void greedyLoopBody(struct AIArgs *args, Move &move, int i);
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
  *(args->m_hints) += "The top " +
                      to_string(customArgs->genericArgs.numMovesToShow) +
                      " highest scoring moves are:\n";
}

void greedyLoopBody(struct AIArgs *args, Move &move, int i) {
  int moveScore = move.score;
  LongLetterString moveAsStr =
      QuackleIO::Util::moveToDetailedString(move).toStdString();
  *(args->m_hints) += to_string(i + 1) + ": " + moveAsStr +
                      ", with a score of " + to_string(moveScore) + ".\n";
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
void champLoopBody(struct AIArgs *args, Move &move, int i);
void champPostLoop(struct AIArgs *args);

struct ChampArgs {
  struct GenericArgs genericArgs = {champPreLoop, champLoopBody, champPostLoop,
                                    3};
};

void champPreLoop(struct AIArgs *args) {
  struct ChampArgs *customArgs = (struct ChampArgs *)args->customArgs;
  *(args->m_hints) += "The top " +
                      to_string(customArgs->genericArgs.numMovesToShow) +
                      " moves with highest win% are:\n";
}

void champLoopBody(struct AIArgs *args, Move &move, int i) {
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

vector<ComputerPlayer *> HintsGenerator::blacklistedAIs() const {
  vector<ComputerPlayer *> blacklist;
  for (auto ai : m_ais) {
    if (ai->isChamp() && !m_shouldGenChampHints) {
      blacklist.push_back(ai);
    }
  }
  return blacklist;
}

vector<ComputerPlayer *> HintsGenerator::whitelistedAIs() const {
  vector<ComputerPlayer *> whitelist;
  for (auto ai : m_ais) {
    if (!ai->isChamp() || m_shouldGenChampHints) {
      whitelist.push_back(ai);
    }
  }
  return whitelist;
}

LongLetterString HintsGenerator::generateHints() {
  LongLetterString appendLater;
  LongLetterString appendNow;
  bool shouldAppendNow;

  for (ComputerPlayer *ai : whitelistedAIs()) {
    ai->moves(NUM_MOVES_TO_GEN);
  }

  for (ComputerPlayer *ai : m_ais) {
    shouldAppendNow = true;
    createAITitle(ai, &appendNow);

    struct AIArgs args = {&appendNow, ai, 0, whitelistedAIs()};
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
      shouldAppendNow = false;
      appendNow += "Hints for " + ai->name() + " have been disabled.\n";
    }
    appendNow += "\n\n";

    if (shouldAppendNow) {
      m_hints += appendNow;
    } else {
      appendLater += appendNow;
    }
    appendNow.clear();
  }

  m_hints += appendLater;
  return m_hints;
}

LongLetterString HintsGenerator::getHints() { return m_hints; }

void HintsGenerator::clearHints() { m_hints.clear(); }
