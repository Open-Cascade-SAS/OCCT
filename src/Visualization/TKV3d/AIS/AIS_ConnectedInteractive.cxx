// Created on: 1997-01-08
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <AIS_ConnectedInteractive.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <BRepTools.hxx>
#include <NCollection_DataMap.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <StdSelect.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ConnectedInteractive, AIS_InteractiveObject)

//=================================================================================================

AIS_ConnectedInteractive::AIS_ConnectedInteractive(
  const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
    : AIS_InteractiveObject(aTypeOfPresentation3d)
{
  //
}

//=================================================================================================

void AIS_ConnectedInteractive::connect(const occ::handle<AIS_InteractiveObject>& theAnotherObj,
                                       const occ::handle<TopLoc_Datum3D>&        theLocation)
{
  if (myReference == theAnotherObj)
  {
    setLocalTransformation(theLocation);
    return;
  }

  occ::handle<AIS_ConnectedInteractive> aConnected =
    occ::down_cast<AIS_ConnectedInteractive>(theAnotherObj);
  if (!aConnected.IsNull())
  {
    myReference = aConnected->myReference;
  }
  else if (theAnotherObj->HasOwnPresentations())
  {
    myReference = theAnotherObj;
  }
  else
  {
    throw Standard_ProgramError(
      "AIS_ConnectedInteractive::Connect() - object without own presentation can not be connected");
  }

  if (!myReference.IsNull())
  {
    if (myReference->HasInteractiveContext()
        && myReference->GetContext()->DisplayStatus(myReference) != AIS_DS_None)
    {
      myReference.Nullify();
      throw Standard_ProgramError("AIS_ConnectedInteractive::Connect() - connected object should "
                                  "NOT be displayed in context");
    }
    myTypeOfPresentation3d = myReference->TypeOfPresentation3d();
  }
  setLocalTransformation(theLocation);
}

//=================================================================================================

void AIS_ConnectedInteractive::Disconnect()
{
  for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const occ::handle<PrsMgr_Presentation>& aPrs = aPrsIter.Value();
    if (!aPrs.IsNull())
    {
      aPrs->DisconnectAll(Graphic3d_TOC_DESCENDANT);
    }
  }
}

//=================================================================================================

void AIS_ConnectedInteractive::Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int                    theMode)
{
  if (HasConnection())
  {
    thePrs->Clear(false);
    thePrs->DisconnectAll(Graphic3d_TOC_DESCENDANT);

    if (!myReference->HasInteractiveContext())
    {
      myReference->SetContext(GetContext());
    }
    thePrsMgr->Connect(this, myReference, theMode, theMode);
    if (thePrsMgr->Presentation(myReference, theMode)->MustBeUpdated())
    {
      thePrsMgr->Update(myReference, theMode);
    }
  }

  if (!thePrs.IsNull())
  {
    thePrs->ReCompute();
  }
}

//=================================================================================================

void AIS_ConnectedInteractive::computeHLR(const occ::handle<Graphic3d_Camera>&   theProjector,
                                          const occ::handle<TopLoc_Datum3D>&     theTransformation,
                                          const occ::handle<Prs3d_Presentation>& thePresentation)
{
  const bool hasTrsf = !theTransformation.IsNull() && theTransformation->Form() != gp_Identity;
  updateShape(!hasTrsf);
  if (myShape.IsNull())
  {
    return;
  }
  if (hasTrsf)
  {
    const TopLoc_Location& aLocation = myShape.Location();
    TopoDS_Shape aShape = myShape.Located(TopLoc_Location(theTransformation->Trsf()) * aLocation);
    AIS_Shape::computeHlrPresentation(theProjector, thePresentation, aShape, myDrawer);
  }
  else
  {
    AIS_Shape::computeHlrPresentation(theProjector, thePresentation, myShape, myDrawer);
  }
}

//=================================================================================================

void AIS_ConnectedInteractive::updateShape(const bool isWithLocation)
{
  occ::handle<AIS_Shape> anAisShape = occ::down_cast<AIS_Shape>(myReference);
  if (anAisShape.IsNull())
  {
    return;
  }

  TopoDS_Shape aShape = anAisShape->Shape();
  if (aShape.IsNull())
  {
    return;
  }

  if (!isWithLocation)
  {
    myShape = aShape;
  }
  else
  {
    myShape = aShape.Moved(TopLoc_Location(Transformation()));
  }
}

//=================================================================================================

void AIS_ConnectedInteractive::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                                const int             theMode)
{
  if (!HasConnection())
  {
    return;
  }

  if (theMode != 0 && myReference->AcceptShapeDecomposition())
  {
    computeSubShapeSelection(theSelection, theMode);
    return;
  }

  if (!myReference->HasSelection(theMode))
  {
    myReference->RecomputePrimitives(theMode);
  }

  const occ::handle<SelectMgr_Selection>& TheRefSel = myReference->Selection(theMode);
  occ::handle<SelectMgr_EntityOwner>      anOwner   = new SelectMgr_EntityOwner(this);

  TopLoc_Location aLocation(Transformation());
  anOwner->SetLocation(aLocation);

  if (TheRefSel->IsEmpty())
  {
    myReference->RecomputePrimitives(theMode);
  }

  for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
         TheRefSel->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    if (const occ::handle<Select3D_SensitiveEntity>& aSensitive = aSelEntIter.Value()->BaseSensitive())
    {
      // Get the copy of SE3D
      if (occ::handle<Select3D_SensitiveEntity> aNewSensitive = aSensitive->GetConnected())
      {
        aNewSensitive->Set(anOwner);
        theSelection->Add(aNewSensitive);
      }
    }
  }
}

//=================================================================================================

void AIS_ConnectedInteractive::computeSubShapeSelection(
  const occ::handle<SelectMgr_Selection>& theSelection,
  const int             theMode)
{
  typedef NCollection_List<occ::handle<Select3D_SensitiveEntity>> SensitiveList;
  typedef NCollection_DataMap<TopoDS_Shape, SensitiveList>   Shapes2EntitiesMap;

  if (!myReference->HasSelection(theMode))
  {
    myReference->RecomputePrimitives(theMode);
  }

  const occ::handle<SelectMgr_Selection>& aRefSel = myReference->Selection(theMode);
  if (aRefSel->IsEmpty() || aRefSel->UpdateStatus() == SelectMgr_TOU_Full)
  {
    myReference->RecomputePrimitives(theMode);
  }

  // Fill in the map of subshapes and corresponding sensitive entities associated with aMode
  Shapes2EntitiesMap aShapes2EntitiesMap;
  for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter(
         aRefSel->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    if (const occ::handle<Select3D_SensitiveEntity>& aSE = aSelEntIter.Value()->BaseSensitive())
    {
      if (occ::handle<StdSelect_BRepOwner> anOwner =
            occ::down_cast<StdSelect_BRepOwner>(aSE->OwnerId()))
      {
        const TopoDS_Shape& aSubShape = anOwner->Shape();
        if (!aShapes2EntitiesMap.IsBound(aSubShape))
        {
          aShapes2EntitiesMap.Bind(aSubShape, SensitiveList());
        }
        aShapes2EntitiesMap(aSubShape).Append(aSE);
      }
    }
  }

  // Fill in selection from aShapes2EntitiesMap
  for (Shapes2EntitiesMap::Iterator aMapIt(aShapes2EntitiesMap); aMapIt.More(); aMapIt.Next())
  {
    const SensitiveList&        aSEList = aMapIt.Value();
    occ::handle<StdSelect_BRepOwner> anOwner =
      new StdSelect_BRepOwner(aMapIt.Key(),
                              this,
                              aSEList.First()->OwnerId()->Priority(),
                              true);
    anOwner->SetLocation(Transformation());
    for (SensitiveList::Iterator aListIt(aSEList); aListIt.More(); aListIt.Next())
    {
      if (occ::handle<Select3D_SensitiveEntity> aNewSE = aListIt.Value()->GetConnected())
      {
        aNewSE->Set(anOwner);
        theSelection->Add(aNewSE);
      }
    }
  }

  StdSelect::SetDrawerForBRepOwner(theSelection, myDrawer);
}
