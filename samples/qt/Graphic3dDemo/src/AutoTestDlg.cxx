#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "Application.h"
#include "AutoTestDlg.h"
#include "global.h"

AutoTestDlg::AutoTestDlg(QWidget* parent)
: QDialog(parent)
{
  setModal( true );
  setWindowTitle(tr("DLG_AUTO_TEST"));
  QString dir = Application::getResourceDir();
  QPixmap autoIcon( dir + tr( "ICON_AUTO_TEST" ) );
  setWindowIcon(autoIcon);

  QGridLayout* pMainLayout = new QGridLayout(this);
  pMainLayout->setMargin(5);
  pMainLayout->setSpacing(5);
  
  QLabel* lab = new QLabel(tr("LAB_START"), this);
  pMainLayout->addWidget(lab, 0, 0);

  lab         = new QLabel(tr("LAB_STOP"), this);
  pMainLayout->addWidget(lab, 1, 0);

  lab         = new QLabel(tr("LAB_STEP"), this);
  pMainLayout->addWidget(lab, 2, 0);

  const int MinNbOfItems = 100000;		// to see noticable effect
  const int MaxNbOfItems = 1000000;		// to avoid too long computations
  const int ItemsStep    = 100000;
  const int MinStep		 = 50000;
  const int StepStep	 = 50000;

  myStartSpin = new QSpinBox(this);
  myStartSpin->setMinimum(MinNbOfItems);
  myStartSpin->setMaximum(MaxNbOfItems);
  myStartSpin->setSingleStep(ItemsStep);
  pMainLayout->addWidget(myStartSpin, 0, 1);
  verify(connect(myStartSpin, SIGNAL(valueChanged(int)), SLOT(checkStepSpinState())));

  myStopSpin  = new QSpinBox(this);
  myStopSpin->setMinimum(MinNbOfItems);
  myStopSpin->setMaximum(MaxNbOfItems);
  myStartSpin->setSingleStep(ItemsStep);
  pMainLayout->addWidget(myStopSpin, 1, 1);
  verify(connect(myStopSpin, SIGNAL(valueChanged(int)), SLOT(setStartMaxValue(int))));
  verify(connect(myStopSpin, SIGNAL(valueChanged(int)), SLOT(checkStepSpinState())));

  const int StopSpinDefaultValue = 2 * myStartSpin -> minimum();
  myStopSpin ->  setValue(StopSpinDefaultValue);
  myStartSpin -> setMaximum(myStopSpin -> value());

  myStepSpin  = new QSpinBox(this);
  myStepSpin->setMinimum(MinStep);
  myStepSpin->setMaximum(MaxNbOfItems - MinNbOfItems);
  myStepSpin->setSingleStep(StepStep);
  pMainLayout->addWidget(myStepSpin, 2, 1);

  lab         = new QLabel(tr("LAB_ITEMS"), this);
  pMainLayout->addWidget(lab, 0, 2);

  lab         = new QLabel(tr("LAB_ITEMS"), this);
  pMainLayout->addWidget(lab, 1, 2);

  lab         = new QLabel(tr("LAB_ITEMS"), this);
  pMainLayout->addWidget(lab, 2, 2);

  QFrame* separator = new QFrame(this);
  separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  pMainLayout->addWidget(separator, 3, 0, 1, 3 );

  QHBoxLayout* hcbox = new QHBoxLayout();
  hcbox->setSpacing(5);

  myTeCheck = new QCheckBox(tr( "MEN_BTN_TEXT" ), this);
  hcbox->addWidget(myTeCheck);
  verify(connect(myTeCheck, SIGNAL(clicked()), this, SLOT(onText())));
  
  hcbox->addSpacing( 5 );

  pMainLayout->addLayout(hcbox, 4, 0, 1, 3);

  QHBoxLayout* hbox = new QHBoxLayout();
  hbox->setSpacing(5);

  QPushButton* btn = new QPushButton(tr("BTN_START"), this);
//  btn->setDefault(true);
  verify(connect(btn, SIGNAL(clicked()), this, SLOT(onStart())));
  btn->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  hbox->addWidget(btn);
  btn              = new QPushButton(tr("BTN_CANCEL"), this);
  verify(connect(btn, SIGNAL(clicked()), this, SLOT(onCancel())));
  btn->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  hbox->addWidget(btn);
  hbox->addSpacing(5);
  pMainLayout->addLayout(hbox, 5, 0, 1, 3);
}

void AutoTestDlg::onStart()
{
  accept();
}

void AutoTestDlg::onCancel()
{
  reject();
}

void AutoTestDlg::setStartMaxValue(int i)
{
	myStartSpin -> setMaximum(i); 
	if (myStartSpin -> value() > myStartSpin -> maximum())
		myStartSpin -> setValue(myStartSpin -> maximum());
}

void AutoTestDlg::checkStepSpinState()
{
	if (isVisible())
	{
		if (myStartSpin -> value() == myStopSpin -> value())
			myStepSpin -> setEnabled(false);
		else
			myStepSpin -> setEnabled(true);
	}
}

void AutoTestDlg::onText()
{
  int MinNbOfItems, MaxNbOfItems, DefaultStartSpinVal, DefaultStopSpinVal,
	  ItemsStep, MinStep, StepStep;

  if (myTeCheck -> isChecked())
  {
	  MinNbOfItems		  = 1000;		// to see noticeable effect
	  MaxNbOfItems		  = 100000;		// to avoid too long computations
	  DefaultStartSpinVal = 10000;
	  DefaultStopSpinVal  = 20000;
	  ItemsStep			  = 10000;
	  MinStep	          = 5000;
	  StepStep			  = 5000;
  }
  else
  {
	  MinNbOfItems		  = 100000;		// to see noticeable effect
	  MaxNbOfItems		  = 1000000;	// to avoid too long computations
	  DefaultStartSpinVal = 100000;
	  DefaultStopSpinVal  = 200000;
	  ItemsStep			  = 100000;
	  MinStep			  = 50000;
	  StepStep			  = 50000;
  }

  myStartSpin -> setRange(MinNbOfItems, MaxNbOfItems);
  myStartSpin -> setSingleStep(ItemsStep);
  myStartSpin -> setValue(DefaultStartSpinVal);

  myStopSpin -> setRange(MinNbOfItems, MaxNbOfItems);
  myStopSpin -> setSingleStep(ItemsStep);

  myStopSpin ->  setValue(DefaultStopSpinVal);
  myStartSpin -> setMaximum(myStopSpin -> value());

  myStepSpin -> setRange(MinStep, MaxNbOfItems - MinNbOfItems);
  myStepSpin -> setSingleStep(StepStep);
  myStepSpin -> setValue(MinStep);
}