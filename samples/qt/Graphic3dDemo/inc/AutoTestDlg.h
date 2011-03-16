#ifndef AUTOTESTDLG_H
#define AUTOTESTDLG_H

#include <qdialog.h>
#include <qspinbox.h> 
#include <qcheckbox.h> 
#include <qobject.h>

class AutoTestDlg : public QDialog
{
  Q_OBJECT

public:
  AutoTestDlg(QWidget * parent=0);
  int getStartNbItems() { return myStartSpin->value(); }
  int getStopNbItems()  { return myStopSpin->value(); }
  int getStep()         { return myStepSpin->value(); }
  int isText()          { return myTeCheck->isChecked(); }

public slots:
  void onStart();
  void onCancel();
  void setStartMaxValue(int i);
  void checkStepSpinState();
  void onText();

private:
  QSpinBox* myStartSpin;
  QSpinBox* myStopSpin;
  QSpinBox* myStepSpin;
  QCheckBox* myTeCheck;
};

#endif  //#ifndef AUTOTESTDLG_H