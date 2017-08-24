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

#ifndef TreeModel_MessageDialog_H
#define TreeModel_MessageDialog_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QString>

class DFBrowser_Module;
class QWidget;

//! \class TreeModel_MessageDialog
//! Dialog providing information and a question.
//! It has a check box to do not the dialog again. In this case the previous value will be used as a result
class TreeModel_MessageDialog : public QDialog
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT TreeModel_MessageDialog (QWidget* theParent, const QString& theInformation, const QString& theQuestion);

  //! Destructor
  virtual ~TreeModel_MessageDialog() {}

  //! Fills message dialog with the information
  //! \param theInformation text
  void SetInformation (const QString& theInformation) { myInformation = theInformation; }

  //! Returns result of the dialog
  //! \bool true if the dialog was accepted
  bool IsAccepted() { return myPreviousAnswer; }

  //! Either perform exec() for the dialog or show tool tip information depending do not be shown again state
  Standard_EXPORT void Start();

private slots:
  //! Processing this checkbox, store result in the dialog field to use by the next dialog start
  //! \param theState current changed state of the check box
  void onDonNotShowToggled (bool theState) { myDoNotShowItAgain = theState; }

  //! Processing action button. Stores accept choice, change dialog state if do not show it again is on
  void onOkClicked();

  //! Processing action button. Stores reject choice, change dialog state if do not show it again is on
  void onCancelClicked();

private:
  //! Change state of the dialog to message tool tip. Only information control will be shown in the dialog
  void setToolTipInfoMode();

private:
  bool myDoNotShowItAgain; //! state if the dialog should not be shown again, the latest result is used as answer
  bool myPreviousAnswer; //! the previous cached result of the dialog

  QString myInformation; //! the information text
  QString myQuestion; //! the question text

  QLabel* myInformationLabel; //! message control
  QCheckBox* myDoNotShowCheckBox; //! choice whether the dialog will be shown again
  QPushButton* myOkButton; //! accept button
  QPushButton* myCancelButton; //! reject button
};


#endif
