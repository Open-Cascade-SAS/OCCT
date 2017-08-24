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

#include <inspector/TreeModel_ItemBase.hxx>

#include <inspector/TreeModel_ItemRole.hxx>

#include <QStringList>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TreeModel_ItemBase::TreeModel_ItemBase (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
 : m_bInitialized (false)
{
  m_pParent = theParent;
  m_iRow = theRow;
  m_iColumn = theColumn;
}

// =======================================================================
// function :  reset
// purpose :
// =======================================================================
void TreeModel_ItemBase::Reset()
{
  for (PositionToItemHash::const_iterator aChildrenIt = m_ChildItems.begin(); aChildrenIt != m_ChildItems.end(); aChildrenIt++)
  {
    TreeModel_ItemBasePtr anItem = aChildrenIt.value();
    if (anItem)
      anItem->Reset();
  }
  m_bInitialized = false;
  mycachedValues.clear();

}

// =======================================================================
// function :  child
// purpose :
// =======================================================================
TreeModel_ItemBasePtr TreeModel_ItemBase::Child (int theRow, int theColumn, const bool isToCreate)
{
  QPair<int, int> aPos = qMakePair (theRow, theColumn);

  if (m_ChildItems.contains (aPos))
    return m_ChildItems[aPos];

  TreeModel_ItemBasePtr anItem;
  if (isToCreate) {
    anItem = createChild (theRow, theColumn);
    if (anItem)
      m_ChildItems[aPos] = anItem;
  }
  return anItem;
}

// =======================================================================
// function :  currentItem
// purpose :
// =======================================================================
const TreeModel_ItemBasePtr TreeModel_ItemBase::currentItem()
{
  return TreeModel_ItemBasePtr (this);
}

// =======================================================================
// function :  cachedValue
// purpose :
// =======================================================================
QVariant TreeModel_ItemBase::cachedValue (const int theItemRole) const
{
  if (mycachedValues.contains (theItemRole))
    return mycachedValues[theItemRole];

  const_cast<TreeModel_ItemBase*>(this)->mycachedValues.insert (theItemRole,
    theItemRole == TreeModel_ItemRole_RowCountRole ? QVariant (initRowCount()) : initValue (theItemRole));

  return mycachedValues.contains (theItemRole) ? mycachedValues[theItemRole] : QVariant();
}
