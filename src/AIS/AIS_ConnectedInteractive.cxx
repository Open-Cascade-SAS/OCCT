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
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdSelect.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_OrientedShapeMapHasher.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ConnectedInteractive,AIS_InteractiveObject)

//=======================================================================
//function : AIS_ConnectedInteractive
//purpose  : 
//=======================================================================
AIS_ConnectedInteractive::AIS_ConnectedInteractive(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d):
AIS_InteractiveObject(aTypeOfPresentation3d)
{
  //
}

//=======================================================================
//function : connect
//purpose  :
//=======================================================================
void AIS_ConnectedInteractive::connect (const Handle(AIS_InteractiveObject)& theAnotherObj,
                                        const Handle(TopLoc_Datum3D)& theLocation)
{
  if (myReference == theAnotherObj)
  {
    setLocalTransformation (theLocation);
    return;
  }

  Handle(AIS_ConnectedInteractive) aConnected = Handle(AIS_ConnectedInteractive)::DownCast (theAnotherObj);
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
    throw Standard_ProgramError("AIS_ConnectedInteractive::Connect() - object without own presentation can not be connected");
  }

  if (!myReference.IsNull())
  {
    if (myReference->HasInteractiveContext()
     && myReference->GetContext()->DisplayStatus (myReference) != AIS_DS_None)
    {
      myReference.Nullify();
      throw Standard_ProgramError("AIS_ConnectedInteractive::Connect() - connected object should NOT be displayed in context");
    }
    myTypeOfPresentation3d = myReference->TypeOfPresentation3d();
  }
  setLocalTransformation (theLocation);
}

//=======================================================================
//function : Disconnect
//purpose  :
//=======================================================================

void AIS_ConnectedInteractive::Disconnect()
{
  for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
  {
    const Handle(PrsMgr_Presentation)& aPrs = aPrsIter.Value();
    if (!aPrs.IsNull())
    {
      aPrs->DisconnectAll (Graphic3d_TOC_DESCENDANT);
    }
  }
}
//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_ConnectedInteractive::Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)&           thePrs,
                                        const Standard_Integer                      theMode)
{
  if (HasConnection())
  {
    thePrs->Clear (Standard_False);
    thePrs->DisconnectAll (Graphic3d_TOC_DESCENDANT);

    if (!myReference->HasInteractiveContext())
    {
      myReference->SetContext (GetContext());
    }
    thePrsMgr->Connect (this, myReference, theMode, theMode);
    if (thePrsMgr->Presentation (myReference, theMode)->MustBeUpdated())
    {
      thePrsMgr->Update (myReference, theMode);
    }
  }

  if (!thePrs.IsNull())
  {
    thePrs->ReCompute();
  }
}

//=======================================================================
//function : computeHLR
//purpose  :
//=======================================================================
void AIS_ConnectedInteractive::computeHLR (const Handle(Graphic3d_Camera)& theProjector,
                                           const Handle(TopLoc_Datum3D)& theTransformation,
                                           const Handle(Prs3d_Presentation)& thePresentation)
{
  const bool hasTrsf = !theTransformation.IsNull()
                     && theTransformation->Form() != gp_Identity;
  updateShape (!hasTrsf);
  if (myShape.IsNull())
  {
    return;
  }
  if (hasTrsf)
  {
    const TopLoc_Location& aLocation = myShape.Location();
    TopoDS_Shape aShape = myShape.Located (TopLoc_Location (theTransformation->Trsf()) * aLocation);
    AIS_Shape::computeHlrPresentation (theProjector, thePresentation, aShape, myDrawer);
  }
  else
  {
    AIS_Shape::computeHlrPresentation (theProjector, thePresentation, myShape, myDrawer);
  }
}

//=======================================================================
//function : updateShape
//purpose  : 
//=======================================================================
void AIS_ConnectedInteractive::updateShape (const Standard_Boolean isWithLocation)
{
  Handle(AIS_Shape) anAisShape = Handle(AIS_Shape)::DownCast (myReference);
  if (anAisShape.IsNull())
  {
    return;
  }
 
  TopoDS_Shape aShape = anAisShape->Shape();
  if (aShape.IsNull())
  {
    return;
  }

  if(!isWithLocation)
  {
    myShape = aShape;
  }
  else
  {
    myShape = aShape.Moved (TopLoc_Location (Transformation()));
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_ConnectedInteractive::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection, 
                                                 const Standard_Integer theMode)
{
  if (!HasConnection())
  {
    return;
  }

  if (theMode != 0 && myReference->AcceptShapeDecomposition())
  {
    computeSubShapeSelection (theSelection, theMode);
    return;
  }

  if (!myReference->HasSelection (theMode))
  {
    myReference->RecomputePrimitives (theMode);
  }

  const Handle(SelectMgr_Selection)& TheRefSel = myReference->Selection (theMode);
  Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this);

  TopLoc_Location aLocation (Transformation());
  anOwner->SetLocation (aLocation);

  if (TheRefSel->IsEmpty())
  {
    myReference->RecomputePrimitives (theMode);
  }

  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (TheRefSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
  {
    if (const Handle(Select3D_SensitiveEntity)& aSensitive = aSelEntIter.Value()->BaseSensitive())
    {
      // Get the copy of SE3D
      if (Handle(Select3D_SensitiveEntity) aNewSensitive = aSensitive->GetConnected())
      {
        aNewSensitive->Set(anOwner);
        theSelection->Add (aNewSensitive);
      }
    }
  }
}

//=======================================================================
//function : ComputeSubShapeSelection 
//purpose  :
//=======================================================================
void AIS_ConnectedInteractive::computeSubShapeSelection (const Handle(SelectMgr_Selection)& theSelection, 
                                                         const Standard_Integer theMode)
{
  typedef NCollection_List<Handle(Select3D_SensitiveEntity)> SensitiveList;
  typedef NCollection_DataMap<TopoDS_Shape, SensitiveList, TopTools_OrientedShapeMapHasher>
    Shapes2EntitiesMap;

  if (!myReference->HasSelection (theMode))
  {
    myReference->RecomputePrimitives (theMode);
  }

  const Handle(SelectMgr_Selection)& aRefSel = myReference->Selection (theMode);
  if (aRefSel->IsEmpty() || aRefSel->UpdateStatus() == SelectMgr_TOU_Full)
  {
    myReference->RecomputePrimitives (theMode);
  }

  // Fill in the map of subshapes and corresponding sensitive entities associated with aMode
  Shapes2EntitiesMap aShapes2EntitiesMap;
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aRefSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
  {
    if (const Handle(Select3D_SensitiveEntity)& aSE = aSelEntIter.Value()->BaseSensitive())
    {
      if (Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast (aSE->OwnerId()))
      {
        const TopoDS_Shape& aSubShape = anOwner->Shape();
        if(!aShapes2EntitiesMap.IsBound (aSubShape))
        {
          aShapes2EntitiesMap.Bind (aSubShape, SensitiveList());
        }
        aShapes2EntitiesMap (aSubShape).Append (aSE);
      }
    }
  }

  // Fill in selection from aShapes2EntitiesMap
  for (Shapes2EntitiesMap::Iterator aMapIt (aShapes2EntitiesMap); aMapIt.More(); aMapIt.Next())
  {
    const SensitiveList& aSEList = aMapIt.Value();
    Handle(StdSelect_BRepOwner) anOwner = new StdSelect_BRepOwner (aMapIt.Key(), this, aSEList.First()->OwnerId()->Priority(), Standard_True);
    anOwner->SetLocation (Transformation());
    for (SensitiveList::Iterator aListIt (aSEList); aListIt.More(); aListIt.Next())
    {
      if (Handle(Select3D_SensitiveEntity) aNewSE = aListIt.Value()->GetConnected())
      {
        aNewSE->Set (anOwner);
        theSelection->Add (aNewSE);
      }
    }
  }

  StdSelect::SetDrawerForBRepOwner (theSelection, myDrawer);  
}
