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

#include "endgameplayer.h"

//#define DEBUG_COMPUTERPLAYER

using namespace Quackle;

EndgamePlayer::EndgamePlayer()
{
	m_name = MARK_UV("Speedy Player");
	m_id = 2;
}

EndgamePlayer::~EndgamePlayer()
{
}

Move EndgamePlayer::move()
{
	return moves(1).back();
}

MoveList EndgamePlayer::moves(int nmoves)
{
    if (currentPosition().bag().size() > 0)
    {
        m_simulator.currentPosition().kibitz(nmoves);
        m_cachedMoves = m_simulator.currentPosition().moves();
        return m_cachedMoves;
    }

    m_endgame.setPosition(currentPosition());

    if (nmoves > 1)
    {
        m_cachedMoves = m_endgame.moves(nmoves);
        return m_cachedMoves;
    }

    MoveList ret;
    Move solution = m_endgame.solve(currentPosition().nestedness());

    ret.push_back(solution);

    m_cachedMoves = ret;
    return ret;
}

void EndgamePlayer::setDispatch(ComputerDispatch *dispatch)
{
	ComputerPlayer::setDispatch(dispatch);
	m_endgame.setDispatch(dispatch);
}
