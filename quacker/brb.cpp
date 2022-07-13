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

#include <QtWidgets>

#include <game.h>

#include "bagdisplay.h"
#include "boarddisplay.h"
#include "brb.h"
#include "geometry.h"
#include "rackdisplay.h"
#include "widgetfactory.h"

BRB::BRB(WidgetFactory *widgetFactory, QWidget *parent) : View(parent) {
  m_widgetFactory = widgetFactory;
  m_topHorizontalLayout = new QHBoxLayout;
  Geometry::setupFramedLayout(m_topHorizontalLayout);
  setLayout(m_topHorizontalLayout);

  QVBoxLayout *leftVerticalLayout = new QVBoxLayout;
  Geometry::setupInnerLayout(leftVerticalLayout);
  m_topHorizontalLayout->addLayout(leftVerticalLayout);

  m_boardDisplay = m_widgetFactory->createBoardDisplay();
  leftVerticalLayout->addWidget(m_boardDisplay);

  m_rackDisplay = m_widgetFactory->createRackDisplay();
  leftVerticalLayout->addWidget(m_rackDisplay);

  m_topHorizontalLayout->setStretchFactor(leftVerticalLayout, 10);

  m_subviews.push_back(m_boardDisplay);
  m_subviews.push_back(m_rackDisplay);
  connectSubviewSignals();
}

BRB::~BRB() {}

View *BRB::getBoardView() const { return m_boardDisplay; }

void BRB::split(QSplitter *splitter) {
  QWidget *rightSide = new QWidget;
  QVBoxLayout *rightVerticalLayout = new QVBoxLayout(rightSide);
  Geometry::setupInnerLayout(rightVerticalLayout);
//  m_topHorizontalLayout->addLayout(rightVerticalLayout);

  m_bagDisplay = m_widgetFactory->createBagDisplay();
  rightVerticalLayout->addWidget(m_bagDisplay);

  m_hintsDisplay = m_widgetFactory->createHintsDisplay();
  rightVerticalLayout->addWidget(m_hintsDisplay);

  m_subviews.push_back(m_bagDisplay);
  m_subviews.push_back(m_hintsDisplay);
  connectSubviewSignals();
  splitter->addWidget(rightSide);
}

void BRB::grabFocus() { m_rackDisplay->grabFocus(); }

void BRB::positionChanged(const Quackle::GamePosition &position) {
  View::positionChanged(position);
}
