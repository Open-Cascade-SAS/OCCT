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

#include <inspector/DFBrowser_TreeLevelLineDelegate.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <QPainter>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_TreeLevelLineDelegate::DFBrowser_TreeLevelLineDelegate (QObject* theParent)
: QItemDelegate (theParent)
{
}

// =======================================================================
// function : paint
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLineDelegate::paint (QPainter* thePainter, const QStyleOptionViewItem& theOption,
                                             const QModelIndex& theIndex) const
{
  // highlight cell
  if (theOption.state & QStyle::State_MouseOver)
    thePainter->fillRect (theOption.rect, DFBrowserPane_Tools::LightHighlightColor());

  // action icon for all indices before the last one
  if (theIndex.column() < theIndex.model()->columnCount()-1)
  {
    QIcon anIcon (":/icons/level_change.png");
    QSize anIconSize (10, 20);
    thePainter->drawPixmap (QRect (theOption.rect.right() - anIconSize.width(), theOption.rect.top(),
                                   anIconSize.width(), anIconSize.height()),
                            anIcon.pixmap (anIconSize.width(), anIconSize.height()));
  }
  // default paint
  QItemDelegate::paint (thePainter, theOption, theIndex);
}
