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

#ifndef ViewControl_Tools_H
#define ViewControl_Tools_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QString>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class ViewControl_TableModelValues;

class QAction;
class QItemSelectionModel;
class QObject;
class QTableView;
class QWidget;

//! \class ViewControl_Tools
//! \brief The tool that gives auxiliary methods for qt elements manipulation
class ViewControl_Tools
{
public:
  //! Returns text of separation row in table
  //! \return string value
  static QString TableSeparator() { return "---------------------------"; }

  //! Creates an action with the given text connected to the slot
  //! \param theText an action text value
  //! \param theSlot a listener of triggered signal of the new action
  //! \param theParent a parent object
  //! \param theContext listener of the action toggle
  //! \return a new action
  Standard_EXPORT static QAction* CreateAction(const QString& theText,
                                               const char*    theSlot,
                                               QObject*       theParent,
                                               QObject*       theContext);

  //! Change palette of the widget to have white foreground
  //! \param theControl a widget to be modified
  Standard_EXPORT static void SetWhiteBackground(QWidget* theControl);

  //! Fills tree view by default sections parameters obtained in view's table model
  //! \param theTableView table view instance
  //! \param theOrientation header orientation
  Standard_EXPORT static void SetDefaultHeaderSections(QTableView*           theTableView,
                                                       const Qt::Orientation theOrientation);

  //! Create table of values on the current selection
  //! It is created if the selection contains only one item and it has a property item
  Standard_EXPORT static ViewControl_TableModelValues* CreateTableModelValues(
    QItemSelectionModel* theSelectionModel);

  //! Convert real value to string value
  //! \param theValue a short real value
  //! \return the string value
  Standard_EXPORT static QVariant ToVariant(const Standard_ShortReal theValue);

  //! Convert real value to string value
  //! \param theValue a real value
  //! \return the string value
  Standard_EXPORT static QVariant ToVariant(const Standard_Real theValue);

  //! Convert real value to real value
  //! \param theValue a string value
  //! \return the real value
  Standard_EXPORT static Standard_ShortReal ToShortRealValue(const QVariant& theValue);

  //! Convert real value to string value
  //! \param theValue a string value
  //! \return the real value
  Standard_EXPORT static Standard_Real ToRealValue(const QVariant& theValue);
};

#endif
