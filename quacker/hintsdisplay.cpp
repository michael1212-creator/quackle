#include <QtWidgets>
#include <game.h>
#include <quackleio/util.h>

#include "geometry.h"
#include "hintsdisplay.h"

HintsDisplay::HintsDisplay(QWidget *parent) : View(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  QWidget *interactive = new QWidget;
  QHBoxLayout *interactiveLayout = new QHBoxLayout(interactive);
  Geometry::setupInnerLayout(layout);
  Geometry::setupInnerLayout(interactiveLayout);

  m_textEdit = new QTextEdit;
  m_textEdit->setReadOnly(true);
  m_textEdit->setFontFamily("Courier");
  m_textEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  m_genChampHints = new QCheckBox(tr("Generate Championship Player hints?\nThis may take some time."));
  m_genChampHints->setChecked(false); //TODO mm: change this so it is saved between game sessions (e.g. see TopLevel::saveSettings or the like)
  connect(m_genChampHints, SIGNAL(stateChanged(int)), this,
          SLOT(genChampHintsChanged()));

  m_genHintsBtn = new QPushButton(tr("Generate Hints"));
  connect(m_genHintsBtn, SIGNAL(clicked()), this, SLOT(genHints()));

  interactiveLayout->addWidget(m_genHintsBtn);
  interactiveLayout->addWidget(m_genChampHints);
  interactiveLayout->setStretchFactor(m_genHintsBtn, 1);
  interactiveLayout->setStretchFactor(m_genChampHints, 200);

  layout->addWidget(interactive);
  layout->addWidget(m_textEdit);

  layout->setStretchFactor(interactive, 1);
  layout->setStretchFactor(m_textEdit, 200);

  showHints(Quackle::LongLetterString());
}

HintsDisplay::~HintsDisplay() {}

void HintsDisplay::genChampHintsChanged() {
  //TODO mm: enable generation of champ hints
//  setGenChampHintsOppos(m_genChampHints->isChecked());
}

void HintsDisplay::genHints() {
  //TODO mm: actually generate the hints

  showHints(Quackle::LongLetterString());
}

void HintsDisplay::positionChanged(const Quackle::GamePosition &position) {
  //TODO mm: clear the hints
}

void HintsDisplay::showHints(const Quackle::LongLetterString &hints) {
  if (hints.empty()) {
    m_textEdit->setPlainText("No hints to give;"
                             "\nPress the 'Generate Hints' button for hints!");
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

  m_textEdit->setPlainText(text);

  const int minimumMaxLineWidth = 32;
  if (maxLineWidth < minimumMaxLineWidth)
    maxLineWidth = minimumMaxLineWidth;

  const int maximumWidth = maxLineWidth + m_textEdit->frameWidth() * 2 +
                           (m_textEdit->verticalScrollBar()->isVisible()
                                ? m_textEdit->verticalScrollBar()->width()
                                : 0) +
                           10;
  m_textEdit->setMaximumSize(maximumWidth, 26 * 100);
  m_textEdit->resize(m_textEdit->maximumSize());
}
