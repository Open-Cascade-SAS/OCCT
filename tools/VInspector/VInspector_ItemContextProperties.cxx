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

#include <inspector/VInspector_ItemContextProperties.hxx>

#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemGraphic3dCLight.hxx>
#include <inspector/VInspector_ItemV3dViewer.hxx>
#include <inspector/VInspector_ItemSelectMgrViewerSelector.hxx>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemContextProperties::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (Column() != 0 || (theItemRole != Qt::DisplayRole && theItemRole != Qt::ToolTipRole))
    return QVariant();

  return "Properties";
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemContextProperties::initRowCount() const
{
  int aLightsCount = 0;

  VInspector_ItemContextPtr aParentContextItem = itemDynamicCast<VInspector_ItemContext>(Parent());
  if (aParentContextItem)
  {
    Handle(AIS_InteractiveContext) aContext = aParentContextItem->GetContext();
    Handle(V3d_Viewer) aViewer = aContext->CurrentViewer();
    if (!aViewer.IsNull())
    {
      if (!aViewer->ActiveViews().IsEmpty())
      {
        Handle(V3d_View) aView = aViewer->ActiveViews().First();
        if (!aView.IsNull())
          aLightsCount = aView->ActiveLights().Extent();
      }
    }
  }
  return 2 + aLightsCount; // V3d_Viewer, SelectMgr_ViewerSelector
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemContextProperties::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemV3dViewer::CreateItem (currentItem(), theRow, theColumn);
  else if (theRow == 1)
    return VInspector_ItemSelectMgrViewerSelector::CreateItem (currentItem(), theRow, theColumn);
  else // lights
  {
    return VInspector_ItemGraphic3dCLight::CreateItem (currentItem(), theRow, theColumn);
  }
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemContextProperties::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemContextProperties*> (this)->Init();
}
