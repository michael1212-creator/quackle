#include <quackleio/util.h>

#include "alphabetparameters.h"
#include "hintsgenerator.h"

#include <algorithm>
#include <catchall.h>
#include <hint.h>
#include <message.h>

using namespace Quackle;

HintsGenerator::HintsGenerator(TopLevel *toplevel) {
  if (toplevel) {
    connect(this, &HintsGenerator::kibitzAs, toplevel, &TopLevel::kibitzAs);
  }
}

void HintsGenerator::movesAs(ComputerPlayer *ai, bool shouldClone,
                             bool updateGameMoves) {
  emit kibitzAs(ai, this, shouldClone, updateGameMoves);
}

HintsGenerator::~HintsGenerator() {
  for (long unsigned int i = 0; i < m_ais.size(); i++) {
    delete m_ais.back();
    m_ais.pop_back();
  }
}

LongLetterString HintsGenerator::createAITitle(ComputerPlayer *ai) {
  LongLetterString ret = "";

  ret += "According to " + ai->name() + "\n";
  ret += "-----------------------------\n";

  return ret;
}

void HintsGenerator::addAI(ComputerPlayer *ai) {
  m_ais.insert(m_ais.end(), ai);
}

void HintsGenerator::addAIs(vector<ComputerPlayer *> ais) {
  m_ais.insert(m_ais.end(), ais.begin(), ais.end());
}

LongLetterString otherAIsRankingsOfMove(struct AIArgs *args, Move &move,
                                        LongLetterString indent = "- ");

struct AIArgs {
  ComputerPlayer *ai;
  void *customArgs;
  vector<ComputerPlayer *> whitelistedAIs;
};

void HintsGenerator::updateAIState(const Quackle::GamePosition &position) {
  for (auto ai : m_ais) {
    ai->clearCachedMoves();
    ai->setPosition(position);
  }
  clearHints();
}

void HintsGenerator::positionChanged(const Quackle::GamePosition &position) {}

LongLetterString
HintsGenerator::committed(const Quackle::GamePosition &position,
                          Quackle::Move &move) {
  LongLetterString otherAiRankings =
      "Your move was: " +
      QuackleIO::Util::moveToDetailedString(move).toStdString();
  struct AIArgs aiargs = {NULL, NULL, whitelistedAIs()};
  otherAiRankings += otherAIsRankingsOfMove(&aiargs, move);

  updateAIState(position);

  return otherAiRankings;
}

#define MIN_NUM_MOVES_TO_GEN 20

struct GenericArgs {
  LongLetterString (*preLoop)(struct AIArgs *);
  LongLetterString (*loopBody)(struct AIArgs *, Move &, int);
  LongLetterString (*postLoop)(struct AIArgs *);
  int numMovesToShow;
};

LongLetterString otherAIsRankingsOfMove(struct AIArgs *args, Move &move,
                                        LongLetterString indent) {
  LongLetterString ret = "";
  ret += "\n";
  for (auto it : args->whitelistedAIs) {
    int moveRank = it->rankMove(move);
    ret += indent + it->name() + " ";
    if (moveRank < 0) {
      if (it->isGreedy() && move.action == Move::Exchange) {
        ret += "Does not generate exchange moves";
      } else {
        ret += "only has top " + to_string(-moveRank) +
               " moves, and this move is not among them";
      }
    } else if (moveRank > 0) {
      ret += "ranks this move as number " + to_string(moveRank);
    } else {
      ret += "moves have not been generated yet";
    }
    ret += ".\n";
  }
  ret += "\n";

  return ret;
}

LongLetterString collectHints(struct AIArgs *args) {
  LongLetterString ret = "";
  if (args->ai->cachedMoves().empty()) {
    ret += "This AI is still thinking. Please check back in a few moments.";
    return ret;
  }

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

  ret += customArgs.preLoop(args);

  int i = 0;
  for (Move &move : args->ai->cachedMoves()) {
    // TODO mm (medium-low): can we make moves clickable, like in the choices
    //  window?
    ret += customArgs.loopBody(args, move, i);
    ret += otherAIsRankingsOfMove(args, move);
    i++;
    if (i >= customArgs.numMovesToShow) {
      break;
    }
  }
  ret += customArgs.postLoop(args);

  return ret;
}

LongLetterString staticPreLoop(struct AIArgs *args);
LongLetterString staticLoopBody(struct AIArgs *args, Move &move, int i);
LongLetterString staticPostLoop(struct AIArgs *args);

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

LongLetterString staticPreLoop(struct AIArgs *args) {
  LongLetterString ret = "";
  struct StaticArgs *customArgs = (struct StaticArgs *)args->customArgs;
  ret += "The top " + to_string(customArgs->genericArgs.numMovesToShow) +
         " moves are:\n";
  return ret;
}

LongLetterString staticLoopBody(struct AIArgs *args, Move &move, int i) {
  LongLetterString ret = "";
  LongLetterString moveAsStr =
      QuackleIO::Util::moveToDetailedString(move).toStdString();
  char buf[16];
  TWO_DP(move.equity);
  ret += to_string(i + 1) + ": " + moveAsStr + ", has valuation " + buf +
         ", coming from:\n";
  ret += move.hint()->hint("  ");

  return ret;
}

LongLetterString staticPostLoop(struct AIArgs *args) {
  LongLetterString ret = "";
  char buf[16];
  struct StaticArgs *customArgs = (struct StaticArgs *)args->customArgs;
  TWO_DP(customArgs->lowestValuation);
  ret += "\nReally bad moves can have a valuation of " + LongLetterString(buf);

  TWO_DP(customArgs->highValuation);
  ret +=
      ", whereas really good moves can have a valuation of its own score, "
      "which is theoretically up to " +
      LongLetterString(buf) +
      ". However, most very good moves will be within the 50 to 100 range.\n";

  return ret;
}

LongLetterString greedyPreLoop(struct AIArgs *args);
LongLetterString greedyLoopBody(struct AIArgs *args, Move &move, int i);
LongLetterString greedyPostLoop(struct AIArgs *args);

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

LongLetterString greedyPreLoop(struct AIArgs *args) {
  LongLetterString ret = "";
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  ret += "The top " + to_string(customArgs->genericArgs.numMovesToShow) +
         " highest scoring moves are:\n";

  return ret;
}

LongLetterString greedyLoopBody(struct AIArgs *args, Move &move, int i) {
  LongLetterString ret = "";
  int moveScore = move.score;
  LongLetterString moveAsStr =
      QuackleIO::Util::moveToDetailedString(move).toStdString();
  ret += to_string(i + 1) + ": " + moveAsStr + ", with a score of " +
         to_string(moveScore) + ".";

  return ret;
}

LongLetterString greedyPostLoop(struct AIArgs *args) {
  LongLetterString ret = "";
  struct GreedyArgs *customArgs = (struct GreedyArgs *)args->customArgs;
  ret += "\nThe highest scoring word ever recorded in Scrabble is " +
         LongLetterString(customArgs->highestScoringWord) + " for " +
         to_string(customArgs->highestScore) + " points.\n" +
         "The theoretical maximum is " +
         to_string(customArgs->theoreticalMaxScore) + " points for " +
         customArgs->highestScoringTheoreticalWord + " (" + customArgs->source +
         ").\n";

  return ret;
}

LongLetterString champPreLoop(struct AIArgs *args);
LongLetterString champLoopBody(struct AIArgs *args, Move &move, int i);
LongLetterString champPostLoop(struct AIArgs *args);

struct ChampArgs {
  struct GenericArgs genericArgs = {champPreLoop, champLoopBody, champPostLoop,
                                    3};
};

LongLetterString champPreLoop(struct AIArgs *args) {
  LongLetterString ret = "";
  struct ChampArgs *customArgs = (struct ChampArgs *)args->customArgs;
  ret += "The top " + to_string(customArgs->genericArgs.numMovesToShow) +
         " moves with highest win% are:\n";

  return ret;
}

LongLetterString champLoopBody(struct AIArgs *args, Move &move, int i) {
  LongLetterString ret = "";
  // TODO mm (medium): do the thing
  LongLetterString moveAsStr =
      QuackleIO::Util::moveToDetailedString(move).toStdString();
  Hint *hint = move.hint();
  char buf[16];
  TWO_DP(100 * move.win);
  ret += to_string(i + 1) + ": " + moveAsStr + ", has win% chance of " + buf +
         "%, coming from:\n";
  ret += move.hint()->hint("  ");

  return ret;
}

LongLetterString champPostLoop(struct AIArgs *args) {
  LongLetterString ret = "";
  ret += "\nHigher win% (starting at 0%, up to 100%) means that the "
         "AI is more confident of a win using this move.\n";

  return ret;
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

void HintsGenerator::generateHints(bool forceUpdateMoves) {
  for (ComputerPlayer *ai : whitelistedAIs()) {
    if (forceUpdateMoves || ai->cachedMoves().empty()) {
      movesAs(ai);
      threadFinishedGeneratingMoves();
    }
  }
}

void HintsGenerator::threadFinishedGeneratingMoves() {
  LongLetterString appendLater;
  LongLetterString appendNow;
  bool shouldAppendNow;
  // TODO mm (medium-low): allow player to choose how many top moves are shown

  clearHints();
  for (ComputerPlayer *ai : m_ais) {
    shouldAppendNow = true;
    bool skipCollection = false;
    appendNow += createAITitle(ai);

    struct AIArgs args = {ai, NULL, whitelistedAIs()};
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
      appendNow += collectHints(&args);
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

  emit hintsUpdated();
}

LongLetterString HintsGenerator::getHints() { return m_hints; }

void HintsGenerator::clearHints() { m_hints.clear(); }
