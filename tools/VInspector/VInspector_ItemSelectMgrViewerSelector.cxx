// Created on: 2020-02-10
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

#include <inspector/VInspector_ItemSelectMgrViewerSelector.hxx>

#include <AIS.hxx>
#include <AIS_InteractiveContext.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemContextProperties.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrViewerSelector::initRowCount() const
{
  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrViewerSelector::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  if (GetViewerSelector().IsNull())
    return Column() == 0 ? "Empty viewer selector" : "";

  return Column() == 0 ? GetViewerSelector()->DynamicType()->Name() : QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrViewerSelector::Init()
{
  VInspector_ItemContextPropertiesPtr aParentItem = itemDynamicCast<VInspector_ItemContextProperties>(Parent());
  Handle(SelectMgr_ViewerSelector) aViewerSelector;
  if (aParentItem)
  {
    VInspector_ItemContextPtr aParentContextItem = itemDynamicCast<VInspector_ItemContext>(aParentItem->Parent());
    if (aParentContextItem)
    {
      Handle(AIS_InteractiveContext) aContext = aParentContextItem->GetContext();
      aViewerSelector = aContext->MainSelector();
    }
  }
  myViewerSelector = aViewerSelector;
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrViewerSelector::Reset()
{
  VInspector_ItemBase::Reset();

  myViewerSelector = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrViewerSelector::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectMgrViewerSelector*>(this)->Init();
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrViewerSelector::initStream (Standard_OStream& theOStream) const
{
  Handle(SelectMgr_ViewerSelector) aViewerSelector = GetViewerSelector();
  if (aViewerSelector.IsNull())
    return;

  aViewerSelector->DumpJson (theOStream);
}
