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

#include <inspector/VInspector_ItemFolderObject.hxx>

#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemSelectMgrFilter.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemFolderObject::initValue (int theItemRole) const
{
  if (Column() == 0 && (theItemRole == Qt::DisplayRole || theItemRole == Qt::ToolTipRole))
  {
    if (parentItemIsContext()) return "Properties";
    else if (Row() == 0) return "Filters";
    else return QVariant();
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemFolderObject::initRowCount() const
{
  return parentItemIsContext() ? 1 : (GetContext().IsNull() ? 0 : GetContext()->Filters().Extent());
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemFolderObject::createChild (int theRow, int theColumn)
{
  if (parentItemIsContext())
    return VInspector_ItemFolderObject::CreateItem (currentItem(), theRow, theColumn);
  else
    return VInspector_ItemSelectMgrFilter::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemFolderObject::Init()
{
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemFolderObject::Reset()
{
  VInspector_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemFolderObject::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemFolderObject*> (this)->Init();
}

// =======================================================================
// function : parentItemIsContext
// purpose :
// =======================================================================
bool VInspector_ItemFolderObject::parentItemIsContext() const
{
  return itemDynamicCast<VInspector_ItemContext> (Parent());
}
