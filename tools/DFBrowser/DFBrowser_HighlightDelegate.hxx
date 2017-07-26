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

#ifndef DFBrowser_HighlightDelegate_H
#define DFBrowser_HighlightDelegate_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QItemDelegate>

//! \class DFBrowser_HighlightDelegate
//! \brief An item delegate to paint in highlight color the cell when the mouse cursor is over it
class DFBrowser_HighlightDelegate : public QItemDelegate
{
public:

  //! Constructor
  DFBrowser_HighlightDelegate (QObject* theParent = 0) : QItemDelegate (theParent) {}

  //! Destructor
  virtual ~DFBrowser_HighlightDelegate() Standard_OVERRIDE {}

  //! Redefine of the parent virtual method to color the cell rectangle in highlight style
  //! \param thePainter a painter
  //! \param theOption a paint options
  //! \param theIndex a view index
  Standard_EXPORT virtual void paint (QPainter* thePainter, const QStyleOptionViewItem& theOption,
                                       const QModelIndex& theIndex) const Standard_OVERRIDE;
};
#endif
