// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#include <inspector/TreeModel_MessageDialog.hxx>

#include <QCheckBox>
#include <QCursor>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TreeModel_MessageDialog::TreeModel_MessageDialog (QWidget* theParent, const QString& theInformation,
                                                  const QString& theQuestion)
: QDialog (theParent), myDoNotShowItAgain (false), myPreviousAnswer (false), myInformation (theInformation),
 myQuestion (theQuestion)
{
  setWindowTitle ("Information");

  QGridLayout* aLayout = new QGridLayout (this);
  QString anInformation = theInformation;
  if (!theQuestion.isEmpty())
    anInformation += QString("\n\n%2").arg (myQuestion);
  myInformationLabel = new QLabel (anInformation, this);
  myInformationLabel->setWordWrap (true);
  aLayout->addWidget (myInformationLabel, 0, 0, 1, 3);

  myDoNotShowCheckBox = new QCheckBox ("Don't show this dialog again. Do the same next time.", this);
  connect (myDoNotShowCheckBox, SIGNAL (toggled (bool)), this, SLOT (onDonNotShowToggled (bool) ));
  aLayout->addWidget (myDoNotShowCheckBox, 1, 0, 1, 3);

  myOkButton = new QPushButton ("Ok", this);
  myCancelButton = new QPushButton ("Cancel", this);
  connect (myOkButton, SIGNAL (clicked()), this, SLOT (onOkClicked() ));
  connect (myCancelButton, SIGNAL (clicked()), this, SLOT (onCancelClicked() ));
  aLayout->addWidget (myOkButton, 2, 1);
  aLayout->addWidget (myCancelButton, 2, 2);

  aLayout->setColumnStretch (0, 1);

  myCancelButton->setDefault (true);

  SetInformation (theInformation);
}

// =======================================================================
// function : Start
// purpose :
// =======================================================================
void TreeModel_MessageDialog::Start()
{
  if (!myDoNotShowItAgain)
  {
    QString anInformation = myInformation;
    if (!myQuestion.isEmpty())
      anInformation += QString("\n\n%2").arg (myQuestion);
    myInformationLabel->setText (anInformation);
    exec();
    return;
  }

  if (IsAccepted())
    return;

  // tool tip information window
  QWidget* aWidget = new QWidget (this, Qt::Popup);
  QVBoxLayout* aLayout = new QVBoxLayout (aWidget);
  aLayout->addWidget (new QLabel(myInformation, aWidget));
  aWidget->move(QCursor::pos());
  aWidget->show();
}

// =======================================================================
// function : onOkClicked
// purpose :
// =======================================================================
void TreeModel_MessageDialog::onOkClicked()
{
  myPreviousAnswer = true;
  if (myDoNotShowItAgain)
    setToolTipInfoMode();

  accept();
}

// =======================================================================
// function : onCancelClicked
// purpose :
// =======================================================================
void TreeModel_MessageDialog::onCancelClicked()
{
  myPreviousAnswer = false;
  if (myDoNotShowItAgain)
    setToolTipInfoMode();

  reject();
}

// =======================================================================
// function : setToolTipInfoMode
// purpose :
// =======================================================================
void TreeModel_MessageDialog::setToolTipInfoMode()
{
  //setWindowFlags (Qt::FramelessWindowHint);
  //myDoNotShowCheckBox->setVisible (false);
  //myOkButton->setVisible (false);
  //myCancelButton->setVisible (false);
}
