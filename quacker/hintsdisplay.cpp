#include <QtWidgets>
#include <computerplayer.h>
#include <game.h>
#include <quackleio/util.h>
#include <resolvent.h>

#include "geometry.h"
#include "hintsdisplay.h"

HintsDisplay::HintsDisplay(TopLevel *toplevel, QWidget *parent) : View(parent) {
  m_hintsGenerator = new Quackle::HintsGenerator(toplevel);
  connect(toplevel, &TopLevel::committed, this, &HintsDisplay::committed);
  connect(toplevel, &TopLevel::newGameSig, this, &HintsDisplay::newGame);

  // TODO mm (low): perhaps add a selection screen for user to choose
  //  which AIs to generate hints?
  // TODO mm (low-medium): instead of creating new AIs here, can we use the
  //  existing ones from ComputerPlayerCollection::fullCollection() ?
  Quackle::ComputerPlayer *torontoPlayer = new Quackle::TorontoPlayer();
  Quackle::ComputerPlayer *staticPlayer = new Quackle::StaticPlayer();
  Quackle::ComputerPlayer *greedyPlayer = new Quackle::GreedyPlayer();

  vector<Quackle::ComputerPlayer *> ais = {torontoPlayer, staticPlayer,
                                           greedyPlayer};

  m_hintsGenerator->addAIs(ais);

  connect(m_hintsGenerator, &Quackle::HintsGenerator::hintsUpdated, this,
          &HintsDisplay::hintsUpdated);

  QVBoxLayout *layout = new QVBoxLayout(this);
  Geometry::setupInnerLayout(layout);

  QWidget *interactiveTickBoxes = new QWidget;
  QHBoxLayout *tickBoxes = new QHBoxLayout(interactiveTickBoxes);
  Geometry::setupInnerLayout(tickBoxes);

  QWidget *interactiveButtonLayout = new QWidget;
  QHBoxLayout *buttonLayout = new QHBoxLayout(interactiveButtonLayout);
  Geometry::setupInnerLayout(buttonLayout);

  m_textEdit = new QTextEdit;
  m_textEdit->setReadOnly(true);
  m_textEdit->setFontFamily(QString("Arial"));
  m_textEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  m_textEdit->setPlainText("No hints to give."
                           "\nStart a game to be able to generate hints!");

  m_genChampHints =
      new QCheckBox(tr("Generate Championship Player\nhints? This may take "
                       "some time."));
  m_genChampHints->setChecked(false);
  connect(m_genChampHints, SIGNAL(stateChanged(int)), this,
          SLOT(genChampHintsChanged()));

  m_forceMovesUpdate = new QCheckBox(
      tr("Force hint regeneration? This\nclears the currently cached hints."));
  m_forceMovesUpdate->setChecked(true);

  m_genHintsBtn = new QPushButton(tr("Generate Hints"));
  m_genHintsBtn->setEnabled(false);
  connect(m_genHintsBtn, SIGNAL(clicked()), this, SLOT(genHints()));
  genChampHintsChanged();

  tickBoxes->addStretch(1);
  tickBoxes->addWidget(m_genChampHints);
  tickBoxes->addWidget(m_forceMovesUpdate);
  tickBoxes->addStretch(1);
  tickBoxes->setStretchFactor(m_genChampHints, 2);
  tickBoxes->setStretchFactor(m_forceMovesUpdate, 2);

  buttonLayout->addStretch(1);
  buttonLayout->addWidget(m_genHintsBtn);
  buttonLayout->setStretchFactor(m_genHintsBtn, 1);
  buttonLayout->setStretchFactor(interactiveButtonLayout, 1);

  QLabel *buttonLabel = new QLabel(tr("Press 'Generate Hints' to see effect of toggling tickboxes."));
  buttonLabel->setBuddy(m_genHintsBtn);
  buttonLayout->addWidget(buttonLabel);
  buttonLayout->addStretch(1);
  buttonLayout->setStretchFactor(buttonLabel, 5);

  layout->addWidget(interactiveTickBoxes);
  layout->addWidget(interactiveButtonLayout);
  layout->addWidget(m_textEdit);

  layout->setStretchFactor(interactiveTickBoxes, 1);
  layout->setStretchFactor(interactiveButtonLayout, 1);
  layout->setStretchFactor(m_textEdit, 9999);
}

HintsDisplay::~HintsDisplay() {
  delete m_hintsGenerator;
  m_hintsGenerator = NULL;
}

void HintsDisplay::clearHints() { showHints(Quackle::LongLetterString()); }

void HintsDisplay::genChampHintsChanged() {
  m_hintsGenerator->genChampHintsChanged(m_genChampHints->isChecked());
}

void HintsDisplay::newGame(const Quackle::GamePosition &position) {
  clearHints();
  m_hintsGenerator->updateAIState(position);
}

void HintsDisplay::genHints() {
  m_hintsGenerator->generateHints(m_forceMovesUpdate->isChecked());
  showHints(m_hintsGenerator->getHints());
}

void HintsDisplay::committed(Quackle::Move &move) {
  showHints(m_hintsGenerator->committed(*m_position, move));
  m_hintsGenerator->updateAIState(*m_position);
}

void HintsDisplay::positionChanged(const Quackle::GamePosition &position) {
  m_genHintsBtn->setEnabled(true);
  m_position = (Quackle::GamePosition *)&position;

  m_hintsGenerator->positionChanged(position);
}

void HintsDisplay::hintsUpdated() { showHints(m_hintsGenerator->getHints()); }

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
