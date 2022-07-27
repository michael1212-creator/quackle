/*
 *  Quackle -- Crossword game artificial intelligence and analysis tool
 *  Copyright (C) 2005-2019 Jason Katz-Brown, John O'Laughlin, and John Fultz.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "evaluator.h"
#include "board.h"
#include "catchall.h"
#include "datamanager.h"
#include "game.h"
#include "strategyparameters.h"

using namespace Quackle;

double Evaluator::equity(const GamePosition &position, const Move &move) const {
  (void)position;
  return move.effectiveScore();
}

double Evaluator::playerConsideration(const GamePosition &position,
                                      const Move &move) const {
  (void)position;
  (void)move;
  return 0;
}

double Evaluator::sharedConsideration(const GamePosition &position,
                                      const Move &move) const {
  (void)position;
  (void)move;
  return 0;
}

double Evaluator::leaveValue(const LetterString &leave) const {
  (void)leave;
  return 0;
}

////////////

double ScorePlusLeaveEvaluator::equity(const GamePosition &position,
                                       const Move &move) const {
  Hint *hint = move.hint();
  auto toAdd = move.effectiveScore();
  ADD_HINT(to_string(toAdd));
  return playerConsideration(position, move) +
         sharedConsideration(position, move) /*= 0*/ + toAdd;
}

double
ScorePlusLeaveEvaluator::playerConsideration(const GamePosition &position,
                                             const Move &move) const {
  return leaveValue((position.currentPlayer().rack() - move).tiles(),
                    move.hint());
}

double
ScorePlusLeaveEvaluator::sharedConsideration(const GamePosition &position,
                                             const Move &move) const {
  (void)position;
  (void)move;
  return 0;
}

double ScorePlusLeaveEvaluator::leaveValue(const LetterString &leave,
                                           Hint *hint) const {
  char buf[16];
  LetterString alphabetized = String::alphabetize(leave);

  // Uncomment 3 lines for the calculations after it
  //  to be done using a lookup table:
  /*
      if (QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves() &&
          QUACKLE_STRATEGY_PARAMETERS->superleave(alphabetized))
        return QUACKLE_STRATEGY_PARAMETERS->superleave(alphabetized);
  */

  double value = 0;

  if (!leave.empty()) {
    double synergy = 0;
    LetterString uniqleave;

    // each letter is assigned a value for how good it is; add it to value
    if (QUACKLE_STRATEGY_PARAMETERS->hasWorths()) {
      for (const auto &leaveIt : leave) {
        double toAdd = QUACKLE_STRATEGY_PARAMETERS->tileWorth(leaveIt);
        TWO_DP(toAdd);
        ADD_HINT(buf);
        value += toAdd;
      }
    }

    // calculate value for pairs of the same letter
    if (QUACKLE_STRATEGY_PARAMETERS->hasSyn2()) {
      for (unsigned int i = 0; i < alphabetized.length() - 1; ++i) {
        if (alphabetized[i] == alphabetized[i + 1]) {
          double toAdd = QUACKLE_STRATEGY_PARAMETERS->syn2(alphabetized[i],
                                                           alphabetized[i]);
          TWO_DP(toAdd);
          ADD_HINT(buf);
          value += toAdd;
        }
      }
    }

    // remove duplicate letters
    uniqleave += alphabetized[0];
    for (unsigned int i = 1; i < alphabetized.length(); ++i) {
      if (uniqleave[uniqleave.length() - 1] != alphabetized[i]) {
        uniqleave += alphabetized[i];
      }
    }

    // consider the value add of each pair of unique letters
    if (uniqleave.length() >= 2 && QUACKLE_STRATEGY_PARAMETERS->hasSyn2()) {
      for (unsigned int i = 0; i < uniqleave.length() - 1; ++i) {
        for (unsigned int j = i + 1; j < uniqleave.length(); ++j) {
          double toAdd =
              QUACKLE_STRATEGY_PARAMETERS->syn2(uniqleave[i], uniqleave[j]);
          TWO_DP(toAdd);
          ADD_HINT(buf);
          synergy += toAdd;
        }
      }

      // letters with a worth <-5.5 are considered bad; does the player hold
      // any?
      bool holding_bad_tile = false;
      for (unsigned int i = 0; i < uniqleave.length(); ++i) {
        if (QUACKLE_STRATEGY_PARAMETERS->tileWorth(uniqleave[i]) < -5.5) {
          holding_bad_tile = true;
        }
      }

      // if there is no bad letter and the pairs of letters we have are good
      //  (synergise well), then add extra value to the leave rack
      if ((synergy > 3.0) && !holding_bad_tile) {
        double toAdd = 1.5 * (synergy - 3.0);
        TWO_DP(toAdd);
        ADD_HINT(buf);
        synergy += toAdd;
      }

      value += synergy;
    }
  }

  int vowels = 0;
  int cons = 0;

  // add extra score for good balance of vowels and consonants
  for (const auto &leaveIt : leave) {
    if (leaveIt != QUACKLE_BLANK_MARK) {
      if (QUACKLE_ALPHABET_PARAMETERS->isVowel(leaveIt)) {
        vowels++;
      } else {
        cons++;
      }
    }
  }

  const float vcvalues[8][8] = {
      {0.0, 0.0, -1.0, -2.5, -5.0, -8.5, -13.5, -18.5},
      {-1.0, 0.0, 0.5, 0.0, -1.5, -5.0, -10.0, 0.0},
      {-3.5, -1.0, 0.5, 1.5, -1.5, -3.0, 0.0, 0.0},
      {-7.0, -3.5, -0.5, 2.5, 0.0, 0.0, 0.0, 0.0},
      {-10.0, -6.5, -3.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {-13.5, -11.5, -8.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {-18.5, -16.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
      {-23.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
  };

  double toAdd = vcvalues[vowels][cons];
  TWO_DP(toAdd);
  ADD_HINT(buf);
  value += toAdd;

  if (value < -40) {
    value = -40;
  }

  return value;
}
