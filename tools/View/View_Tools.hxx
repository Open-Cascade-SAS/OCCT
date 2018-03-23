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

#ifndef View_Tools_H
#define View_Tools_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QMap>
#include <QString>
#include <Standard_WarningsRestore.hxx>

class View_Window;

class QAction;
class QObject;

//! \class View_Tools
//! \brief The tool that gives auxiliary methods for qt elements manipulation
class View_Tools
{
public:

  //! Creates an action with the given text connected to the slot
  //! \param theText an action text value
  //! \param theSlot a listener of triggered signal of the new action
  //! \param theParent a parent object
  //! \param theContext listener of the action toggle
  //! \return a new action
  Standard_EXPORT static QAction* CreateAction (const QString& theText, const char* theSlot,
                                                QObject* theParent, QObject* theContext);

  //! Save state of three view in a container in form: key, value. It saves:
  //! - visibiblity of columns,
  //! - columns width
  //! \param theTreeView a view instance
  //! \param theItems [out] properties
  //! \param thePrefix peference item prefix
  Standard_EXPORT static void SaveState (View_Window* theView, QMap<QString, QString>& theItems,
                                         const QString& thePrefix = QString());
  //! Restore state of three view by a container
  //! \param theTreeView a view instance
  //! \param theKey property key
  //! \param theValue property value
  //! \param thePrefix peference item prefix
  //! \return boolean value whether the property is applyed to the tree view
  Standard_EXPORT static bool RestoreState (View_Window* theView, const QString& theKey, const QString& theValue,
                                            const QString& thePrefix = QString());
};

#endif
