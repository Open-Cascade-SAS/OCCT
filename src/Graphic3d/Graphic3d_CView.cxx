// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Graphic3d_CView.hxx>

#include <Aspect_NeutralWindow.hxx>
#include <Aspect_OpenVRSession.hxx>
#include <Graphic3d_CubeMapPacked.hxx>
#include <Graphic3d_Layer.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_StructureManager.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_CView,Graphic3d_DataStructureManager)

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
Graphic3d_CView::Graphic3d_CView (const Handle(Graphic3d_StructureManager)& theMgr)
: myId (0),
  //
  myParentView (nullptr),
  myIsSubviewComposer (Standard_False),
  mySubviewCorner (Aspect_TOTP_LEFT_UPPER),
  mySubviewSize (1.0, 1.0),
  //
  myStructureManager (theMgr),
  myCamera (new Graphic3d_Camera()),
  myIsInComputedMode (Standard_False),
  myIsActive (Standard_False),
  myIsRemoved (Standard_False),
  myBackfacing (Graphic3d_TypeOfBackfacingModel_Auto),
  myVisualization (Graphic3d_TOV_WIREFRAME),
  //
  myBgColor (Quantity_NOC_BLACK),
  myBackgroundType (Graphic3d_TOB_NONE),
  myToUpdateSkydome (Standard_False),
  //
  myUnitFactor (1.0)
{
  myId = myStructureManager->Identification (this);
}

//=======================================================================
//function : Destructor
//purpose  :
//=======================================================================
Graphic3d_CView::~Graphic3d_CView()
{
  myXRSession.Nullify();
  if (!IsRemoved())
  {
    myStructureManager->UnIdentification (this);
  }
}

// =======================================================================
// function : SetBackgroundSkydome
// purpose  :
// =======================================================================
void Graphic3d_CView::SetBackgroundSkydome (const Aspect_SkydomeBackground& theAspect,
                                            Standard_Boolean theToUpdatePBREnv)
{
  myToUpdateSkydome = true;
  mySkydomeAspect = theAspect;
  myCubeMapBackground = new Graphic3d_CubeMapPacked ("");
  SetBackgroundType (Graphic3d_TOB_CUBEMAP);
  if (theToUpdatePBREnv
      && !myCubeMapIBL.IsNull())
  {
    SetImageBasedLighting (false);
    SetImageBasedLighting (true);
  }
}

// =======================================================================
// function : Activate
// purpose  :
// =======================================================================
void Graphic3d_CView::Activate()
{
  if (!IsActive())
  {
    myIsActive = Standard_True;

    // Activation of a new view =>
    // Display structures that can be displayed in this new view.
    // All structures with status
    // Displayed in ViewManager are returned and displayed in
    // the view directly, if the structure is not already
    // displayed and if the view accepts it in its context.
    Graphic3d_MapOfStructure aDisplayedStructs;
    myStructureManager->DisplayedStructures (aDisplayedStructs);
    for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aDisplayedStructs); aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct = aStructIter.Key();
      if (IsDisplayed (aStruct))
      {
        continue;
      }

      // If the structure can be displayed in the new context of the view, it is displayed.
      const Graphic3d_TypeOfAnswer anAnswer = acceptDisplay (aStruct->Visual());
      if (anAnswer == Graphic3d_TOA_YES
       || anAnswer == Graphic3d_TOA_COMPUTE)
      {
        Display (aStruct);
      }
    }
  }

  Update();
}

// =======================================================================
// function : Deactivate
// purpose  :
// =======================================================================
void Graphic3d_CView::Deactivate()
{
  if (IsActive())
  {
    // Deactivation of a view =>
    // Removal of structures displayed in this view.
    // All structures with status
    // Displayed in ViewManager are returned and removed from
    // the view directly, if the structure is not already
    // displayed and if the view accepts it in its context.
    Graphic3d_MapOfStructure aDisplayedStructs;
    myStructureManager->DisplayedStructures (aDisplayedStructs);
    for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aDisplayedStructs); aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct = aStructIter.Key();
      if (!IsDisplayed (aStruct))
      {
        continue;
      }

      const Graphic3d_TypeOfAnswer anAnswer = acceptDisplay (aStruct->Visual());
      if (anAnswer == Graphic3d_TOA_YES
       || anAnswer == Graphic3d_TOA_COMPUTE)
      {
        Erase (aStruct);
      }
    }

    Update();
    myIsActive = Standard_False;
  }
}

// ========================================================================
// function : Remove
// purpose  :
// ========================================================================
void Graphic3d_CView::Remove()
{
  if (IsRemoved())
  {
    return;
  }

  if (myParentView != nullptr)
  {
    myParentView->RemoveSubview (this);
    myParentView = nullptr;
  }
  {
    NCollection_Sequence<Handle(Graphic3d_CView)> aSubviews = mySubviews;
    mySubviews.Clear();
    for (const Handle(Graphic3d_CView)& aViewIter : aSubviews)
    {
      aViewIter->Remove();
    }
  }

  Graphic3d_MapOfStructure aDisplayedStructs (myStructsDisplayed);
  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aDisplayedStructs); aStructIter.More(); aStructIter.Next())
  {
    Erase (aStructIter.Value());
  }

  myStructsToCompute.Clear();
  myStructsComputed .Clear();
  myStructsDisplayed.Clear();

  if (!myStructureManager.IsNull())
  {
    myStructureManager->UnIdentification (this);
  }

  myIsActive  = Standard_False;
  myIsRemoved = Standard_True;
}

// ========================================================================
// function : AddSubview
// purpose  :
// ========================================================================
void Graphic3d_CView::AddSubview (const Handle(Graphic3d_CView)& theView)
{
  mySubviews.Append (theView);
}

// ========================================================================
// function : RemoveSubview
// purpose  :
// ========================================================================
bool Graphic3d_CView::RemoveSubview (const Graphic3d_CView* theView)
{
  for (NCollection_Sequence<Handle(Graphic3d_CView)>::Iterator aViewIter (mySubviews); aViewIter.More(); aViewIter.Next())
  {
    if (aViewIter.Value() == theView)
    {
      mySubviews.Remove (aViewIter);
      return true;
    }
  }
  return false;
}

// ========================================================================
// function : Resized
// purpose  :
// ========================================================================
void Graphic3d_CView::Resized()
{
  if (IsSubview())
  {
    Handle(Aspect_NeutralWindow) aWindow = Handle(Aspect_NeutralWindow)::DownCast(Window());
    SubviewResized (aWindow);
  }
}

//! Calculate offset in pixels from fraction.
static int getSubViewOffset (double theOffset, int theWinSize)
{
  if (theOffset >= 1.0)
  {
    return int(theOffset);
  }
  else
  {
    return int(theOffset * theWinSize);
  }
}

// ========================================================================
// function : SubviewResized
// purpose  :
// ========================================================================
void Graphic3d_CView::SubviewResized (const Handle(Aspect_NeutralWindow)& theWindow)
{
  if (!IsSubview()
   || theWindow.IsNull())
  {
    return;
  }

  const Graphic3d_Vec2i aWinSize (myParentView->Window()->Dimensions());
  Graphic3d_Vec2i aViewSize (Graphic3d_Vec2d(aWinSize) * mySubviewSize);
  if (mySubviewSize.x() > 1.0)
  {
    aViewSize.x() = (int)mySubviewSize.x();
  }
  if (mySubviewSize.y() > 1.0)
  {
    aViewSize.y() = (int)mySubviewSize.y();
  }

  Graphic3d_Vec2i anOffset (getSubViewOffset (mySubviewOffset.x(), aWinSize.x()),
                            getSubViewOffset (mySubviewOffset.y(), aWinSize.y()));
  mySubviewTopLeft = (aWinSize - aViewSize) / 2; // Aspect_TOTP_CENTER
  if ((mySubviewCorner & Aspect_TOTP_LEFT) != 0)
  {
    mySubviewTopLeft.x() = anOffset.x();
  }
  else if ((mySubviewCorner & Aspect_TOTP_RIGHT) != 0)
  {
    mySubviewTopLeft.x() = Max (aWinSize.x() - anOffset.x() - aViewSize.x(), 0);
  }

  if ((mySubviewCorner & Aspect_TOTP_TOP) != 0)
  {
    mySubviewTopLeft.y() = anOffset.y();
  }
  else if ((mySubviewCorner & Aspect_TOTP_BOTTOM) != 0)
  {
    mySubviewTopLeft.y() = Max (aWinSize.y() - anOffset.y() - aViewSize.y(), 0);
  }

  mySubviewTopLeft += mySubviewMargins;
  aViewSize -= mySubviewMargins * 2;

  const int aRight = Min(mySubviewTopLeft.x() + aViewSize.x(), aWinSize.x());
  aViewSize.x() = aRight - mySubviewTopLeft.x();

  const int aBot = Min(mySubviewTopLeft.y() + aViewSize.y(), aWinSize.y());
  aViewSize.y() = aBot - mySubviewTopLeft.y();

  theWindow->SetSize (aViewSize.x(), aViewSize.y());
}

// ========================================================================
// function : SetComputedMode
// purpose  :
// ========================================================================
void Graphic3d_CView::SetComputedMode (const Standard_Boolean theMode)
{
  if (( theMode &&  myIsInComputedMode)
   || (!theMode && !myIsInComputedMode))
  {
    return;
  }

  myIsInComputedMode = theMode;
  if (!myIsInComputedMode)
  {
    for (Graphic3d_MapOfStructure::Iterator aStructIter (myStructsDisplayed); aStructIter.More(); aStructIter.Next())
    {
      const Handle(Graphic3d_Structure)& aStruct  = aStructIter.Key();
      const Graphic3d_TypeOfAnswer        anAnswer = acceptDisplay (aStruct->Visual());
      if (anAnswer != Graphic3d_TOA_COMPUTE)
      {
        continue;
      }

      const Standard_Integer anIndex = IsComputed (aStruct);
      if (anIndex != 0)
      {
        const Handle(Graphic3d_Structure)& aStructComp = myStructsComputed.Value (anIndex);
        eraseStructure   (aStructComp->CStructure());
        displayStructure (aStruct->CStructure(), aStruct->DisplayPriority());
        Update (aStruct->GetZLayer());
      }
    }
    return;
  }

  for (Graphic3d_MapOfStructure::Iterator aDispStructIter (myStructsDisplayed); aDispStructIter.More(); aDispStructIter.Next())
  {
    const Handle(Graphic3d_Structure)& aStruct  = aDispStructIter.Key();
    const Graphic3d_TypeOfAnswer anAnswer = acceptDisplay (aStruct->Visual());
    if (anAnswer != Graphic3d_TOA_COMPUTE)
    {
      continue;
    }

    const Standard_Integer anIndex = IsComputed (aStruct);
    if (anIndex != 0)
    {
      eraseStructure   (aStruct->CStructure());
      displayStructure (myStructsComputed.Value (anIndex)->CStructure(), aStruct->DisplayPriority());

      Display (aStruct);
      if (aStruct->IsHighlighted())
      {
        const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.Value (anIndex);
        if (!aCompStruct->IsHighlighted())
        {
          aCompStruct->Highlight (aStruct->HighlightStyle(), Standard_False);
        }
      }
    }
    else
    {
      Handle(Graphic3d_Structure) aCompStruct;
      aStruct->computeHLR (myCamera, aCompStruct);
      if (aCompStruct.IsNull())
      {
        continue;
      }
      aCompStruct->SetHLRValidation (Standard_True);

      const Standard_Boolean toComputeWireframe = myVisualization == Graphic3d_TOV_WIREFRAME
                                                && aStruct->ComputeVisual() != Graphic3d_TOS_SHADING;
      const Standard_Boolean toComputeShading   = myVisualization == Graphic3d_TOV_SHADING
                                                && aStruct->ComputeVisual() != Graphic3d_TOS_WIREFRAME;
      if (toComputeWireframe) aCompStruct->SetVisual (Graphic3d_TOS_WIREFRAME);
      if (toComputeShading  ) aCompStruct->SetVisual (Graphic3d_TOS_SHADING);

      if (aStruct->IsHighlighted())
      {
        aCompStruct->Highlight (aStruct->HighlightStyle(), Standard_False);
      }

      Standard_Boolean hasResult = Standard_False;
      const Standard_Integer aNbToCompute = myStructsToCompute.Length();
      const Standard_Integer aStructId    = aStruct->Identification();
      for (Standard_Integer aToCompStructIter = 1; aToCompStructIter <= aNbToCompute; ++aToCompStructIter)
      {
        if (myStructsToCompute.Value (aToCompStructIter)->Identification() == aStructId)
        {
          hasResult = Standard_True;
          myStructsComputed.ChangeValue (aToCompStructIter) = aCompStruct;
          break;
        }
      }

      if (!hasResult)
      {
        myStructsToCompute.Append (aStruct);
        myStructsComputed .Append (aCompStruct);
      }

      aCompStruct->CalculateBoundBox();
      eraseStructure   (aStruct->CStructure());
      displayStructure (aCompStruct->CStructure(), aStruct->DisplayPriority());
    }
  }
  Update();
}

// =======================================================================
// function : ReCompute
// purpose  :
// =======================================================================
void Graphic3d_CView::ReCompute (const Handle(Graphic3d_Structure)& theStruct)
{
  theStruct->CalculateBoundBox();
  if (!theStruct->IsMutable()
   && !theStruct->CStructure()->IsForHighlight
   && !theStruct->CStructure()->IsInfinite)
  {
    const Graphic3d_ZLayerId aLayerId = theStruct->GetZLayer();
    InvalidateBVHData (aLayerId);
  }

  if (!ComputedMode()
   || !IsActive()
   || !theStruct->IsDisplayed())
  {
    return;
  }

  const Graphic3d_TypeOfAnswer anAnswer = acceptDisplay (theStruct->Visual());
  if (anAnswer != Graphic3d_TOA_COMPUTE)
  {
    return;
  }

  const Standard_Integer anIndex = IsComputed (theStruct);
  if (anIndex == 0)
  {
    return;
  }

  // compute + validation
  Handle(Graphic3d_Structure) aCompStructOld = myStructsComputed.ChangeValue (anIndex);
  Handle(Graphic3d_Structure) aCompStruct    = aCompStructOld;
  aCompStruct->SetTransformation (Handle(TopLoc_Datum3D)());
  theStruct->computeHLR (myCamera, aCompStruct);
  if (aCompStruct.IsNull())
  {
    return;
  }

  aCompStruct->SetHLRValidation (Standard_True);
  aCompStruct->CalculateBoundBox();

  // of which type will be the computed?
  const Standard_Boolean toComputeWireframe = myVisualization == Graphic3d_TOV_WIREFRAME
                                           && theStruct->ComputeVisual() != Graphic3d_TOS_SHADING;
  const Standard_Boolean toComputeShading   = myVisualization == Graphic3d_TOV_SHADING
                                           && theStruct->ComputeVisual() != Graphic3d_TOS_WIREFRAME;
  if (toComputeWireframe)
  {
    aCompStruct->SetVisual (Graphic3d_TOS_WIREFRAME);
  }
  else if (toComputeShading)
  {
    aCompStruct->SetVisual (Graphic3d_TOS_SHADING);
  }

  if (theStruct->IsHighlighted())
  {
    aCompStruct->Highlight (theStruct->HighlightStyle(), Standard_False);
  }

  // The previous calculation is removed and the new one is displayed
  eraseStructure   (aCompStructOld->CStructure());
  displayStructure (aCompStruct->CStructure(), theStruct->DisplayPriority());

  // why not just replace existing items?
  //myStructsToCompute.ChangeValue (anIndex) = theStruct;
  //myStructsComputed .ChangeValue (anIndex) = aCompStruct;

  // hlhsr and the new associated compute are added
  myStructsToCompute.Append (theStruct);
  myStructsComputed .Append (aCompStruct);

  // hlhsr and the new associated compute are removed
  myStructsToCompute.Remove (anIndex);
  myStructsComputed .Remove (anIndex);
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void Graphic3d_CView::Update (const Graphic3d_ZLayerId theLayerId)
{
  InvalidateZLayerBoundingBox (theLayerId);
}

// =======================================================================
// function : InvalidateZLayerBoundingBox
// purpose  :
// =======================================================================
void Graphic3d_CView::InvalidateZLayerBoundingBox (const Graphic3d_ZLayerId theLayerId)
{
  if (Handle(Graphic3d_Layer) aLayer = Layer (theLayerId))
  {
    aLayer->InvalidateBoundingBox();
    return;
  }

  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter (Layers()); aLayerIter.More(); aLayerIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayer = aLayerIter.Value();
    if (aLayer->NbOfTransformPersistenceObjects() > 0)
    {
      aLayer->InvalidateBoundingBox();
    }
  }
}

// =======================================================================
// function : DisplayedStructures
// purpose  :
// =======================================================================
void Graphic3d_CView::DisplayedStructures (Graphic3d_MapOfStructure& theStructures) const
{
  for (Graphic3d_MapOfStructure::Iterator aStructIter (myStructsDisplayed); aStructIter.More(); aStructIter.Next())
  {
    theStructures.Add (aStructIter.Key());
  }
}

// =======================================================================
// function : MinMaxValues
// purpose  :
// =======================================================================
Bnd_Box Graphic3d_CView::MinMaxValues (const Standard_Boolean theToIncludeAuxiliary) const
{
  if (!IsDefined())
  {
    return Bnd_Box();
  }

  const Handle(Graphic3d_Camera)& aCamera = Camera();
  Graphic3d_Vec2i aWinSize;
  Window()->Size (aWinSize.x(), aWinSize.y());

  Bnd_Box aResult;
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter (Layers()); aLayerIter.More(); aLayerIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayer = aLayerIter.Value();
    Bnd_Box aBox = aLayer->BoundingBox (Identification(),
                                        aCamera,
                                        aWinSize.x(), aWinSize.y(),
                                        theToIncludeAuxiliary);
    aResult.Add (aBox);
  }
  return aResult;
}

// =======================================================================
// function : ConsiderZoomPersistenceObjects
// purpose  :
// =======================================================================
Standard_Real Graphic3d_CView::ConsiderZoomPersistenceObjects()
{
  if (!IsDefined())
  {
    return 1.0;
  }

  const Handle(Graphic3d_Camera)& aCamera = Camera();
  Graphic3d_Vec2i aWinSize;
  Window()->Size (aWinSize.x(), aWinSize.y());

  Standard_Real aMaxCoef = 1.0;
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter (Layers()); aLayerIter.More(); aLayerIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayer = aLayerIter.Value();
    aMaxCoef = Max (aMaxCoef, aLayer->considerZoomPersistenceObjects (Identification(), aCamera, aWinSize.x(), aWinSize.y()));
  }

  return aMaxCoef;
}

// =======================================================================
// function : MinMaxValues
// purpose  :
// =======================================================================
Bnd_Box Graphic3d_CView::MinMaxValues (const Graphic3d_MapOfStructure& theSet,
                                       const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Bnd_Box aResult;
  const Standard_Integer aViewId = Identification();

  Handle(Graphic3d_Camera) aCamera = Camera();
  Standard_Integer aWinWidth  = 0;
  Standard_Integer aWinHeight = 0;
  if (IsDefined())
  {
    Window()->Size (aWinWidth, aWinHeight);
  }

  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (theSet); aStructIter.More(); aStructIter.Next())
  {
    const Handle(Graphic3d_Structure)& aStructure = aStructIter.Key();
    if (aStructure->IsEmpty()
    || !aStructure->CStructure()->IsVisible (aViewId))
    {
      continue;
    }

    // "FitAll" operation ignores object with transform persistence parameter
    if (!aStructure->TransformPersistence().IsNull())
    {
      // Panning and 2d persistence apply changes to projection or/and its translation components.
      // It makes them incompatible with z-fitting algorithm. Ignored by now.
      if (!theToIgnoreInfiniteFlag
       || aStructure->TransformPersistence()->IsTrihedronOr2d())
      {
        continue;
      }
    }

    Bnd_Box aBox = aStructure->MinMaxValues (theToIgnoreInfiniteFlag);

    if (aBox.IsWhole() || aBox.IsVoid())
    {
      continue;
    }

    if (!aStructure->TransformPersistence().IsNull())
    {
      const Graphic3d_Mat4d& aProjectionMat = aCamera->ProjectionMatrix();
      const Graphic3d_Mat4d& aWorldViewMat  = aCamera->OrientationMatrix();
      aStructure->TransformPersistence()->Apply (aCamera, aProjectionMat, aWorldViewMat, aWinWidth, aWinHeight, aBox);
    }

    // To prevent float overflow at camera parameters calculation and further
    // rendering, bounding boxes with at least one vertex coordinate out of
    // float range are skipped by view fit algorithms
    if (Abs (aBox.CornerMax().X()) >= ShortRealLast() ||
        Abs (aBox.CornerMax().Y()) >= ShortRealLast() ||
        Abs (aBox.CornerMax().Z()) >= ShortRealLast() ||
        Abs (aBox.CornerMin().X()) >= ShortRealLast() ||
        Abs (aBox.CornerMin().Y()) >= ShortRealLast() ||
        Abs (aBox.CornerMin().Z()) >= ShortRealLast())
    {
      continue;
    }

    aResult.Add (aBox);
  }
  return aResult;
}

// =======================================================================
// function : acceptDisplay
// purpose  :
// =======================================================================
Graphic3d_TypeOfAnswer Graphic3d_CView::acceptDisplay (const Graphic3d_TypeOfStructure theStructType) const
{
  switch (theStructType)
  {
    case Graphic3d_TOS_ALL:
    {
      return Graphic3d_TOA_YES; // The structure accepts any type of view
    }
    case Graphic3d_TOS_SHADING:
    {
      return myVisualization == Graphic3d_TOV_SHADING
           ? Graphic3d_TOA_YES
           : Graphic3d_TOA_NO;
    }
    case Graphic3d_TOS_WIREFRAME:
    {
      return myVisualization == Graphic3d_TOV_WIREFRAME
           ? Graphic3d_TOA_YES
           : Graphic3d_TOA_NO;
    }
    case Graphic3d_TOS_COMPUTED:
    {
      return (myVisualization == Graphic3d_TOV_SHADING || myVisualization == Graphic3d_TOV_WIREFRAME)
           ?  Graphic3d_TOA_COMPUTE
           :  Graphic3d_TOA_NO;
    }
  }
  return Graphic3d_TOA_NO;
}

// =======================================================================
// function : Compute
// purpose  :
// =======================================================================
void Graphic3d_CView::Compute()
{
  // force HLRValidation to False on all structures calculated in the view
  for (Graphic3d_SequenceOfStructure::Iterator aStructIter (myStructsComputed); aStructIter.More(); aStructIter.Next())
  {
    aStructIter.Value()->SetHLRValidation (Standard_False);
  }

  if (!ComputedMode())
  {
    return;
  }

  // Change of orientation or of projection type =>
  // Remove structures that were calculated for the previous orientation.
  // Recalculation of new structures.
  NCollection_Sequence<Handle(Graphic3d_Structure)> aStructsSeq;
  for (Graphic3d_MapOfStructure::Iterator aStructIter (myStructsDisplayed); aStructIter.More(); aStructIter.Next())
  {
    const Graphic3d_TypeOfAnswer anAnswer = acceptDisplay (aStructIter.Key()->Visual());
    if (anAnswer == Graphic3d_TOA_COMPUTE)
    {
      aStructsSeq.Append (aStructIter.Key()); // if the structure was calculated, it is recalculated
    }
  }

  for (NCollection_Sequence<Handle(Graphic3d_Structure)>::Iterator aStructIter (aStructsSeq); aStructIter.More(); aStructIter.Next())
  {
    Display (aStructIter.ChangeValue());
  }
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Graphic3d_CView::Clear (Graphic3d_Structure* theStructure,
                             const Standard_Boolean theWithDestruction)
{
  const Standard_Integer anIndex = IsComputed (theStructure);
  if (anIndex != 0)
  {
    const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.Value (anIndex);
    aCompStruct->GraphicClear (theWithDestruction);
    aCompStruct->SetHLRValidation (Standard_False);
  }
}

// =======================================================================
// function : Connect
// purpose  :
// =======================================================================
void Graphic3d_CView::Connect (const Graphic3d_Structure* theMother,
                               const Graphic3d_Structure* theDaughter)
{
  Standard_Integer anIndexM = IsComputed (theMother);
  Standard_Integer anIndexD = IsComputed (theDaughter);
  if (anIndexM != 0
   && anIndexD != 0)
  {
    const Handle(Graphic3d_Structure)& aStructM = myStructsComputed.Value (anIndexM);
    const Handle(Graphic3d_Structure)& aStructD = myStructsComputed.Value (anIndexD);
    aStructM->GraphicConnect (aStructD);
  }
}

// =======================================================================
// function : Disconnect
// purpose  :
// =======================================================================
void Graphic3d_CView::Disconnect (const Graphic3d_Structure* theMother,
                                  const Graphic3d_Structure* theDaughter)
{
  Standard_Integer anIndexM = IsComputed (theMother);
  Standard_Integer anIndexD = IsComputed (theDaughter);
  if (anIndexM != 0
   && anIndexD != 0)
  {
    const Handle(Graphic3d_Structure)& aStructM = myStructsComputed.Value (anIndexM);
    const Handle(Graphic3d_Structure)& aStructD = myStructsComputed.Value (anIndexD);
    aStructM->GraphicDisconnect (aStructD);
  }
}

// =======================================================================
// function : Display
// purpose  :
// =======================================================================
void Graphic3d_CView::Display (const Handle(Graphic3d_Structure)& theStructure)
{
  if (!IsActive())
  {
    return;
  }

  // If Display on a structure present in the list of calculated structures while it is not
  // or more, of calculated type =>
  // - removes it as well as the associated old computed
  // THis happens when hlhsr becomes again of type e non computed after SetVisual.
  Standard_Integer anIndex = IsComputed (theStructure);
  if (anIndex != 0
   && theStructure->Visual() != Graphic3d_TOS_COMPUTED)
  {
    myStructsToCompute.Remove (anIndex);
    myStructsComputed .Remove (anIndex);
    anIndex = 0;
  }

  Graphic3d_TypeOfAnswer anAnswer = acceptDisplay (theStructure->Visual());
  if (anAnswer == Graphic3d_TOA_NO)
  {
    return;
  }

  if (!ComputedMode())
  {
    anAnswer = Graphic3d_TOA_YES;
  }

  if (anAnswer == Graphic3d_TOA_YES)
  {
    if (!myStructsDisplayed.Add (theStructure))
    {
      return;
    }

    theStructure->CalculateBoundBox();
    displayStructure (theStructure->CStructure(), theStructure->DisplayPriority());
    Update (theStructure->GetZLayer());
    return;
  }
  else if (anAnswer != Graphic3d_TOA_COMPUTE)
  {
    return;
  }

  if (anIndex != 0)
  {
    // Already computed, is COMPUTED still valid?
    const Handle(Graphic3d_Structure)& anOldStruct = myStructsComputed.Value (anIndex);
    if (anOldStruct->HLRValidation())
    {
      // Case COMPUTED valid, to be displayed
      if (!myStructsDisplayed.Add (theStructure))
      {
        return;
      }

      displayStructure (anOldStruct->CStructure(), theStructure->DisplayPriority());
      Update (anOldStruct->GetZLayer());
      return;
    }
    else
    {
      // Case COMPUTED invalid
      // Is there another valid representation?
      // Find in the sequence of already calculated structures
      // 1/ Structure having the same Owner as <AStructure>
      // 2/ That is not <AStructure>
      // 3/ The COMPUTED which of is valid
      const Standard_Integer aNewIndex = HaveTheSameOwner (theStructure);
      if (aNewIndex != 0)
      {
        // Case of COMPUTED invalid, WITH a valid of replacement; to be displayed
        if (!myStructsDisplayed.Add (theStructure))
        {
          return;
        }

        const Handle(Graphic3d_Structure)& aNewStruct = myStructsComputed.Value (aNewIndex);
        myStructsComputed.SetValue (anIndex, aNewStruct);
        displayStructure (aNewStruct->CStructure(), theStructure->DisplayPriority());
        Update (aNewStruct->GetZLayer());
        return;
      }
      else
      {
        // Case COMPUTED invalid, WITHOUT a valid of replacement
        // COMPUTED is removed if displayed
        if (myStructsDisplayed.Contains (theStructure))
        {
          eraseStructure (anOldStruct->CStructure());
        }
      }
    }
  }

  // Compute + Validation
  Handle(Graphic3d_Structure) aStruct;
  if (anIndex != 0)
  {
    aStruct = myStructsComputed.Value (anIndex);
    aStruct->SetTransformation (Handle(TopLoc_Datum3D)());
  }
  theStructure->computeHLR (myCamera, aStruct);
  if (aStruct.IsNull())
  {
    return;
  }
  aStruct->SetHLRValidation (Standard_True);

  // TOCOMPUTE and COMPUTED associated to sequences are added
  myStructsToCompute.Append (theStructure);
  myStructsComputed .Append (aStruct);

  // The previous are removed if necessary
  if (anIndex != 0)
  {
    myStructsToCompute.Remove (anIndex);
    myStructsComputed .Remove (anIndex);
  }

  // Of which type will be the computed?
  const Standard_Boolean toComputeWireframe = myVisualization == Graphic3d_TOV_WIREFRAME
                                           && theStructure->ComputeVisual() != Graphic3d_TOS_SHADING;
  const Standard_Boolean toComputeShading   = myVisualization == Graphic3d_TOV_SHADING
                                           && theStructure->ComputeVisual() != Graphic3d_TOS_WIREFRAME;
  if (!toComputeShading && !toComputeWireframe)
  {
    anAnswer = Graphic3d_TOA_NO;
  }
  else
  {
    aStruct->SetVisual (toComputeWireframe ? Graphic3d_TOS_WIREFRAME : Graphic3d_TOS_SHADING);
    anAnswer = acceptDisplay (aStruct->Visual());
  }

  if (theStructure->IsHighlighted())
  {
    aStruct->Highlight (theStructure->HighlightStyle(), Standard_False);
  }

  // It is displayed only if the calculated structure
  // has a proper type corresponding to the one of the view.
  if (anAnswer == Graphic3d_TOA_NO)
  {
    return;
  }

  myStructsDisplayed.Add (theStructure);
  displayStructure (aStruct->CStructure(), theStructure->DisplayPriority());

  Update (aStruct->GetZLayer());
}

// =======================================================================
// function : Erase
// purpose  :
// =======================================================================
void Graphic3d_CView::Erase (const Handle(Graphic3d_Structure)& theStructure)
{
  if (!IsDisplayed (theStructure))
  {
    return;
  }

  const Graphic3d_TypeOfAnswer anAnswer = myIsInComputedMode ? acceptDisplay (theStructure->Visual()) : Graphic3d_TOA_YES;
  if (anAnswer != Graphic3d_TOA_COMPUTE)
  {
    eraseStructure (theStructure->CStructure());
  }

  const Standard_Integer anIndex = !myStructsToCompute.IsEmpty() ? IsComputed (theStructure) : 0;
  if (anIndex != 0)
  {
    if (anAnswer == Graphic3d_TOA_COMPUTE
     && myIsInComputedMode)
    {
      const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
      eraseStructure (aCompStruct->CStructure());
    }
    myStructsComputed .Remove (anIndex);
    myStructsToCompute.Remove (anIndex);
  }

  myStructsDisplayed.Remove (theStructure);
  Update (theStructure->GetZLayer());
}

// =======================================================================
// function : Highlight
// purpose  :
// =======================================================================
void Graphic3d_CView::Highlight (const Handle(Graphic3d_Structure)& theStructure)
{
  const Standard_Integer anIndex = IsComputed (theStructure);
  if (anIndex != 0)
  {
    const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
    aCompStruct->Highlight (theStructure->HighlightStyle(), Standard_False);
  }
}

// =======================================================================
// function : SetTransform
// purpose  :
// =======================================================================
void Graphic3d_CView::SetTransform (const Handle(Graphic3d_Structure)& theStructure,
                                    const Handle(TopLoc_Datum3D)& theTrsf)
{
  const Standard_Integer anIndex = IsComputed (theStructure);
  if (anIndex != 0)
  {
    // Test is somewhat light !
    // trsf is transferred only if it is :
    // a translation
    // a scale
    if (!theTrsf.IsNull()
      && (theTrsf->Form() == gp_Translation
       || theTrsf->Form() == gp_Scale
       || theTrsf->Form() == gp_CompoundTrsf))
    {
      ReCompute (theStructure);
    }
    else
    {
      const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
      aCompStruct->GraphicTransform (theTrsf);
    }
  }

  theStructure->CalculateBoundBox();
  if (!theStructure->IsMutable()
   && !theStructure->CStructure()->IsForHighlight
   && !theStructure->CStructure()->IsInfinite)
  {
    const Graphic3d_ZLayerId aLayerId = theStructure->GetZLayer();
    InvalidateBVHData (aLayerId);
  }
}

// =======================================================================
// function : UnHighlight
// purpose  :
// =======================================================================
void Graphic3d_CView::UnHighlight (const Handle(Graphic3d_Structure)& theStructure)
{
  Standard_Integer anIndex = IsComputed (theStructure);
  if (anIndex != 0)
  {
    const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
    aCompStruct->CStructure()->GraphicUnhighlight();
  }
}

// ========================================================================
// function : IsComputed
// purpose  :
// ========================================================================
Standard_Boolean Graphic3d_CView::IsComputed (const Standard_Integer theStructId,
                                              Handle(Graphic3d_Structure)& theComputedStruct) const
{
  theComputedStruct.Nullify();
  if (!ComputedMode())
    return Standard_False;

  const Standard_Integer aNbStructs = myStructsToCompute.Length();
  for (Standard_Integer aStructIter = 1; aStructIter <= aNbStructs; ++aStructIter)
  {
    if (myStructsToCompute.Value (aStructIter)->Identification() == theStructId)
    {
      theComputedStruct = myStructsComputed (aStructIter);
      return Standard_True;
    }
  }
  return Standard_False;
}

// =======================================================================
// function : IsComputed
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_CView::IsComputed (const Graphic3d_Structure* theStructure) const
{
  const Standard_Integer aStructId  = theStructure->Identification();
  Standard_Integer aStructIndex = 1;
  for (Graphic3d_SequenceOfStructure::Iterator aStructIter (myStructsToCompute); aStructIter.More(); aStructIter.Next(), ++aStructIndex)
  {
    const Handle(Graphic3d_Structure)& aStruct = aStructIter.Value();
    if (aStruct->Identification() == aStructId)
    {
      return aStructIndex;
    }
  }
  return 0;
}

// =======================================================================
// function : IsDisplayed
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_CView::IsDisplayed (const Handle(Graphic3d_Structure)& theStructure) const
{
  return myStructsDisplayed.Contains (theStructure);
}

// =======================================================================
// function : ChangePriority
// purpose  :
// =======================================================================
void Graphic3d_CView::ChangePriority (const Handle(Graphic3d_Structure)& theStructure,
                                      const Graphic3d_DisplayPriority /*theOldPriority*/,
                                      const Graphic3d_DisplayPriority theNewPriority)
{
  if (!IsActive()
  ||  !IsDisplayed (theStructure))
  {
    return;
  }

  if (!myIsInComputedMode)
  {
    changePriority (theStructure->CStructure(), theNewPriority);
    return;
  }

  const Standard_Integer              anIndex  = IsComputed (theStructure);
  const Handle(Graphic3d_CStructure)& aCStruct = anIndex != 0
                                               ? myStructsComputed.Value (anIndex)->CStructure()
                                               : theStructure->CStructure();

  changePriority (aCStruct, theNewPriority);
}

// =======================================================================
// function : ChangeZLayer
// purpose  :
// =======================================================================
void Graphic3d_CView::ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure,
                                    const Graphic3d_ZLayerId theLayerId)
{
  if (!IsActive()
  ||  !IsDisplayed (theStructure))
  {
    return;
  }

  if (!myIsInComputedMode)
  {
    changeZLayer (theStructure->CStructure(), theLayerId);
    return;
  }

  const Standard_Integer       anIndex  = IsComputed (theStructure);
  Handle(Graphic3d_CStructure) aCStruct = anIndex != 0
                                       ? myStructsComputed.Value (anIndex)->CStructure()
                                       : theStructure->CStructure();

  changeZLayer (aCStruct, theLayerId);
}

// =======================================================================
// function : HaveTheSameOwner
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_CView::HaveTheSameOwner (const Handle(Graphic3d_Structure)& theStructure) const
{
  // Find in the sequence of already calculated structures
  // 1/ Structure with the same Owner as <AStructure>
  // 2/ Which is not <AStructure>
  // 3/ COMPUTED which of is valid
  const Standard_Integer aNbToCompStructs = myStructsToCompute.Length();
  for (Standard_Integer aStructIter = 1; aStructIter <= aNbToCompStructs; ++aStructIter)
  {
    const Handle(Graphic3d_Structure)& aStructToComp = myStructsToCompute.Value (aStructIter);
    if (aStructToComp->Owner()          == theStructure->Owner()
     && aStructToComp->Identification() != theStructure->Identification())
    {
      const Handle(Graphic3d_Structure)& aStructComp = myStructsComputed.Value (aStructIter);
      if (aStructComp->HLRValidation())
      {
        return aStructIter;
      }
    }
  }
  return 0;
}

// =======================================================================
// function : CopySettings
// purpose  :
// =======================================================================
void Graphic3d_CView::CopySettings (const Handle(Graphic3d_CView)& theOther)
{
  ChangeRenderingParams() = theOther->RenderingParams();
  SetBackground            (theOther->Background());
  SetGradientBackground    (theOther->GradientBackground());
  SetBackgroundImage       (theOther->BackgroundImage());
  SetBackgroundImageStyle  (theOther->BackgroundImageStyle());
  SetTextureEnv            (theOther->TextureEnv());
  SetShadingModel          (theOther->ShadingModel());
  SetBackfacingModel       (theOther->BackfacingModel());
  SetCamera                (new Graphic3d_Camera (theOther->Camera()));
  SetLights                (theOther->Lights());
  SetClipPlanes            (theOther->ClipPlanes());
}

// =======================================================================
// function : SetShadingModel
// purpose  :
// =======================================================================
void Graphic3d_CView::SetShadingModel (Graphic3d_TypeOfShadingModel theModel)
{
  if (theModel == Graphic3d_TypeOfShadingModel_DEFAULT)
  {
    throw Standard_ProgramError ("Graphic3d_CView::SetShadingModel() - attempt to set invalid Shading Model!");
  }

  myRenderParams.ShadingModel = theModel;
}

// =======================================================================
// function : SetUnitFactor
// purpose  :
// =======================================================================
void Graphic3d_CView::SetUnitFactor (Standard_Real theFactor)
{
  if (theFactor <= 0.0)
  {
    throw Standard_ProgramError ("Graphic3d_CView::SetUnitFactor() - invalid unit factor");
  }
  myUnitFactor = theFactor;
  if (!myXRSession.IsNull())
  {
    myXRSession->SetUnitFactor (theFactor);
  }
}

// =======================================================================
// function : IsActiveXR
// purpose  :
// =======================================================================
bool Graphic3d_CView::IsActiveXR() const
{
  return !myXRSession.IsNull()
       && myXRSession->IsOpen();
}

// =======================================================================
// function : InitXR
// purpose  :
// =======================================================================
bool Graphic3d_CView::InitXR()
{
  if (myXRSession.IsNull())
  {
    myXRSession = new Aspect_OpenVRSession();
    myXRSession->SetUnitFactor (myUnitFactor);
  }
  if (!myXRSession->IsOpen())
  {
    myXRSession->Open();
    if (myBackXRCamera.IsNull())
    {
      // backup camera properties
      myBackXRCamera = new Graphic3d_Camera (myCamera);
    }
  }
  return myXRSession->IsOpen();
}

// =======================================================================
// function : ReleaseXR
// purpose  :
// =======================================================================
void Graphic3d_CView::ReleaseXR()
{
  if (!myXRSession.IsNull())
  {
    if (myXRSession->IsOpen()
    && !myBackXRCamera.IsNull())
    {
      // restore projection properties overridden by HMD
      myCamera->SetFOV2d (myBackXRCamera->FOV2d());
      myCamera->SetFOVy  (myBackXRCamera->FOVy());
      myCamera->SetAspect(myBackXRCamera->Aspect());
      myCamera->SetIOD   (myBackXRCamera->GetIODType(), myBackXRCamera->IOD());
      myCamera->SetZFocus(myBackXRCamera->ZFocusType(), myBackXRCamera->ZFocus());
      myCamera->ResetCustomProjection();
      myBackXRCamera.Nullify();
    }
    myXRSession->Close();
  }
}

//=======================================================================
//function : ProcessXRInput
//purpose  :
//=======================================================================
void Graphic3d_CView::ProcessXRInput()
{
  if (myRenderParams.StereoMode == Graphic3d_StereoMode_OpenVR
   && myCamera->ProjectionType() == Graphic3d_Camera::Projection_Stereo)
  {
    InitXR();
  }
  else
  {
    ReleaseXR();
  }

  if (!IsActiveXR())
  {
    myBaseXRCamera.Nullify();
    myPosedXRCamera.Nullify();
    return;
  }

  myXRSession->ProcessEvents();
  Invalidate();

  myCamera->SetFOV2d (myRenderParams.HmdFov2d);
  myCamera->SetAspect(myXRSession->Aspect());
  myCamera->SetFOVy  (myXRSession->FieldOfView());
  myCamera->SetIOD   (Graphic3d_Camera::IODType_Absolute, myXRSession->IOD());
  myCamera->SetZFocus(Graphic3d_Camera::FocusType_Absolute, 1.0 * myUnitFactor);

  // VR APIs tend to decompose camera orientation-projection matrices into the following components:
  // @begincode
  //   Model * [View * Eye^-1] * [Projection]
  // @endcode
  // so that Eye position is encoded into Orientation matrix, and there should be 2 Orientation matrices and 2 Projection matrices to make the stereo.
  // Graphic3d_Camera historically follows different decomposition, with Eye position encoded into Projection matrix,
  // so that there is only 1 Orientation matrix (matching mono view) and 2 Projection matrices.
  if (myXRSession->HasProjectionFrustums())
  {
    // note that this definition does not include a small forward/backward offset from head to eye
    myCamera->SetCustomStereoFrustums (myXRSession->ProjectionFrustum (Aspect_Eye_Left),
                                       myXRSession->ProjectionFrustum (Aspect_Eye_Right));
  }
  else
  {
    const Graphic3d_Mat4d aPoseL = myXRSession->HeadToEyeTransform (Aspect_Eye_Left);
    const Graphic3d_Mat4d aPoseR = myXRSession->HeadToEyeTransform (Aspect_Eye_Right);
    const Graphic3d_Mat4d aProjL = myXRSession->ProjectionMatrix (Aspect_Eye_Left,  myCamera->ZNear(), myCamera->ZFar());
    const Graphic3d_Mat4d aProjR = myXRSession->ProjectionMatrix (Aspect_Eye_Right, myCamera->ZNear(), myCamera->ZFar());
    myCamera->SetCustomStereoProjection (aProjL, aPoseL, aProjR, aPoseR);
  }
  myBaseXRCamera = myCamera;
  if (myPosedXRCamera.IsNull())
  {
    myPosedXRCamera = new Graphic3d_Camera();
  }
  SynchronizeXRBaseToPosedCamera();
}

//=======================================================================
//function : SynchronizeXRBaseToPosedCamera
//purpose  :
//=======================================================================
void Graphic3d_CView::SynchronizeXRBaseToPosedCamera()
{
  if (!myPosedXRCamera.IsNull())
  {
    ComputeXRPosedCameraFromBase (*myPosedXRCamera, myXRSession->HeadPose());
  }
}

//=======================================================================
//function : ComputeXRPosedCameraFromBase
//purpose  :
//=======================================================================
void Graphic3d_CView::ComputeXRPosedCameraFromBase (Graphic3d_Camera& theCam,
                                                    const gp_Trsf& theXRTrsf) const
{
  theCam.Copy (myBaseXRCamera);

  // convert head pose into camera transformation
  const gp_Ax3 anAxVr    (gp::Origin(),  gp::DZ(), gp::DX());
  const gp_Ax3 aCameraCS (gp::Origin(), -myBaseXRCamera->Direction(), -myBaseXRCamera->SideRight());
  gp_Trsf aTrsfCS;
  aTrsfCS.SetTransformation (aCameraCS, anAxVr);
  const gp_Trsf aTrsfToCamera = aTrsfCS * theXRTrsf * aTrsfCS.Inverted();
  gp_Trsf aTrsfToEye;
  aTrsfToEye.SetTranslation (myBaseXRCamera->Eye().XYZ());

  const gp_Trsf aTrsf = aTrsfToEye * aTrsfToCamera;
  const gp_Dir anUpNew  = myBaseXRCamera->Up().Transformed (aTrsf);
  const gp_Dir aDirNew  = myBaseXRCamera->Direction().Transformed (aTrsf);
  const gp_Pnt anEyeNew = gp::Origin().Translated (aTrsf.TranslationPart());
  theCam.SetUp (anUpNew);
  theCam.SetDirectionFromEye (aDirNew);
  theCam.MoveEyeTo (anEyeNew);
}

//=======================================================================
//function : SynchronizeXRPosedToBaseCamera
//purpose  :
//=======================================================================
void Graphic3d_CView::SynchronizeXRPosedToBaseCamera()
{
  if (myPosedXRCameraCopy.IsNull()
   || myPosedXRCamera.IsNull()
   || myBaseXRCamera.IsNull()
   || myCamera != myPosedXRCamera)
  {
    return;
  }

  if (myPosedXRCameraCopy->Eye().IsEqual (myPosedXRCamera->Eye(), gp::Resolution())
   && (myPosedXRCameraCopy->Distance() - myPosedXRCamera->Distance()) <= gp::Resolution()
   && myPosedXRCameraCopy->Direction().IsEqual (myPosedXRCamera->Direction(), gp::Resolution())
   && myPosedXRCameraCopy->Up().IsEqual (myPosedXRCamera->Up(), gp::Resolution()))
  {
    // avoid floating point math in case of no changes
    return;
  }

  // re-compute myBaseXRCamera from myPosedXRCamera by applying reversed head pose transformation
  ComputeXRBaseCameraFromPosed (myPosedXRCamera, myXRSession->HeadPose());
  myPosedXRCameraCopy->Copy (myPosedXRCamera);
}

//=======================================================================
//function : ComputeXRBaseCameraFromPosed
//purpose  :
//=======================================================================
void Graphic3d_CView::ComputeXRBaseCameraFromPosed (const Graphic3d_Camera& theCamPosed,
                                                    const gp_Trsf& thePoseTrsf)
{
  const gp_Ax3 anAxVr    (gp::Origin(),  gp::DZ(), gp::DX());
  const gp_Ax3 aCameraCS (gp::Origin(), -myBaseXRCamera->Direction(), -myBaseXRCamera->SideRight());
  gp_Trsf aTrsfCS;
  aTrsfCS.SetTransformation (aCameraCS, anAxVr);
  const gp_Trsf aTrsfToCamera  = aTrsfCS * thePoseTrsf * aTrsfCS.Inverted();
  const gp_Trsf aTrsfCamToHead = aTrsfToCamera.Inverted();
  const gp_Dir anUpNew  = theCamPosed.Up().Transformed (aTrsfCamToHead);
  const gp_Dir aDirNew  = theCamPosed.Direction().Transformed (aTrsfCamToHead);
  const gp_Pnt anEyeNew = theCamPosed.Eye().Translated (aTrsfToCamera.TranslationPart().Reversed());
  myBaseXRCamera->SetUp (anUpNew);
  myBaseXRCamera->SetDirectionFromEye (aDirNew);
  myBaseXRCamera->MoveEyeTo (anEyeNew);
}

//=======================================================================
//function : TurnViewXRCamera
//purpose  :
//=======================================================================
void Graphic3d_CView::TurnViewXRCamera (const gp_Trsf& theTrsfTurn)
{
  // use current eye position as an anchor
  const Handle(Graphic3d_Camera)& aCamBase = myBaseXRCamera;
  gp_Trsf aHeadTrsfLocal;
  aHeadTrsfLocal.SetTranslationPart (myXRSession->HeadPose().TranslationPart());
  const gp_Pnt anEyeAnchor = PoseXRToWorld (aHeadTrsfLocal).TranslationPart();

  // turn the view
  aCamBase->SetDirectionFromEye (aCamBase->Direction().Transformed (theTrsfTurn));

  // recompute new eye
  const gp_Ax3 anAxVr    (gp::Origin(),  gp::DZ(), gp::DX());
  const gp_Ax3 aCameraCS (gp::Origin(), -aCamBase->Direction(), -aCamBase->SideRight());
  gp_Trsf aTrsfCS;
  aTrsfCS.SetTransformation (aCameraCS, anAxVr);
  const gp_Trsf aTrsfToCamera = aTrsfCS * aHeadTrsfLocal * aTrsfCS.Inverted();
  const gp_Pnt anEyeNew = anEyeAnchor.Translated (aTrsfToCamera.TranslationPart().Reversed());
  aCamBase->MoveEyeTo (anEyeNew);

  SynchronizeXRBaseToPosedCamera();
}

//=======================================================================
//function : SetupXRPosedCamera
//purpose  :
//=======================================================================
void Graphic3d_CView::SetupXRPosedCamera()
{
  if (!myPosedXRCamera.IsNull())
  {
    myCamera = myPosedXRCamera;
    if (myPosedXRCameraCopy.IsNull())
    {
      myPosedXRCameraCopy = new Graphic3d_Camera();
    }
    myPosedXRCameraCopy->Copy (myPosedXRCamera);
  }
}

//=======================================================================
//function : UnsetXRPosedCamera
//purpose  :
//=======================================================================
void Graphic3d_CView::UnsetXRPosedCamera()
{
  if (myCamera == myPosedXRCamera
  && !myBaseXRCamera.IsNull())
  {
    SynchronizeXRPosedToBaseCamera();
    myCamera = myBaseXRCamera;
  }
}

//=======================================================================
//function : DiagnosticInformation
//purpose  :
//=======================================================================
void Graphic3d_CView::DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                             Graphic3d_DiagnosticInfo theFlags) const
{
  if ((theFlags & Graphic3d_DiagnosticInfo_Device) != 0
   && !myXRSession.IsNull())
  {
    TCollection_AsciiString aVendor  = myXRSession->GetString (Aspect_XRSession::InfoString_Vendor);
    TCollection_AsciiString aDevice  = myXRSession->GetString (Aspect_XRSession::InfoString_Device);
    TCollection_AsciiString aTracker = myXRSession->GetString (Aspect_XRSession::InfoString_Tracker);
    TCollection_AsciiString aSerial  = myXRSession->GetString (Aspect_XRSession::InfoString_SerialNumber);
    TCollection_AsciiString aDisplay = TCollection_AsciiString()
                                     + myXRSession->RecommendedViewport().x() + "x" + myXRSession->RecommendedViewport().y()
                                     + "@" + (int )Round (myXRSession->DisplayFrequency())
                                     + " [FOVy: " + (int )Round (myXRSession->FieldOfView()) + "]";

    theDict.ChangeFromIndex (theDict.Add ("VRvendor",  aVendor))  = aVendor;
    theDict.ChangeFromIndex (theDict.Add ("VRdevice",  aDevice))  = aDevice;
    theDict.ChangeFromIndex (theDict.Add ("VRtracker", aTracker)) = aTracker;
    theDict.ChangeFromIndex (theDict.Add ("VRdisplay", aDisplay)) = aDisplay;
    theDict.ChangeFromIndex (theDict.Add ("VRserial",  aSerial))  = aSerial;
  }
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Graphic3d_CView::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, Graphic3d_DataStructureManager);

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myId)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myRenderParams)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myBgColor)
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myStructureManager)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myCamera.get())

  for (Graphic3d_SequenceOfStructure::Iterator anIter (myStructsToCompute); anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_Structure)& aStructToCompute = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aStructToCompute.get())
  }

  for (Graphic3d_SequenceOfStructure::Iterator anIter (myStructsComputed); anIter.More(); anIter.Next())
  {
    const Handle(Graphic3d_Structure)& aStructComputed = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aStructComputed.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsInComputedMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsActive)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsRemoved)
  
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myVisualization)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myBackXRCamera.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myBaseXRCamera.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myPosedXRCamera.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myPosedXRCameraCopy.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myUnitFactor)
}
