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

void HintsDisplay::showHints(const Quackle::LongLetterString &hints) {
  if (hints.empty())
  {
    m_label->setText(tr("No hints to give."));
    return;
  }

  QFontMetrics metrics(m_textEdit->currentFont());
  int maxLineWidth = 0;

  QString text;

  text.append(QString::fromStdString(hints));

  #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    // Kill Qt 5.13 deprecation warning without breaking pre-5.11 builds
    const int lineWidth = metrics.horizontalAdvance(text);
  #else
    const int lineWidth = metrics.width(text);
  #endif

  if (lineWidth > maxLineWidth)
    maxLineWidth = lineWidth;

  m_label->setText("Your hint:");
  m_textEdit->setPlainText(text);

  const int minimumMaxLineWidth = 32;
  if (maxLineWidth < minimumMaxLineWidth)
    maxLineWidth = minimumMaxLineWidth;

  const int maximumWidth = maxLineWidth + m_textEdit->frameWidth() * 2 + (m_textEdit->verticalScrollBar()->isVisible()? m_textEdit->verticalScrollBar()->width() : 0) + 10;
  m_textEdit->setMaximumSize(maximumWidth, 26 * 100);
  m_textEdit->resize(m_textEdit->maximumSize());
}
