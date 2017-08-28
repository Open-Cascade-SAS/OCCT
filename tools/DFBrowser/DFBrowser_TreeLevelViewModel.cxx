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

#include <inspector/DFBrowser_TreeLevelViewModel.hxx>

#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_ItemRole.hxx>
#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>
#include <inspector/DFBrowser_TreeLevelView.hxx>

#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <inspector/TreeModel_ModelBase.hxx>

#include <QApplication>
#include <QColor>
#include <QFont>
#include <QIcon>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowser_TreeLevelViewModel::Init (const QModelIndex& theTreeIndex)
{
  myIndex = theTreeIndex;
  TreeModel_ItemBasePtr anItem  = TreeModel_ModelBase::GetItemByIndex (theTreeIndex);
  myRowCount = anItem ? anItem->rowCount() : 0;
  if (!anItem)
    return;
  EmitLayoutChanged();
}

// =======================================================================
// function : GetTreeViewIndex
// purpose :
// =======================================================================
QModelIndex DFBrowser_TreeLevelViewModel::GetTreeViewIndex (const QModelIndex& theIndex) const
{
  return myIndex.model()->index (theIndex.row(), 0, myIndex);
}

// =======================================================================
// function : headerData
// purpose :
// =======================================================================
QVariant DFBrowser_TreeLevelViewModel::headerData (int theSection, Qt::Orientation theOrientation, int theRole) const
{
  return (theOrientation == Qt::Horizontal && theRole == Qt::DisplayRole && theSection == 1) ? QVariant (tr ("Name")) : QVariant();
}

// =======================================================================
// function : index
// purpose :
// =======================================================================
QModelIndex DFBrowser_TreeLevelViewModel::index (int theRow, int theColumn, const QModelIndex& theParent) const
{
  if (!hasIndex(theRow, theColumn, theParent))
    return QModelIndex();
  return createIndex(theRow, theColumn);
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant DFBrowser_TreeLevelViewModel::data (const QModelIndex& theIndex, int theRole) const
{
  QModelIndex anIndex = myIndex.model()->index (theIndex.row(), 0, myIndex);

  if ( !anIndex.isValid() )
    return QVariant ("undefined");

  QVariant aValue;
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  if (theIndex.column() == 0)
  {
    DFBrowser_ItemBasePtr aDBrowserItem = itemDynamicCast<DFBrowser_ItemBase> (anItemBase);
    bool aPrevValue = aDBrowserItem->SetUseAdditionalInfo (false);
    aValue = anItemBase->data (anIndex, theRole);
    aDBrowserItem->SetUseAdditionalInfo (aPrevValue);
  }
  else { // column = 1
    if (theRole == Qt::DisplayRole || theRole == Qt::ToolTipRole)
    {
      DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);
      if (anItem)
        aValue = anItem->GetAttributeInfo (DFBrowser_ItemRole_AdditionalInfo);
    }
  }
  if (theIndex.column() == 0 && theRole == Qt::FontRole) // method name is in italic
  {
    QFont aFont = qApp->font();
    aFont.setItalic (true);
    return aFont;
  }
  if (theIndex.column() == 0 && theRole == Qt::ForegroundRole) // method name is light gray
    return QColor (Qt::darkGray).darker(150);

  return aValue;
}

// =======================================================================
// function : flags
// purpose :
// =======================================================================
Qt::ItemFlags DFBrowser_TreeLevelViewModel::flags (const QModelIndex& theIndex) const
{
  if (!theIndex.isValid())
    return 0;
  Qt::ItemFlags aFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  return aFlags;
}
