#ifndef QUACKLE_QUACKER_HINTSDISPLAY_H_
#define QUACKLE_QUACKER_HINTSDISPLAY_H_

#include <vector>

#include "alphabetparameters.h"
#include "hintsgenerator.h"
#include "view.h"

class QLabel;
class QTextEdit;
class QCheckBox;
class QPushButton;
class HintsGenerator;

class HintsDisplay : public View {
  Q_OBJECT

public:
  HintsDisplay(QWidget *parent = 0);
  virtual ~HintsDisplay();

public slots:
  virtual void positionChanged(const Quackle::GamePosition &position);

protected slots:
  virtual void showHints(const Quackle::LongLetterString &hints);
  void genChampHintsChanged();
  void genHints();

private:
  void clearHints();

  QCheckBox *m_genChampHints;
  QTextEdit *m_textEdit;
  QPushButton *m_genHintsBtn;
  Quackle::HintsGenerator *m_hintsGenerator;

  //Used to tell us whether the positionChanged was a commit
  // (vs. only just visual, such as when a player selects a move from generated
  // choices)
  Quackle::LongLetterString m_unseenTiles;
};

#endif // QUACKLE_QUACKER_HINTSDISPLAY_H_
