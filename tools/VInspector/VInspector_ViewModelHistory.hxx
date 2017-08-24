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

#include <QByteArray>
#include <QHash>
#include <QMap>
#include <QObject>

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
  virtual ~VInspector_ViewModelHistory() Standard_OVERRIDE {};

  //! Append new element under the root item
  Standard_EXPORT void AddElement (const VInspector_CallBackMode& theMode, const QList<QVariant>& theInfo);

  //! Returns container of pointer information for selected presentation or owner item
  //! \param theIndex an index of tree view model
  //! \param container of strings
  Standard_EXPORT QStringList GetSelectedPointers (const QModelIndex& theIndex);

  //! Returns root item by column
  //! \param theColumn an index of the column
  //! \return root item instance
  virtual TreeModel_ItemBasePtr RootItem(const int theColumn) const Standard_OVERRIDE { return myRootItems[theColumn]; }

  //! Returns count of columns in the model
  //! \param theParent an index of the parent item
  //! \return integer value
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 6; }

  //! Returns the header data for the given role and section in the header with the specified orientation.
  //! \param theSection the header section. For horizontal headers - column number, for vertical headers - row number.
  //! \param theOrientation a header orientation
  //! \param theRole a data role
  //! \return the header data
  Standard_EXPORT virtual QVariant headerData (int theSection, Qt::Orientation theOrientation,
                                               int theRole = Qt::DisplayRole ) const Standard_OVERRIDE;

private:

  QMap<int, TreeModel_ItemBasePtr> myRootItems; //!< container of root items, for each column own root item
};

#endif
