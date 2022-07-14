#ifndef QUACKLE_QUACKER_HINTSDISPLAY_H_
#define QUACKLE_QUACKER_HINTSDISPLAY_H_

#include "alphabetparameters.h"
#include "view.h"

class QLabel;
class QTextEdit;
class QCheckBox;
class QPushButton;

class HintsDisplay : public View {
  Q_OBJECT

public:
  HintsDisplay(QWidget *parent = 0);
  virtual ~HintsDisplay();

public slots:
  virtual void positionChanged(const Quackle::GamePosition &position);

protected slots:
  virtual void showHints(const Quackle::LongLetterString &tiles);
  void genChampHintsChanged();
  void genHints();

private:
  QLabel *m_label;
  QCheckBox *m_genChampHints;
  QTextEdit *m_textEdit;
  QPushButton *m_genHintsBtn;
};

#endif // QUACKLE_QUACKER_HINTSDISPLAY_H_
