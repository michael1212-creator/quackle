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

#include "catchall.h"
#include "board.h"
#include "datamanager.h"
#include "game.h"
#include "message.h"
#include "strategyparameters.h"

using namespace Quackle;

double CatchallEvaluator::equity(const GamePosition &position,
                                 const Move &move) const {
  Hint *hint = move.hint();
  if (position.board().isEmpty()) { // starting player
    ADD_HINT("Because the board is currently empty:");
    double adjustment = 0;

    if (move.action == Move::Place) {
      // this assumes symmetry between horizontal and vertical start:

      int start =
          move.startcol; // between 1 and 7 (inclusive); assumes horizontal word
      if (move.startrow < start) {
        start = move.startrow; // otherwise the word is vertical;
      }

      LetterString wordTiles = move.tiles();
      int length = wordTiles.length();

      // count (in the form of a bit vector) how many consonants there are
      int consbits = 0;
      for (signed int i = wordTiles.length() - 1; i >= 0; i--) {
        consbits <<= 1;
        // any blank letter has already been assigned an actual value;
        //  this just deals with that so that it is correctly counted
        if (QUACKLE_ALPHABET_PARAMETERS->isVowel(
                QUACKLE_ALPHABET_PARAMETERS->clearBlankness(wordTiles[i]))) {
          consbits |= 1;
        }
      }

      // add weighting based on the consonant placement of the word in
      // conjunction with its location and length
      adjustment =
          QUACKLE_STRATEGY_PARAMETERS->vcPlace(start, length, consbits);
      //      TWO_DP(adjustment);
      //      ADD_HINT("  " + LongLetterString(buf) +
      //               ": for the location and length of word, along with the
      //               ordering " "of consonants and vowels in the word. At this
      //               point of " "the game, horizontal vs. vertical placement
      //               is irrelevant due " "to symmetry.");
      ADD_HINT(adjustment,
               ": for the location and length of word, along with the ordering "
               "of consonants and vowels in the word. At this point of "
               "the game, horizontal vs. vertical placement is irrelevant due "
               "to symmetry.",
               "  ");
    } else {
      //'favour' exchange (as opposed to word placement) on initial turn
      // weighted by 3.5

      adjustment = 3.5;
      //      TWO_DP(adjustment);
      //      ADD_HINT("  " + LongLetterString(buf) +
      //               ": exchange moves get this as an extra by default "
      //               "on first placement.");
      ADD_HINT(
          adjustment,
          ": Since we are first, we can use this advantage to improve our "
          "rack, and wait for opponent to potentially open up score multiplier "
          "spaces",
          "  ");
    }

    // Finally, use other equity evaluator to determine rest of equity
    return ScorePlusLeaveEvaluator::equity(position, move) + adjustment;
  } else if (position.bag().size() > 0) {
    // if there are still tiles in the bag (i.e. we are not in an endgame
    // situation)
    int leftInBagPlusSeven =
        position.bag().size() - move.usedTiles().length() + 7;
    // index-6 represents how many tiles would be left in the bag after move
    double heuristicArray[12] = {-8.0, 0.0,  -0.5, -2.0, -3.5, -2.0,
                                 2.0,  10.0, 7.0,  4.0,  -1.0, -2.0};
    double timingHeuristic = 0.0;
    if (leftInBagPlusSeven <= 12) {
      timingHeuristic = heuristicArray[leftInBagPlusSeven - 1];
      //      TWO_DP(timingHeuristic);
      //      ADD_HINT(LongLetterString(buf) +
      //               ": since there would be 5 or less tiles left in the bag
      //               after " "this move, but the move doesn't finish the game.
      //               We want to be " "able to keep our options open, and
      //               having more tiles is what " "allows this. It also
      //               decreases the possibility of the opponent " "closing the
      //               game. Has range [-8, 10].");
      ADD_HINT(timingHeuristic,
               ": since there would be 5 or less tiles left in the bag after "
               "this move, but the move doesn't finish the game. We want to be "
               "able to keep our options open, and having more tiles is what "
               "allows this. It also decreases the possibility of the opponent "
               "closing the game. Has range [-8, 10].");
    }
    return ScorePlusLeaveEvaluator::equity(position, move) + timingHeuristic;
  } else {
    // When there are no more tiles in the bag; endgame situation
//    ADD_HINT(to_string(move.score) + ": for the score the move gives us.");
    ADD_HINT(move.score, ": for the score the move gives us.");
    return endgameResult(position, move) + move.score;
  }
}

double CatchallEvaluator::endgameResult(const GamePosition &position,
                                        const Move &move) const {
  Hint *hint = move.hint();
  Rack leave = position.currentPlayer().rack() - move;

  ADD_HINT("We know the exact rack of the opponent:");
  if (leave.empty()) {
    // the move ends the game
    ADD_HINT("Because the this move would end the game:", "  ");

    // add opposing player's sum of tiles score to deadwood
    double deadwood = 0;
    for (PlayerList::const_iterator it = position.players().begin();
         it != position.players().end(); ++it) {
      if (!(*it == position.currentPlayer())) {
        double toAdd = it->rack().score();

        char buf[16];
        TWO_DP(toAdd);
        ADD_HINT(2 * toAdd,
                 ": 2*" + LongLetterString(buf) + " for " + it->name() +
                     "'s rack score.",
                 "  ");
        deadwood += toAdd;
      }
    }

    ADD_HINT("(Doubled as we prefer to end the game ourselves due to the "
             "score bonus)",
             "  ");
    return deadwood * 2;
  }

  // If the move doesn't end the game, the opponent might have a chance of
  // ending the game, which can increase their score by a significant enough
  // amount, dependent on our rack.
  // The constant is there because we like ending the game (?)
  ADD_HINT(-8.00 - 2.61 * leave.score(),
           ": " + LongLetterString("-8.00-2.61*") + to_string(leave.score()) +
               ": This move would not end the game, meaning the opponent might "
               "have a chance to do so. This can increase their score "
               "proportionally to our remaining rack score.",
           "  ");
  return -8.00 - 2.61 * leave.score();
}
