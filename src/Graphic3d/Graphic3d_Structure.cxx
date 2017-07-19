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
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_PriorityDefinitionError.hxx>
#include <Graphic3d_StructureDefinitionError.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TransformError.hxx>
#include <Graphic3d_Vector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

#include "Graphic3d_Structure.pxx"

#include <stdio.h>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Structure,Standard_Transient)

//=============================================================================
//function : Graphic3d_Structure
//purpose  :
//=============================================================================
Graphic3d_Structure::Graphic3d_Structure (const Handle(Graphic3d_StructureManager)& theManager)
: myStructureManager(theManager.operator->()),
  myComputeVisual   (Graphic3d_TOS_ALL),
  myOwner           (NULL),
  myVisual          (Graphic3d_TOS_ALL)
{
  myCStructure = theManager->GraphicDriver()->CreateStructure (theManager);
}

//=============================================================================
//function : Graphic3d_Structure
//purpose  :
//=============================================================================
Graphic3d_Structure::Graphic3d_Structure (const Handle(Graphic3d_StructureManager)& theManager,
                                          const Handle(Graphic3d_Structure)&        thePrs)
: myStructureManager(theManager.operator->()),
  myComputeVisual   (thePrs->myComputeVisual),
  myOwner           (thePrs->myOwner),
  myVisual          (thePrs->myVisual)
{
  myCStructure = thePrs->myCStructure->ShadowLink (theManager);
}

//=============================================================================
//function : ~Graphic3d_Structure
//purpose  :
//=============================================================================
Graphic3d_Structure::~Graphic3d_Structure()
{
  // as myStructureManager can be already destroyed,
  // avoid attempts to access it
  myStructureManager = NULL;
  Remove();
}

//=============================================================================
//function : Clear
//purpose  :
//=============================================================================
void Graphic3d_Structure::Clear (const Standard_Boolean theWithDestruction)
{
  if (IsDeleted()) return;

  // clean groups in graphics driver at first
  GraphicClear (theWithDestruction);

  myCStructure->ContainsFacet = 0;
  myStructureManager->Clear (this, theWithDestruction);

  Update (true);
}

//=======================================================================
//function : CalculateBoundBox
//purpose  : Calculates AABB of a structure.
//=======================================================================
void Graphic3d_Structure::CalculateBoundBox()
{
  Graphic3d_BndBox3d aBox;
  addTransformed (aBox, Standard_True);
  myCStructure->ChangeBoundingBox() = aBox;
}

//=============================================================================
//function : Remove
//purpose  :
//=============================================================================
void Graphic3d_Structure::Remove()
{
  if (IsDeleted()) return;

  // clean groups in graphics driver at first; this is also should be done
  // to avoid unwanted group cleaning in group's destructor
  // Pass Standard_False to Clear(..) method to avoid updating in
  // structure manager, it isn't necessary, besides of it structure manager
  // could be already destroyed and invalid pointers used in structure;
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (myCStructure->Groups()); aGroupIter.More(); aGroupIter.Next())
  {
    aGroupIter.ChangeValue()->Clear (Standard_False);
  }

  Standard_Address APtr = (void *) this;
  // It is necessary to remove the eventual pointer on the structure
  // that can be destroyed, in the list of descendants
  // of ancestors of this structure and in the list of ancestors
  // of descendants of the same structure.

  for (Standard_Integer aStructIdx = 1, aNbDesc = myDescendants.Size(); aStructIdx <= aNbDesc; ++aStructIdx)
  {
    ((Graphic3d_Structure *)myDescendants.FindKey (aStructIdx))->Remove (APtr, Graphic3d_TOC_ANCESTOR);
  }

  for (Standard_Integer aStructIdx = 1, aNbAnces = myAncestors.Size(); aStructIdx <= aNbAnces; ++aStructIdx)
  {
    ((Graphic3d_Structure *)myAncestors.FindKey (aStructIdx))->Remove (APtr, Graphic3d_TOC_DESCENDANT);
  }

  // Destruction of me in the graphic library
  const Standard_Integer aStructId = myCStructure->Id;
  myCStructure->GraphicDriver()->RemoveIdentification(aStructId);
  myCStructure->GraphicDriver()->RemoveStructure (myCStructure);
  myCStructure.Nullify();
}

//=============================================================================
//function : Display
//purpose  :
//=============================================================================
void Graphic3d_Structure::Display()
{
  if (IsDeleted()) return;

  if (!myCStructure->stick)
  {
    myCStructure->stick = 1;
    myStructureManager->Display (this);
  }

  if (myCStructure->visible != 1)
  {
    myCStructure->visible = 1;
    myCStructure->OnVisibilityChanged();
  }
}

//=============================================================================
//function : SetIsForHighlight
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetIsForHighlight (const Standard_Boolean isForHighlight)
{
  myCStructure->IsForHighlight = isForHighlight;
}

//=============================================================================
//function : SetDisplayPriority
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetDisplayPriority (const Standard_Integer thePriority)
{
  if (IsDeleted()
   || thePriority == myCStructure->Priority)
  {
    return;
  }

  myCStructure->PreviousPriority = myCStructure->Priority;
  myCStructure->Priority         = thePriority;

  if (myCStructure->Priority != myCStructure->PreviousPriority)
  {
    Graphic3d_PriorityDefinitionError_Raise_if ((myCStructure->Priority > Structure_MAX_PRIORITY)
                                             || (myCStructure->Priority < Structure_MIN_PRIORITY),
                                                "Bad value for StructurePriority");
    if (myCStructure->stick)
    {
      myStructureManager->ChangeDisplayPriority (this, myCStructure->PreviousPriority, myCStructure->Priority);
    }
  }
}

//=============================================================================
//function : ResetDisplayPriority
//purpose  :
//=============================================================================
void Graphic3d_Structure::ResetDisplayPriority()
{
  if (IsDeleted()
   || myCStructure->Priority == myCStructure->PreviousPriority)
  {
    return;
  }

  const Standard_Integer aPriority = myCStructure->Priority;
  myCStructure->Priority = myCStructure->PreviousPriority;
  if (myCStructure->stick)
  {
    myStructureManager->ChangeDisplayPriority (this, aPriority, myCStructure->Priority);
  }
}

//=============================================================================
//function : DisplayPriority
//purpose  :
//=============================================================================
Standard_Integer Graphic3d_Structure::DisplayPriority() const
{
  return myCStructure->Priority;
}

//=============================================================================
//function : Erase
//purpose  :
//=============================================================================
void Graphic3d_Structure::Erase()
{
  if (IsDeleted())
  {
    return;
  }

  if (myCStructure->stick)
  {
    myCStructure->stick = 0;
    myStructureManager->Erase (this);
  }
}

//=============================================================================
//function : Highlight
//purpose  :
//=============================================================================
void Graphic3d_Structure::Highlight (const Handle(Graphic3d_PresentationAttributes)& theStyle,
                                     const Standard_Boolean theToUpdateMgr)
{
  if (IsDeleted())
  {
    return;
  }

  SetDisplayPriority (Structure_MAX_PRIORITY - 1);

  myCStructure->GraphicHighlight (theStyle, this);

  if (!theToUpdateMgr)
  {
    return;
  }

  if (myCStructure->stick)
  {
    myStructureManager->Highlight (this);
  }

  Update();
}

//=============================================================================
//function : SetVisible
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetVisible (const Standard_Boolean theValue)
{
  if (IsDeleted()) return;

  const unsigned isVisible = theValue ? 1 : 0;
  if (myCStructure->visible == isVisible)
  {
    return;
  }

  myCStructure->visible = isVisible;
  myCStructure->OnVisibilityChanged();
  Update (true);
}

//=============================================================================
//function : UnHighlight
//purpose  :
//=============================================================================
void Graphic3d_Structure::UnHighlight()
{
  if (IsDeleted()) return;

  if (myCStructure->highlight)
  {
    myCStructure->highlight = 0;

    myCStructure->GraphicUnhighlight();
    myStructureManager->UnHighlight (this);

    ResetDisplayPriority();
    Update();
  }
}

//=============================================================================
//function : HighlightStyle
//purpose  :
//=============================================================================
const Handle(Graphic3d_PresentationAttributes)& Graphic3d_Structure::HighlightStyle() const
{
  return myCStructure->HighlightStyle();
}

//=============================================================================
//function : IsDisplayed
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::IsDisplayed() const
{
  return myCStructure->stick ? Standard_True : Standard_False;
}

//=============================================================================
//function : IsDeleted
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::IsDeleted() const
{
  return myCStructure.IsNull();
}

//=============================================================================
//function : IsHighlighted
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::IsHighlighted() const
{
  return myCStructure->highlight ? Standard_True : Standard_False;
}

//=============================================================================
//function : IsVisible
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::IsVisible() const
{
  return myCStructure->visible ? Standard_True : Standard_False;
}

//=============================================================================
//function : IsTransformed
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::IsTransformed() const
{
  return !myCStructure->Transformation().IsNull()
       && myCStructure->Transformation()->Form() != gp_Identity;
}

//=============================================================================
//function : ContainsFacet
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::ContainsFacet() const
{
  if (IsDeleted())
  {
    return Standard_False;
  }
  else if (myCStructure->ContainsFacet > 0)
  {
    // if one of groups contains at least one facet, the structure contains it too
    return Standard_True;
  }

  // stop at the first descendant containing at least one facet
  for (Graphic3d_IndexedMapOfAddress::Iterator anIter (myDescendants); anIter.More(); anIter.Next())
  {
    if (((const Graphic3d_Structure *)anIter.Value())->ContainsFacet())
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=============================================================================
//function : IsEmpty
//purpose  :
//=============================================================================
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
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (myCStructure->Groups()); aGroupIter.More(); aGroupIter.Next())
  {
    if (!aGroupIter.Value()->IsEmpty())
    {
      return Standard_False;
    }
  }

  // stop at the first non-empty descendant
  for (Graphic3d_IndexedMapOfAddress::Iterator anIter (myDescendants); anIter.More(); anIter.Next())
  {
    if (!((const Graphic3d_Structure* )anIter.Value())->IsEmpty())
    {
      return Standard_False;
    }
  }
  return Standard_True;
}

//=============================================================================
//function : GroupsWithFacet
//purpose  :
//=============================================================================
void Graphic3d_Structure::GroupsWithFacet (const Standard_Integer theDelta)
{
  myCStructure->ContainsFacet = myCStructure->ContainsFacet + theDelta;
  if (myCStructure->ContainsFacet < 0)
  {
    myCStructure->ContainsFacet = 0;
  }
}

//=============================================================================
//function : Compute
//purpose  :
//=============================================================================
void Graphic3d_Structure::Compute()
{
  // Implemented by Presentation
}

//=============================================================================
//function : Compute
//purpose  :
//=============================================================================
Handle(Graphic3d_Structure) Graphic3d_Structure::Compute (const Handle(Graphic3d_DataStructureManager)& )
{
  // Implemented by Presentation
  return this;
}

//=============================================================================
//function : Compute
//purpose  :
//=============================================================================
Handle(Graphic3d_Structure) Graphic3d_Structure::Compute (const Handle(Graphic3d_DataStructureManager)& ,
                                                          const Handle(Geom_Transformation)& )
{
  // Implemented by Presentation
  return this;
}

//=============================================================================
//function : Compute
//purpose  :
//=============================================================================
void Graphic3d_Structure::Compute (const Handle(Graphic3d_DataStructureManager)& ,
                                   Handle(Graphic3d_Structure)& )
{
  // Implemented by Presentation
}

//=============================================================================
//function : Compute
//purpose  :
//=============================================================================
void Graphic3d_Structure::Compute (const Handle(Graphic3d_DataStructureManager)& ,
                                   const Handle(Geom_Transformation)& ,
                                   Handle(Graphic3d_Structure)& )
{
  // Implemented by Presentation
}

//=============================================================================
//function : ReCompute
//purpose  :
//=============================================================================
void Graphic3d_Structure::ReCompute()
{
  myStructureManager->ReCompute (this);
}

//=============================================================================
//function : ReCompute
//purpose  :
//=============================================================================
void Graphic3d_Structure::ReCompute (const Handle(Graphic3d_DataStructureManager)& theProjector)
{
  myStructureManager->ReCompute (this, theProjector);
}

//=============================================================================
//function : SetInfiniteState
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetInfiniteState (const Standard_Boolean theToSet)
{
  myCStructure->IsInfinite = theToSet ? 1 : 0;
}

//=============================================================================
//function : IsInfinite
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::IsInfinite() const
{
  return IsDeleted()
       || myCStructure->IsInfinite;
}

//=============================================================================
//function : GraphicClear
//purpose  :
//=============================================================================
void Graphic3d_Structure::GraphicClear (const Standard_Boolean theWithDestruction)
{
  if (myCStructure.IsNull())
  {
    return;
  }

  // clean and empty each group
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (myCStructure->Groups()); aGroupIter.More(); aGroupIter.Next())
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

//=============================================================================
//function : GraphicConnect
//purpose  :
//=============================================================================
void Graphic3d_Structure::GraphicConnect (const Handle(Graphic3d_Structure)& theDaughter)
{
  myCStructure->Connect (*theDaughter->myCStructure);
}

//=============================================================================
//function : GraphicDisconnect
//purpose  :
//=============================================================================
void Graphic3d_Structure::GraphicDisconnect (const Handle(Graphic3d_Structure)& theDaughter)
{
  myCStructure->Disconnect (*theDaughter->myCStructure);
}

//=============================================================================
//function : Groups
//purpose  :
//=============================================================================
const Graphic3d_SequenceOfGroup& Graphic3d_Structure::Groups() const
{
  return myCStructure->Groups();
}

//=============================================================================
//function : NumberOfGroups
//purpose  :
//=============================================================================
Standard_Integer Graphic3d_Structure::NumberOfGroups() const
{
  return myCStructure->Groups().Length();
}

//=============================================================================
//function : SetVisual
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetVisual (const Graphic3d_TypeOfStructure theVisual)
{
  if (IsDeleted()
   || myVisual == theVisual)
  {
    return;
  }

  if (!myCStructure->stick)
  {
    myVisual = theVisual;
    SetComputeVisual (theVisual);
  }
  else
  {
    Erase();
    myVisual = theVisual;
    SetComputeVisual (theVisual);
    Display();
  }
}

//=============================================================================
//function : SetZoomLimit
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetZoomLimit (const Standard_Real theLimitInf,
                                        const Standard_Real theLimitSup)
{
  (void )theLimitInf;
  (void )theLimitSup;
  Graphic3d_StructureDefinitionError_Raise_if (theLimitInf <= 0.0,
                                               "Bad value for ZoomLimit inf");
  Graphic3d_StructureDefinitionError_Raise_if (theLimitSup <= 0.0,
                                               "Bad value for ZoomLimit sup");
  Graphic3d_StructureDefinitionError_Raise_if (theLimitSup < theLimitInf,
                                               "ZoomLimit sup < ZoomLimit inf");
}

//=============================================================================
//function : Visual
//purpose  :
//=============================================================================
Graphic3d_TypeOfStructure Graphic3d_Structure::Visual() const
{
  return myVisual;
}

//=============================================================================
//function : AcceptConnection
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::AcceptConnection (const Handle(Graphic3d_Structure)& theStructure1,
                                                        const Handle(Graphic3d_Structure)& theStructure2,
                                                        const Graphic3d_TypeOfConnection   theType)
{
  // cycle detection
  Graphic3d_MapOfStructure aSet;
  Graphic3d_Structure::Network (theStructure2, theType, aSet);
  return !aSet.Contains (theStructure1);
}

//=============================================================================
//function : Ancestors
//purpose  :
//=============================================================================
void Graphic3d_Structure::Ancestors (Graphic3d_MapOfStructure& theSet) const
{
  for (Graphic3d_IndexedMapOfAddress::Iterator anIter (myAncestors); anIter.More(); anIter.Next())
  {
    theSet.Add ((Graphic3d_Structure* )anIter.Value());
  }
}

//=============================================================================
//function : SetOwner
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetOwner (const Standard_Address theOwner)
{
  myOwner = theOwner;
}

//=============================================================================
//function : Owner
//purpose  :
//=============================================================================
Standard_Address Graphic3d_Structure::Owner() const
{
  return myOwner;
}

//=============================================================================
//function : Descendants
//purpose  :
//=============================================================================
void Graphic3d_Structure::Descendants (Graphic3d_MapOfStructure& theSet) const
{
  for (Graphic3d_IndexedMapOfAddress::Iterator anIter (myDescendants); anIter.More(); anIter.Next())
  {
    theSet.Add ((Graphic3d_Structure* )anIter.Value());
  }
}

//=============================================================================
//function : AppendAncestor
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::AppendAncestor (const Standard_Address theAncestor)
{
  const Standard_Integer aSize = myAncestors.Size();

  return myAncestors.Add (theAncestor) > aSize; // new object
}

//=============================================================================
//function : AppendDescendant
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::AppendDescendant (const Standard_Address theDescendant)
{
  const Standard_Integer aSize = myDescendants.Size();

  return myDescendants.Add (theDescendant) > aSize; // new object
}

//=============================================================================
//function : RemoveAncestor
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::RemoveAncestor (const Standard_Address theAncestor)
{
  const Standard_Integer anIndex = myAncestors.FindIndex (theAncestor);

  if (anIndex != 0)
  {
    myAncestors.Swap (anIndex, myAncestors.Size());
    myAncestors.RemoveLast();
  }

  return anIndex != 0; // object was found
}

//=============================================================================
//function : RemoveDescendant
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::RemoveDescendant (const Standard_Address theDescendant)
{
  const Standard_Integer anIndex = myDescendants.FindIndex (theDescendant);

  if (anIndex != 0)
  {
    myDescendants.Swap (anIndex, myDescendants.Size());
    myDescendants.RemoveLast();
  }

  return anIndex != 0; // object was found
}

//=============================================================================
//function : Connect
//purpose  :
//=============================================================================
void Graphic3d_Structure::Connect (const Handle(Graphic3d_Structure)& theStructure,
                                   const Graphic3d_TypeOfConnection   theType,
                                   const Standard_Boolean             theWithCheck)
{
  if (IsDeleted())
  {
    return;
  }

  // cycle detection
  if (theWithCheck
   && !Graphic3d_Structure::AcceptConnection (this, theStructure, theType))
  {
    return;
  }

  const Standard_Address aStructure = theStructure.operator->();

  if (theType == Graphic3d_TOC_DESCENDANT)
  {
    if (!AppendDescendant (aStructure))
    {
      return;
    }

    CalculateBoundBox();
    theStructure->Connect (this, Graphic3d_TOC_ANCESTOR);

    GraphicConnect (theStructure);
    myStructureManager->Connect (this, theStructure);

    Update (true);
  }
  else // Graphic3d_TOC_ANCESTOR
  {
    if (!AppendAncestor (aStructure))
    {
      return;
    }

    CalculateBoundBox();
    theStructure->Connect (this, Graphic3d_TOC_DESCENDANT);

    // myStructureManager->Connect is called in case if connection between parent and child
  }
}

//=============================================================================
//function : Disconnect
//purpose  :
//=============================================================================
void Graphic3d_Structure::Disconnect (const Handle(Graphic3d_Structure)& theStructure)
{
  if (IsDeleted())
  {
    return;
  }

  const Standard_Address aStructure = theStructure.operator->();

  if (RemoveDescendant (aStructure))
  {
    theStructure->Disconnect (this);

    GraphicDisconnect (theStructure);
    myStructureManager->Disconnect (this, theStructure);

    CalculateBoundBox();
    Update (true);
  }
  else if (RemoveAncestor (aStructure))
  {
    theStructure->Disconnect (this);
    CalculateBoundBox();

    // no call of myStructureManager->Disconnect in case of an ancestor
  }
}

//=============================================================================
//function : DisconnectAll
//purpose  :
//=============================================================================
void Graphic3d_Structure::DisconnectAll (const Graphic3d_TypeOfConnection theType)
{
  if (IsDeleted()) return;

  switch (theType)
  {
    case Graphic3d_TOC_DESCENDANT:
    {
      for (Standard_Integer anIdx = 1, aLength = myDescendants.Size(); anIdx <= aLength; ++anIdx)
      {
        // Value (1) instead of Value (i) as myDescendants
        // is modified by :
        // Graphic3d_Structure::Disconnect (AStructure)
        // that takes AStructure from myDescendants
        ((Graphic3d_Structure* )(myDescendants.FindKey (1)))->Disconnect (this);
      }
      break;
    }
    case Graphic3d_TOC_ANCESTOR:
    {
      for (Standard_Integer anIdx = 1, aLength = myAncestors.Size(); anIdx <= aLength; ++anIdx)
      {
        // Value (1) instead of Value (i) as myAncestors
        // is modified by :
        // Graphic3d_Structure::Disconnect (AStructure)
        // that takes AStructure from myAncestors
        ((Graphic3d_Structure* )(myAncestors.FindKey (1)))->Disconnect (this);
      }
      break;
    }
  }
}

//=============================================================================
//function : SetTransform
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetTransformation (const Handle(Geom_Transformation)& theTrsf)
{
  if (IsDeleted()) return;

  const Standard_Boolean wasTransformed = IsTransformed();

  if (!theTrsf.IsNull()
    && theTrsf->Trsf().Form() == gp_Identity)
  {
    myCStructure->SetTransformation (Handle(Geom_Transformation)());
  }
  else
  {
    myCStructure->SetTransformation (theTrsf);
  }

  // If transformation, no validation of hidden already calculated parts
  if (IsTransformed() || (!IsTransformed() && wasTransformed))
  {
    ReCompute();
  }

  myStructureManager->SetTransform (this, theTrsf);

  Update (true);
}

//=============================================================================
//function : MinMaxValues
//purpose  :
//=============================================================================
Bnd_Box Graphic3d_Structure::MinMaxValues (const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox3d aBox;
  addTransformed (aBox, theToIgnoreInfiniteFlag);
  if (!aBox.IsValid())
  {
    return Bnd_Box();
  }

  Bnd_Box aResult;
  aResult.Update (aBox.CornerMin().x(), aBox.CornerMin().y(), aBox.CornerMin().z(),
                  aBox.CornerMax().x(), aBox.CornerMax().y(), aBox.CornerMax().z());

  Standard_Real aLimMin = ShortRealFirst() + 1.0;
  Standard_Real aLimMax = ShortRealLast()  - 1.0;
  gp_Pnt aMin = aResult.CornerMin();
  gp_Pnt aMax = aResult.CornerMax();
  if (aMin.X() < aLimMin && aMin.Y() < aLimMin && aMin.Z() < aLimMin
   && aMax.X() > aLimMax && aMax.Y() > aLimMax && aMax.Z() > aLimMax)
  {
    //For structure which infinite in all three dimensions the Whole bounding box will be returned
    aResult.SetWhole();
  }
  return aResult;
}

//=============================================================================
//function : Identification
//purpose  :
//=============================================================================
Standard_Integer Graphic3d_Structure::Identification() const
{
  return myCStructure->Id;
}

//=============================================================================
//function : SetTransformPersistence
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetTransformPersistence (const Handle(Graphic3d_TransformPers)& theTrsfPers)
{
  if (IsDeleted())
  {
    return;
  }

  myCStructure->SetTransformPersistence (theTrsfPers);
}

//=============================================================================
//function : Remove
//purpose  :
//=============================================================================
void Graphic3d_Structure::Remove (const Standard_Address           thePtr,
                                  const Graphic3d_TypeOfConnection theType)
{
  if (theType == Graphic3d_TOC_DESCENDANT)
  {
    RemoveDescendant (thePtr);
  }
  else
  {
    RemoveAncestor (thePtr);
  }
}

//=============================================================================
//function : NewGroup
//purpose  :
//=============================================================================
Handle(Graphic3d_Group) Graphic3d_Structure::NewGroup()
{
  return myCStructure->NewGroup (this);
}

//=============================================================================
//function : Remove
//purpose  :
//=============================================================================
void Graphic3d_Structure::Remove (const Handle(Graphic3d_Group)& theGroup)
{
  if (theGroup.IsNull()
   || theGroup->myStructure != this)
  {
    return;
  }

  myCStructure->RemoveGroup (theGroup);
  theGroup->myStructure = NULL;
}

//=============================================================================
//function : StructureManager
//purpose  :
//=============================================================================
Handle(Graphic3d_StructureManager) Graphic3d_Structure::StructureManager() const
{
  return myStructureManager;
}

//=============================================================================
//function : minMaxCoord
//purpose  :
//=============================================================================
Graphic3d_BndBox4f Graphic3d_Structure::minMaxCoord() const
{
  Graphic3d_BndBox4f aBnd;
  for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (myCStructure->Groups()); aGroupIter.More(); aGroupIter.Next())
  {
    aBnd.Combine (aGroupIter.Value()->BoundingBox());
  }
  return aBnd;
}

//=============================================================================
//function : addTransformed
//purpose  :
//=============================================================================
void Graphic3d_Structure::getBox (Graphic3d_BndBox3d&    theBox,
                                  const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox4f aBoxF = minMaxCoord();
  if (aBoxF.IsValid())
  {
    theBox = Graphic3d_BndBox3d (Graphic3d_Vec3d ((Standard_Real )aBoxF.CornerMin().x(),
                                                  (Standard_Real )aBoxF.CornerMin().y(),
                                                  (Standard_Real )aBoxF.CornerMin().z()),
                                 Graphic3d_Vec3d ((Standard_Real )aBoxF.CornerMax().x(),
                                                  (Standard_Real )aBoxF.CornerMax().y(),
                                                  (Standard_Real )aBoxF.CornerMax().z()));
    if (IsInfinite()
    && !theToIgnoreInfiniteFlag)
    {
      const Graphic3d_Vec3d aDiagVec = theBox.CornerMax() - theBox.CornerMin();
      if (aDiagVec.SquareModulus() >= 500000.0 * 500000.0)
      {
        // bounding borders of infinite line has been calculated as own point in center of this line
        theBox = Graphic3d_BndBox3d ((theBox.CornerMin() + theBox.CornerMax()) * 0.5);
      }
      else
      {
        theBox = Graphic3d_BndBox3d (Graphic3d_Vec3d (RealFirst(), RealFirst(), RealFirst()),
                                     Graphic3d_Vec3d (RealLast(),  RealLast(),  RealLast()));
        return;
      }
    }
  }
}

//=============================================================================
//function : addTransformed
//purpose  :
//=============================================================================
void Graphic3d_Structure::addTransformed (Graphic3d_BndBox3d&    theBox,
                                          const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox3d aCombinedBox, aBox;
  getBox (aCombinedBox, theToIgnoreInfiniteFlag);

  for (Graphic3d_IndexedMapOfAddress::Iterator anIter (myDescendants); anIter.More(); anIter.Next())
  {
    const Graphic3d_Structure* aStruct = (const Graphic3d_Structure* )anIter.Value();
    aStruct->getBox (aBox, theToIgnoreInfiniteFlag);
    aCombinedBox.Combine (aBox);
  }

  aBox = aCombinedBox;
  if (aBox.IsValid())
  {
    if (!myCStructure->Transformation().IsNull())
    {
      TransformBoundaries (myCStructure->Transformation()->Trsf(),
                           aBox.CornerMin().x(), aBox.CornerMin().y(), aBox.CornerMin().z(),
                           aBox.CornerMax().x(), aBox.CornerMax().y(), aBox.CornerMax().z());
    }

    // if box is still valid after transformation
    if (aBox.IsValid())
    {
      theBox.Combine (aBox);
    }
    else // it was infinite, return untransformed
    {
      theBox.Combine (aCombinedBox);
    }
  }
}

//=============================================================================
//function : Transforms
//purpose  :
//=============================================================================
void Graphic3d_Structure::Transforms (const gp_Trsf& theTrsf,
                                      const Standard_Real theX,    const Standard_Real theY,    const Standard_Real theZ,
                                      Standard_Real&      theNewX, Standard_Real&      theNewY, Standard_Real&      theNewZ)
{
  const Standard_Real aRL = RealLast();
  const Standard_Real aRF = RealFirst();
  theNewX = theX;
  theNewY = theY;
  theNewZ = theZ;
  if ((theX == aRF) || (theY == aRF) || (theZ == aRF)
   || (theX == aRL) || (theY == aRL) || (theZ == aRL))
  {
    return;
  }

  theTrsf.Transforms (theNewX, theNewY, theNewZ);
}

//=============================================================================
//function : Transforms
//purpose  :
//=============================================================================
void Graphic3d_Structure::TransformBoundaries (const gp_Trsf& theTrsf,
                                               Standard_Real& theXMin,
                                               Standard_Real& theYMin,
                                               Standard_Real& theZMin,
                                               Standard_Real& theXMax,
                                               Standard_Real& theYMax,
                                               Standard_Real& theZMax)
{
  Standard_Real aXMin, aYMin, aZMin, aXMax, aYMax, aZMax, anU, aV, aW;

  Graphic3d_Structure::Transforms (theTrsf, theXMin, theYMin, theZMin, aXMin, aYMin, aZMin);
  Graphic3d_Structure::Transforms (theTrsf, theXMax, theYMax, theZMax, aXMax, aYMax, aZMax);

  Graphic3d_Structure::Transforms (theTrsf, theXMin, theYMin, theZMax, anU, aV, aW);
  aXMin = Min (anU, aXMin); aXMax = Max (anU, aXMax);
  aYMin = Min (aV,  aYMin); aYMax = Max (aV,  aYMax);
  aZMin = Min (aW,  aZMin); aZMax = Max (aW,  aZMax);

  Graphic3d_Structure::Transforms (theTrsf, theXMax, theYMin, theZMax, anU, aV, aW);
  aXMin = Min (anU, aXMin); aXMax = Max (anU, aXMax);
  aYMin = Min (aV,  aYMin); aYMax = Max (aV,  aYMax);
  aZMin = Min (aW,  aZMin); aZMax = Max (aW,  aZMax);

  Graphic3d_Structure::Transforms (theTrsf, theXMax, theYMin, theZMin, anU, aV, aW);
  aXMin = Min (anU, aXMin); aXMax = Max (anU, aXMax);
  aYMin = Min (aV,  aYMin); aYMax = Max (aV,  aYMax);
  aZMin = Min (aW,  aZMin); aZMax = Max (aW,  aZMax);

  Graphic3d_Structure::Transforms (theTrsf, theXMax, theYMax, theZMin, anU, aV, aW);
  aXMin = Min (anU, aXMin); aXMax = Max (anU, aXMax);
  aYMin = Min (aV,  aYMin); aYMax = Max (aV,  aYMax);
  aZMin = Min (aW,  aZMin); aZMax = Max (aW,  aZMax);

  Graphic3d_Structure::Transforms (theTrsf, theXMin, theYMax, theZMax, anU, aV, aW);
  aXMin = Min (anU, aXMin); aXMax = Max (anU, aXMax);
  aYMin = Min (aV,  aYMin); aYMax = Max (aV,  aYMax);
  aZMin = Min (aW,  aZMin); aZMax = Max (aW,  aZMax);

  Graphic3d_Structure::Transforms (theTrsf, theXMin, theYMax, theZMin, anU, aV, aW);
  aXMin = Min (anU, aXMin); aXMax = Max (anU, aXMax);
  aYMin = Min (aV,  aYMin); aYMax = Max (aV,  aYMax);
  aZMin = Min (aW,  aZMin); aZMax = Max (aW,  aZMax);

  theXMin = aXMin;
  theYMin = aYMin;
  theZMin = aZMin;
  theXMax = aXMax;
  theYMax = aYMax;
  theZMax = aZMax;
}

//=============================================================================
//function : Network
//purpose  :
//=============================================================================
void Graphic3d_Structure::Network (const Handle(Graphic3d_Structure)& theStructure,
                                   const Graphic3d_TypeOfConnection   theType,
                                   Graphic3d_MapOfStructure&          theSet)
{
  Graphic3d_MapOfStructure aSetD, aSetA;
  theStructure->Descendants (aSetD);
  theStructure->Ancestors   (aSetA);
  theSet.Add (theStructure);
  switch (theType)
  {
    case Graphic3d_TOC_DESCENDANT:
      for (Graphic3d_MapIteratorOfMapOfStructure anIter (aSetD); anIter.More(); anIter.Next())
      {
        Graphic3d_Structure::Network (anIter.Key(), theType, theSet);
      }
      break;
    case Graphic3d_TOC_ANCESTOR:
      for (Graphic3d_MapIteratorOfMapOfStructure anIter (aSetA); anIter.More(); anIter.Next())
      {
        Graphic3d_Structure::Network (anIter.Key (), theType, theSet);
      }
      break;
  }
}

//=============================================================================
//function : PrintNetwork
//purpose  :
//=============================================================================
void Graphic3d_Structure::PrintNetwork (const Handle(Graphic3d_Structure)& theStructure,
                                        const Graphic3d_TypeOfConnection   theType)
{
  Graphic3d_MapOfStructure aSet;
  Graphic3d_Structure::Network (theStructure, theType, aSet);
  for (Graphic3d_MapIteratorOfMapOfStructure anIter (aSet); anIter.More(); anIter.Next())
  {
    std::cout << "\tIdent " << (anIter.Key())->Identification () << "\n";
  }
  std::cout << std::flush;
}

//=============================================================================
//function : Update
//purpose  :
//=============================================================================
void Graphic3d_Structure::Update (const bool theUpdateLayer) const
{
  if (IsDeleted())
  {
    return;
  }

  myStructureManager->Update (theUpdateLayer ? myCStructure->ZLayer() : Graphic3d_ZLayerId_UNKNOWN);
}

//=============================================================================
//function : GraphicTransform
//purpose  :
//=============================================================================
void Graphic3d_Structure::GraphicTransform (const Handle(Geom_Transformation)& theTrsf)
{
  myCStructure->SetTransformation (theTrsf);
}

//=============================================================================
//function : ComputeVisual
//purpose  :
//=============================================================================
Graphic3d_TypeOfStructure Graphic3d_Structure::ComputeVisual() const
{
  return myComputeVisual;
}

//=============================================================================
//function : SetComputeVisual
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetComputeVisual (const Graphic3d_TypeOfStructure theVisual)
{
  // The ComputeVisual is saved only if the structure is declared TOS_ALL, TOS_WIREFRAME or TOS_SHADING.
  // This declaration permits to calculate proper representation of the structure calculated by Compute instead of passage to TOS_COMPUTED.
  if (theVisual != Graphic3d_TOS_COMPUTED)
  {
    myComputeVisual = theVisual;
  }
}

//=============================================================================
//function : SetHLRValidation
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetHLRValidation (const Standard_Boolean theFlag)
{
  myCStructure->HLRValidation = theFlag ? 1 : 0;
}

//=============================================================================
//function : HLRValidation
//purpose  :
//=============================================================================
Standard_Boolean Graphic3d_Structure::HLRValidation() const
{
  // Hidden parts stored in <me> are valid if :
  // 1/ the owner is defined.
  // 2/ they are not invalid.
  return myOwner != NULL
      && myCStructure->HLRValidation != 0;
}

//=======================================================================
//function : SetZLayer
//purpose  :
//=======================================================================
void Graphic3d_Structure::SetZLayer (const Graphic3d_ZLayerId theLayerId)
{
  // if the structure is not displayed, unable to change its display layer
  if (IsDeleted ())
    return;

  myStructureManager->ChangeZLayer (this, theLayerId);
  myCStructure->SetZLayer (theLayerId);
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================
Graphic3d_ZLayerId Graphic3d_Structure::GetZLayer() const
{
  return myCStructure->ZLayer();
}

//=======================================================================
//function : SetClipPlanes
//purpose  :
//=======================================================================
void Graphic3d_Structure::SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  myCStructure->SetClipPlanes (thePlanes);
}

//=======================================================================
//function : GetClipPlanes
//purpose  :
//=======================================================================
const Handle(Graphic3d_SequenceOfHClipPlane)& Graphic3d_Structure::ClipPlanes() const
{
  return myCStructure->ClipPlanes();
}

//=======================================================================
//function : SetMutable
//purpose  :
//=======================================================================
void Graphic3d_Structure::SetMutable (const Standard_Boolean theIsMutable)
{
  myCStructure->IsMutable = theIsMutable;
}

//=======================================================================
//function : IsMutable
//purpose  :
//=======================================================================
Standard_Boolean Graphic3d_Structure::IsMutable() const
{
  return myCStructure->IsMutable;
}
