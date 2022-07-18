#include "hintsgenerator.h"
#include "statichint.h"
#include "alphabetparameters.h"

using namespace Quackle;

HintsGenerator::HintsGenerator() {}

HintsGenerator::~HintsGenerator() {}

void HintsGenerator::createAITitle(ComputerPlayer *ai) {
  m_hints += "According to " + ai->name() + "\n";
  m_hints += "-----------------------------\n\n";
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

LongLetterString HintsGenerator::generateHints() {
  for (ComputerPlayer *ai : m_ais) {
    createAITitle(ai);

    //TODO mm (high): do the thing
    if (ai->isStatic()) {

    } else if (ai->isGreedy()) {

    } else if (ai->isChamp()) {

    } else {

    }
  }
  return m_hints;
}

LongLetterString HintsGenerator::getHints() {
  return m_hints;
}

void HintsGenerator::clearHints() {
  m_hints = "";
}
