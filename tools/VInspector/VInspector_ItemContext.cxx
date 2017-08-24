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

#include <AIS_ListOfInteractive.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <QStringList>

//#define DEBUG_FREE_OWNERS

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemContext::initRowCount() const
{
  if (Column() != 0)
    return 0;

  int aNbPresentations = 0;
  if (!GetContext().IsNull())
  {
    AIS_ListOfInteractive aListOfIO;
    GetContext()->DisplayedObjects (aListOfIO);
    GetContext()->ErasedObjects(aListOfIO);
    aNbPresentations = aListOfIO.Extent();
  }
  // owners without Presentation
#ifdef DEBUG_FREE_OWNERS
  int aRows = 0;
  // only local context is processed: TODO for global context
  Handle(AIS_InteractiveContext) aContext = GetContext();
  if (!aContext.IsNull()) {
    NCollection_List<Handle(SelectBasics_EntityOwner)> anActiveOwners;
    aContext->MainSelector()->ActiveOwners(anActiveOwners);

    Handle(SelectMgr_EntityOwner) anOwner;
    for (NCollection_List<Handle(SelectBasics_EntityOwner)>::Iterator anOwnersIt(anActiveOwners);
      anOwnersIt.More(); anOwnersIt.Next())
    {
      anOwner = Handle(SelectMgr_EntityOwner)::DownCast(anOwnersIt.Value());
      if (anOwner.IsNull())
        continue;
      Handle(AIS_InteractiveObject) anAISObj = Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
      if (anAISObj.IsNull())
        aRows++;
    }
  }

  // owners in Global Context
  if (!aContext.IsNull())
  {
    NCollection_List<Handle(SelectBasics_EntityOwner)> anActiveOwners;
    aContext->MainSelector()->ActiveOwners(anActiveOwners);
  }
  if (aRows > 0)
    aNbPresentations += aRows;
#endif
#ifdef DEBUG_FREE_OWNERS
  NCollection_List<Handle(SelectBasics_EntityOwner)> anEmptySelectableOwners;
  NCollection_List<Handle(SelectBasics_EntityOwner)> anOwners =
    VInspector_Tools::ActiveOwners (GetContext(), anEmptySelectableOwners);
  if (anEmptySelectableOwners.Size() > 0)
    aNbPresentations += 1;
#endif
  return aNbPresentations;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemContext::initValue (const int theItemRole) const
{
  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  switch (Column())
  {
    case 0: return GetContext()->DynamicType()->Name();
    case 1: return rowCount();
    case 4:
    {
      if (theItemRole == Qt::ToolTipRole)
        return "Selected Owners";
      else
      {
        Handle(AIS_InteractiveObject) anEmptyIO;
        int aSelectedCount = VInspector_Tools::SelectedOwners (GetContext(), anEmptyIO, false);
        return aSelectedCount > 0 ? QString::number (aSelectedCount) : "";
      }
    }
    case 7:
    {
      if (theItemRole == Qt::ToolTipRole)
        return QString ("All Owners/Active Owners");
      else
      {
        NCollection_List<Handle(SelectBasics_EntityOwner)> anEmptySelectableOwners;
        NCollection_List<Handle(SelectBasics_EntityOwner)> anOwners =
                 VInspector_Tools::ActiveOwners (GetContext(), anEmptySelectableOwners);
        int anActiveOwners = anOwners.Size();
        anOwners = VInspector_Tools::ContextOwners (GetContext());
        return QString ("%1 / %2").arg (anOwners.Size()).arg (anActiveOwners);
      }
    }
    case 8:
    {
      if (theItemRole == Qt::ToolTipRole)
        return QString ("DeviationCoefficient");
      else
        return GetContext()->DeviationCoefficient();
    }
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemContext::createChild (int theRow, int theColumn)
{
  return VInspector_ItemPresentableObject::CreateItem (currentItem(), theRow, theColumn);
}
