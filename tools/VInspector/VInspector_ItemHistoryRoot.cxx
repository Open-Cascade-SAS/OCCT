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

#include <inspector/VInspector_ItemHistoryRoot.hxx>

#include <inspector/VInspector_ItemHistoryType.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ItemHistoryRoot::VInspector_ItemHistoryRoot(TreeModel_ItemBasePtr theParent,
                                                       const int theRow, const int theColumn)
: VInspector_ItemBase (theParent, theRow, theColumn), myFirstIndex (0), myLastIndex (0), myInfoMaxSize (10)
{
}

// =======================================================================
// function : AddElement
// purpose :
// =======================================================================
void VInspector_ItemHistoryRoot::AddElement (const VInspector_CallBackMode& theMode, const QList<QVariant>& theInfo)
{
  if (!myLastIndex)
  {
    myInfoMap[myLastIndex] = VInspector_ItemHistoryTypeInfo(theMode, theInfo);
    myLastIndex++;
  }
  else
  {
    if (myInfoMap[myLastIndex].myMode == theMode)
      myInfoMap[myLastIndex].AddElement(theInfo);
    else
    {
      myLastIndex++;
      myInfoMap[myLastIndex] = VInspector_ItemHistoryTypeInfo(theMode, theInfo);
    }
  }

  // clear cache
  if (myInfoMap.size() >= myInfoMaxSize)
  {
    myInfoMap.remove (myFirstIndex);
    myFirstIndex++;
  }
}

// =======================================================================
// function : GetTypeInfo
// purpose :
// =======================================================================
const VInspector_ItemHistoryTypeInfo& VInspector_ItemHistoryRoot::GetTypeInfo (const int theChildRowIndex)
{
  int anInfoMapIndex = theChildRowIndex + myFirstIndex;
  return myInfoMap[anInfoMapIndex];
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemHistoryRoot::initValue (const int theRole) const
{
  if (theRole != Qt::DisplayRole && theRole != Qt::EditRole && theRole != Qt::ToolTipRole)
    return QVariant();
 
  switch (Column())
  {
    case 0: return "History";
    case 1: return theRole == Qt::ToolTipRole ? QVariant ("Count of children") : QVariant (rowCount());
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemHistoryRoot::createChild (int theRow, int theColumn)
{
  return VInspector_ItemHistoryType::CreateItem (currentItem(), theRow, theColumn);
}

