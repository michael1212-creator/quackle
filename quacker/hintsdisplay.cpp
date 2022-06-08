#include <math.h>

#include <QtWidgets>

#include <game.h>
#include <quackleio/util.h>

#include "geometry.h"

#include "hintsdisplay.h"

HintsDisplay::HintsDisplay(QWidget *parent)
    : View(parent)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  Geometry::setupInnerLayout(layout);

  m_textEdit = new QTextEdit;
  m_textEdit->setReadOnly(true);
  m_textEdit->setFontFamily("Courier");
  m_textEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  m_label = new QLabel;
  m_label->setWordWrap(true);
  m_label->setBuddy(m_textEdit);
  layout->addWidget(m_label);
  layout->addWidget(m_textEdit);

  layout->setStretchFactor(m_textEdit, 15);

  showHints(Quackle::LongLetterString());
}

HintsDisplay::~HintsDisplay()
{
}

void HintsDisplay::positionChanged(const Quackle::GamePosition &position)
{
  showHints("Hello, world!");
}

void HintsDisplay::showHints(const Quackle::LongLetterString &tiles) {
  QFontMetrics metrics(m_textEdit->currentFont());
  int maxLineWidth = 0;

  QString line = "AAAAA AAAAA AAAAA";
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    // Kill Qt 5.13 deprecation warning without breaking pre-5.11 builds
    const int lineWidth = metrics.horizontalAdvance(line);
  #else
    const int lineWidth = metrics.width(line);
  #endif

  if (lineWidth > maxLineWidth)
    maxLineWidth = lineWidth;

  const int minimumMaxLineWidth = 16;
  if (maxLineWidth < minimumMaxLineWidth)
    maxLineWidth = minimumMaxLineWidth;

  const int maximumWidth = 3 * (maxLineWidth + m_textEdit->frameWidth() * 2 + (m_textEdit->verticalScrollBar()->isVisible()? m_textEdit->verticalScrollBar()->width() : 0) + 10) / 2;
  m_textEdit->setMaximumSize(maximumWidth, 26 * 100);
  m_textEdit->resize(m_textEdit->maximumSize());
}
