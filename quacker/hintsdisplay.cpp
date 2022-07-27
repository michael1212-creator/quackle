#include <QtWidgets>
#include <computerplayer.h>
#include <game.h>
#include <quackleio/util.h>
#include <resolvent.h>

#include "geometry.h"
#include "hintsdisplay.h"

HintsDisplay::HintsDisplay(QWidget *parent) : View(parent) {
  // logic part
  m_hintsGenerator = new Quackle::HintsGenerator();

  // TODO mm (medium-low): perhaps add a selection screen for user to choose
  // which AIs to generate hints?
  // TODO mm (low): instead of creating new AIs here, can we use the existing
  // ones from ComputerPlayerCollection::fullCollection() ?
  Quackle::ComputerPlayer *torontoPlayer = new Quackle::TorontoPlayer();
  Quackle::ComputerPlayer *staticPlayer = new Quackle::StaticPlayer();
  Quackle::ComputerPlayer *greedyPlayer = new Quackle::GreedyPlayer();

  vector<Quackle::ComputerPlayer *> ais = {torontoPlayer, staticPlayer,
                                           greedyPlayer};

  m_hintsGenerator->addAIs(ais);

  // visual part
  QVBoxLayout *layout = new QVBoxLayout(this);
  QWidget *interactive = new QWidget;
  QHBoxLayout *interactiveLayout = new QHBoxLayout(interactive);
  Geometry::setupInnerLayout(layout);
  Geometry::setupInnerLayout(interactiveLayout);

  m_textEdit = new QTextEdit;
  m_textEdit->setReadOnly(true);
  m_textEdit->setFontFamily("Courier");
  m_textEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  m_genChampHints = new QCheckBox(
      tr("Generate Championship Player hints?\nThis may take some time."));
  m_genChampHints->setChecked(
      false); // TODO mm (low): change this so it is saved between game sessions
              // (e.g. see TopLevel::saveSettings or the like)
  connect(m_genChampHints, SIGNAL(stateChanged(int)), this,
          SLOT(genChampHintsChanged()));

  m_forceMovesUpdate = new QCheckBox(tr("Force move recalculation?"));
  m_forceMovesUpdate->setChecked(
      false); // TODO mm (low): change this so it is saved between game sessions
              // (e.g. see TopLevel::saveSettings or the like)

  m_genHintsBtn = new QPushButton(tr("Generate Hints"));
  connect(m_genHintsBtn, SIGNAL(clicked()), this, SLOT(genHints()));
  genChampHintsChanged();

  interactiveLayout->addWidget(m_genHintsBtn);
  interactiveLayout->addWidget(m_genChampHints);
  interactiveLayout->addWidget(m_forceMovesUpdate);
  interactiveLayout->setStretchFactor(m_genHintsBtn, 1);
  interactiveLayout->setStretchFactor(m_genChampHints, 2);
  interactiveLayout->setStretchFactor(m_forceMovesUpdate, 2);

  layout->addWidget(interactive);
  layout->addWidget(m_textEdit);

  layout->setStretchFactor(interactive, 1);
  layout->setStretchFactor(m_textEdit, 200);

  showHints(Quackle::LongLetterString());
}

HintsDisplay::~HintsDisplay() {
  delete m_hintsGenerator;
  m_hintsGenerator = NULL;
}

void HintsDisplay::clearHints() {
  m_hintsGenerator->clearHints();
  showHints(Quackle::LongLetterString());
}

void HintsDisplay::genChampHintsChanged() {
  m_hintsGenerator->genChampHintsChanged(m_genChampHints->isChecked());
}

void HintsDisplay::genHints() {
  // TODO mm (medium): before a new game is started, this button should be
  //  disabled
  clearHints();
  showHints(m_hintsGenerator->generateHints(m_forceMovesUpdate->isChecked()));
}

void HintsDisplay::positionChanged(const Quackle::GamePosition &position) {
  if (position.unseenBag().tiles() != m_unseenTiles) {
    clearHints();
    m_hintsGenerator->positionChanged(position);
  }

  m_unseenTiles = position.unseenBag().tiles();
}

void HintsDisplay::showHints(const Quackle::LongLetterString &hints) {
  if (hints.empty()) {
    m_textEdit->setPlainText("No hints to give."
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
