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
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_StructureManager.hxx>

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
Graphic3d_CView::Graphic3d_CView (const Handle(Graphic3d_StructureManager)& theMgr)
: myStructureManager       (theMgr),
  myHiddenObjects          (new Graphic3d_NMapOfTransient()),
  myIsInComputedMode       (Standard_False),
  myIsActive               (Standard_False),
  myIsRemoved              (Standard_False),
  myVisualization          (Graphic3d_TOV_WIREFRAME)
{
  myId = myStructureManager->Identification (this);
}

//=======================================================================
//function : Destructor
//purpose  :
//=======================================================================
Graphic3d_CView::~Graphic3d_CView()
{
  if (!IsRemoved())
  {
    myStructureManager->UnIdentification (this);
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
        Display (aStruct, Aspect_TOU_WAIT);
      }
    }
  }

  Update (myStructureManager->UpdateMode());
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
        Erase (aStruct, Aspect_TOU_WAIT);
      }
    }

    Update (myStructureManager->UpdateMode());
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

  Graphic3d_MapOfStructure aDisplayedStructs (myStructsDisplayed);

  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (aDisplayedStructs); aStructIter.More(); aStructIter.Next())
  {
    Erase (aStructIter.Value(), Aspect_TOU_WAIT);
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
      }
    }
    return;
  }

  for (Graphic3d_MapOfStructure::Iterator aDispStructIter (myStructsDisplayed); aDispStructIter.More(); aDispStructIter.Next())
  {
    Handle(Graphic3d_Structure) aStruct  = aDispStructIter.Key();
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

      Display (aStruct, Aspect_TOU_WAIT);
      if (aStruct->IsHighlighted())
      {
        const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.Value (anIndex);
        if (!aCompStruct->IsHighlighted())
        {
          aCompStruct->Highlight (Aspect_TOHM_COLOR, aStruct->HighlightColor(), Standard_False);
        }
      }
    }
    else
    {
      TColStd_Array2OfReal aTrsf (0, 3, 0, 3);
      aStruct->Transform (aTrsf);
      Handle(Graphic3d_Structure) aCompStruct = aStruct->IsTransformed() ? aStruct->Compute (this, aTrsf) : aStruct->Compute (this);
      aCompStruct->SetHLRValidation (Standard_True);

      const Standard_Boolean toComputeWireframe = myVisualization == Graphic3d_TOV_WIREFRAME
                                                && aStruct->ComputeVisual() != Graphic3d_TOS_SHADING;
      const Standard_Boolean toComputeShading   = myVisualization == Graphic3d_TOV_SHADING
                                                && aStruct->ComputeVisual() != Graphic3d_TOS_WIREFRAME;
      if (toComputeWireframe) aCompStruct->SetVisual (Graphic3d_TOS_WIREFRAME);
      if (toComputeShading  ) aCompStruct->SetVisual (Graphic3d_TOS_SHADING);

      if (aStruct->IsHighlighted())
      {
        aCompStruct->Highlight (Aspect_TOHM_COLOR, aStruct->HighlightColor(), Standard_False);
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

      eraseStructure   (aStruct->CStructure());
      displayStructure (aCompStruct->CStructure(), aStruct->DisplayPriority());
    }
  }
  Update (myStructureManager->UpdateMode());
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
    const Standard_Integer aLayerId = theStruct->DisplayPriority();
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
  TColStd_Array2OfReal anIdent (0, 3, 0, 3);
  for (Standard_Integer aRow = 0; aRow <= 3; ++aRow)
  {
    for (Standard_Integer aCol = 0; aCol <= 3; ++aCol)
    {
      anIdent (aRow, aCol) = (aRow == aCol ? 1.0 : 0.0);
    }
  }
  TColStd_Array2OfReal aTrsf (0, 3, 0, 3);
  theStruct->Transform (aTrsf);

  Handle(Graphic3d_Structure) aCompStructOld = myStructsComputed.ChangeValue (anIndex);
  Handle(Graphic3d_Structure) aCompStruct    = aCompStructOld;
  aCompStruct->SetTransform (anIdent, Graphic3d_TOC_REPLACE);
  theStruct->IsTransformed() ? theStruct->Compute (this, aTrsf, aCompStruct)
                             : theStruct->Compute (this,        aCompStruct);
  aCompStruct->SetHLRValidation (Standard_True);

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
    aCompStruct->Highlight (Aspect_TOHM_COLOR, theStruct->HighlightColor(), Standard_False);
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
void Graphic3d_CView::Update (const Aspect_TypeOfUpdate theUpdateMode)
{
  myMinMax.Invalidate();
  if (theUpdateMode == Aspect_TOU_ASAP)
  {
    Compute();
    Redraw();
  }
}

// =======================================================================
// function : ContainsFacet
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_CView::ContainsFacet() const
{
  for (Graphic3d_MapOfStructure::Iterator aStructIter (myStructsDisplayed); aStructIter.More(); aStructIter.Next())
  {
    if (aStructIter.Key()->ContainsFacet())
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

// =======================================================================
// function : ContainsFacet
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_CView::ContainsFacet (const Graphic3d_MapOfStructure& theSet) const
{
  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (theSet); aStructIter.More(); aStructIter.Next())
  {
    if (aStructIter.Key()->ContainsFacet())
    {
      return Standard_True;
    }
  }
  return Standard_False;
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

//! Auxiliary method for MinMaxValues() method
inline void addStructureBndBox (const Handle(Graphic3d_Structure)& theStruct,
                                const Standard_Boolean             theToIgnoreInfiniteFlag,
                                Bnd_Box&                           theBndBox)
{
  if (!theStruct->IsVisible())
  {
    return;
  }
  else if (theStruct->IsInfinite()
       && !theToIgnoreInfiniteFlag)
  {
    // XMin, YMin .... ZMax are initialized by means of infinite line data
    const Bnd_Box aBox = theStruct->MinMaxValues (Standard_False);
    if (!aBox.IsWhole()
     && !aBox.IsVoid())
    {
      theBndBox.Add (aBox);
    }
    return;
  }

  // Only non-empty and non-infinite structures
  // are taken into account for calculation of MinMax
  if (theStruct->IsEmpty()
   || theStruct->TransformPersistenceMode() != Graphic3d_TMF_None)
  {
    return;
  }

  // "FitAll" operation ignores object with transform persistence parameter
  const Bnd_Box aBox = theStruct->MinMaxValues (theToIgnoreInfiniteFlag);

  // To prevent float overflow at camera parameters calculation and further
  // rendering, bounding boxes with at least one vertex coordinate out of
  // float range are skipped by view fit algorithms
  if (Abs (aBox.CornerMax().X()) >= ShortRealLast() ||
      Abs (aBox.CornerMax().Y()) >= ShortRealLast() ||
      Abs (aBox.CornerMax().Z()) >= ShortRealLast() ||
      Abs (aBox.CornerMin().X()) >= ShortRealLast() ||
      Abs (aBox.CornerMin().Y()) >= ShortRealLast() ||
      Abs (aBox.CornerMin().Z()) >= ShortRealLast())
    return;

  theBndBox.Add (aBox);
}

// =======================================================================
// function : MinMaxValues
// purpose  :
// =======================================================================
Bnd_Box Graphic3d_CView::MinMaxValues (const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  if (myMinMax.IsOutdated (theToIgnoreInfiniteFlag))
  {
    myMinMax.BoundingBox (theToIgnoreInfiniteFlag) = MinMaxValues (myStructsDisplayed, theToIgnoreInfiniteFlag);
    myMinMax.IsOutdated  (theToIgnoreInfiniteFlag) = Standard_False;
  }

  return myMinMax.BoundingBox (theToIgnoreInfiniteFlag);
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
  for (Graphic3d_MapIteratorOfMapOfStructure aStructIter (theSet); aStructIter.More(); aStructIter.Next())
  {
    const Handle(Graphic3d_Structure)& aStructure = aStructIter.Key();
    if (!aStructIter.Value()->IsVisible())
    {
      continue;
    }
    else if (!aStructIter.Value()->CStructure()->ViewAffinity.IsNull()
          && !aStructIter.Value()->CStructure()->ViewAffinity->IsVisible (aViewId))
    {
      continue;
    }

    addStructureBndBox (aStructure, theToIgnoreInfiniteFlag, aResult);
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
  const Standard_Integer aNbCompStructs = myStructsComputed.Length();
  for (Standard_Integer aStructIter = 1; aStructIter <= aNbCompStructs; ++aStructIter)
  {
    myStructsComputed.Value (aStructIter)->SetHLRValidation (Standard_False);
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
    Display (aStructIter.ChangeValue(), Aspect_TOU_WAIT);
  }
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Graphic3d_CView::Clear (const Handle(Graphic3d_Structure)& theStructure,
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
void Graphic3d_CView::Connect (const Handle(Graphic3d_Structure)& theMother,
                               const Handle(Graphic3d_Structure)& theDaughter)
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
void Graphic3d_CView::Disconnect (const Handle(Graphic3d_Structure)& theMother,
                                  const Handle(Graphic3d_Structure)& theDaughter)
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
  Display (theStructure, myStructureManager->UpdateMode());
}

// =======================================================================
// function : Display
// purpose  :
// =======================================================================
void Graphic3d_CView::Display (const Handle(Graphic3d_Structure)& theStructure,
                               const Aspect_TypeOfUpdate theUpdateMode)
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
    Update (theUpdateMode);
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
      Update (theUpdateMode);
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
        Update (theUpdateMode);
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
  TColStd_Array2OfReal aTrsf (0, 3, 0, 3);
  theStructure->Transform (aTrsf);
  if (anIndex != 0)
  {
    TColStd_Array2OfReal anIdent (0, 3, 0, 3);
    for (Standard_Integer ii = 0; ii <= 3; ++ii)
    {
      for (Standard_Integer jj = 0; jj <= 3; ++jj)
      {
        anIdent (ii, jj) = (ii == jj ? 1.0 : 0.0);
      }
    }

    aStruct = myStructsComputed.Value (anIndex);
    aStruct->SetTransform (anIdent, Graphic3d_TOC_REPLACE);
    if (theStructure->IsTransformed())
    {
      theStructure->Compute (this, aTrsf, aStruct);
    }
    else
    {
      theStructure->Compute (this, aStruct);
    }
  }
  else
  {
    aStruct = theStructure->IsTransformed()
            ? theStructure->Compute (this, aTrsf)
            : theStructure->Compute (this);
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
    aStruct->Highlight (Aspect_TOHM_COLOR, theStructure->HighlightColor(), Standard_False);
  }

  // It is displayed only if the calculated structure
  // has a proper type corresponding to the one of the view.
  if (anAnswer == Graphic3d_TOA_NO)
  {
    return;
  }

  myStructsDisplayed.Add (theStructure);
  displayStructure (aStruct->CStructure(), theStructure->DisplayPriority());

  Update (theUpdateMode);
}

// =======================================================================
// function : Erase
// purpose  :
// =======================================================================
void Graphic3d_CView::Erase (const Handle(Graphic3d_Structure)& theStructure)
{
  Erase (theStructure, myStructureManager->UpdateMode());
}

// =======================================================================
// function : Erase
// purpose  :
// =======================================================================
void Graphic3d_CView::Erase (const Handle(Graphic3d_Structure)& theStructure,
                             const Aspect_TypeOfUpdate theUpdateMode)
{
  if (!IsDisplayed (theStructure))
  {
    return;
  }

  Graphic3d_TypeOfAnswer anAnswer = acceptDisplay (theStructure->Visual());
  if (!ComputedMode())
  {
    anAnswer = Graphic3d_TOA_YES;
  }

  if (anAnswer != Graphic3d_TOA_COMPUTE)
  {
    eraseStructure (theStructure->CStructure());
  }
  else if (anAnswer == Graphic3d_TOA_COMPUTE && myIsInComputedMode)
  {
    const Standard_Integer anIndex = IsComputed (theStructure);
    if (anIndex != 0)
    {
      const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
      eraseStructure (aCompStruct->CStructure());
    }
  }
  myStructsDisplayed.Remove (theStructure);
  Update (theUpdateMode);
}

// =======================================================================
// function : Highlight
// purpose  :
// =======================================================================
void Graphic3d_CView::Highlight (const Handle(Graphic3d_Structure)& theStructure,
                                 const Aspect_TypeOfHighlightMethod theMethod)
{
  const Standard_Integer anIndex = IsComputed (theStructure);
  if (anIndex != 0)
  {
    const Handle(Graphic3d_Structure)& aCompStruct = myStructsComputed.ChangeValue (anIndex);
    aCompStruct->Highlight (theMethod, theStructure->HighlightColor(), Standard_False);
  }
}

// =======================================================================
// function : SetTransform
// purpose  :
// =======================================================================
void Graphic3d_CView::SetTransform (const Handle(Graphic3d_Structure)& theStructure,
                                    const TColStd_Array2OfReal& theTrsf)
{
  const Standard_Integer anIndex = IsComputed (theStructure);
  if (anIndex != 0)
  {
    // Test is somewhat light !
    // trsf is transferred only if it is :
    // a translation
    // a scale
    if (theTrsf (0, 1) != 0.0 || theTrsf (0, 2) != 0.0
     || theTrsf (1, 0) != 0.0 || theTrsf (1, 2) != 0.0
     || theTrsf (2, 0) != 0.0 || theTrsf (2, 1) != 0.0)
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
    aCompStruct->GraphicUnHighlight();
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
Standard_Integer Graphic3d_CView::IsComputed (const Handle(Graphic3d_Structure)& theStructure) const
{
  const Standard_Integer aStructId  = theStructure->Identification();
  const Standard_Integer aNbStructs = myStructsToCompute.Length();
  for (Standard_Integer aStructIter = 1; aStructIter <= aNbStructs; ++aStructIter)
  {
    const Handle(Graphic3d_Structure)& aStruct = myStructsToCompute.Value (aStructIter);
    if (aStruct->Identification() == aStructId)
    {
      return aStructIter;
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
                                      const Standard_Integer /*theOldPriority*/,
                                      const Standard_Integer theNewPriority)
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
  SetAntialiasingEnabled   (theOther->IsAntialiasingEnabled());
  SetBackground            (theOther->Background());
  SetGradientBackground    (theOther->GradientBackground());
  SetBackgroundImage       (theOther->BackgroundImage());
  SetBackgroundImageStyle  (theOther->BackgroundImageStyle());
  SetTextureEnv            (theOther->TextureEnv());
  SetCullingEnabled        (theOther->IsCullingEnabled());
  SetShadingModel          (theOther->ShadingModel());
  SetSurfaceDetailType     (theOther->SurfaceDetailType());
  SetBackfacingModel       (theOther->BackfacingModel());
  SetCamera                (new Graphic3d_Camera (theOther->Camera()));
  SetBackZClippingOn       (theOther->BackZClippingIsOn());
  SetFrontZClippingOn      (theOther->FrontZClippingIsOn());
  SetZClippingBackPlane    (theOther->ZClippingBackPlane());
  SetZClippingFrontPlane   (theOther->ZClippingFrontPlane());
  SetDepthCueingOn         (theOther->DepthCueingIsOn());
  SetDepthCueingBackPlane  (theOther->DepthCueingBackPlane());
  SetDepthCueingFrontPlane (theOther->DepthCueingFrontPlane());
  SetGLLightEnabled        (theOther->IsGLLightEnabled());
  SetLights                (theOther->Lights());
  SetClipPlanes            (theOther->ClipPlanes());
}
