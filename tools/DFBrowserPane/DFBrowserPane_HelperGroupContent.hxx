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

#ifndef DFBrowserPane_HelperGroupContent_H
#define DFBrowserPane_HelperGroupContent_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QGroupBox>

class QWidget;

//! \class DFBrowserPane_HelperGroupContent
//! \brief Creates a check group box for the given control. If checked, the control is enabled,
//! otherwise it sets it as disabled
class DFBrowserPane_HelperGroupContent : public QGroupBox
{
Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_HelperGroupContent (const QString& theTitle, QWidget* theParent, QWidget* theControl);

  //! Destructor
  virtual ~DFBrowserPane_HelperGroupContent() Standard_OVERRIDE {}

protected slots:
  //! Set enable/disable current control depending on check box state
  //! \param theState state of the check box
  void onChecked (bool theState);

private:

  QWidget* myControl; //!< the source control, that will be enabled/disabled
};

#endif
