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


#include <inspector/VInspector_ItemBase.hxx>
#include <inspector/VInspector_ItemContext.hxx>

#include <inspector/ViewControl_Tools.hxx>

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemBase::Reset()
{
  myPresentationShape = TopoDS_Shape();
  TreeModel_ItemBase::Reset();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemBase::initValue (const int theItemRole) const
{
  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  switch (Column())
  {
    case 3: return Standard_Dump::GetPointerInfo (Object(), true).ToCString();
  }

  return TreeModel_ItemBase::initValue (theItemRole);
}

// =======================================================================
// function : GetContext
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) VInspector_ItemBase::GetContext() const
{
  if (!myContext.IsNull())
    return myContext;
  
  Handle(AIS_InteractiveContext) aContext;
  if (dynamic_cast<const VInspector_ItemContext*> (this))
  {
    return Handle(AIS_InteractiveContext)(); // context has not be set yet
  }
  else
  {
    TreeModel_ItemBasePtr anItem = Parent();
    while (anItem)
    {
      if (VInspector_ItemContextPtr aContextItem = itemDynamicCast<VInspector_ItemContext>(anItem))
      {
        aContext = aContextItem->GetContext();
        break;
      }
      anItem = anItem->Parent();
    }
  }
  if (!aContext.IsNull())
    const_cast<VInspector_ItemBase*>(this)->SetContext (aContext);

  return myContext;
}

// =======================================================================
// function : GetContext
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemBase::GetPresentationShape() const
{
  if (Column() != TreeModel_ColumnType_Name)
    return TopoDS_Shape();
  
  initItem();
  return myPresentationShape;
}
