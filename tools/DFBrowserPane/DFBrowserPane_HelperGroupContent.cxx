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

#include <DFBrowserPane_HelperGroupContent.hxx>

#include <QHBoxLayout>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_HelperGroupContent::DFBrowserPane_HelperGroupContent (const QString& theTitle, QWidget* theParent,
                                                                    QWidget* theControl)
: QGroupBox (theTitle, theParent), myControl (0)
{
  setCheckable (true);

  myControl = theControl;
  QHBoxLayout* aLayout = new QHBoxLayout (this);
  aLayout->addWidget (myControl);

  connect (this, SIGNAL (clicked (bool)), this, SLOT (onChecked(bool)));

  setChecked (false);
  onChecked (isChecked());
}

// =======================================================================
// function : onChecked
// purpose :
// =======================================================================
void DFBrowserPane_HelperGroupContent::onChecked (bool theState)
{
  if (myControl)
    myControl->setVisible (theState);
}
