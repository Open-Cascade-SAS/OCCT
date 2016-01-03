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


#include <Aspect_PolygonOffsetMode.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_PriorityDefinitionError.hxx>
#include <Graphic3d_Structure.hxx>
#include "Graphic3d_Structure.pxx"
#include <Graphic3d_StructureDefinitionError.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_TransformError.hxx>
#include <Graphic3d_Vector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Structure,MMgt_TShared)

//=============================================================================
//function : Graphic3d_Structure
//purpose  :
//=============================================================================
Graphic3d_Structure::Graphic3d_Structure (const Handle(Graphic3d_StructureManager)& theManager)
: myStructureManager      (theManager.operator->()),
  myFirstStructureManager (theManager.operator->()),
  myComputeVisual         (Graphic3d_TOS_ALL),
  myHighlightColor        (Quantity_NOC_WHITE),
  myHighlightMethod       (Aspect_TOHM_COLOR),
  myOwner                 (NULL),
  myVisual                (Graphic3d_TOS_ALL)
{
  myCStructure = theManager->GraphicDriver()->CreateStructure (theManager);

  // default aspects
  Handle(Graphic3d_AspectLine3d)     aAspectLine3d     = new Graphic3d_AspectLine3d();
  Handle(Graphic3d_AspectText3d)     aAspectText3d     = new Graphic3d_AspectText3d();
  Handle(Graphic3d_AspectMarker3d)   aAspectMarker3d   = new Graphic3d_AspectMarker3d();
  Handle(Graphic3d_AspectFillArea3d) aAspectFillArea3d = new Graphic3d_AspectFillArea3d();
  theManager->PrimitivesAspect (aAspectLine3d, aAspectText3d, aAspectMarker3d, aAspectFillArea3d);
  aAspectFillArea3d->SetPolygonOffsets (Aspect_POM_Fill, 1.0, 0.0);

  // update the associated CStructure
  UpdateStructure (aAspectLine3d, aAspectText3d, aAspectMarker3d, aAspectFillArea3d);
}

//=============================================================================
//function : Graphic3d_Structure
//purpose  :
//=============================================================================
Graphic3d_Structure::Graphic3d_Structure (const Handle(Graphic3d_StructureManager)& theManager,
                                          const Handle(Graphic3d_Structure)&        thePrs)
: myStructureManager      (theManager.operator->()),
  myFirstStructureManager (theManager.operator->()),
  myComputeVisual         (thePrs->myComputeVisual),
  myHighlightColor        (thePrs->myHighlightColor),
  myHighlightMethod       (thePrs->myHighlightMethod),
  myOwner                 (thePrs->myOwner),
  myVisual                (thePrs->myVisual)
{
  myCStructure = thePrs->myCStructure->ShadowLink (theManager);

  // default aspects
  Handle(Graphic3d_AspectLine3d)     aAspectLine3d     = new Graphic3d_AspectLine3d();
  Handle(Graphic3d_AspectText3d)     aAspectText3d     = new Graphic3d_AspectText3d();
  Handle(Graphic3d_AspectMarker3d)   aAspectMarker3d   = new Graphic3d_AspectMarker3d();
  Handle(Graphic3d_AspectFillArea3d) aAspectFillArea3d = new Graphic3d_AspectFillArea3d();
  theManager->PrimitivesAspect (aAspectLine3d, aAspectText3d, aAspectMarker3d, aAspectFillArea3d);
  aAspectFillArea3d->SetPolygonOffsets (Aspect_POM_Fill, 1.0, 0.0);

  // update the associated CStructure
  UpdateStructure (aAspectLine3d, aAspectText3d, aAspectMarker3d, aAspectFillArea3d);
}

//=============================================================================
//function : ~Graphic3d_Structure
//purpose  :
//=============================================================================
Graphic3d_Structure::~Graphic3d_Structure()
{
  // as myFirstStructureManager can be already destroyed,
  // avoid attempts to access it
  myFirstStructureManager = NULL;
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

  Update();
}

//=======================================================================
//function : CalculateBoundBox
//purpose  : Calculates AABB of a structure.
//=======================================================================
void Graphic3d_Structure::CalculateBoundBox()
{
  Graphic3d_BndBox4d aBox;
  addTransformed (aBox, Standard_True);
  if (aBox.IsValid())
  {
    Graphic3d_Vec4 aMinPt (RealToShortReal (aBox.CornerMin().x()),
                           RealToShortReal (aBox.CornerMin().y()),
                           RealToShortReal (aBox.CornerMin().z()),
                           1.0f);
    Graphic3d_Vec4 aMaxPt (RealToShortReal (aBox.CornerMax().x()),
                           RealToShortReal (aBox.CornerMax().y()),
                           RealToShortReal (aBox.CornerMax().z()),
                           1.0f);
    myCStructure->ChangeBoundingBox() = Graphic3d_BndBox4f (aMinPt, aMaxPt);
  }
  else
  {
    myCStructure->ChangeBoundingBox().Clear();
  }
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
  myCStructure->GraphicDriver()->RemoveStructure (myCStructure);
  myCStructure.Nullify();

  // Liberation of the identification if the destroyed structure
  // in the first manager that performs creation of the structure.
  if (myFirstStructureManager != NULL)
  {
    myFirstStructureManager->Remove (aStructId);
  }
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
void Graphic3d_Structure::Highlight (const Aspect_TypeOfHighlightMethod theMethod,
                                     const Quantity_Color&              theColor,
                                     const Standard_Boolean             theToUpdateMgr)
{
  if (IsDeleted())
  {
    return;
  }

  myHighlightColor = theColor;

  // Highlight on already Highlighted structure.
  if (myCStructure->highlight)
  {
    Aspect_TypeOfUpdate anUpdateMode = myStructureManager->UpdateMode();
    if (anUpdateMode == Aspect_TOU_WAIT)
    {
      UnHighlight();
    }
    else
    {
      // To avoid call of method : Update()
      // Not useful and can be costly.
      myStructureManager->SetUpdateMode (Aspect_TOU_WAIT);
      UnHighlight();
      myStructureManager->SetUpdateMode (anUpdateMode);
    }
  }

  SetDisplayPriority (Structure_MAX_PRIORITY - 1);

  GraphicHighlight (theMethod);

  if (!theToUpdateMgr)
  {
    return;
  }

  if (myCStructure->stick)
  {
    myStructureManager->Highlight (this, theMethod);
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
  Update();
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

    GraphicUnHighlight();
    myStructureManager->UnHighlight (this);

    ResetDisplayPriority();
    Update();
  }
}

//=============================================================================
//function : HighlightColor
//purpose  :
//=============================================================================
const Quantity_Color& Graphic3d_Structure::HighlightColor() const
{
  return myHighlightColor;
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
  return !myCStructure->Transformation.IsIdentity();
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
//function : PrimitivesAspect
//purpose  :
//=============================================================================
void Graphic3d_Structure::PrimitivesAspect (Handle(Graphic3d_AspectLine3d)&     theAspLine,
                                            Handle(Graphic3d_AspectText3d)&     theAspText,
                                            Handle(Graphic3d_AspectMarker3d)&   theAspMarker,
                                            Handle(Graphic3d_AspectFillArea3d)& theAspFill) const
{
  theAspLine   = Line3dAspect();
  theAspText   = Text3dAspect();
  theAspMarker = Marker3dAspect();
  theAspFill   = FillArea3dAspect();
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
                                                          const TColStd_Array2OfReal& )
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
                                   const TColStd_Array2OfReal& ,
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
//function : Line3dAspect
//purpose  :
//=============================================================================
Handle(Graphic3d_AspectLine3d) Graphic3d_Structure::Line3dAspect() const
{
  const Standard_Real anRGB[3] =
  {
    Standard_Real (myCStructure->ContextLine.Color.r),
    Standard_Real (myCStructure->ContextLine.Color.g),
    Standard_Real (myCStructure->ContextLine.Color.b)
  };
  Quantity_Color aColor;
  aColor.SetValues (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  Aspect_TypeOfLine aLType = Aspect_TypeOfLine (myCStructure->ContextLine.LineType);
  Standard_Real     aWidth = Standard_Real     (myCStructure->ContextLine.Width);

  Handle(Graphic3d_AspectLine3d) anAspLine = new Graphic3d_AspectLine3d (aColor, aLType, aWidth);
  anAspLine->SetShaderProgram (myCStructure->ContextLine.ShaderProgram);
  return anAspLine;
}

//=============================================================================
//function : Text3dAspect
//purpose  :
//=============================================================================
Handle(Graphic3d_AspectText3d) Graphic3d_Structure::Text3dAspect() const
{
  const Standard_Real anRGB[3] =
  {
    Standard_Real (myCStructure->ContextText.Color.r),
    Standard_Real (myCStructure->ContextText.Color.g),
    Standard_Real (myCStructure->ContextText.Color.b)
  };
  Quantity_Color aColor;
  aColor.SetValues (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  Standard_CString         aFont       = Standard_CString         (myCStructure->ContextText.Font);
  Standard_Real            anExpansion = Standard_Real            (myCStructure->ContextText.Expan);
  Standard_Real            aSpace      = Standard_Real            (myCStructure->ContextText.Space);
  Aspect_TypeOfStyleText   aStyle      = Aspect_TypeOfStyleText   (myCStructure->ContextText.Style);
  Aspect_TypeOfDisplayText aDispType   = Aspect_TypeOfDisplayText (myCStructure->ContextText.DisplayType);

  Handle(Graphic3d_AspectText3d) anAspText = new Graphic3d_AspectText3d (aColor, aFont, anExpansion, aSpace, aStyle, aDispType);
  anAspText->SetShaderProgram (myCStructure->ContextText.ShaderProgram);
  return anAspText;
}

//=============================================================================
//function : Marker3dAspect
//purpose  :
//=============================================================================
Handle(Graphic3d_AspectMarker3d) Graphic3d_Structure::Marker3dAspect() const
{
  const Standard_Real anRGB[3] =
  {
    Standard_Real (myCStructure->ContextMarker.Color.r),
    Standard_Real (myCStructure->ContextMarker.Color.g),
    Standard_Real (myCStructure->ContextMarker.Color.b)
  };
  Quantity_Color aColor;
  aColor.SetValues (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  Aspect_TypeOfMarker aMType = myCStructure->ContextMarker.MarkerType;
  Standard_Real       aScale = Standard_Real (myCStructure->ContextMarker.Scale);

  Handle(Graphic3d_AspectMarker3d) anAspMarker = new Graphic3d_AspectMarker3d (aMType, aColor, aScale);
  anAspMarker->SetShaderProgram (myCStructure->ContextMarker.ShaderProgram);
  return anAspMarker;
}

//=============================================================================
//function : FillArea3dAspect
//purpose  :
//=============================================================================
Handle(Graphic3d_AspectFillArea3d) Graphic3d_Structure::FillArea3dAspect() const
{
  // Back Material
  Graphic3d_MaterialAspect aBack;
  aBack.SetShininess    (Standard_Real (myCStructure->ContextFillArea.Back.Shininess));
  aBack.SetAmbient      (Standard_Real (myCStructure->ContextFillArea.Back.Ambient));
  aBack.SetDiffuse      (Standard_Real (myCStructure->ContextFillArea.Back.Diffuse));
  aBack.SetSpecular     (Standard_Real (myCStructure->ContextFillArea.Back.Specular));
  aBack.SetTransparency (Standard_Real (myCStructure->ContextFillArea.Back.Transparency));
  aBack.SetEmissive     (Standard_Real (myCStructure->ContextFillArea.Back.Emission));
  if (myCStructure->ContextFillArea.Back.IsAmbient == 1)
    aBack.SetReflectionModeOn  (Graphic3d_TOR_AMBIENT);
  else
    aBack.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
  if (myCStructure->ContextFillArea.Back.IsDiffuse == 1)
    aBack.SetReflectionModeOn  (Graphic3d_TOR_DIFFUSE);
  else
    aBack.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
  if (myCStructure->ContextFillArea.Back.IsSpecular == 1)
    aBack.SetReflectionModeOn  (Graphic3d_TOR_SPECULAR);
  else
    aBack.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
  if (myCStructure->ContextFillArea.Back.IsEmission == 1)
    aBack.SetReflectionModeOn  (Graphic3d_TOR_EMISSION);
  else
    aBack.SetReflectionModeOff (Graphic3d_TOR_EMISSION);

  Quantity_Color aColor (Standard_Real (myCStructure->ContextFillArea.Back.ColorSpec.r),
                         Standard_Real (myCStructure->ContextFillArea.Back.ColorSpec.g),
                         Standard_Real (myCStructure->ContextFillArea.Back.ColorSpec.b), Quantity_TOC_RGB);
  aBack.SetSpecularColor (aColor);

  aColor.SetValues (Standard_Real (myCStructure->ContextFillArea.Back.ColorAmb.r),
                    Standard_Real (myCStructure->ContextFillArea.Back.ColorAmb.g),
                    Standard_Real (myCStructure->ContextFillArea.Back.ColorAmb.b), Quantity_TOC_RGB);
  aBack.SetAmbientColor (aColor);

  aColor.SetValues (Standard_Real (myCStructure->ContextFillArea.Back.ColorDif.r),
                    Standard_Real (myCStructure->ContextFillArea.Back.ColorDif.g),
                    Standard_Real (myCStructure->ContextFillArea.Back.ColorDif.b), Quantity_TOC_RGB);
  aBack.SetDiffuseColor (aColor);

  aColor.SetValues (Standard_Real (myCStructure->ContextFillArea.Back.ColorEms.r),
                    Standard_Real (myCStructure->ContextFillArea.Back.ColorEms.g),
                    Standard_Real (myCStructure->ContextFillArea.Back.ColorEms.b), Quantity_TOC_RGB);
  aBack.SetEmissiveColor (aColor);

  aBack.SetEnvReflexion (myCStructure->ContextFillArea.Back.EnvReflexion);
  aBack.SetMaterialType (myCStructure->ContextFillArea.Back.IsPhysic ? Graphic3d_MATERIAL_PHYSIC : Graphic3d_MATERIAL_ASPECT);

  aBack.SetRefractionIndex (Standard_Real (myCStructure->ContextFillArea.Back.RefractionIndex));
  aBack.SetBSDF (myCStructure->ContextFillArea.Back.BSDF);

  // Front Material
  Graphic3d_MaterialAspect aFront;
  aFront.SetShininess    (Standard_Real (myCStructure->ContextFillArea.Front.Shininess));
  aFront.SetAmbient      (Standard_Real (myCStructure->ContextFillArea.Front.Ambient));
  aFront.SetDiffuse      (Standard_Real (myCStructure->ContextFillArea.Front.Diffuse));
  aFront.SetSpecular     (Standard_Real (myCStructure->ContextFillArea.Front.Specular));
  aFront.SetTransparency (Standard_Real (myCStructure->ContextFillArea.Front.Transparency));
  aFront.SetEmissive     (Standard_Real (myCStructure->ContextFillArea.Front.Emission));
  if (myCStructure->ContextFillArea.Front.IsAmbient == 1)
    aFront.SetReflectionModeOn  (Graphic3d_TOR_AMBIENT);
  else
    aFront.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
  if (myCStructure->ContextFillArea.Front.IsDiffuse == 1)
    aFront.SetReflectionModeOn  (Graphic3d_TOR_DIFFUSE);
  else
    aFront.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
  if (myCStructure->ContextFillArea.Front.IsSpecular == 1)
    aFront.SetReflectionModeOn  (Graphic3d_TOR_SPECULAR);
  else
    aFront.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
  if (myCStructure->ContextFillArea.Front.Emission == 1)
    aFront.SetReflectionModeOn  (Graphic3d_TOR_EMISSION);
  else
    aFront.SetReflectionModeOff (Graphic3d_TOR_EMISSION);

  aColor.SetValues (Standard_Real (myCStructure->ContextFillArea.Front.ColorSpec.r),
                    Standard_Real (myCStructure->ContextFillArea.Front.ColorSpec.g),
                    Standard_Real (myCStructure->ContextFillArea.Front.ColorSpec.b), Quantity_TOC_RGB);
  aFront.SetSpecularColor (aColor);

  aColor.SetValues (Standard_Real (myCStructure->ContextFillArea.Front.ColorAmb.r),
                    Standard_Real (myCStructure->ContextFillArea.Front.ColorAmb.g),
                    Standard_Real (myCStructure->ContextFillArea.Front.ColorAmb.b), Quantity_TOC_RGB);
  aFront.SetAmbientColor (aColor);

  aColor.SetValues (Standard_Real (myCStructure->ContextFillArea.Front.ColorDif.r),
                    Standard_Real (myCStructure->ContextFillArea.Front.ColorDif.g),
                    Standard_Real (myCStructure->ContextFillArea.Front.ColorDif.b), Quantity_TOC_RGB);
  aFront.SetDiffuseColor (aColor);

  aColor.SetValues (Standard_Real (myCStructure->ContextFillArea.Front.ColorEms.r),
                    Standard_Real (myCStructure->ContextFillArea.Front.ColorEms.g),
                    Standard_Real (myCStructure->ContextFillArea.Front.ColorEms.b), Quantity_TOC_RGB);
  aFront.SetEmissiveColor (aColor);

  aFront.SetEnvReflexion (myCStructure->ContextFillArea.Front.EnvReflexion);
  aFront.SetMaterialType (myCStructure->ContextFillArea.Front.IsPhysic ? Graphic3d_MATERIAL_PHYSIC : Graphic3d_MATERIAL_ASPECT);

  aFront.SetRefractionIndex (Standard_Real (myCStructure->ContextFillArea.Front.RefractionIndex));
  aFront.SetBSDF (myCStructure->ContextFillArea.Front.BSDF);

  Quantity_Color anIntColor  (Standard_Real (myCStructure->ContextFillArea.IntColor.r),
                              Standard_Real (myCStructure->ContextFillArea.IntColor.g),
                              Standard_Real (myCStructure->ContextFillArea.IntColor.b), Quantity_TOC_RGB);
  Quantity_Color anEdgeColor (Standard_Real (myCStructure->ContextFillArea.EdgeColor.r),
                              Standard_Real (myCStructure->ContextFillArea.EdgeColor.g),
                              Standard_Real (myCStructure->ContextFillArea.EdgeColor.b), Quantity_TOC_RGB);
  Handle(Graphic3d_AspectFillArea3d) anAspFill = new Graphic3d_AspectFillArea3d (Aspect_InteriorStyle (myCStructure->ContextFillArea.Style),
                                                                                 anIntColor, anEdgeColor,
                                                                                 Aspect_TypeOfLine    (myCStructure->ContextFillArea.LineType),
                                                                                 Standard_Real        (myCStructure->ContextFillArea.Width),
                                                                                 aFront, aBack);

  // Edges
  if (myCStructure->ContextFillArea.Edge == 1)
    anAspFill->SetEdgeOn();
  else
    anAspFill->SetEdgeOff();
  // Hatch
  anAspFill->SetHatchStyle (Aspect_HatchStyle (myCStructure->ContextFillArea.Hatch));
  // Materials
  // Front and Back face
  if (myCStructure->ContextFillArea.Distinguish == 1)
    anAspFill->SetDistinguishOn();
  else
    anAspFill->SetDistinguishOff();
  if (myCStructure->ContextFillArea.BackFace == 1)
    anAspFill->SuppressBackFace();
  else
    anAspFill->AllowBackFace();
  // Texture
  anAspFill->SetTextureMap (myCStructure->ContextFillArea.Texture.TextureMap);
  if (myCStructure->ContextFillArea.Texture.doTextureMap == 1)
  {
    anAspFill->SetTextureMapOn();
  }
  else
  {
    anAspFill->SetTextureMapOff();
  }
  anAspFill->SetShaderProgram  (myCStructure->ContextFillArea.ShaderProgram);
  anAspFill->SetPolygonOffsets (myCStructure->ContextFillArea.PolygonOffsetMode,
                                myCStructure->ContextFillArea.PolygonOffsetFactor,
                                myCStructure->ContextFillArea.PolygonOffsetUnits);
  return anAspFill;
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
//function : SetPrimitivesAspect
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& theAspLine)
{
  if (IsDeleted()) return;

  Standard_Real     aWidth;
  Quantity_Color    aColor;
  Aspect_TypeOfLine aLType;
  theAspLine->Values (aColor, aLType, aWidth);

  myCStructure->ContextLine.Color.r        = float (aColor.Red());
  myCStructure->ContextLine.Color.g        = float (aColor.Green());
  myCStructure->ContextLine.Color.b        = float (aColor.Blue());
  myCStructure->ContextLine.LineType       = int   (aLType);
  myCStructure->ContextLine.Width          = float (aWidth);
  myCStructure->ContextLine.ShaderProgram  = theAspLine->ShaderProgram();
  myCStructure->ContextLine.IsDef          = 1;

  myCStructure->UpdateAspects();

  // Attributes are "IsSet" during the first update of context (line, marker...)
  myCStructure->ContextLine.IsSet     = 1;
  myCStructure->ContextFillArea.IsSet = 1;
  myCStructure->ContextMarker.IsSet   = 1;
  myCStructure->ContextText.IsSet     = 1;

  Update();
}

//=============================================================================
//function : SetPrimitivesAspect
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspFill)
{
  if (IsDeleted()) return;

  Standard_Real        anRGB[3];
  Standard_Real        aWidth;
  Quantity_Color       anIntColor;
  Quantity_Color       aBackIntColor;
  Quantity_Color       anEdgeColor;
  Aspect_TypeOfLine    aLType;
  Aspect_InteriorStyle aStyle;
  theAspFill->Values (aStyle, anIntColor, aBackIntColor, anEdgeColor, aLType, aWidth);

  anIntColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  myCStructure->ContextFillArea.Style      = aStyle;
  myCStructure->ContextFillArea.IntColor.r = float (anRGB[0]);
  myCStructure->ContextFillArea.IntColor.g = float (anRGB[1]);
  myCStructure->ContextFillArea.IntColor.b = float (anRGB[2]);

  if (theAspFill->Distinguish())
  {
    aBackIntColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  }
  myCStructure->ContextFillArea.BackIntColor.r = float(anRGB[0]);
  myCStructure->ContextFillArea.BackIntColor.g = float(anRGB[1]);
  myCStructure->ContextFillArea.BackIntColor.b = float(anRGB[2]);

  // Edges
  myCStructure->ContextFillArea.Edge        = theAspFill->Edge () ? 1 : 0;
  myCStructure->ContextFillArea.EdgeColor.r = float (anEdgeColor.Red());
  myCStructure->ContextFillArea.EdgeColor.g = float (anEdgeColor.Green());
  myCStructure->ContextFillArea.EdgeColor.b = float (anEdgeColor.Blue());
  myCStructure->ContextFillArea.LineType    = aLType;
  myCStructure->ContextFillArea.Width       = float (aWidth);
  myCStructure->ContextFillArea.Hatch       = theAspFill->HatchStyle();

  // Front and Back face
  myCStructure->ContextFillArea.Distinguish = theAspFill->Distinguish() ? 1 : 0;
  myCStructure->ContextFillArea.BackFace    = theAspFill->BackFace()    ? 1 : 0;

  // Back Material
  const Graphic3d_MaterialAspect& aBack = theAspFill->BackMaterial();
  // Light specificity
  myCStructure->ContextFillArea.Back.Shininess       = float (aBack.Shininess());
  myCStructure->ContextFillArea.Back.Ambient         = float (aBack.Ambient());
  myCStructure->ContextFillArea.Back.Diffuse         = float (aBack.Diffuse());
  myCStructure->ContextFillArea.Back.Specular        = float (aBack.Specular());
  myCStructure->ContextFillArea.Back.Transparency    = float (aBack.Transparency());
  myCStructure->ContextFillArea.Back.RefractionIndex = float (aBack.RefractionIndex());
  myCStructure->ContextFillArea.Back.BSDF            = aBack.BSDF();
  myCStructure->ContextFillArea.Back.Emission        = float (aBack.Emissive());

  // Reflection mode
  myCStructure->ContextFillArea.Back.IsAmbient    = (aBack.ReflectionMode (Graphic3d_TOR_AMBIENT)  ? 1 : 0);
  myCStructure->ContextFillArea.Back.IsDiffuse    = (aBack.ReflectionMode (Graphic3d_TOR_DIFFUSE)  ? 1 : 0);
  myCStructure->ContextFillArea.Back.IsSpecular   = (aBack.ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0);
  myCStructure->ContextFillArea.Back.IsEmission   = (aBack.ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0);

  // Material type
  //JR/Hp
  myCStructure->ContextFillArea.Back.IsPhysic = (aBack.MaterialType (Graphic3d_MATERIAL_PHYSIC) ? 1 : 0 );

  // Specular Color
  myCStructure->ContextFillArea.Back.ColorSpec.r  = float (aBack.SpecularColor().Red());
  myCStructure->ContextFillArea.Back.ColorSpec.g  = float (aBack.SpecularColor().Green());
  myCStructure->ContextFillArea.Back.ColorSpec.b  = float (aBack.SpecularColor().Blue());

  // Ambient color
  myCStructure->ContextFillArea.Back.ColorAmb.r   = float (aBack.AmbientColor().Red());
  myCStructure->ContextFillArea.Back.ColorAmb.g   = float (aBack.AmbientColor().Green());
  myCStructure->ContextFillArea.Back.ColorAmb.b   = float (aBack.AmbientColor().Blue());

  // Diffuse color
  myCStructure->ContextFillArea.Back.ColorDif.r   = float (aBack.DiffuseColor().Red());
  myCStructure->ContextFillArea.Back.ColorDif.g   = float (aBack.DiffuseColor().Green());
  myCStructure->ContextFillArea.Back.ColorDif.b   = float (aBack.DiffuseColor().Blue());

  // Emissive color
  myCStructure->ContextFillArea.Back.ColorEms.r   = float (aBack.EmissiveColor().Red());
  myCStructure->ContextFillArea.Back.ColorEms.g   = float (aBack.EmissiveColor().Green());
  myCStructure->ContextFillArea.Back.ColorEms.b   = float (aBack.EmissiveColor().Blue());

  myCStructure->ContextFillArea.Back.EnvReflexion =
    float ((theAspFill->BackMaterial ()).EnvReflexion());

  // Front Material
  const Graphic3d_MaterialAspect& aFront = theAspFill->FrontMaterial();
  // Light specificity
  myCStructure->ContextFillArea.Front.Shininess       = float (aFront.Shininess());
  myCStructure->ContextFillArea.Front.Ambient         = float (aFront.Ambient());
  myCStructure->ContextFillArea.Front.Diffuse         = float (aFront.Diffuse());
  myCStructure->ContextFillArea.Front.Specular        = float (aFront.Specular());
  myCStructure->ContextFillArea.Front.Transparency    = float (aFront.Transparency());
  myCStructure->ContextFillArea.Front.RefractionIndex = float (aFront.RefractionIndex());
  myCStructure->ContextFillArea.Front.BSDF            = aFront.BSDF();
  myCStructure->ContextFillArea.Front.Emission        = float (aFront.Emissive());

  // Reflection mode
  myCStructure->ContextFillArea.Front.IsAmbient    = (aFront.ReflectionMode (Graphic3d_TOR_AMBIENT)  ? 1 : 0);
  myCStructure->ContextFillArea.Front.IsDiffuse    = (aFront.ReflectionMode (Graphic3d_TOR_DIFFUSE)  ? 1 : 0);
  myCStructure->ContextFillArea.Front.IsSpecular   = (aFront.ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0);
  myCStructure->ContextFillArea.Front.IsEmission   = (aFront.ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0);

  // Materail type
  //JR/Hp
  myCStructure->ContextFillArea.Front.IsPhysic     = (aFront.MaterialType (Graphic3d_MATERIAL_PHYSIC) ? 1 : 0);

  // Specular Color
  myCStructure->ContextFillArea.Front.ColorSpec.r  = float (aFront.SpecularColor().Red());
  myCStructure->ContextFillArea.Front.ColorSpec.g  = float (aFront.SpecularColor().Green());
  myCStructure->ContextFillArea.Front.ColorSpec.b  = float (aFront.SpecularColor().Blue());

  // Ambient color
  myCStructure->ContextFillArea.Front.ColorAmb.r   = float (aFront.AmbientColor().Red());
  myCStructure->ContextFillArea.Front.ColorAmb.g   = float (aFront.AmbientColor().Green());
  myCStructure->ContextFillArea.Front.ColorAmb.b   = float (aFront.AmbientColor().Blue());

  // Diffuse color
  myCStructure->ContextFillArea.Front.ColorDif.r   = float (aFront.DiffuseColor().Red());
  myCStructure->ContextFillArea.Front.ColorDif.g   = float (aFront.DiffuseColor().Green());
  myCStructure->ContextFillArea.Front.ColorDif.b   = float (aFront.DiffuseColor().Blue());

  // Emissive color
  myCStructure->ContextFillArea.Front.ColorEms.r   = float (aFront.EmissiveColor().Red());
  myCStructure->ContextFillArea.Front.ColorEms.g   = float (aFront.EmissiveColor().Green());
  myCStructure->ContextFillArea.Front.ColorEms.b   = float (aFront.EmissiveColor().Blue());

  myCStructure->ContextFillArea.Front.EnvReflexion = float (aFront.EnvReflexion());

  myCStructure->ContextFillArea.IsDef = 1; // Definition material ok

  myCStructure->ContextFillArea.Texture.TextureMap   = theAspFill->TextureMap();
  myCStructure->ContextFillArea.Texture.doTextureMap = theAspFill->TextureMapState() ? 1 : 0;
  myCStructure->ContextFillArea.ShaderProgram        = theAspFill->ShaderProgram();

  Standard_Integer   aPolyMode;
  Standard_ShortReal aPolyFactor, aPolyUnits;
  theAspFill->PolygonOffsets (aPolyMode, aPolyFactor, aPolyUnits);
  myCStructure->ContextFillArea.PolygonOffsetMode   = aPolyMode;
  myCStructure->ContextFillArea.PolygonOffsetFactor = aPolyFactor;
  myCStructure->ContextFillArea.PolygonOffsetUnits  = aPolyUnits;

  myCStructure->UpdateAspects();

  // Attributes are "IsSet" during the first update of context (line, marker...)
  myCStructure->ContextLine.IsSet     = 1;
  myCStructure->ContextFillArea.IsSet = 1;
  myCStructure->ContextMarker.IsSet   = 1;
  myCStructure->ContextText.IsSet     = 1;

  Update();
}

//=============================================================================
//function : SetPrimitivesAspect
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& theAspText)
{
  if (IsDeleted()) return;

  Standard_CString         aFont;
  Standard_Real            aSpace, anExpansion, aTextAngle;
  Quantity_Color           aColor, aColorSub;
  Aspect_TypeOfStyleText   aStyle;
  Aspect_TypeOfDisplayText aDispType;
  Standard_Boolean         isTextZoomable;
  Font_FontAspect          aTextFontAspect;
  theAspText->Values (aColor, aFont, anExpansion, aSpace, aStyle, aDispType, aColorSub, isTextZoomable, aTextAngle, aTextFontAspect);

  myCStructure->ContextText.Color.r         = float (aColor.Red());
  myCStructure->ContextText.Color.g         = float (aColor.Green());
  myCStructure->ContextText.Color.b         = float (aColor.Blue());
  myCStructure->ContextText.Font            = aFont;
  myCStructure->ContextText.Expan           = float (anExpansion);
  myCStructure->ContextText.Space           = float (aSpace);
  myCStructure->ContextText.Style           = aStyle;
  myCStructure->ContextText.DisplayType     = aDispType;
  myCStructure->ContextText.ColorSubTitle.r = float (aColorSub.Red());
  myCStructure->ContextText.ColorSubTitle.g = float (aColorSub.Green());
  myCStructure->ContextText.ColorSubTitle.b = float (aColorSub.Blue());
  myCStructure->ContextText.TextZoomable    = isTextZoomable;
  myCStructure->ContextText.TextAngle       = float (aTextAngle);
  myCStructure->ContextText.TextFontAspect  = aTextFontAspect;
  myCStructure->ContextText.ShaderProgram   = theAspText->ShaderProgram();

  myCStructure->ContextText.IsDef = 1;

  myCStructure->UpdateAspects();

  // Attributes are "IsSet" during the first update of a context (line, marker...)
  myCStructure->ContextLine.IsSet     = 1;
  myCStructure->ContextFillArea.IsSet = 1;
  myCStructure->ContextMarker.IsSet   = 1;
  myCStructure->ContextText.IsSet     = 1;

  Update();
}

//=============================================================================
//function : SetPrimitivesAspect
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& theAspMarker)
{
  if (IsDeleted()) return;

  Standard_Real       aScale;
  Quantity_Color      aColor;
  Aspect_TypeOfMarker aMType;
  theAspMarker->Values (aColor, aMType, aScale);

  myCStructure->ContextMarker.Color.r       = float (aColor.Red());
  myCStructure->ContextMarker.Color.g       = float (aColor.Green());
  myCStructure->ContextMarker.Color.b       = float (aColor.Blue());
  myCStructure->ContextMarker.MarkerType    = aMType;
  myCStructure->ContextMarker.Scale         = float (aScale);
  myCStructure->ContextMarker.ShaderProgram = theAspMarker->ShaderProgram();
  myCStructure->ContextMarker.IsDef         = 1;

  myCStructure->UpdateAspects();

  // Attributes are "IsSet" during the first update of a context (line, marker...)
  myCStructure->ContextLine.IsSet     = 1;
  myCStructure->ContextFillArea.IsSet = 1;
  myCStructure->ContextMarker.IsSet   = 1;
  myCStructure->ContextText.IsSet     = 1;

  Update();
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
    Aspect_TypeOfUpdate anUpdateMode  = myStructureManager->UpdateMode();
    if (anUpdateMode == Aspect_TOU_WAIT)
    {
      Erase();
      myVisual = theVisual;
      SetComputeVisual (theVisual);
      Display();
    }
    else {
      // To avoid calling method : Update ()
      // Not useful and can be costly.
      myStructureManager->SetUpdateMode (Aspect_TOU_WAIT);
      Erase();
      myVisual = theVisual;
      SetComputeVisual (theVisual);
      myStructureManager->SetUpdateMode (anUpdateMode);
      Display();
    }
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

    Update();
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
    Update();
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
void Graphic3d_Structure::SetTransform (const TColStd_Array2OfReal&       theMatrix,
                                        const Graphic3d_TypeOfComposition theType)
{
  if (IsDeleted()) return;

  Standard_Real valuetrsf;
  Standard_Real valueoldtrsf;
  Standard_Real valuenewtrsf;
  TColStd_Array2OfReal aNewTrsf  (0, 3, 0, 3);
  TColStd_Array2OfReal aMatrix44 (0, 3, 0, 3);

  // Assign the new transformation in an array [0..3][0..3]
  // Avoid problems if the user has defined matrix [1..4][1..4]
  //                                            or [3..6][-1..2] !!
  Standard_Integer lr = theMatrix.LowerRow();
  Standard_Integer ur = theMatrix.UpperRow();
  Standard_Integer lc = theMatrix.LowerCol();
  Standard_Integer uc = theMatrix.UpperCol();

  if ((ur - lr + 1 != 4) || (uc - lc + 1 != 4))
  {
    Graphic3d_TransformError::Raise ("Transform : not a 4x4 matrix");
  }

  const Standard_Boolean wasTransformed = IsTransformed();
  switch (theType)
  {
    case Graphic3d_TOC_REPLACE:
    {
      // Update of CStructure
      for (Standard_Integer i = 0; i <= 3; ++i)
      {
        for (Standard_Integer j = 0; j <= 3; ++j)
        {
          myCStructure->Transformation.ChangeValue (i, j) = float (theMatrix (lr + i, lc + j));
          aNewTrsf (i, j) = theMatrix (lr + i, lc + j);
        }
      }
      break;
    }
    case Graphic3d_TOC_POSTCONCATENATE:
    {
      // To simplify management of indices
      for (Standard_Integer i = 0; i <= 3; ++i)
      {
        for (Standard_Integer j = 0; j <= 3; ++j)
        {
          aMatrix44 (i, j) = theMatrix (lr + i, lc + j);
        }
      }

      // Calculation of the product of matrices
      for (Standard_Integer i = 0; i <= 3; ++i)
      {
        for (Standard_Integer j = 0; j <= 3; ++j)
        {
          aNewTrsf (i, j) = 0.0;
          for (Standard_Integer k = 0; k <= 3; ++k)
          {
            valueoldtrsf = myCStructure->Transformation.GetValue (i, k);
            valuetrsf    = aMatrix44 (k, j);
            valuenewtrsf = aNewTrsf (i, j) + valueoldtrsf * valuetrsf;
            aNewTrsf (i, j) = valuenewtrsf;
          }
        }
      }

      // Update of CStructure
      for (Standard_Integer i = 0; i <= 3; ++i)
      {
        for (Standard_Integer j = 0; j <= 3; ++j)
        {
          myCStructure->Transformation.ChangeValue (i, j) = float (aNewTrsf (i, j));
        }
      }
      break;
    }
  }

  // If transformation, no validation of hidden already calculated parts
  if (IsTransformed() || (!IsTransformed() && wasTransformed))
  {
    ReCompute();
  }

  myCStructure->UpdateTransformation();
  myStructureManager->SetTransform (this, aNewTrsf);

  Update();
}

//=============================================================================
//function : Transform
//purpose  :
//=============================================================================
void Graphic3d_Structure::Transform (TColStd_Array2OfReal& theMatrix) const
{

  Standard_Integer lr = theMatrix.LowerRow ();
  Standard_Integer ur = theMatrix.UpperRow ();
  Standard_Integer lc = theMatrix.LowerCol ();
  Standard_Integer uc = theMatrix.UpperCol ();

  if ((ur - lr + 1 != 4) || (uc - lc + 1 != 4))
    Graphic3d_TransformError::Raise ("Transform : not a 4x4 matrix");

  for (Standard_Integer i = 0; i <= 3; ++i)
  {
    for (Standard_Integer j = 0; j <= 3; ++j)
    {
      theMatrix (lr + i, lc + j) = myCStructure->Transformation.GetValue (i, j);
    }
  }
}


//=============================================================================
//function : MinMaxValues
//purpose  :
//=============================================================================
Bnd_Box Graphic3d_Structure::MinMaxValues (const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox4d aBox;
  Bnd_Box aResult;
  addTransformed (aBox, theToIgnoreInfiniteFlag);
  if (aBox.IsValid())
  {
    aResult.Add (gp_Pnt (aBox.CornerMin().x(),
                         aBox.CornerMin().y(),
                         aBox.CornerMin().z()));
    aResult.Add (gp_Pnt (aBox.CornerMax().x(),
                         aBox.CornerMax().y(),
                         aBox.CornerMax().z()));

    Standard_Real aLimMin = ShortRealFirst() + 1.0;
    Standard_Real aLimMax = ShortRealLast() - 1.0;
    gp_Pnt aMin = aResult.CornerMin();
    gp_Pnt aMax = aResult.CornerMax();
    if (aMin.X() < aLimMin && aMin.Y() < aLimMin && aMin.Z() < aLimMin &&
        aMax.X() > aLimMax && aMax.Y() > aLimMax && aMax.Z() > aLimMax)
    {
      //For structure which infinite in all three dimensions the Whole bounding box will be returned
      aResult.SetWhole();
    }
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
void Graphic3d_Structure::SetTransformPersistence (const Graphic3d_TransModeFlags& theFlag)
{
  SetTransformPersistence (theFlag, gp_Pnt (0.0, 0.0, 0.0));
}

//=============================================================================
//function : SetTransformPersistence
//purpose  :
//=============================================================================
void Graphic3d_Structure::SetTransformPersistence (const Graphic3d_TransModeFlags& theFlag,
                                                   const gp_Pnt&                   thePoint)
{
  if (IsDeleted()) return;

  myCStructure->TransformPersistence.Flags     = theFlag;
  myCStructure->TransformPersistence.Point.x() = thePoint.X();
  myCStructure->TransformPersistence.Point.y() = thePoint.Y();
  myCStructure->TransformPersistence.Point.z() = thePoint.Z();
}

//=============================================================================
//function : TransformPersistenceMode
//purpose  :
//=============================================================================
Graphic3d_TransModeFlags Graphic3d_Structure::TransformPersistenceMode() const
{
  return myCStructure->TransformPersistence.Flags;
}

//=============================================================================
//function : TransformPersistencePoint
//purpose  :
//=============================================================================
gp_Pnt Graphic3d_Structure::TransformPersistencePoint() const
{
  gp_Pnt aPnt (0.0, 0.0, 0.0);
  aPnt.SetX (myCStructure->TransformPersistence.Point.x());
  aPnt.SetY (myCStructure->TransformPersistence.Point.y());
  aPnt.SetZ (myCStructure->TransformPersistence.Point.z());
  return aPnt;
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
void Graphic3d_Structure::getBox (Graphic3d_BndBox4d&    theBox,
                                  const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox4f aBoxF = minMaxCoord();
  if (aBoxF.IsValid())
  {
    theBox = Graphic3d_BndBox4d (Graphic3d_Vec4d ((Standard_Real )aBoxF.CornerMin().x(),
                                                  (Standard_Real )aBoxF.CornerMin().y(),
                                                  (Standard_Real )aBoxF.CornerMin().z(),
                                                  (Standard_Real )aBoxF.CornerMin().w()),
                                 Graphic3d_Vec4d ((Standard_Real )aBoxF.CornerMax().x(),
                                                  (Standard_Real )aBoxF.CornerMax().y(),
                                                  (Standard_Real )aBoxF.CornerMax().z(),
                                                  (Standard_Real )aBoxF.CornerMax().w()));
    if (IsInfinite()
    && !theToIgnoreInfiniteFlag)
    {
      const Graphic3d_Vec4d aDiagVec = theBox.CornerMax() - theBox.CornerMin();
      if (aDiagVec.xyz().SquareModulus() >= 500000.0 * 500000.0)
      {
        // bounding borders of infinite line has been calculated as own point in center of this line
        theBox = Graphic3d_BndBox4d ((theBox.CornerMin() + theBox.CornerMax()) * 0.5);
      }
      else
      {
        theBox = Graphic3d_BndBox4d (Graphic3d_Vec4d (RealFirst(), RealFirst(), RealFirst(), 1.0),
                                     Graphic3d_Vec4d (RealLast(),  RealLast(),  RealLast(),  1.0));
        return;
      }
    }
  }
}

//=============================================================================
//function : addTransformed
//purpose  :
//=============================================================================
void Graphic3d_Structure::addTransformed (Graphic3d_BndBox4d&    theBox,
                                          const Standard_Boolean theToIgnoreInfiniteFlag) const
{
  Graphic3d_BndBox4d aCombinedBox, aBox;
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
    TColStd_Array2OfReal aTrsf (0, 3, 0, 3);
    Transform (aTrsf);
    TransformBoundaries (aTrsf, aBox.CornerMin().x(), aBox.CornerMin().y(), aBox.CornerMin().z(),
                                aBox.CornerMax().x(), aBox.CornerMax().y(), aBox.CornerMax().z());

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
void Graphic3d_Structure::Transforms (const TColStd_Array2OfReal& theTrsf,
                                      const Standard_Real theX,    const Standard_Real theY,    const Standard_Real theZ,
                                      Standard_Real&      theNewX, Standard_Real&      theNewY, Standard_Real&      theNewZ)
{
  const Standard_Real aRL = RealLast();
  const Standard_Real aRF = RealFirst();
  if ((theX == aRF) || (theY == aRF) || (theZ == aRF)
   || (theX == aRL) || (theY == aRL) || (theZ == aRL))
  {
    theNewX = theX;
    theNewY = theY;
    theNewZ = theZ;
  }
  else
  {
    Standard_Real A, B, C, D;
    A       = theTrsf (0, 0);
    B       = theTrsf (0, 1);
    C       = theTrsf (0, 2);
    D       = theTrsf (0, 3);
    theNewX = A * theX + B * theY + C * theZ + D;
    A       = theTrsf (1, 0);
    B       = theTrsf (1, 1);
    C       = theTrsf (1, 2);
    D       = theTrsf (1, 3);
    theNewY = A * theX + B * theY + C * theZ + D;
    A       = theTrsf (2, 0);
    B       = theTrsf (2, 1);
    C       = theTrsf (2, 2);
    D       = theTrsf (2, 3);
    theNewZ = A * theX + B * theY + C * theZ + D;
  }
}

//=============================================================================
//function : Transforms
//purpose  :
//=============================================================================
Graphic3d_Vector Graphic3d_Structure::Transforms (const TColStd_Array2OfReal& theTrsf,
                                                  const Graphic3d_Vector&     theCoord)
{
  Standard_Real anXYZ[3];
  Graphic3d_Structure::Transforms (theTrsf,
                                   theCoord.X(), theCoord.Y(), theCoord.Z(),
                                   anXYZ[0], anXYZ[1], anXYZ[2]);
  return Graphic3d_Vector (anXYZ[0], anXYZ[1], anXYZ[2]);
}

//=============================================================================
//function : Transforms
//purpose  :
//=============================================================================
Graphic3d_Vertex Graphic3d_Structure::Transforms (const TColStd_Array2OfReal& theTrsf,
                                                  const Graphic3d_Vertex&     theCoord)
{
  Standard_Real anXYZ[3];
  Graphic3d_Structure::Transforms (theTrsf,
                                   theCoord.X(), theCoord.Y(), theCoord.Z(),
                                   anXYZ[0], anXYZ[1], anXYZ[2]);
  return Graphic3d_Vertex (anXYZ[0], anXYZ[1], anXYZ[2]);
}

//=============================================================================
//function : Transforms
//purpose  :
//=============================================================================
void Graphic3d_Structure::TransformBoundaries (const TColStd_Array2OfReal& theTrsf,
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
void Graphic3d_Structure::Update() const
{
  if (IsDeleted())
  {
    return;
  }

  myStructureManager->Update (myStructureManager->UpdateMode());
}

//=============================================================================
//function : UpdateStructure
//purpose  :
//=============================================================================
void Graphic3d_Structure::UpdateStructure (const Handle(Graphic3d_AspectLine3d)&     theAspLine,
                                           const Handle(Graphic3d_AspectText3d)&     theAspText,
                                           const Handle(Graphic3d_AspectMarker3d)&   theAspMarker,
                                           const Handle(Graphic3d_AspectFillArea3d)& theAspFill)
{
  Standard_CString          aFont;
  Standard_Real             aSpace, anExpansion, aWidth, aScale;
  Quantity_Color            aColor, anIntColor, aBackIntColor, anEdgeColor, aColorSub;
  Aspect_TypeOfLine         aLType;
  Aspect_TypeOfMarker       aMType;
  Aspect_InteriorStyle      aStyle;
  Aspect_TypeOfStyleText    aStyleT;
  Aspect_TypeOfDisplayText  aDisplayType;
  Standard_Boolean          aTextZoomable;
  Standard_Real             aTextAngle;
  Font_FontAspect           aTextFontAspect;

  theAspLine->Values (aColor, aLType, aWidth);
  myCStructure->ContextLine.Color.r        = float (aColor.Red());
  myCStructure->ContextLine.Color.g        = float (aColor.Green());
  myCStructure->ContextLine.Color.b        = float (aColor.Blue());
  myCStructure->ContextLine.LineType       = aLType;
  myCStructure->ContextLine.Width          = float (aWidth);
  myCStructure->ContextLine.ShaderProgram  = theAspLine->ShaderProgram();

  theAspMarker->Values (aColor, aMType, aScale);
  myCStructure->ContextMarker.Color.r      = float (aColor.Red());
  myCStructure->ContextMarker.Color.g      = float (aColor.Green());
  myCStructure->ContextMarker.Color.b      = float (aColor.Blue());
  myCStructure->ContextMarker.MarkerType   = aMType;
  myCStructure->ContextMarker.Scale        = float (aScale);
  myCStructure->ContextMarker.ShaderProgram = theAspMarker->ShaderProgram();

  theAspText->Values (aColor, aFont, anExpansion, aSpace, aStyleT, aDisplayType, aColorSub, aTextZoomable, aTextAngle, aTextFontAspect);
  myCStructure->ContextText.Color.r          = float (aColor.Red());
  myCStructure->ContextText.Color.g          = float (aColor.Green());
  myCStructure->ContextText.Color.b          = float (aColor.Blue());
  myCStructure->ContextText.Font             = aFont;
  myCStructure->ContextText.Expan            = float (anExpansion);
  myCStructure->ContextText.Style            = aStyleT;
  myCStructure->ContextText.DisplayType      = aDisplayType;
  myCStructure->ContextText.Space            = float (aSpace);
  myCStructure->ContextText.ColorSubTitle.r  = float (aColorSub.Red());
  myCStructure->ContextText.ColorSubTitle.g  = float (aColorSub.Green());
  myCStructure->ContextText.ColorSubTitle.b  = float (aColorSub.Blue());
  myCStructure->ContextText.TextZoomable     = aTextZoomable;
  myCStructure->ContextText.TextAngle        = float (aTextAngle);
  myCStructure->ContextText.TextFontAspect   = aTextFontAspect;
  myCStructure->ContextText.ShaderProgram    = theAspText->ShaderProgram();

  Standard_Real anRGB[3];
  theAspFill->Values (aStyle, anIntColor, aBackIntColor, anEdgeColor, aLType, aWidth);
  anIntColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  myCStructure->ContextFillArea.Style      = aStyle;
  myCStructure->ContextFillArea.IntColor.r = float (anRGB[0]);
  myCStructure->ContextFillArea.IntColor.g = float (anRGB[1]);
  myCStructure->ContextFillArea.IntColor.b = float (anRGB[2]);

  if (theAspFill->Distinguish())
  {
    aBackIntColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  }
  myCStructure->ContextFillArea.BackIntColor.r = float (anRGB[0]);
  myCStructure->ContextFillArea.BackIntColor.g = float (anRGB[1]);
  myCStructure->ContextFillArea.BackIntColor.b = float (anRGB[2]);

  // Edges
  myCStructure->ContextFillArea.Edge               = theAspFill->Edge () ? 1:0;
  myCStructure->ContextFillArea.EdgeColor.r        = float (anEdgeColor.Red());
  myCStructure->ContextFillArea.EdgeColor.g        = float (anEdgeColor.Green());
  myCStructure->ContextFillArea.EdgeColor.b        = float (anEdgeColor.Blue());
  myCStructure->ContextFillArea.LineType           = aLType;
  myCStructure->ContextFillArea.Width              = float (aWidth);
  myCStructure->ContextFillArea.Hatch              = theAspFill->HatchStyle();

  // Front and Back face
  myCStructure->ContextFillArea.Distinguish        = theAspFill->Distinguish() ? 1 : 0;
  myCStructure->ContextFillArea.BackFace           = theAspFill->BackFace()    ? 1 : 0;
  // Back Material
  const Graphic3d_MaterialAspect& aBack = theAspFill->BackMaterial();
  // Light specificity
  myCStructure->ContextFillArea.Back.Shininess     = float (aBack.Shininess());
  myCStructure->ContextFillArea.Back.Ambient       = float (aBack.Ambient());
  myCStructure->ContextFillArea.Back.Diffuse       = float (aBack.Diffuse());
  myCStructure->ContextFillArea.Back.Specular      = float (aBack.Specular());
  myCStructure->ContextFillArea.Back.Transparency  = float (aBack.Transparency());
  myCStructure->ContextFillArea.Back.Emission      = float (aBack.Emissive());

  // Reflection mode
  myCStructure->ContextFillArea.Back.IsAmbient     = (aBack.ReflectionMode (Graphic3d_TOR_AMBIENT)  ? 1 : 0);
  myCStructure->ContextFillArea.Back.IsDiffuse     = (aBack.ReflectionMode (Graphic3d_TOR_DIFFUSE)  ? 1 : 0);
  myCStructure->ContextFillArea.Back.IsSpecular    = (aBack.ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0);
  myCStructure->ContextFillArea.Back.IsEmission    = (aBack.ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0);

  // Material type
  myCStructure->ContextFillArea.Back.IsPhysic      = (aBack.MaterialType (Graphic3d_MATERIAL_PHYSIC) ? 1 : 0);

  // Specular color
  myCStructure->ContextFillArea.Back.ColorSpec.r   = float (aBack.SpecularColor().Red());
  myCStructure->ContextFillArea.Back.ColorSpec.g   = float (aBack.SpecularColor().Green());
  myCStructure->ContextFillArea.Back.ColorSpec.b   = float (aBack.SpecularColor().Blue());

  // Ambient color
  myCStructure->ContextFillArea.Back.ColorAmb.r    = float (aBack.AmbientColor().Red());
  myCStructure->ContextFillArea.Back.ColorAmb.g    = float (aBack.AmbientColor().Green());
  myCStructure->ContextFillArea.Back.ColorAmb.b    = float (aBack.AmbientColor().Blue());

  // Diffuse color
  myCStructure->ContextFillArea.Back.ColorDif.r    = float (aBack.DiffuseColor().Red());
  myCStructure->ContextFillArea.Back.ColorDif.g    = float (aBack.DiffuseColor().Green());
  myCStructure->ContextFillArea.Back.ColorDif.b    = float (aBack.DiffuseColor().Blue());

  // Emissive color
  myCStructure->ContextFillArea.Back.ColorEms.r    = float (aBack.EmissiveColor().Red());
  myCStructure->ContextFillArea.Back.ColorEms.g    = float (aBack.EmissiveColor().Green());
  myCStructure->ContextFillArea.Back.ColorEms.b    = float (aBack.EmissiveColor().Blue());

  myCStructure->ContextFillArea.Back.EnvReflexion  = float (aBack.EnvReflexion());

  // Front Material
  const Graphic3d_MaterialAspect& aFront = theAspFill->FrontMaterial();
  // Light specificity
  myCStructure->ContextFillArea.Front.Shininess    = float (aFront.Shininess());
  myCStructure->ContextFillArea.Front.Ambient      = float (aFront.Ambient());
  myCStructure->ContextFillArea.Front.Diffuse      = float (aFront.Diffuse());
  myCStructure->ContextFillArea.Front.Specular     = float (aFront.Specular());
  myCStructure->ContextFillArea.Front.Transparency = float (aFront.Transparency());
  myCStructure->ContextFillArea.Front.Emission     = float (aFront.Emissive());

  // Reflection mode
  myCStructure->ContextFillArea.Front.IsAmbient    = (aFront.ReflectionMode (Graphic3d_TOR_AMBIENT)  ? 1 : 0);
  myCStructure->ContextFillArea.Front.IsDiffuse    = (aFront.ReflectionMode (Graphic3d_TOR_DIFFUSE)  ? 1 : 0);
  myCStructure->ContextFillArea.Front.IsSpecular   = (aFront.ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0);
  myCStructure->ContextFillArea.Front.IsEmission   = (aFront.ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0);

  // Material type
  myCStructure->ContextFillArea.Front.IsPhysic     = (aFront.MaterialType (Graphic3d_MATERIAL_PHYSIC) ? 1 : 0);

  // Specular color
  myCStructure->ContextFillArea.Front.ColorSpec.r  = float (aFront.SpecularColor().Red());
  myCStructure->ContextFillArea.Front.ColorSpec.g  = float (aFront.SpecularColor().Green());
  myCStructure->ContextFillArea.Front.ColorSpec.b  = float (aFront.SpecularColor().Blue());

  // Ambient color
  myCStructure->ContextFillArea.Front.ColorAmb.r   = float (aFront.AmbientColor().Red());
  myCStructure->ContextFillArea.Front.ColorAmb.g   = float (aFront.AmbientColor().Green());
  myCStructure->ContextFillArea.Front.ColorAmb.b   = float (aFront.AmbientColor().Blue());

  // Diffuse color
  myCStructure->ContextFillArea.Front.ColorDif.r   = float (aFront.DiffuseColor().Red());
  myCStructure->ContextFillArea.Front.ColorDif.g   = float (aFront.DiffuseColor().Green());
  myCStructure->ContextFillArea.Front.ColorDif.b   = float (aFront.DiffuseColor().Blue());

  // Emissive color
  myCStructure->ContextFillArea.Front.ColorEms.r   = float (aFront.EmissiveColor().Red());
  myCStructure->ContextFillArea.Front.ColorEms.g   = float (aFront.EmissiveColor().Green());
  myCStructure->ContextFillArea.Front.ColorEms.b   = float (aFront.EmissiveColor().Blue());

  myCStructure->ContextFillArea.Front.EnvReflexion = float (aFront.EnvReflexion());

  myCStructure->ContextFillArea.Texture.TextureMap   = theAspFill->TextureMap();
  myCStructure->ContextFillArea.Texture.doTextureMap = theAspFill->TextureMapState() ? 1 : 0;
  myCStructure->ContextFillArea.ShaderProgram        = theAspFill->ShaderProgram();

  Standard_Integer   aPolyMode;
  Standard_ShortReal aPolyFactor, aPolyUnits;
  theAspFill->PolygonOffsets (aPolyMode, aPolyFactor, aPolyUnits);
  myCStructure->ContextFillArea.PolygonOffsetMode   = aPolyMode;
  myCStructure->ContextFillArea.PolygonOffsetFactor = aPolyFactor;
  myCStructure->ContextFillArea.PolygonOffsetUnits  = aPolyUnits;
}

//=============================================================================
//function : GraphicHighlight
//purpose  :
//=============================================================================
void Graphic3d_Structure::GraphicHighlight (const Aspect_TypeOfHighlightMethod theMethod)
{
  Standard_Real anRGB[3];
  myCStructure->highlight = 1;
  myHighlightMethod = theMethod;
  switch (theMethod)
  {
    case Aspect_TOHM_COLOR:
    {
      myHighlightColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
      myCStructure->HighlightWithColor (Graphic3d_Vec3 (float (anRGB[0]), float (anRGB[1]), float (anRGB[2])), Standard_True);
      break;
    }
    case Aspect_TOHM_BOUNDBOX:
    {
      myHighlightColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
      myCStructure->HighlightColor.r = float (anRGB[0]);
      myCStructure->HighlightColor.g = float (anRGB[1]);
      myCStructure->HighlightColor.b = float (anRGB[2]);
      myCStructure->HighlightWithBndBox (this, Standard_True);
      break;
    }
  }
}

//=============================================================================
//function : GraphicTransform
//purpose  :
//=============================================================================
void Graphic3d_Structure::GraphicTransform (const TColStd_Array2OfReal& theMatrix)
{
  for (Standard_Integer i = 0; i <= 3; ++i)
  {
    for (Standard_Integer j = 0; j <= 3; ++j)
    {
      myCStructure->Transformation.ChangeValue (i, j) = float (theMatrix (i, j));
    }
  }
  myCStructure->UpdateTransformation();
}

//=============================================================================
//function : GraphicUnHighlight
//purpose  :
//=============================================================================
void Graphic3d_Structure::GraphicUnHighlight()
{
  myCStructure->highlight = 0;
  switch (myHighlightMethod)
  {
    case Aspect_TOHM_COLOR:
      myCStructure->HighlightWithColor (Graphic3d_Vec3 (0.0f, 0.0f, 0.0f), Standard_False);
      break;
    case Aspect_TOHM_BOUNDBOX:
      myCStructure->HighlightWithBndBox (this, Standard_False);
      break;
  }
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
void Graphic3d_Structure::SetClipPlanes (const Graphic3d_SequenceOfHClipPlane& thePlanes)
{
  myCStructure->SetClipPlanes (thePlanes);
}

//=======================================================================
//function : GetClipPlanes
//purpose  :
//=======================================================================
const Graphic3d_SequenceOfHClipPlane& Graphic3d_Structure::GetClipPlanes() const
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
