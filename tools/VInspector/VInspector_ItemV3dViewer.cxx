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

#include <inspector/VInspector_ItemV3dViewer.hxx>

#include <AIS.hxx>
#include <AIS_InteractiveContext.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemContextProperties.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemV3dViewer::initRowCount() const
{
  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemV3dViewer::initValue(const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue(theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole
      && theItemRole != Qt::ToolTipRole)
    return QVariant();

  if (GetViewer().IsNull())
    return Column() == 0 ? "Empty viewer" : "";

  return Column() == 0 ? GetViewer()->DynamicType()->Name() : QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemV3dViewer::Init()
{
  VInspector_ItemContextPropertiesPtr aParentItem =
    itemDynamicCast<VInspector_ItemContextProperties>(Parent());
  Handle(V3d_Viewer) aViewer;
  if (aParentItem)
  {
    VInspector_ItemContextPtr aParentContextItem =
      itemDynamicCast<VInspector_ItemContext>(aParentItem->Parent());
    if (aParentContextItem)
    {
      Handle(AIS_InteractiveContext) aContext = aParentContextItem->GetContext();
      aViewer                                 = aContext->CurrentViewer();
    }
  }
  myViewer = aViewer;
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemV3dViewer::Reset()
{
  VInspector_ItemBase::Reset();

  myViewer = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemV3dViewer::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemV3dViewer*>(this)->Init();
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemV3dViewer::initStream(Standard_OStream& theOStream) const
{
  Handle(V3d_Viewer) aViewer = GetViewer();
  if (aViewer.IsNull())
    return;

  aViewer->DumpJson(theOStream);
}
