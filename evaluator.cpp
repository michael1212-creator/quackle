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

double Evaluator::leaveValue(const LetterString &leave, Hint *hint) const {
  (void)leave;
  return 0;
}

////////////

double ScorePlusLeaveEvaluator::equity(const GamePosition &position,
                                       const Move &move) const {
  Hint *hint = move.hint();
  auto toAdd = move.effectiveScore();

  ADD_HINT((toAdd, ": for the score the move gives us.</div>", "<div style=\"margin-left:16px\">"));
  return playerConsideration(position, move) +
         sharedConsideration(position, move) /*= 0*/ + toAdd;
}

double ScorePlusLeaveEvaluator::playerConsideration(
    const GamePosition &position, const Move &move) const {
  auto leave = (position.currentPlayer().rack() - move).tiles();
  if (leave.empty()) {
    Hint *hint = move.hint();
    ADD_HINT(
        ("<br/><br/>As this move is a bingo, there is no rack leave to evaluate.<br/><br/>"));
    return 0;
  }
  return leaveValue(leave, move.hint());
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
  LetterString alphabetized = String::alphabetize(leave);

  // Uncomment 3 lines for the calculations after it
  //  to be done using a lookup table:
  /*
      if (QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves() &&
          QUACKLE_STRATEGY_PARAMETERS->superleave(alphabetized))
        return QUACKLE_STRATEGY_PARAMETERS->superleave(alphabetized);
  */

  ADD_HINT(("<br/>We now look at our rack leave (leftover tiles - " +
            QUACKLE_ALPHABET_PARAMETERS->userVisible(leave) + ")."));

  double value = 0;

  ADD_HINT(("<div style = \"margin-left: 16px\">"));

  if (!leave.empty()) {
    LetterString uniqleave;

    if (QUACKLE_STRATEGY_PARAMETERS->hasWorths()) {
      // each letter is assigned a value for how good it is; add it to value

      ADD_HINT(("Each letter, duplicate letters and two unique letters factor "
                "into the valuation calculation. These are always the same for "
                "the same letters:"));
      ADD_HINT(("Single letters:"));
      ADD_HINT(("<ul>"));
      for (const auto &leaveIt : leave) {
        double toAdd = QUACKLE_STRATEGY_PARAMETERS->tileWorth(leaveIt);

        ADD_HINT(
            (toAdd,
             ": " + QUACKLE_ALPHABET_PARAMETERS->userVisible(leaveIt) + "</li>",
             "<li>"));
        value += toAdd;
      }
      ADD_HINT(("</ul>"));
    }

    if (QUACKLE_STRATEGY_PARAMETERS->hasSyn2()) {
      // calculate value for pairs of the same letter
      bool hintAdded = false;
      for (unsigned int i = 0; i < alphabetized.length() - 1; ++i) {
        if (alphabetized[i] == alphabetized[i + 1]) {
          if (!hintAdded) {

            ADD_HINT(("Duplicate letters:"));
            ADD_HINT(("<ul>"));
            hintAdded = true;
          }
          double toAdd = QUACKLE_STRATEGY_PARAMETERS->syn2(alphabetized[i],
                                                           alphabetized[i]);

          ADD_HINT((
              toAdd,
              ": " + QUACKLE_ALPHABET_PARAMETERS->userVisible(alphabetized[i]) +
                  QUACKLE_ALPHABET_PARAMETERS->userVisible(alphabetized[i]) +
                  "</li>",
              "<li>"));
          value += toAdd;
        }
      }
      if (!hintAdded) {
        ADD_HINT(("The rack leave from this move has no duplicate letters."));
      } else {
        ADD_HINT(("</ul>"));
      }
    }

    // remove duplicate letters
    uniqleave += alphabetized[0];
    for (unsigned int i = 1; i < alphabetized.length(); ++i) {
      if (uniqleave[uniqleave.length() - 1] != alphabetized[i]) {
        uniqleave += alphabetized[i];
      }
    }

    if (uniqleave.length() >= 2 && QUACKLE_STRATEGY_PARAMETERS->hasSyn2()) {
      double synergy = 0;
      // consider the value add of each pair of unique letters
      int numberOfSynergies = 0;
      ADD_HINT(("Two letter synergies:"));
      ADD_HINT(("<ul>"));
      for (unsigned int i = 0; i < uniqleave.length() - 1; ++i) {
        for (unsigned int j = i + 1; j < uniqleave.length(); ++j) {
          double toAdd =
              QUACKLE_STRATEGY_PARAMETERS->syn2(uniqleave[i], uniqleave[j]);
          ADD_HINT((toAdd,
                    ": " +
                        QUACKLE_ALPHABET_PARAMETERS->userVisible(uniqleave[i]) +
                        QUACKLE_ALPHABET_PARAMETERS->userVisible(uniqleave[j]) +
                        "</li>",
                    "<li>"));
          synergy += toAdd;
          numberOfSynergies++;
        }
      }
      ADD_HINT(("</ul>"));

      if (numberOfSynergies > 1) {
        ADD_HINT((synergy, "", "Total (two letter) synergy = "));
      }

      // letters with a worth <-5.5 are considered bad; does the player hold
      // any?
      bool holding_bad_tile = false;
      for (unsigned int i = 0; i < uniqleave.length(); ++i) {
        if (QUACKLE_STRATEGY_PARAMETERS->tileWorth(uniqleave[i]) < -5.5) {
          holding_bad_tile = true;
        }
      }

      ADD_HINT(("If we have no letters which are very bad (-5.5 or "
                "less), and good synergy (3 or more) of non-duplicate letters, "
                "we can get a bonus."));
      // if there is no bad letter and the pairs of letters we have are good
      //  (synergise well), then add extra value to the leave rack
      if ((synergy > 3.0) && !holding_bad_tile) {
        double toAdd = 1.5 * (synergy - 3.0);
        ADD_HINT((toAdd, " = 1.5*(synergy-3): our bonus valuation.</div>",
                  "<div style = \"margin-left: 16px\">"));
        synergy += toAdd;
      }

      value += synergy;
    } else if (QUACKLE_STRATEGY_PARAMETERS->hasSyn2()) {
      assert(uniqleave.length() == 1);
      ADD_HINT(("It is possible to gain value from two letter synergies, "
                "however our rack leave consists of only one letter."));
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
  ADD_HINT((toAdd, ": vowel-consonant (v:c) balance of " + to_string(vowels) +
                       "v:" + to_string(cons) +
                       "c. This is maximised around a ratio of 3v:3c."));
  value += toAdd;

  ADD_HINT(("</div>"));

  if (value < -40) {
    ADD_HINT(("-40 is the lowest we would ever need to consider."));
    value = -40;
  }

  return value;
}
