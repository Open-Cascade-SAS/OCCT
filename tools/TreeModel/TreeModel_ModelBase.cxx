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

#include <inspector/TreeModel_ModelBase.hxx>

#include <inspector/TreeModel_ItemBase.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TreeModel_ModelBase::TreeModel_ModelBase (QObject* theParent)
: QAbstractItemModel (theParent), m_pRootItem (0)
{
}

// =======================================================================
// function :  GetItemByIndex
// purpose :
// =======================================================================
TreeModel_ItemBasePtr TreeModel_ModelBase::GetItemByIndex (const QModelIndex& theIndex)
{
  TreeModel_ItemBase* anItem = (TreeModel_ItemBase*)theIndex.internalPointer();
  return TreeModel_ItemBasePtr (anItem);
}

// =======================================================================
// function :  reset
// purpose :
// =======================================================================
void TreeModel_ModelBase::Reset()
{
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
    RootItem (aColId)->Reset();
}

// =======================================================================
// function :  index
// purpose :
// =======================================================================
QModelIndex TreeModel_ModelBase::index (int theRow, int theColumn, const QModelIndex& theParent) const
{
  if (!hasIndex (theRow, theColumn, theParent))
    return QModelIndex();

  // to create index on the root item
  if (!theParent.isValid())
    return createIndex (theRow, theColumn, getIndexValue (RootItem (theColumn)));

  TreeModel_ItemBasePtr aParentItem;
  if (!theParent.isValid())
    aParentItem = RootItem (theColumn);
  else
    aParentItem = GetItemByIndex (theParent);

  TreeModel_ItemBasePtr aChildItem = aParentItem->Child (theRow, theColumn);
  return aChildItem ? createIndex (theRow, theColumn, getIndexValue (aChildItem)) : QModelIndex();
}

// =======================================================================
// function :  data
// purpose :
// =======================================================================
QVariant TreeModel_ModelBase::data (const QModelIndex& theIndex, int theRole) const
{
  if (!theIndex.isValid())
    return QVariant ("undefined");

  TreeModel_ItemBasePtr anItem = GetItemByIndex (theIndex);
  return anItem->data (theIndex, theRole);
}

// =======================================================================
// function :  parent
// purpose :
// =======================================================================
QModelIndex TreeModel_ModelBase::parent (const QModelIndex& theIndex) const
{
  if (!theIndex.isValid())
    return QModelIndex();

  TreeModel_ItemBasePtr aChildItem = GetItemByIndex (theIndex);
  TreeModel_ItemBasePtr aParentItem = aChildItem ? aChildItem->Parent() : TreeModel_ItemBasePtr();

  if (!aParentItem)
    return QModelIndex();

  return createIndex (aParentItem->Row(), aParentItem->Column(), getIndexValue (aParentItem));
}

// =======================================================================
// function :  flags
// purpose :
// =======================================================================
Qt::ItemFlags TreeModel_ModelBase::flags (const QModelIndex& theIndex) const
{
  if (!theIndex.isValid())
    return 0;
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// =======================================================================
// function :  rowCount
// purpose :
// =======================================================================
int TreeModel_ModelBase::rowCount (const QModelIndex& theParent) const
{
  // to create index on the root item
  if (!theParent.isValid())
    return 1;

  TreeModel_ItemBasePtr aParentItem;
  if (!theParent.isValid())
    aParentItem = RootItem (0);
  else
    aParentItem = GetItemByIndex (theParent);

  return aParentItem ? aParentItem->rowCount() : 0;
}

// =======================================================================
// function :  emitLayoutChanged
// purpose :
// =======================================================================
void TreeModel_ModelBase::EmitLayoutChanged()
{
  emit layoutChanged();
}

// =======================================================================
// function :  getIndexValue
// purpose :
// =======================================================================
void* TreeModel_ModelBase::getIndexValue (const TreeModel_ItemBasePtr& theItem)
{
  return theItem.data();
}
