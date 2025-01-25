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

#include <inspector/VInspector_Tools.hxx>

#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <inspector/Convert_Tools.hxx>

#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_Selection.hxx>
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Graphic3d_IndexBuffer.hxx>
#include <Graphic3d_Buffer.hxx>
#include <Graphic3d_BoundBuffer.hxx>

#include <SelectMgr_StateOfSelection.hxx>
#include <SelectMgr_TypeOfUpdate.hxx>
#include <SelectMgr_TypeOfBVHUpdate.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

#include <TopoDS_Compound.hxx>

#include <sstream>

// =======================================================================
// function : GetShapeTypeInfo
// purpose :
// =======================================================================
TCollection_AsciiString VInspector_Tools::GetShapeTypeInfo(const TopAbs_ShapeEnum& theType)
{
  Standard_SStream aSStream;
  TopAbs::Print(theType, aSStream);
  return aSStream.str().c_str();
}

// =======================================================================
// function : SelectedOwners
// purpose :
// =======================================================================
int VInspector_Tools::SelectedOwners(const Handle(AIS_InteractiveContext)& theContext,
                                     const Handle(AIS_InteractiveObject)&  theObject,
                                     const bool                            theShapeInfoOnly)
{
  QStringList anObjects;
  if (theContext.IsNull())
    return 0;

  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  for (theContext->InitSelected(); theContext->MoreSelected(); theContext->NextSelected())
  {
    Handle(SelectMgr_EntityOwner) anOwner = theContext->SelectedOwner();
    if (anOwner.IsNull()) // TODO: check why it is possible
      continue;

    if (!theObject.IsNull())
    {
      Handle(AIS_InteractiveObject) anOwnerPresentation =
        Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
      if (anOwnerPresentation != theObject)
        continue;
    }
    Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(anOwner);
    if (theShapeInfoOnly && BROwnr.IsNull())
      continue;

    Standard_Transient* anOwnerPtr = anOwner.get();
    if (aSelectedIds.contains((size_t)anOwnerPtr))
      continue;
    aSelectedIds.append((size_t)anOwnerPtr);

    anObjects.append(Standard_Dump::GetPointerInfo(anOwnerPtr, true).ToCString());
  }
  return anObjects.size();
}

// =======================================================================
// function : IsOwnerSelected
// purpose :
// =======================================================================
bool VInspector_Tools::IsOwnerSelected(const Handle(AIS_InteractiveContext)& theContext,
                                       const Handle(SelectMgr_EntityOwner)&  theOwner)
{
  bool anIsSelected = false;
  for (theContext->InitSelected(); theContext->MoreSelected() && !anIsSelected;
       theContext->NextSelected())
    anIsSelected = theContext->SelectedOwner() == theOwner;
  return anIsSelected;
}

// =======================================================================
// function : ContextOwners
// purpose :
// =======================================================================
NCollection_List<Handle(SelectMgr_EntityOwner)> VInspector_Tools::ContextOwners(
  const Handle(AIS_InteractiveContext)& theContext)
{
  NCollection_List<Handle(SelectMgr_EntityOwner)> aResultOwners;
  if (theContext.IsNull())
    return aResultOwners;

  AIS_ListOfInteractive aListOfIO;
  theContext->DisplayedObjects(aListOfIO);
  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  for (AIS_ListIteratorOfListOfInteractive aIt(aListOfIO); aIt.More(); aIt.Next())
  {
    Handle(AIS_InteractiveObject) anIO = aIt.Value();
    if (anIO.IsNull())
      continue;
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter(anIO->Selections()); aSelIter.More();
         aSelIter.Next())
    {
      Handle(SelectMgr_Selection) aSelection = aSelIter.Value();
      if (aSelection.IsNull())
        continue;
      for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
             aSelection->Entities());
           aSelEntIter.More();
           aSelEntIter.Next())
      {
        Handle(SelectMgr_SensitiveEntity) anEntity = aSelEntIter.Value();
        if (anEntity.IsNull())
          continue;
        const Handle(Select3D_SensitiveEntity)& aBase      = anEntity->BaseSensitive();
        Handle(SelectMgr_EntityOwner)           anOwner    = aBase->OwnerId();
        Standard_Transient*                     anOwnerPtr = anOwner.get();
        if (aSelectedIds.contains((size_t)anOwnerPtr))
          continue;
        aSelectedIds.append((size_t)anOwnerPtr);
        aResultOwners.Append(anOwner);
      }
    }
  }
  return aResultOwners;
}

// =======================================================================
// function : ActiveOwners
// purpose :
// =======================================================================
NCollection_List<Handle(SelectMgr_EntityOwner)> VInspector_Tools::ActiveOwners(
  const Handle(AIS_InteractiveContext)&            theContext,
  NCollection_List<Handle(SelectMgr_EntityOwner)>& theEmptySelectableOwners)
{
  NCollection_List<Handle(SelectMgr_EntityOwner)> aResultOwners;

  // only local context is processed: TODO for global context
  Handle(AIS_InteractiveContext) aContext = theContext;
  if (aContext.IsNull())
    return aResultOwners;
  NCollection_List<Handle(SelectMgr_EntityOwner)> anActiveOwners;
  // OCCT BUG:1 - equal pointer owners are appears in the list
  aContext->MainSelector()->ActiveOwners(anActiveOwners);
  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  Handle(SelectMgr_EntityOwner) anOwner;
  for (NCollection_List<Handle(SelectMgr_EntityOwner)>::Iterator anOwnersIt(anActiveOwners);
       anOwnersIt.More();
       anOwnersIt.Next())
  {
    anOwner = anOwnersIt.Value();
    if (anOwner.IsNull())
      continue;

    Standard_Transient* anOwnerPtr = anOwner.get();
    if (aSelectedIds.contains((size_t)anOwnerPtr))
      continue;
    aSelectedIds.append((size_t)anOwnerPtr);

    aResultOwners.Append(anOwner);
    Handle(SelectMgr_SelectableObject) aSelectable = anOwner->Selectable();
    if (aSelectable.IsNull()
        || !theContext->IsDisplayed(Handle(AIS_InteractiveObject)::DownCast(aSelectable)))
      theEmptySelectableOwners.Append(anOwner);
  }
  return aResultOwners;
}

// =======================================================================
// function : AddOrRemoveSelectedShapes
// purpose :
// =======================================================================
void VInspector_Tools::AddOrRemoveSelectedShapes(
  const Handle(AIS_InteractiveContext)&                  theContext,
  const NCollection_List<Handle(SelectMgr_EntityOwner)>& theOwners)
{
  // TODO: the next two rows are to be removed later
  theContext->UnhilightSelected(false);
  theContext->ClearSelected(false);

  theContext->UnhilightSelected(Standard_False);

  for (NCollection_List<Handle(SelectMgr_EntityOwner)>::Iterator anOwnersIt(theOwners);
       anOwnersIt.More();
       anOwnersIt.Next())
  {
    Handle(SelectMgr_EntityOwner) anOwner = anOwnersIt.Value();
    theContext->AddOrRemoveSelected(anOwner, Standard_False);
  }
  theContext->UpdateCurrentViewer();
}

// =======================================================================
// function : AddOrRemovePresentations
// purpose :
// =======================================================================
void VInspector_Tools::AddOrRemovePresentations(
  const Handle(AIS_InteractiveContext)&                  theContext,
  const NCollection_List<Handle(AIS_InteractiveObject)>& thePresentations)
{
  // TODO: the next two rows are to be removed later
  theContext->UnhilightSelected(false);
  theContext->ClearSelected(false);

  for (NCollection_List<Handle(AIS_InteractiveObject)>::Iterator anIOIt(thePresentations);
       anIOIt.More();
       anIOIt.Next())
    theContext->AddOrRemoveSelected(anIOIt.Value(), false);

  theContext->UpdateCurrentViewer();
}

// =======================================================================
// function : GetInfo
// purpose :
// =======================================================================
QList<QVariant> VInspector_Tools::GetInfo(Handle(AIS_InteractiveObject)& theObject)
{
  QList<QVariant> anInfo;
  anInfo.append(theObject->DynamicType()->Name());
  anInfo.append(Standard_Dump::GetPointerInfo(theObject, true).ToCString());

  Handle(AIS_Shape) aShapeIO = Handle(AIS_Shape)::DownCast(theObject);
  if (aShapeIO.IsNull())
    return anInfo;

  const TopoDS_Shape& aShape = aShapeIO->Shape();
  if (!aShape.IsNull())
    anInfo.append(VInspector_Tools::GetShapeTypeInfo(aShape.ShapeType()).ToCString());

  return anInfo;
}

// =======================================================================
// function : GetHighlightInfo
// purpose :
// =======================================================================
QList<QVariant> VInspector_Tools::GetHighlightInfo(const Handle(AIS_InteractiveContext)& theContext)
{
  QList<QVariant> aValues;
  if (theContext.IsNull())
    return aValues;

  QStringList   aSelectedNames;
  QStringList   aSelectedPointers;
  QStringList   aSelectedTypes;
  QStringList   aSelectedOwners;
  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  for (theContext->InitDetected(); theContext->MoreDetected(); theContext->NextDetected())
  {
    Handle(SelectMgr_EntityOwner) anOwner = theContext->DetectedOwner();
    if (anOwner.IsNull())
      continue;
    Standard_Transient* anOwnerPtr = anOwner.get();
    if (aSelectedIds.contains((size_t)anOwnerPtr))
      continue;
    aSelectedIds.append((size_t)anOwnerPtr);
    Handle(AIS_InteractiveObject) anIO =
      Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
    if (anIO.IsNull())
      continue;
    QList<QVariant> anIOInfo = VInspector_Tools::GetInfo(anIO);
    if (anIOInfo.size() == 3)
    {
      aSelectedNames.append(anIOInfo[0].toString());
      aSelectedPointers.append(anIOInfo[1].toString());
      aSelectedTypes.append(anIOInfo[2].toString());
    }
    aSelectedOwners.append(Standard_Dump::GetPointerInfo(anOwnerPtr, true).ToCString());
  }
  aValues.append(aSelectedNames.join(", "));
  aValues.append(aSelectedPointers.join(", "));
  aValues.append(aSelectedTypes.join(", "));
  aValues.append(aSelectedOwners.join(", "));

  return aValues;
}

// =======================================================================
// function : GetSelectedInfo
// purpose :
// =======================================================================
QList<QVariant> VInspector_Tools::GetSelectedInfo(const Handle(AIS_InteractiveContext)& theContext)
{
  QList<QVariant> aValues;
  if (theContext.IsNull())
    return aValues;

  QStringList   aSelectedNames;
  QStringList   aSelectedPointers;
  QStringList   aSelectedTypes;
  QStringList   aSelectedOwners;
  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  for (theContext->InitSelected(); theContext->MoreSelected(); theContext->NextSelected())
  {
    Handle(SelectMgr_EntityOwner) anOwner = theContext->SelectedOwner();
    if (anOwner.IsNull())
      continue;
    Standard_Transient* anOwnerPtr = anOwner.get();
    if (aSelectedIds.contains((size_t)anOwnerPtr))
      continue;
    aSelectedIds.append((size_t)anOwnerPtr);
    Handle(AIS_InteractiveObject) anIO =
      Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
    if (anIO.IsNull())
      continue;

    QList<QVariant> anIOInfo = VInspector_Tools::GetInfo(anIO);
    if (anIOInfo.size() == 3)
    {
      aSelectedNames.append(anIOInfo[0].toString());
      aSelectedPointers.append(anIOInfo[1].toString());
      aSelectedTypes.append(anIOInfo[2].toString());
    }
    aSelectedOwners.append(Standard_Dump::GetPointerInfo(anOwnerPtr, true).ToCString());
  }
  aValues.append(aSelectedNames.join(", "));
  aValues.append(aSelectedPointers.join(", "));
  aValues.append(aSelectedTypes.join(", "));
  aValues.append(aSelectedOwners.join(", "));
  return aValues;
}

// =======================================================================
// function : GetSelectedInfoPointers
// purpose :
// =======================================================================
QString VInspector_Tools::GetSelectedInfoPointers(const Handle(AIS_InteractiveContext)& theContext)
{
  QList<QVariant> aSelectedInfo = VInspector_Tools::GetSelectedInfo(theContext);
  return aSelectedInfo.size() > 2 ? aSelectedInfo[1].toString() : QString();
}

namespace
{
static Standard_CString VInspector_Table_PrintDisplayActionType[5] = {"None",
                                                                      "Display",
                                                                      "Redisplay",
                                                                      "Erase",
                                                                      "Remove"};
}

//=======================================================================
// function : DisplayActionTypeToString
// purpose  :
//=======================================================================
Standard_CString VInspector_Tools::DisplayActionTypeToString(View_DisplayActionType theType)
{
  return VInspector_Table_PrintDisplayActionType[theType];
}

//=======================================================================
// function : DisplayActionTypeFromString
// purpose  :
//=======================================================================
Standard_Boolean VInspector_Tools::DisplayActionTypeFromString(Standard_CString theTypeString,
                                                               View_DisplayActionType& theType)
{
  const TCollection_AsciiString aName(theTypeString);
  for (Standard_Integer aTypeIter = 0; aTypeIter <= View_DisplayActionType_RemoveId; ++aTypeIter)
  {
    Standard_CString aTypeName = VInspector_Table_PrintDisplayActionType[aTypeIter];
    if (aName == aTypeName)
    {
      theType = View_DisplayActionType(aTypeIter);
      return Standard_True;
    }
  }
  return Standard_False;
}
