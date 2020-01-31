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

#ifndef VInspector_ViewModelHistory_H
#define VInspector_ViewModelHistory_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>
#include <inspector/VInspector_CallBackMode.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QByteArray>
#include <QHash>
#include <QMap>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

#include <inspector/TreeModel_ModelBase.hxx>

//! \class VInspector_ViewModelHistory
//! The class that visualizes methods called in AIS interactive context.
class VInspector_ViewModelHistory : public TreeModel_ModelBase
{
  Q_OBJECT

public:

  //! Constructor
  Standard_EXPORT VInspector_ViewModelHistory (QObject* theParent, const int theHistoryTypesMaxAmount = 50);

  //! Destructor
  virtual ~VInspector_ViewModelHistory() {}

  //! Creates model columns and root items.
  Standard_EXPORT virtual void InitColumns() Standard_OVERRIDE;

  //! Append new element under the root item
  Standard_EXPORT void AddElement (const VInspector_CallBackMode& theMode, const QList<QVariant>& theInfo);

  //! Returns container of pointer information for selected presentation or owner item
  //! \param theIndex an index of tree view model
  //! \param container of strings
  Standard_EXPORT QStringList GetSelectedPointers (const QModelIndex& theIndex);

protected:

  //! Creates root item
  //! \param theColumnId index of a column
  Standard_EXPORT virtual TreeModel_ItemBasePtr createRootItem (const int theColumnId) Standard_OVERRIDE;

private:
  int myHistoryTypesMaxAmount; //!< maximum types in history
};

#endif
