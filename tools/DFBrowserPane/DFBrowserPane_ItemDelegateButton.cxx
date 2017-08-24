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

#include <inspector/DFBrowserPane_ItemDelegateButton.hxx>

#include <QAbstractItemModel>
#include <QEvent>
#include <QPainter>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_ItemDelegateButton::DFBrowserPane_ItemDelegateButton (QObject* theParent, const QString& theIcon)
: QStyledItemDelegate (theParent)
{
  myIcon = QIcon (theIcon);
}

// =======================================================================
// function : paint
// purpose :
// =======================================================================
void DFBrowserPane_ItemDelegateButton::paint (QPainter* thePainter, const QStyleOptionViewItem& theOption,
                                              const QModelIndex& theIndex) const
{
  if (!myFreeRows.contains (theIndex.row()))
    thePainter->drawPixmap (theOption.rect, myIcon.pixmap (20,20));
}

// =======================================================================
// function : editorEvent
// purpose :
// =======================================================================
bool DFBrowserPane_ItemDelegateButton::editorEvent (QEvent* theEvent, QAbstractItemModel* theModel,
                                                    const QStyleOptionViewItem& theOption, const QModelIndex& theIndex)
{
  if (theEvent->type() == QEvent::MouseButtonPress && !myFreeRows.contains (theIndex.row()))
    emit buttonPressed (theIndex);

  return QStyledItemDelegate::editorEvent (theEvent, theModel, theOption, theIndex);
}
