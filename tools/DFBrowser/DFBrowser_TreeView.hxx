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

#ifndef DFBrowser_TreeView_H
#define DFBrowser_TreeView_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QTreeView>

//! \class DFBrowser_TreeView
//! Extended tree view control with possibility to set predefined size.
class DFBrowser_TreeView : public QTreeView
{
public:

  //! Constructor
  Standard_EXPORT DFBrowser_TreeView (QWidget* theParent)
  : QTreeView (theParent), myDefaultWidth (-1), myDefaultHeight (-1) {}

  //! Destructor
  virtual ~DFBrowser_TreeView() {}

  //! Sets default size of control, that is used by the first control show
  //! \param theDefaultWidth the width value
  //! \param theDefaultHeight the height value
  Standard_EXPORT void SetPredefinedSize(int theDefaultWidth, int theDefaultHeight);

  //! Returns predefined size if both values are positive, otherwise parent size hint
  Standard_EXPORT virtual QSize sizeHint() const Standard_OVERRIDE;

private:

  int myDefaultWidth; //!< default width, -1 if it should not be used
  int myDefaultHeight; //!< default height, -1 if it should not be used
};

#endif
