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

void collectHints(LongLetterString *m_hints, ComputerPlayer *ai,
               void (*preLoop)(LongLetterString *, ComputerPlayer *, void *),
               void (*loopBody)(LongLetterString *, ComputerPlayer *, Hint, int, void *),
               void (*postLoop)(LongLetterString *, ComputerPlayer *, void *),
               void *args) {
  int i = 0;
  preLoop(m_hints, ai, args);
  for (Hint hint : ai->hints()) {
    loopBody(m_hints, ai, hint, i, args);
    i++;
  }
  postLoop(m_hints, ai, args);
}

void staticPreLoop(LongLetterString *m_hints, ComputerPlayer *, void *) {
  // TODO mm (high): do the thing
  *m_hints += "Static\n";
}

void staticLoopBody(LongLetterString *m_hints, ComputerPlayer *, Hint, int i, void *) {
  // TODO mm (high): do the thing
  *m_hints += "Static Hint " + to_string(i) + "\n";
}

struct StaticArgs {/*TODO mm (medium): Is this needed?*/};

void greedyPreLoop(LongLetterString *m_hints, ComputerPlayer *, void *) {
  // TODO mm (high): do the thing
  *m_hints += "Greedy\n";
}

void greedyLoopBody(LongLetterString *m_hints, ComputerPlayer *, Hint, int i, void *) {
  // TODO mm (high): do the thing
  *m_hints += "Greedy Hint " + to_string(i) + "\n";
}

struct GreedyArgs {/*TODO mm (medium): Is this needed?*/};

void champPreLoop(LongLetterString *m_hints, ComputerPlayer *, void *) {
  // TODO mm (high): do the thing
  *m_hints += "Champ\n";
}

void champLoopBody(LongLetterString *m_hints, ComputerPlayer *, Hint, int i, void *) {
  // TODO mm (high): do the thing
  *m_hints += "Champ Hint " + to_string(i) + "\n";
}

struct ChampArgs {/*TODO mm (medium): Is this needed?*/};

void postLoop(LongLetterString *m_hints, ComputerPlayer *, void *) {
  *m_hints += "\n";
}

LongLetterString HintsGenerator::generateHints() {
  for (ComputerPlayer *ai : m_ais) {
    createAITitle(ai);

    if (ai->isStatic()) {
      struct StaticArgs args;
      collectHints(&m_hints, ai, staticPreLoop, staticLoopBody, postLoop, &args);
    } else if (ai->isGreedy()) {
      struct GreedyArgs args;
      collectHints(&m_hints, ai, greedyPreLoop, greedyLoopBody, postLoop, &args);
    } else if (ai->isChamp()) {
      struct ChampArgs args;
      collectHints(&m_hints, ai, champPreLoop, champLoopBody, postLoop, &args);
    } else {
      m_hints +=
          "Hints for " + ai->name() + " have not been implemented yet.\n\n";
    }
  }
  return m_hints;
}

LongLetterString HintsGenerator::getHints() { return m_hints; }

void HintsGenerator::clearHints() { m_hints = ""; }
