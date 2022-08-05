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

#include <iostream>

#include "bogowinplayer.h"
#include "clock.h"
#include "datamanager.h"
#include "endgameplayer.h"
#include "gameparameters.h"
#include "strategyparameters.h"
//#include "enumerator.h"

//#define DEBUG_COMPUTERPLAYER

using namespace Quackle;

SmartBogowin::SmartBogowin() {
  m_name = MARK_UV("Smart Bogowin");
  m_id = 110;
  m_minIterationsPerSecond = 2;
  m_maxIterationsPerSecond = 5;

  m_nestedMinIterationsPerSecond = 1;
  m_nestedMaxIterationsPerSecond = 1;

  m_parameters.secondsPerTurn = 20;

  m_additionalInitialCandidates = 13;

  m_inferring = false;
}

SmartBogowin::~SmartBogowin() {}

double SmartBogowin::bogopoints(Move &move) {
  if (move.win == 1)
    return move.equity + 1000;
  if (move.win == 0)
    return move.equity - 1000;

  int spread = m_simulator.currentPosition().spread(
      m_simulator.currentPosition().currentPlayer().id());
  int tiles = m_simulator.currentPosition().bag().size() + 7;

  if (move.win <= QUACKLE_STRATEGY_PARAMETERS->bogowin(-300, tiles, 0))
    return move.equity - 1000;
  if (move.win >= QUACKLE_STRATEGY_PARAMETERS->bogowin(300, tiles, 0))
    return move.equity + 1000;

  for (int x = -300; x <= 299; x++) {
    double lower = QUACKLE_STRATEGY_PARAMETERS->bogowin(x, tiles, 0);
    double upper = QUACKLE_STRATEGY_PARAMETERS->bogowin(x + 1, tiles, 0);

    if ((move.win >= lower) && (move.win <= upper)) {
      double fraction = (move.win - lower) / (upper - lower);
      return (double)x + fraction - spread;
    }
  }

  return 0;
}

Move SmartBogowin::move() { return moves(1).back(); }

MoveList SmartBogowin::moves(int nmoves) {
  Stopwatch stopwatch;

  if (currentPosition().bag().empty()) {
    // if the current bag is empty, then this is the wrong AI for move
    // generation;
    //  Call the end-game AI to do the work;
    signalFractionDone(0);
    EndgamePlayer endgame;
    endgame.setPosition(currentPosition());
    m_cachedMoves = endgame.moves(nmoves);
    return m_cachedMoves;
  }

  const int zerothPrune = 33;
  int plies = 2;

  if (currentPosition().bag().size() <= QUACKLE_PARAMETERS->rackSize() * 2 ) {
    // if there are 14 or less remaining tiles in the bag
    plies = -1;
  }

  const int initialCandidates = m_additionalInitialCandidates /* 13 */ + nmoves /* 15(?) */;

  currentPosition().kibitz(initialCandidates);

  // update the simulator to include all of the moves which the above kibitz
  // generated (these are also probably also the only ones?)
  m_simulator.setIncludedMoves(m_simulator.currentPosition().moves());

  // only keep the moves with equity at most 'zerothPrune' below the equity of
  // the move with highest equity; however if there are more than
  // 'initialCandidates' such moves, discard the rest.
  m_simulator.pruneTo(zerothPrune, initialCandidates);

  // Some moves are considered as immune to pruning; make sure they are included
  m_simulator.makeSureConsideredMovesAreIncluded();
  m_simulator.setIgnoreOppos(false);

  MoveList staticMoves = m_simulator.moves(true, false);
  m_simulator.moveConsideredMovesToBeginning(staticMoves); // doesn't seem to consider sortedness

  MoveList firstMove;
  MoveList simmedMoves;

  MoveList::const_iterator it = staticMoves.begin();

  firstMove.push_back(*it);

  signalFractionDone(0);

  m_simulator.setIncludedMoves(firstMove);
  m_simulator.simulate(plies, minIterations() /* 20 if nested, 40 if not OR 66 and 132 (if called from resolvent) */);

  Move best = *(m_simulator.moves(true, true).begin());
  simmedMoves.push_back(best);

  double bestbp = bogopoints(best);

  for (++it; it != staticMoves.end(); ++it) {
    signalFractionDone(
        max(static_cast<float>(simmedMoves.size()) /
                static_cast<float>(staticMoves.size()),
            static_cast<float>(stopwatch.elapsed()) /
                static_cast<float>(m_parameters.secondsPerTurn)));

    if (shouldAbort()) {
      goto sort_and_return;
    }

    MoveList lookFurther;
    lookFurther.push_back(*it);
    m_simulator.setIncludedMoves(lookFurther);
    m_simulator.simulate(plies, minIterations());
    Move move = *m_simulator.moves(true, true).begin();
    double movebp = bogopoints(move);

    if (movebp + 1.96 * 35.0 / sqrt((double)minIterations()) > bestbp) {
      m_simulator.simulate(plies, maxIterations() - minIterations());
      Move move2 = *m_simulator.moves(true, true).begin();
      movebp = bogopoints(move2);
      simmedMoves.push_back(move2);

      if (move2.win > best.win) {
        best = move2;
        bestbp = movebp;
      }
    } else {
      simmedMoves.push_back(move);
    }

    if (stopwatch.exceeded(m_parameters.secondsPerTurn)) {
      goto sort_and_return;
    }
  }

sort_and_return:
  MoveList::sort(simmedMoves, MoveList::Win);
  MoveList ret;
  MoveList::const_iterator simmedEnd = simmedMoves.end();
  int i = 0;
  for (MoveList::const_iterator simmedIt = simmedMoves.begin();
       (simmedIt != simmedEnd); ++i, ++simmedIt) {
    if (i < nmoves || m_simulator.isConsideredMove(*simmedIt))
      ret.push_back(*simmedIt);
  }

  m_cachedMoves = ret;
  return ret;
}
