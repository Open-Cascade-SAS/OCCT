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

#include <inspector/DFBrowser_TreeLevelLineModel.hxx>

#include <inspector/DFBrowser_ItemBase.hxx>
#include <inspector/TreeModel_ModelBase.hxx>

#include <QIcon>
#include <QWidget>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowser_TreeLevelLineModel::Init (const QModelIndex& theTreeIndex)
{
  myTreeIndex = theTreeIndex;
  myLevelItems.clear();

  if (theTreeIndex.isValid())
  {
    myLevelItems.prepend (theTreeIndex);
    QModelIndex aParent = theTreeIndex.parent();
    while (aParent.isValid())
    {
      myLevelItems.prepend (aParent);
      aParent = aParent.parent();
    }
  }
  emit layoutChanged();
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant DFBrowser_TreeLevelLineModel::data (const QModelIndex& theIndex, int theRole) const
{
  QVariant aValue;
  int aColumns = theIndex.column();
  if (aColumns < myLevelItems.size())
  {
    QModelIndex aTreeIndex = myLevelItems[aColumns];
    if (theRole == Qt::DecorationRole) //! do not show icons presented in tree view
      return aValue;
    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (aTreeIndex);
    if (anItemBase)
    {
      DFBrowser_ItemBasePtr aDBrowserItem = itemDynamicCast<DFBrowser_ItemBase> (anItemBase);
      bool aPrevValue = aDBrowserItem->SetUseAdditionalInfo (false);
      aValue = aDBrowserItem->data (aTreeIndex, theRole);
      aDBrowserItem->SetUseAdditionalInfo (aPrevValue);

    }
    if (theRole == Qt::DisplayRole)
      aValue = aValue.toString() + "  "; //! TEMPORARY to leave place for the action icon
  }
  return aValue;
}

// =======================================================================
// function : headerData
// purpose :
// =======================================================================
QVariant DFBrowser_TreeLevelLineModel::headerData (int theSection, Qt::Orientation theOrientation, int theRole) const
{
  QVariant aValue;
  if (theOrientation == Qt::Horizontal && theSection < myLevelItems.size())
  {
    QModelIndex aTreeIndex = myLevelItems[theSection];
    if (!aTreeIndex.isValid()) // level change action
    {
      if (theRole == Qt::SizeHintRole)
        aValue = QSize (2, 2);
      else if (theRole == Qt::DisplayRole)
        aValue = "";
    }
  }
  return aValue;
}
