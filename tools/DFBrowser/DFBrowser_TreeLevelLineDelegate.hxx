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

#ifndef DFBrowser_TreeLevelLineDelegate_H
#define DFBrowser_TreeLevelLineDelegate_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QItemDelegate>

//! \class DFBrowser_TreeLevelLineDelegate
//! Extending standard item delegate by:
//! - <level change.png> icon. It exists for all columns excepting the last column.
//! - highlight cell by mouse move over the cell
class DFBrowser_TreeLevelLineDelegate : public QItemDelegate
{

public:

  //! Constructor
  Standard_EXPORT DFBrowser_TreeLevelLineDelegate (QObject* theParent = 0);

  //! Destructor
  virtual ~DFBrowser_TreeLevelLineDelegate() {}

  //! Draw an icon in the cell and highlight cell if mouse is over the cell
  //! \param thePainter a painter
  //! \param theOption a paint options
  //! \param theIndex a view index
  Standard_EXPORT virtual void paint (QPainter* thePainter, const QStyleOptionViewItem& theOption,
                                      const QModelIndex& theIndex) const Standard_OVERRIDE;
};

#endif
