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

#include <inspector/VInspector_ItemHistoryType.hxx>

#include <inspector/VInspector_CallBack.hxx>
#include <inspector/VInspector_ItemHistoryRoot.hxx>
#include <inspector/VInspector_ItemHistoryElement.hxx>
#include <inspector/VInspector_Tools.hxx>

// =======================================================================
// function : PointerInfo
// purpose :
// =======================================================================
QString VInspector_ItemHistoryType::PointerInfo() const
{
  if (rowCount() <= 0)
    return QString();

  VInspector_ItemHistoryRootPtr aParentItem = itemDynamicCast<VInspector_ItemHistoryRoot>(Parent());
  const VInspector_ItemHistoryTypeInfo& aTypeInfo = aParentItem->GetTypeInfo(Row());
  QList<QVariant> anElements = aTypeInfo.myElements[rowCount() - 1]; // the last item
  return anElements.size() > 1 ? anElements[1].toString() : QString();
}

// =======================================================================
// function : PointerInfo
// purpose :
// =======================================================================
QString VInspector_ItemHistoryType::OwnerInfo() const
{
  if (rowCount() <= 0)
    return QString();

  VInspector_ItemHistoryRootPtr aParentItem = itemDynamicCast<VInspector_ItemHistoryRoot>(Parent());
  const VInspector_ItemHistoryTypeInfo& aTypeInfo = aParentItem->GetTypeInfo(Row());
  QList<QVariant> anElements = aTypeInfo.myElements[rowCount() - 1]; // the last item
  return anElements.size() > 3 ? anElements[3].toString() : QString();
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant VInspector_ItemHistoryType::initValue(const int theRole) const
{
  if (theRole != Qt::DisplayRole && theRole != Qt::EditRole && theRole != Qt::ToolTipRole)
    return QVariant();

  VInspector_ItemHistoryRootPtr aParentItem = itemDynamicCast<VInspector_ItemHistoryRoot>(Parent());
  const VInspector_ItemHistoryTypeInfo& aTypeInfo = aParentItem->GetTypeInfo(Row());
  QList<QVariant> anElements = rowCount() > 0 ? aTypeInfo.myElements[rowCount() - 1] : QList<QVariant>(); // the last item
  int anInfoSize = anElements.size();
  switch (Column())
  {
    case 0: return VInspector_CallBack::GetInfo(aTypeInfo.myMode);
    case 1: return rowCount();
    case 2: return anInfoSize > 1 ? anElements[1].toString() : QVariant(); // pointer info
    case 3: return anInfoSize > 2 ? anElements[2].toString() : QVariant(); // shape type
    case 4: return anInfoSize > 0 ? anElements[0].toString() : QVariant(); // AIS name
    case 5: return anInfoSize > 3 ? anElements[3].toString() : QVariant(); // owner info
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : rowCount
// purpose :
// =======================================================================
int VInspector_ItemHistoryType::initRowCount() const
{
  VInspector_ItemHistoryRootPtr aParentItem = itemDynamicCast<VInspector_ItemHistoryRoot>(Parent());
  const VInspector_ItemHistoryTypeInfo& aTypeInfo = aParentItem->GetTypeInfo(Row());
  return aTypeInfo.myElements.size();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemHistoryType::createChild (int theRow, int theColumn)
{
  return VInspector_ItemHistoryElement::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemHistoryType::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemHistoryType*>(this)->Init();
  // an empty method to don't initialize the main label, as it was not cleared in Reset()
}
