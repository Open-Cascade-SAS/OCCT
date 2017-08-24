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

#include <inspector/VInspector_ItemHistoryElement.hxx>

#include <inspector/VInspector_ItemHistoryRoot.hxx>
#include <inspector/VInspector_ItemHistoryType.hxx>
#include <inspector/VInspector_ItemHistoryElement.hxx>
#include <inspector/VInspector_Tools.hxx>

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant VInspector_ItemHistoryElement::initValue(const int theRole) const
{
  if (theRole != Qt::DisplayRole && theRole != Qt::EditRole && theRole != Qt::ToolTipRole)
    return QVariant();

  switch (Column())
  {
    case 0: return getName();
    case 1: return QVariant();
    case 2: return GetPointerInfo();
    case 3: return GetShapeTypeInfo();
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemHistoryElement::Init()
{
  VInspector_ItemHistoryTypePtr aParentItem = itemDynamicCast<VInspector_ItemHistoryType>(Parent());
  VInspector_ItemHistoryRootPtr aRootItem = itemDynamicCast<VInspector_ItemHistoryRoot>(aParentItem->Parent());

  const VInspector_ItemHistoryTypeInfo& aTypeInfo = aRootItem->GetTypeInfo (aParentItem->Row());
  QList<QList<QVariant>> anElements = aTypeInfo.myElements;

  int aRowId = Row();
  QList<QVariant> anInfo = anElements.at(aRowId);
  int anInfoSize = anInfo.size();

  myName = anInfoSize > 0 ? anInfo[0].toString() : QString ("Element: %1").arg (aRowId);
  myPointerInfo = anInfoSize > 1 ? anInfo[1].toString() : QString();
  myShapeTypeInfo = anInfoSize > 2 ? anInfo[2].toString() : QString();

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemHistoryElement::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemHistoryElement*>(this)->Init();
  // an empty method to don't initialize the main label, as it was not cleared in Reset()
}

// =======================================================================
// function : getName
// purpose :
// =======================================================================
QString VInspector_ItemHistoryElement::getName() const
{
  initItem();
  return myName;
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
QString VInspector_ItemHistoryElement::GetPointerInfo() const
{
  initItem();
  return myPointerInfo;
}

// =======================================================================
// function : GetShapeTypeInfo
// purpose :
// =======================================================================
QString VInspector_ItemHistoryElement::GetShapeTypeInfo() const
{
  initItem();
  return myShapeTypeInfo;
}
