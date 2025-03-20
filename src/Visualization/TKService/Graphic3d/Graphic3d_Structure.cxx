// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Graphic3d_Structure.hxx>

#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_PriorityDefinitionError.hxx>
#include <Graphic3d_StructureDefinitionError.hxx>
#include <Graphic3d_StructureManager.hxx>

#include <Standard_Dump.hxx>

#include <stdio.h>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Structure, Standard_Transient)

//=================================================================================================

Graphic3d_Structure::Graphic3d_Structure(const Handle(Graphic3d_StructureManager)& theManager,
                                         const Handle(Graphic3d_Structure)&        theLinkPrs)
    : myStructureManager(theManager.get()),
      myOwner(NULL),
      myVisual(Graphic3d_TOS_ALL),
      myComputeVisual(Graphic3d_TOS_ALL)
{
  if (!theLinkPrs.IsNull())
  {
    myOwner = theLinkPrs->myOwner;
    if (theLinkPrs->myVisual != Graphic3d_TOS_COMPUTED)
    {
      myVisual = theLinkPrs->myVisual;
    }
    myComputeVisual = theLinkPrs->myComputeVisual;
    myCStructure    = theLinkPrs->myCStructure->ShadowLink(theManager);
  }
  else
  {
    myCStructure = theManager->GraphicDriver()->CreateStructure(theManager);
  }
}

//=================================================================================================

Graphic3d_Structure::~Graphic3d_Structure()
{
  // as myStructureManager can be already destroyed,
  // avoid attempts to access it
  myStructureManager = NULL;
  Remove();
}

//=================================================================================================

void Graphic3d_Structure::clear(const Standard_Boolean theWithDestruction)
{
  if (IsDeleted())
    return;

  // clean groups in graphics driver at first
  GraphicClear(theWithDestruction);

  myCStructure->SetGroupTransformPersistence(false);
  myStructureManager->Clear(this, theWithDestruction);

  Update(true);
}

//=======================================================================
// function : CalculateBoundBox
// purpose  : Calculates AABB of a structure.
//=======================================================================
void Graphic3d_Structure::CalculateBoundBox()
{
  Graphic3d_BndBox3d aBox;
  addTransformed(aBox, Standard_True);
  myCStructure->ChangeBoundingBox() = aBox;
}

//=================================================================================================

void Graphic3d_Structure::Remove()
{
  if (IsDeleted())
    return;

  // clean groups in graphics driver at first; this is also should be done
  // to avoid unwanted group cleaning in group's destructor
  // Pass Standard_False to Clear(..) method to avoid updating in
  // structure manager, it isn't necessary, besides of it structure manager
  // could be already destroyed and invalid pointers used in structure;
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter(myCStructure->Groups()); aGroupIter.More();
       aGroupIter.Next())
  {
    aGroupIter.ChangeValue()->Clear(Standard_False);
  }

  // It is necessary to remove the eventual pointer on the structure that can be destroyed, in the
  // list of descendants of ancestors of this structure and in the list of ancestors of descendants
  // of the same structure.
  for (Standard_Integer aStructIdx = 1, aNbDesc = myDescendants.Size(); aStructIdx <= aNbDesc;
       ++aStructIdx)
  {
    myDescendants.FindKey(aStructIdx)->Remove(this, Graphic3d_TOC_ANCESTOR);
  }

  for (Standard_Integer aStructIdx = 1, aNbAnces = myAncestors.Size(); aStructIdx <= aNbAnces;
       ++aStructIdx)
  {
    myAncestors.FindKey(aStructIdx)->Remove(this, Graphic3d_TOC_DESCENDANT);
  }

  // Destruction of me in the graphic library
  const Standard_Integer aStructId = myCStructure->Identification();
  myCStructure->GraphicDriver()->RemoveIdentification(aStructId);
  myCStructure->GraphicDriver()->RemoveStructure(myCStructure);
  myCStructure.Nullify();
}

//=================================================================================================

void Graphic3d_Structure::Display()
{
  if (IsDeleted())
    return;

  if (!myCStructure->stick)
  {
    myCStructure->stick = 1;
    myStructureManager->Display(this);
  }

  if (myCStructure->visible != 1)
  {
    myCStructure->visible = 1;
    myCStructure->OnVisibilityChanged();
  }
}

//=================================================================================================

void Graphic3d_Structure::SetDisplayPriority(const Graphic3d_DisplayPriority thePriority)
{
  if (IsDeleted() || thePriority == myCStructure->Priority())
  {
    return;
  }

  Graphic3d_PriorityDefinitionError_Raise_if((thePriority > Graphic3d_DisplayPriority_Topmost)
                                               || (thePriority < Graphic3d_DisplayPriority_Bottom),
                                             "Bad value for StructurePriority");

  myCStructure->SetPreviousPriority(myCStructure->Priority());
  myCStructure->SetPriority(thePriority);
  if (myCStructure->Priority() != myCStructure->PreviousPriority())
  {
    if (myCStructure->stick)
    {
      myStructureManager->ChangeDisplayPriority(this,
                                                myCStructure->PreviousPriority(),
                                                myCStructure->Priority());
    }
  }
}

//=================================================================================================

void Graphic3d_Structure::ResetDisplayPriority()
{
  if (IsDeleted() || myCStructure->Priority() == myCStructure->PreviousPriority())
  {
    return;
  }

  const Graphic3d_DisplayPriority aPriority = myCStructure->Priority();
  myCStructure->SetPriority(myCStructure->PreviousPriority());
  if (myCStructure->stick)
  {
    myStructureManager->ChangeDisplayPriority(this, aPriority, myCStructure->Priority());
  }
}

//=================================================================================================

void Graphic3d_Structure::erase()
{
  if (IsDeleted())
  {
    return;
  }

  if (myCStructure->stick)
  {
    myCStructure->stick = 0;
    myStructureManager->Erase(this);
  }
}

//=================================================================================================

void Graphic3d_Structure::Highlight(const Handle(Graphic3d_PresentationAttributes)& theStyle,
                                    const Standard_Boolean                          theToUpdateMgr)
{
  if (IsDeleted())
  {
    return;
  }

  SetDisplayPriority(Graphic3d_DisplayPriority_Highlight);
  myCStructure->GraphicHighlight(theStyle);
  if (!theToUpdateMgr)
  {
    return;
  }

  if (myCStructure->stick)
  {
    myStructureManager->Highlight(this);
  }

  Update();
}

//=================================================================================================

void Graphic3d_Structure::SetVisible(const Standard_Boolean theValue)
{
  if (IsDeleted())
    return;

  const unsigned isVisible = theValue ? 1 : 0;
  if (myCStructure->visible == isVisible)
  {
    return;
  }

  myCStructure->visible = isVisible;
  myCStructure->OnVisibilityChanged();
  Update(true);
}

//=================================================================================================

void Graphic3d_Structure::UnHighlight()
{
  if (IsDeleted())
    return;

  if (myCStructure->highlight)
  {
    myCStructure->highlight = 0;

    myCStructure->GraphicUnhighlight();
    myStructureManager->UnHighlight(this);

    ResetDisplayPriority();
    Update();
  }
}

//=================================================================================================

Standard_Boolean Graphic3d_Structure::IsEmpty() const
{
  if (IsDeleted())
  {
    return Standard_True;
  }

  // structure is empty:
  // - if all these groups are empty
  // - or if all groups are empty and all their descendants are empty
  // - or if all its descendants are empty
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter(myCStructure->Groups()); aGroupIter.More();
       aGroupIter.Next())
  {
    if (!aGroupIter.Value()->IsEmpty())
    {
      return Standard_False;
    }
  }

  // stop at the first non-empty descendant
  for (NCollection_IndexedMap<Graphic3d_Structure*>::Iterator anIter(myDescendants); anIter.More();
       anIter.Next())
  {
    if (!anIter.Value()->IsEmpty())
    {
      return Standard_False;
    }
  }
  return Standard_True;
}

//=================================================================================================

void Graphic3d_Structure::ReCompute()
{
  myStructureManager->ReCompute(this);
}

//=================================================================================================

void Graphic3d_Structure::ReCompute(const Handle(Graphic3d_DataStructureManager)& theProjector)
{
  myStructureManager->ReCompute(this, theProjector);
}

//=================================================================================================

void Graphic3d_Structure::GraphicClear(const Standard_Boolean theWithDestruction)
{
  if (myCStructure.IsNull())
  {
    return;
  }

  // clean and empty each group
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter(myCStructure->Groups()); aGroupIter.More();
       aGroupIter.Next())
  {
    aGroupIter.ChangeValue()->Clear();
  }
  if (!theWithDestruction)
  {
    return;
  }

  while (!myCStructure->Groups().IsEmpty())
  {
    Handle(Graphic3d_Group) aGroup = myCStructure->Groups().First();
    aGroup->Remove();
  }
  myCStructure->Clear();
}

//=================================================================================================

void Graphic3d_Structure::SetVisual(const Graphic3d_TypeOfStructure theVisual)
{
  if (IsDeleted() || myVisual == theVisual)
  {
    return;
  }

  if (!myCStructure->stick)
  {
    myVisual = theVisual;
    SetComputeVisual(theVisual);
  }
  else
  {
    erase();
    myVisual = theVisual;
    SetComputeVisual(theVisual);
    Display();
  }
}

//=================================================================================================

void Graphic3d_Structure::SetZoomLimit(const Standard_Real theLimitInf,
                                       const Standard_Real theLimitSup)
{
  (void)theLimitInf;
  (void)theLimitSup;
  Graphic3d_StructureDefinitionError_Raise_if(theLimitInf <= 0.0, "Bad value for ZoomLimit inf");
  Graphic3d_StructureDefinitionError_Raise_if(theLimitSup <= 0.0, "Bad value for ZoomLimit sup");
  Graphic3d_StructureDefinitionError_Raise_if(theLimitSup < theLimitInf,
                                              "ZoomLimit sup < ZoomLimit inf");
}

//=================================================================================================

Standard_Boolean Graphic3d_Structure::AcceptConnection(Graphic3d_Structure*       theStructure1,
                                                       Graphic3d_Structure*       theStructure2,
                                                       Graphic3d_TypeOfConnection theType)
{
  // cycle detection
  NCollection_Map<Graphic3d_Structure*> aSet;
  Graphic3d_Structure::Network(theStructure2, theType, aSet);
  return !aSet.Contains(theStructure1);
}

//=================================================================================================

void Graphic3d_Structure::Ancestors(Graphic3d_MapOfStructure& theSet) const
{
  for (NCollection_IndexedMap<Graphic3d_Structure*>::Iterator anIter(myAncestors); anIter.More();
       anIter.Next())
  {
    theSet.Add(anIter.Value());
  }
}

//=================================================================================================

void Graphic3d_Structure::Descendants(Graphic3d_MapOfStructure& theSet) const
{
  for (NCollection_IndexedMap<Graphic3d_Structure*>::Iterator anIter(myDescendants); anIter.More();
       anIter.Next())
  {
    theSet.Add(anIter.Value());
  }
}

//=================================================================================================

Standard_Boolean Graphic3d_Structure::AppendAncestor(Graphic3d_Structure* theAncestor)
{
  const Standard_Integer aSize = myAncestors.Size();

  return myAncestors.Add(theAncestor) > aSize; // new object
}

//=================================================================================================

Standard_Boolean Graphic3d_Structure::AppendDescendant(Graphic3d_Structure* theDescendant)
{
  const Standard_Integer aSize = myDescendants.Size();

  return myDescendants.Add(theDescendant) > aSize; // new object
}

//=================================================================================================

Standard_Boolean Graphic3d_Structure::RemoveAncestor(Graphic3d_Structure* theAncestor)
{
  const Standard_Integer anIndex = myAncestors.FindIndex(theAncestor);

  if (anIndex != 0)
  {
    myAncestors.Swap(anIndex, myAncestors.Size());
    myAncestors.RemoveLast();
  }

  return anIndex != 0; // object was found
}

//=================================================================================================

Standard_Boolean Graphic3d_Structure::RemoveDescendant(Graphic3d_Structure* theDescendant)
{
  const Standard_Integer anIndex = myDescendants.FindIndex(theDescendant);

  if (anIndex != 0)
  {
    myDescendants.Swap(anIndex, myDescendants.Size());
    myDescendants.RemoveLast();
  }

  return anIndex != 0; // object was found
}

//=================================================================================================

void Graphic3d_Structure::Connect(Graphic3d_Structure*       theStructure,
                                  Graphic3d_TypeOfConnection theType,
                                  Standard_Boolean           theWithCheck)
{
  if (IsDeleted())
  {
    return;
  }

  // cycle detection
  if (theWithCheck && !Graphic3d_Structure::AcceptConnection(this, theStructure, theType))
  {
    return;
  }

  if (theType == Graphic3d_TOC_DESCENDANT)
  {
    if (!AppendDescendant(theStructure))
    {
      return;
    }

    CalculateBoundBox();
    theStructure->Connect(this, Graphic3d_TOC_ANCESTOR);

    GraphicConnect(theStructure);
    myStructureManager->Connect(this, theStructure);

    Update(true);
  }
  else // Graphic3d_TOC_ANCESTOR
  {
    if (!AppendAncestor(theStructure))
    {
      return;
    }

    CalculateBoundBox();
    theStructure->Connect(this, Graphic3d_TOC_DESCENDANT);

    // myStructureManager->Connect is called in case if connection between parent and child
  }
}

//=================================================================================================

void Graphic3d_Structure::Disconnect(Graphic3d_Structure* theStructure)
{
  if (IsDeleted())
  {
    return;
  }

  if (RemoveDescendant(theStructure))
  {
    theStructure->Disconnect(this);

    GraphicDisconnect(theStructure);
    myStructureManager->Disconnect(this, theStructure);

    CalculateBoundBox();
    Update(true);
  }
  else if (RemoveAncestor(theStructure))
  {
    theStructure->Disconnect(this);
    CalculateBoundBox();

    // no call of myStructureManager->Disconnect in case of an ancestor
  }
}

//=================================================================================================

void Graphic3d_Structure::DisconnectAll(const Graphic3d_TypeOfConnection theType)
{
  if (IsDeleted())
    return;

  switch (theType)
  {
    case Graphic3d_TOC_DESCENDANT: {
      for (Standard_Integer anIdx = 1, aLength = myDescendants.Size(); anIdx <= aLength; ++anIdx)
      {
        // Value (1) instead of Value (i) as myDescendants
        // is modified by :
        // Graphic3d_Structure::Disconnect (AStructure)
        // that takes AStructure from myDescendants
        myDescendants.FindKey(1)->Disconnect(this);
      }
      break;
    }
    case Graphic3d_TOC_ANCESTOR: {
      for (Standard_Integer anIdx = 1, aLength = myAncestors.Size(); anIdx <= aLength; ++anIdx)
      {
        // Value (1) instead of Value (i) as myAncestors
        // is modified by :
        // Graphic3d_Structure::Disconnect (AStructure)
        // that takes AStructure from myAncestors
        myAncestors.FindKey(1)->Disconnect(this);
      }
      break;
    }
  }
}

//=================================================================================================

void Graphic3d_Structure::SetTransformation(const Handle(TopLoc_Datum3D)& theTrsf)
{
  if (IsDeleted())
    return;

  const Standard_Boolean wasTransformed = IsTransformed();

  if (!theTrsf.IsNull() && theTrsf->Trsf().Form() == gp_Identity)
  {
    myCStructure->SetTransformation(Handle(TopLoc_Datum3D)());
  }
  else
  {
    myCStructure->SetTransformation(theTrsf);
  }

  // If transformation, no validation of hidden already calculated parts
  if (IsTransformed() || (!IsTransformed() && wasTransformed))
  {
    ReCompute();
  }

  myStructureManager->SetTransform(this, theTrsf);

  Update(true);
}

//=================================================================================================

Bnd_Box Graphic3d_Structure::MinMaxValues(const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox3d aBox;
  addTransformed(aBox, theToIgnoreInfiniteFlag);
  if (!aBox.IsValid())
  {
    return Bnd_Box();
  }

  Bnd_Box aResult;
  aResult.Update(aBox.CornerMin().x(),
                 aBox.CornerMin().y(),
                 aBox.CornerMin().z(),
                 aBox.CornerMax().x(),
                 aBox.CornerMax().y(),
                 aBox.CornerMax().z());

  constexpr Standard_Real aLimMin = ShortRealFirst() + 1.0;
  constexpr Standard_Real aLimMax = ShortRealLast() - 1.0;
  gp_Pnt                  aMin    = aResult.CornerMin();
  gp_Pnt                  aMax    = aResult.CornerMax();
  if (aMin.X() < aLimMin && aMin.Y() < aLimMin && aMin.Z() < aLimMin && aMax.X() > aLimMax
      && aMax.Y() > aLimMax && aMax.Z() > aLimMax)
  {
    // For structure which infinite in all three dimensions the Whole bounding box will be returned
    aResult.SetWhole();
  }
  return aResult;
}

//=================================================================================================

void Graphic3d_Structure::SetTransformPersistence(
  const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  if (IsDeleted())
  {
    return;
  }

  myCStructure->SetTransformPersistence(theTrsfPers);
}

//=================================================================================================

void Graphic3d_Structure::Remove(Graphic3d_Structure*             thePtr,
                                 const Graphic3d_TypeOfConnection theType)
{
  if (theType == Graphic3d_TOC_DESCENDANT)
  {
    RemoveDescendant(thePtr);
  }
  else
  {
    RemoveAncestor(thePtr);
  }
}

//=================================================================================================

Handle(Graphic3d_Group) Graphic3d_Structure::NewGroup()
{
  return myCStructure->NewGroup(this);
}

//=================================================================================================

void Graphic3d_Structure::Remove(const Handle(Graphic3d_Group)& theGroup)
{
  if (theGroup.IsNull() || theGroup->myStructure != this)
  {
    return;
  }

  myCStructure->RemoveGroup(theGroup);
  theGroup->myStructure = NULL;
}

//=================================================================================================

Handle(Graphic3d_StructureManager) Graphic3d_Structure::StructureManager() const
{
  return myStructureManager;
}

//=================================================================================================

Graphic3d_BndBox4f Graphic3d_Structure::minMaxCoord() const
{
  Graphic3d_BndBox4f aBnd;
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter(myCStructure->Groups()); aGroupIter.More();
       aGroupIter.Next())
  {
    if (!aGroupIter.Value()->TransformPersistence().IsNull())
    {
      continue; // should be translated to current view orientation to make sense
    }

    aBnd.Combine(aGroupIter.Value()->BoundingBox());
  }
  return aBnd;
}

//=================================================================================================

void Graphic3d_Structure::getBox(Graphic3d_BndBox3d&    theBox,
                                 const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox4f aBoxF = minMaxCoord();
  if (aBoxF.IsValid())
  {
    theBox = Graphic3d_BndBox3d(Graphic3d_Vec3d((Standard_Real)aBoxF.CornerMin().x(),
                                                (Standard_Real)aBoxF.CornerMin().y(),
                                                (Standard_Real)aBoxF.CornerMin().z()),
                                Graphic3d_Vec3d((Standard_Real)aBoxF.CornerMax().x(),
                                                (Standard_Real)aBoxF.CornerMax().y(),
                                                (Standard_Real)aBoxF.CornerMax().z()));
    if (IsInfinite() && !theToIgnoreInfiniteFlag)
    {
      const Graphic3d_Vec3d aDiagVec = theBox.CornerMax() - theBox.CornerMin();
      if (aDiagVec.SquareModulus() >= 500000.0 * 500000.0)
      {
        // bounding borders of infinite line has been calculated as own point in center of this line
        theBox = Graphic3d_BndBox3d((theBox.CornerMin() + theBox.CornerMax()) * 0.5);
      }
      else
      {
        theBox = Graphic3d_BndBox3d(Graphic3d_Vec3d(RealFirst(), RealFirst(), RealFirst()),
                                    Graphic3d_Vec3d(RealLast(), RealLast(), RealLast()));
        return;
      }
    }
  }
}

//=================================================================================================

void Graphic3d_Structure::addTransformed(Graphic3d_BndBox3d&    theBox,
                                         const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox3d aCombinedBox, aBox;
  getBox(aCombinedBox, theToIgnoreInfiniteFlag);

  for (NCollection_IndexedMap<Graphic3d_Structure*>::Iterator anIter(myDescendants); anIter.More();
       anIter.Next())
  {
    const Graphic3d_Structure* aStruct = anIter.Value();
    aStruct->getBox(aBox, theToIgnoreInfiniteFlag);
    aCombinedBox.Combine(aBox);
  }

  aBox = aCombinedBox;
  if (aBox.IsValid())
  {
    if (!myCStructure->Transformation().IsNull())
    {
      TransformBoundaries(myCStructure->Transformation()->Trsf(),
                          aBox.CornerMin().x(),
                          aBox.CornerMin().y(),
                          aBox.CornerMin().z(),
                          aBox.CornerMax().x(),
                          aBox.CornerMax().y(),
                          aBox.CornerMax().z());
    }

    // if box is still valid after transformation
    if (aBox.IsValid())
    {
      theBox.Combine(aBox);
    }
    else // it was infinite, return untransformed
    {
      theBox.Combine(aCombinedBox);
    }
  }
}

//=================================================================================================

void Graphic3d_Structure::Transforms(const gp_Trsf&      theTrsf,
                                     const Standard_Real theX,
                                     const Standard_Real theY,
                                     const Standard_Real theZ,
                                     Standard_Real&      theNewX,
                                     Standard_Real&      theNewY,
                                     Standard_Real&      theNewZ)
{
  constexpr Standard_Real aRL = RealLast();
  constexpr Standard_Real aRF = RealFirst();
  theNewX                     = theX;
  theNewY                     = theY;
  theNewZ                     = theZ;
  if ((theX == aRF) || (theY == aRF) || (theZ == aRF) || (theX == aRL) || (theY == aRL)
      || (theZ == aRL))
  {
    return;
  }

  theTrsf.Transforms(theNewX, theNewY, theNewZ);
}

//=================================================================================================

void Graphic3d_Structure::TransformBoundaries(const gp_Trsf& theTrsf,
                                              Standard_Real& theXMin,
                                              Standard_Real& theYMin,
                                              Standard_Real& theZMin,
                                              Standard_Real& theXMax,
                                              Standard_Real& theYMax,
                                              Standard_Real& theZMax)
{
  Standard_Real aXMin, aYMin, aZMin, aXMax, aYMax, aZMax, anU, aV, aW;

  Graphic3d_Structure::Transforms(theTrsf, theXMin, theYMin, theZMin, aXMin, aYMin, aZMin);
  Graphic3d_Structure::Transforms(theTrsf, theXMax, theYMax, theZMax, aXMax, aYMax, aZMax);

  Graphic3d_Structure::Transforms(theTrsf, theXMin, theYMin, theZMax, anU, aV, aW);
  aXMin = Min(anU, aXMin);
  aXMax = Max(anU, aXMax);
  aYMin = Min(aV, aYMin);
  aYMax = Max(aV, aYMax);
  aZMin = Min(aW, aZMin);
  aZMax = Max(aW, aZMax);

  Graphic3d_Structure::Transforms(theTrsf, theXMax, theYMin, theZMax, anU, aV, aW);
  aXMin = Min(anU, aXMin);
  aXMax = Max(anU, aXMax);
  aYMin = Min(aV, aYMin);
  aYMax = Max(aV, aYMax);
  aZMin = Min(aW, aZMin);
  aZMax = Max(aW, aZMax);

  Graphic3d_Structure::Transforms(theTrsf, theXMax, theYMin, theZMin, anU, aV, aW);
  aXMin = Min(anU, aXMin);
  aXMax = Max(anU, aXMax);
  aYMin = Min(aV, aYMin);
  aYMax = Max(aV, aYMax);
  aZMin = Min(aW, aZMin);
  aZMax = Max(aW, aZMax);

  Graphic3d_Structure::Transforms(theTrsf, theXMax, theYMax, theZMin, anU, aV, aW);
  aXMin = Min(anU, aXMin);
  aXMax = Max(anU, aXMax);
  aYMin = Min(aV, aYMin);
  aYMax = Max(aV, aYMax);
  aZMin = Min(aW, aZMin);
  aZMax = Max(aW, aZMax);

  Graphic3d_Structure::Transforms(theTrsf, theXMin, theYMax, theZMax, anU, aV, aW);
  aXMin = Min(anU, aXMin);
  aXMax = Max(anU, aXMax);
  aYMin = Min(aV, aYMin);
  aYMax = Max(aV, aYMax);
  aZMin = Min(aW, aZMin);
  aZMax = Max(aW, aZMax);

  Graphic3d_Structure::Transforms(theTrsf, theXMin, theYMax, theZMin, anU, aV, aW);
  aXMin = Min(anU, aXMin);
  aXMax = Max(anU, aXMax);
  aYMin = Min(aV, aYMin);
  aYMax = Max(aV, aYMax);
  aZMin = Min(aW, aZMin);
  aZMax = Max(aW, aZMax);

  theXMin = aXMin;
  theYMin = aYMin;
  theZMin = aZMin;
  theXMax = aXMax;
  theYMax = aYMax;
  theZMax = aZMax;
}

//=================================================================================================

void Graphic3d_Structure::Network(Graphic3d_Structure*                   theStructure,
                                  const Graphic3d_TypeOfConnection       theType,
                                  NCollection_Map<Graphic3d_Structure*>& theSet)
{
  theSet.Add(theStructure);
  switch (theType)
  {
    case Graphic3d_TOC_DESCENDANT: {
      for (NCollection_IndexedMap<Graphic3d_Structure*>::Iterator anIter(
             theStructure->myDescendants);
           anIter.More();
           anIter.Next())
      {
        Graphic3d_Structure::Network(anIter.Value(), theType, theSet);
      }
      break;
    }
    case Graphic3d_TOC_ANCESTOR: {
      for (NCollection_IndexedMap<Graphic3d_Structure*>::Iterator anIter(theStructure->myAncestors);
           anIter.More();
           anIter.Next())
      {
        Graphic3d_Structure::Network(anIter.Value(), theType, theSet);
      }
      break;
    }
  }
}

//=================================================================================================

void Graphic3d_Structure::PrintNetwork(const Handle(Graphic3d_Structure)& theStructure,
                                       const Graphic3d_TypeOfConnection   theType)
{
  NCollection_Map<Graphic3d_Structure*> aSet;
  Graphic3d_Structure::Network(theStructure.get(), theType, aSet);
  for (NCollection_Map<Graphic3d_Structure*>::Iterator anIter(aSet); anIter.More(); anIter.Next())
  {
    std::cout << "\tIdent " << (anIter.Key())->Identification() << "\n";
  }
  std::cout << std::flush;
}

//=================================================================================================

void Graphic3d_Structure::Update(const bool theUpdateLayer) const
{
  if (IsDeleted())
  {
    return;
  }

  myStructureManager->Update(theUpdateLayer ? myCStructure->ZLayer() : Graphic3d_ZLayerId_UNKNOWN);
}

//=================================================================================================

void Graphic3d_Structure::SetZLayer(const Graphic3d_ZLayerId theLayerId)
{
  // if the structure is not displayed, unable to change its display layer
  if (IsDeleted())
    return;

  myStructureManager->ChangeZLayer(this, theLayerId);
  myCStructure->SetZLayer(theLayerId);
}

//=================================================================================================

void Graphic3d_Structure::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myStructureManager)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myCStructure.get())

  for (NCollection_IndexedMap<Graphic3d_Structure*>::Iterator anIter(myAncestors); anIter.More();
       anIter.Next())
  {
    Graphic3d_Structure* anAncestor = anIter.Value();
    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, anAncestor)
  }

  for (NCollection_IndexedMap<Graphic3d_Structure*>::Iterator anIter(myDescendants); anIter.More();
       anIter.Next())
  {
    Graphic3d_Structure* aDescendant = anIter.Value();
    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, aDescendant)
  }

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myOwner)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVisual)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myComputeVisual)
}
