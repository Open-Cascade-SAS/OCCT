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

#include <inspector/VInspector_ItemContext.hxx>

#include <AIS.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <inspector/VInspector_ItemContextProperties.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemContext::initRowCount() const
{
  if (Column() != 0)
    return 0;

  int aNbProperties = 1; // item to visualize Viewer information of context

  Handle(AIS_InteractiveContext) aContext = Handle(AIS_InteractiveContext)::DownCast(Object());
  if (aContext.IsNull())
    return 0;

  AIS_ListOfInteractive aListOfIO;
  aContext->DisplayedObjects(aListOfIO);
  aContext->ErasedObjects(aListOfIO);
  int aNbPresentations = 0;
  for (AIS_ListIteratorOfListOfInteractive aListOfIOIt(aListOfIO); aListOfIOIt.More();
       aListOfIOIt.Next())
  {
    if (aListOfIOIt.Value()->Parent())
      continue; // child presentation
    aNbPresentations++;
  }

  return aNbProperties + aNbPresentations;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemContext::initValue(const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue(theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole
      && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(AIS_InteractiveContext) aContext = Handle(AIS_InteractiveContext)::DownCast(Object());
  if (aContext.IsNull())
    return Column() == 0 ? "Empty context" : "";

  switch (Column())
  {
    case 0:
      return aContext->DynamicType()->Name();
    case 4: {
      Handle(AIS_InteractiveObject) anEmptyIO;
      int aSelectedCount = VInspector_Tools::SelectedOwners(aContext, anEmptyIO, false);
      return aSelectedCount > 0 ? QString::number(aSelectedCount) : "";
    }
    case 6:
      return aContext->DeviationCoefficient();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemContext::Init()
{
  Handle(AIS_InteractiveContext) aContext = GetContext();
  if (aContext.IsNull())
    return;

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemContext::Reset()
{
  VInspector_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemContext::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemContext*>(this)->Init();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemContext::createChild(int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemContextProperties::CreateItem(currentItem(), theRow, theColumn);
  else
    return VInspector_ItemPresentableObject::CreateItem(currentItem(), theRow, theColumn);
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemContext::initStream(Standard_OStream& theOStream) const
{
  Handle(AIS_InteractiveContext) aContext = GetContext();
  if (aContext.IsNull())
    return;

  aContext->DumpJson(theOStream);
}
