//
// Created by mihej on 14/07/22.
//

#include <QString>

#include <quackleio/util.h>
#include "hintsgenerator.h"

using namespace Quackle;

HintsGenerator::HintsGenerator() {}

HintsGenerator::~HintsGenerator() {}

void HintsGenerator::createAITitle(ComputerPlayer *ai) {
  //TODO mm: extract the AI's name
  m_hints += QString("According to %1\n")
      .arg(QuackleIO::Util::uvStringToQString(ai->name())).toStdString();
  m_hints.append("-----------------------------");
}

void HintsGenerator::addAI(ComputerPlayer *ai) {
  m_ais.insert(m_ais.end() == m_ais.begin() ? m_ais.end() : m_ais.end() - 1, ai);
  // TODO mm: actually add the AI to the hints generators
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
  //TODO mm: do the thing

  return m_hints;
}

LongLetterString HintsGenerator::getHints() {
  return m_hints;
}

void HintsGenerator::clearHints() {
  m_hints = "";
}
