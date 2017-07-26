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

#ifndef DFBrowserPane_ItemDelegateButton_H
#define DFBrowserPane_ItemDelegateButton_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QString>
#include <QIcon>

class QObject;
class QPainter;
class QEvent;
class QAbstractItemModel;

//! \class DFBrowserPane_ItemDelegateButton
//! \brief It paints an icon in all rows of the view in a separate column.
//! It is possible to set rows where this icon is not shown.
//! Click on the cell where the icon exists emits buttonPressed signal
class DFBrowserPane_ItemDelegateButton : public QStyledItemDelegate
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_ItemDelegateButton (QObject* theParent, const QString& theIcon);

  //! Destructor
  virtual ~DFBrowserPane_ItemDelegateButton() {}

public:

  //! Stores indices of rows where the icon should not be shown
  //! \param theRows an indices of rows
  void SetFreeRows (const QList<int>& theRows) { myFreeRows = theRows; }

  //! Draw an icon in the cell
  //! \param thePainter a painter
  //! \param theOption a paint options
  //! \param theIndex a view index
  Standard_EXPORT virtual void paint (QPainter* thePainter, const QStyleOptionViewItem& theOption,
                                      const QModelIndex& theIndex) const Standard_OVERRIDE;

  //! Emits pressed signal if event type is mouse button pressed and there is icon for this index
  //! After signal it calls the parent method
  //! \param theEvent a processed event
  //! \param theModel a current view model
  //! \param theOption display options
  //! \param theIndex an edited item
  Standard_EXPORT virtual bool editorEvent (QEvent* theEvent, QAbstractItemModel* theModel,
                            const QStyleOptionViewItem& theOption, const QModelIndex& theIndex) Standard_OVERRIDE;
signals:

  //! Signal about button pressing
  //! \param theIndex an index of clicked item
  void buttonPressed (const QModelIndex& theIndex);

private:

  QIcon myIcon; //!< an item icon
  QList<int> myFreeRows; //!< container of row indices where icon is not used
};

#endif
