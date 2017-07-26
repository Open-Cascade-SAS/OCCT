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

#include <TInspectorEXE_OpenFileViewModel.hxx>

#include <QApplication>
#include <QFileInfo>
#include <QIcon>
#include <QPainter>

const int ICON_SIZE = 40;

// =======================================================================
// function : paint
// purpose :
// =======================================================================
void TInspectorEXE_OpenFileItemDelegate::paint (QPainter* thePainter, const QStyleOptionViewItem& theOption,
                                                const QModelIndex& theIndex) const
{
  // highlight cell
  if (theOption.state & QStyle::State_MouseOver)
    thePainter->fillRect (theOption.rect, myColor);

  // action icon for all indices before the last one
  QIcon anIcon (":/icons/folder_import.png");
  QSize anIconSize (ICON_SIZE, ICON_SIZE);
  int aWidth = theOption.rect.width();
  int aCenter = aWidth / 2.;
  int aHalf = anIconSize.width() / 2.;
  int aMargin = qApp->style()->pixelMetric (QStyle::PM_HeaderMargin);
  thePainter->drawPixmap (QRect (theOption.rect.left() + (aCenter - aHalf),
                          theOption.rect.top() + aMargin,
                          anIconSize.width(),
                          anIconSize.height()),
                          anIcon.pixmap(anIconSize.width(), anIconSize.height()));
  // default paint
  QItemDelegate::paint (thePainter, theOption, theIndex);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void TInspectorEXE_OpenFileViewModel::Init (const QStringList& theValues)
{
  myValues = theValues;
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant TInspectorEXE_OpenFileViewModel::data (const QModelIndex& theIndex, int theRole) const
{
  switch (theRole)
  {
    case Qt::DisplayRole: return QFileInfo (myValues[theIndex.column()]).fileName();
    case Qt::ToolTipRole: return myValues[theIndex.column()];
    case Qt::TextAlignmentRole: return QVariant (Qt::AlignBottom | Qt::AlignHCenter);
    default: break;
  }
  return QVariant();
}
