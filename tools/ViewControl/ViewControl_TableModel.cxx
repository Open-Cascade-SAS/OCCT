// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <inspector/ViewControl_TableModel.hxx>

// =======================================================================
// function : SetModelValues
// purpose :
// =======================================================================
void ViewControl_TableModel::SetModelValues(ViewControl_TableModelValues* theModelValues)
{
  if (myModelValues)
    delete myModelValues;

  myModelValues = theModelValues;
}

// =======================================================================
// function : columnCount
// purpose :
// =======================================================================
int ViewControl_TableModel::columnCount(const QModelIndex& theParent) const
{
  if (!myModelValues)
    return 0;

  return myModelValues->ColumnCount(theParent);
}

// =======================================================================
// function : rowCount
// purpose :
// =======================================================================
int ViewControl_TableModel::rowCount(const QModelIndex& theParent) const
{
  if (!myModelValues)
    return 0;

  return myModelValues->RowCount(theParent);
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant ViewControl_TableModel::data(const QModelIndex& theIndex, int theRole) const
{
  if (!myModelValues)
    return QVariant();

  int aRow = theIndex.row(), aColumn = theIndex.column();
  return myModelValues->Data(aRow, aColumn, theRole);
}

// =======================================================================
// function : setData
// purpose :
// =======================================================================
bool ViewControl_TableModel::setData(const QModelIndex& theIndex,
                                     const QVariant&    theValue,
                                     int                theRole)
{
  if (!myModelValues)
    return false;

  int  aRow = theIndex.row(), aColumn = theIndex.column();
  bool aResult = myModelValues->SetData(aRow, aColumn, theValue, theRole);

  emit dataChanged(theIndex, theIndex);

  return aResult;
}
