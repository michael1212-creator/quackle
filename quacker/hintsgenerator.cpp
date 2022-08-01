#include <quackleio/util.h>

#include "alphabetparameters.h"
#include "hint.h"
#include "hintsgenerator.h"

#include <algorithm>
#include <catchall.h>

using namespace Quackle;

HintsGenerator::HintsGenerator(TopLevel *toplevel) {
  if (toplevel) {
    connect(this, SIGNAL(kibitzAs(Quackle::ComputerPlayer *, bool, bool)),
            toplevel, SLOT(kibitzAs(Quackle::ComputerPlayer *, bool, bool)));
  }
}

void HintsGenerator::movesAs(ComputerPlayer *ai, bool shouldClone,
                             bool updateGameMoves) {
  emit kibitzAs(ai, shouldClone, updateGameMoves);
}

HintsGenerator::~HintsGenerator() {
  for (long unsigned int i = 0; i < m_ais.size(); i++) {
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
    // TODO mm (medium): rate player's move which they just made (just show its
    //  ranking in various AI's cached lists)
    ai->setPosition(position);
    ai->clearCachedMoves();
  }
}

struct AIArgs {
  LongLetterString *m_hints;
  ComputerPlayer *ai;
  void *customArgs;
  vector<ComputerPlayer *> whitelistedAIs;
};

#define MIN_NUM_MOVES_TO_GEN 20

struct GenericArgs {
  void (*preLoop)(struct AIArgs *);
  void (*loopBody)(struct AIArgs *, Move &, int);
  void (*postLoop)(struct AIArgs *);
  int numMovesToShow;
};

void otherAIsRankingsOfMove(struct AIArgs *args, Move &move,
                            LongLetterString indent = "- ") {
  *(args->m_hints) += "\n";
  for (auto it : args->whitelistedAIs) {
    int moveRank = it->rankMove(move);
    *(args->m_hints) += indent + it->name() + " ";
    if (moveRank < 0) {
      if (it->isGreedy() && move.action == Move::Exchange) {
        *(args->m_hints) += "Does not generate exchange moves";
      } else {
        *(args->m_hints) += "only has top " + to_string(-moveRank) +
                            " moves, and this move is not among them";
      }
    } else if (moveRank > 0) {
      *(args->m_hints) += "ranks this move as number " + to_string(moveRank);
    } else {
      *(args->m_hints) += "moves have not been generated yet";
    }
    *(args->m_hints) += ".\n";
  }
  *(args->m_hints) += "\n";
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
    // TODO mm (medium-low): can we make moves clickable, like in the choices
    //  window?
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

#define THEORETICAL_MAX_SCORE 1778
#define HIGHEST_SCORE 365

struct StaticArgs {
  struct GenericArgs genericArgs = {staticPreLoop, staticLoopBody,
                                    staticPostLoop, 3};

  union {
    double lowestValuation = -40;
    double lowestEquity;
  };
  union {
    double highValuation = THEORETICAL_MAX_SCORE;
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
  LongLetterString moveAsStr =
      QuackleIO::Util::moveToDetailedString(move).toStdString();
  char buf[16];
  TWO_DP(move.equity);
  *(args->m_hints) += to_string(i + 1) + ": " + moveAsStr + ", has valuation " +
                      buf + ", coming from:\n";
  *(args->m_hints) += move.hint()->hint("  ");
}

void staticPostLoop(struct AIArgs *args) {
  char buf[16];
  struct StaticArgs *customArgs = (struct StaticArgs *)args->customArgs;
  TWO_DP(customArgs->lowestValuation);
  *(args->m_hints) +=
      "\nReally bad moves can have a valuation of " + LongLetterString(buf);

  TWO_DP(customArgs->highValuation);
  *(args->m_hints) +=
      ", whereas really good moves can have a valuation of its own score, "
      "which is theoretically up to " +
      LongLetterString(buf) +
      ". However, most very good moves will be within the 50 to 100 range.\n";
}

void greedyPreLoop(struct AIArgs *args);
void greedyLoopBody(struct AIArgs *args, Move &move, int i);
void greedyPostLoop(struct AIArgs *args);

struct GreedyArgs {
  struct GenericArgs genericArgs = {greedyPreLoop, greedyLoopBody,
                                    greedyPostLoop, 5};

  int highestScore = HIGHEST_SCORE;
  int theoreticalMaxScore = THEORETICAL_MAX_SCORE;
  char highestScoringWord[9] = "QUIXOTRY";
  char highestScoringTheoreticalWord[16] = "OXYPHENBUTAZONE";
  char source[69] =
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
                      ", with a score of " + to_string(moveScore) + ".";
}

void greedyPostLoop(struct AIArgs *args) {
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  *(args->m_hints) +=
      "\nThe highest scoring word ever recorded in Scrabble is " +
      LongLetterString(customArgs->highestScoringWord) + " for " +
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
  // TODO mm (low-medium): do the thing
  LongLetterString moveAsStr =
      QuackleIO::Util::moveToDetailedString(move).toStdString();
  Hint *hint = move.hint();
  char buf[16];
  TWO_DP(100 * move.win);
  *(args->m_hints) += to_string(i + 1) + ": " + moveAsStr +
                      ", has win% chance of " + buf + "%, coming from:\n";
  *(args->m_hints) += move.hint()->hint("  ");
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
    if (!(ai->isChamp()) || (ai->isChamp() && m_shouldGenChampHints)) {
      whitelist.push_back(ai);
    }
  }
  return whitelist;
}

#define MAX_ARG_SIZE                                                           \
  std::max({sizeof(struct StaticArgs), sizeof(struct GreedyArgs),              \
            sizeof(struct ChampArgs)})

union CustomArgs {
  unsigned char buf[MAX_ARG_SIZE];
};

LongLetterString HintsGenerator::generateHints(bool forceUpdateMoves) {
  for (ComputerPlayer *ai : whitelistedAIs()) {
    if (forceUpdateMoves || ai->cachedMoves().empty()) {
      // TODO mm (medium-high): can we make this non-blocking?
      //  check out TopLevel::kibitz(int, ComputerPlayer *) for inspiration.
      movesAs(ai);
    }
  }

  LongLetterString appendLater;
  LongLetterString appendNow;
  bool shouldAppendNow;
  // TODO mm (medium-high): if a hint is not shown, mention ir as a way we could
  //  have potentially increased the valuation of a move. Also, hide 0 score
  //  valuations with this method
  for (ComputerPlayer *ai : m_ais) {
    shouldAppendNow = true;
    bool skipCollection = false;
    createAITitle(ai, &appendNow);

    struct AIArgs args = {&appendNow, ai, NULL, whitelistedAIs()};
    union CustomArgs customArgs;
    if (ai->isStatic()) {
      struct StaticArgs staticArgs;
      *((struct StaticArgs *)(&customArgs)) = staticArgs;
    } else if (ai->isGreedy()) {
      struct GreedyArgs greedyArgs;
      *((struct GreedyArgs *)(&customArgs)) = greedyArgs;
    } else if (ai->isChamp() && m_shouldGenChampHints) {
      struct ChampArgs champArgs;
      *((struct ChampArgs *)(&customArgs)) = champArgs;
    } else {
      shouldAppendNow = false;
      skipCollection = true;
      appendNow += "Hints for " + ai->name() + " have been disabled.\n";
    }

    if (!skipCollection) {
      args.customArgs = &customArgs;
      collectHints(&args);
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
