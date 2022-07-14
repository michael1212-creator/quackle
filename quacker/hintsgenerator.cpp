//
// Created by mihej on 14/07/22.
//

#include "hintsgenerator.h"

using namespace Quackle;

HintsGenerator::HintsGenerator() {}

HintsGenerator::~HintsGenerator() {}

void HintsGenerator::addAI(ComputerPlayer *ai) {
  m_ais.insert(m_ais.end()-1, ai);
  //TODO mm: actually add the AI to the hints generators
}

void HintsGenerator::addAIs(vector<ComputerPlayer *> ais) {
  m_ais.insert(m_ais.end()-1, ais.begin(), ais.end());
}
