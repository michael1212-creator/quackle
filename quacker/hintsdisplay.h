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
class TopLevel;

class HintsDisplay : public View {
  Q_OBJECT

public:
  HintsDisplay(TopLevel *toplevel = 0, QWidget *parent = 0);
  virtual ~HintsDisplay();

public slots:
  virtual void positionChanged(const Quackle::GamePosition &position);

protected slots:
  void hintsUpdated();
  virtual void showHints(const Quackle::LongLetterString &hints);
  void genChampHintsChanged();
  void genHints();
  void committed(Quackle::Move &move);

private:
  void clearHints();

  QCheckBox *m_genChampHints;
  QCheckBox *m_forceMovesUpdate;
  QTextEdit *m_textEdit;
  QPushButton *m_genHintsBtn;
  Quackle::HintsGenerator *m_hintsGenerator;
  Quackle::GamePosition m_position;
};

#endif // QUACKLE_QUACKER_HINTSDISPLAY_H_
