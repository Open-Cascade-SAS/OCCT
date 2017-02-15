// Created on: 1995-10-09
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1995-1999 Matra Datavision
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

#include <AIS_Trihedron.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_TrihedronOwner.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>

#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_ToolSphere.hxx>

#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Trihedron, AIS_InteractiveObject)

//=======================================================================
//function : AIS_Trihedron
//purpose  :
//=======================================================================
AIS_Trihedron::AIS_Trihedron (const Handle(Geom_Axis2Placement)& theComponent)
: myHasOwnSize (Standard_False),
  myHasOwnTextColor (Standard_False),
  myHasOwnArrowColor (Standard_False),
  myTrihDispMode (Prs3d_DM_WireFrame),
  myComponent (theComponent)
{
  // selection priorities
  mySelectionPriority.Bind (Prs3d_DP_None,   5); // complete triedron: priority 5 (same as faces)
  mySelectionPriority.Bind (Prs3d_DP_Origin, 8); // origin: priority 8
  for (int aPartIter = Prs3d_DP_XAxis; aPartIter <= Prs3d_DP_ZAxis; ++aPartIter)
  {
    mySelectionPriority.Bind ((Prs3d_DatumParts )aPartIter, 7); // axes: priority: 7
  }
  for (int aPartIter = Prs3d_DP_XOYAxis; aPartIter <= Prs3d_DP_XOZAxis; ++aPartIter)
  {
    mySelectionPriority.Bind ((Prs3d_DatumParts )aPartIter, 5); // planes: priority: 5
  }

  // trihedron labels
  myLabel.Bind (Prs3d_DP_XAxis, "X");
  myLabel.Bind (Prs3d_DP_YAxis, "Y");
  myLabel.Bind (Prs3d_DP_ZAxis, "Z");
}

//=======================================================================
//function : SetComponent
//purpose  :
//=======================================================================
void AIS_Trihedron::SetComponent (const Handle(Geom_Axis2Placement)& theComponent)
{
  myComponent = theComponent;
  LoadRecomputable (AIS_WireFrame);
}

//=======================================================================
//function : setOwnDatumAspect
//purpose  :
//=======================================================================
void AIS_Trihedron::setOwnDatumAspect()
{
  if (myDrawer->HasOwnDatumAspect())
    return;

  Handle(Prs3d_DatumAspect) aNewAspect = new Prs3d_DatumAspect();
  myDrawer->SetDatumAspect(aNewAspect);

  if (myDrawer->Link().IsNull())
    return;

  *myDrawer->DatumAspect()->TextAspect()->Aspect() =
                                     *myDrawer->Link()->DatumAspect()->TextAspect()->Aspect();
  *myDrawer->DatumAspect()->PointAspect()->Aspect() =
                                     *myDrawer->Link()->DatumAspect()->PointAspect()->Aspect();
  *myDrawer->DatumAspect()->ArrowAspect()->Aspect() =
                                     *myDrawer->Link()->DatumAspect()->ArrowAspect()->Aspect();

  for (int aPartIter = Prs3d_DP_Origin; aPartIter <= Prs3d_DP_XOZAxis; ++aPartIter)
  {
    const Prs3d_DatumParts aPart = (Prs3d_DatumParts )aPartIter;
    if (!aNewAspect->LineAspect(aPart).IsNull())
      *myDrawer->DatumAspect()->LineAspect(aPart)->Aspect() =
                                 *myDrawer->Link()->DatumAspect()->LineAspect(aPart)->Aspect();
    if (!aNewAspect->ShadingAspect(aPart).IsNull())
      *myDrawer->DatumAspect()->ShadingAspect(aPart)->Aspect() =
                                 *myDrawer->Link()->DatumAspect()->ShadingAspect(aPart)->Aspect();
  }
}

//=======================================================================
//function : SetSize
//purpose  :
//=======================================================================
void AIS_Trihedron::SetSize(const Standard_Real aValue)
{
  myHasOwnSize = Standard_True;

  setOwnDatumAspect();
  myDrawer->DatumAspect()->SetAxisLength(aValue, aValue, aValue);

  Update();
  UpdateSelection();
}

//=======================================================================
//function : UnsetSize
//purpose  :
//=======================================================================
void AIS_Trihedron::UnsetSize()
{
  if (!myHasOwnSize)
  {
    return;
  }

  myHasOwnSize = Standard_False;
  if (hasOwnColor)
  {
    const Handle(Prs3d_DatumAspect) DA = myDrawer->HasLink()
                                       ? myDrawer->Link()->DatumAspect()
                                       : new Prs3d_DatumAspect();
    myDrawer->DatumAspect()->SetAxisLength (DA->AxisLength (Prs3d_DP_XAxis),
                                            DA->AxisLength (Prs3d_DP_YAxis),
                                            DA->AxisLength (Prs3d_DP_ZAxis));
  }
  else
  {
    Update();
  }
  UpdateSelection();
}

//=======================================================================
//function : Size
//purpose  :
//=======================================================================
Standard_Real AIS_Trihedron::Size() const 
{
  return myDrawer->DatumAspect()->AxisLength(Prs3d_DP_XAxis);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_Trihedron::Compute (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                             const Handle(Prs3d_Presentation)& thePrs,
                             const Standard_Integer theMode)
{
  if (theMode != 0)
  {
    return;
  }

  thePrs->SetInfiniteState (Standard_True);

  gp_Ax2 anAxis (myComponent->Ax2());
  updatePrimitives (myDrawer->DatumAspect(), myTrihDispMode, anAxis.Location(),
                    anAxis.XDirection(), anAxis.YDirection(), anAxis.Direction());
  computePresentation (thePrsMgr, thePrs);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_Trihedron::Compute (const Handle(Prs3d_Projector)& theProjector,
                             const Handle(Geom_Transformation)& theTrsf,
                             const Handle(Prs3d_Presentation)& thePrs)
{
  PrsMgr_PresentableObject::Compute (theProjector, theTrsf, thePrs);
}

//=======================================================================
//function : ComputeSelection
//purpose  :
//=======================================================================
void AIS_Trihedron::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                      const Standard_Integer theMode)
{
  Handle(Prs3d_DatumAspect) anAspect = myDrawer->DatumAspect();
  switch (theMode)
  {
    case AIS_TrihedronSelectionMode_EntireObject:
    {
      Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (
                                        this, mySelectionPriority.Find (Prs3d_DP_None));
      const bool isShadingMode = myTrihDispMode == Prs3d_DM_Shaded;
      for (int aPartIter = isShadingMode ? Prs3d_DP_Origin : Prs3d_DP_XAxis; aPartIter <= Prs3d_DP_ZAxis;
           ++aPartIter)
      {
        const Prs3d_DatumParts aPart = (Prs3d_DatumParts )aPartIter;
        if (!anAspect->DrawDatumPart (aPart))
        {
          continue;
        }
        theSelection->Add (createSensitiveEntity (aPart, anOwner));
      }
      break;
    }
    case AIS_TrihedronSelectionMode_Origin:
    {
      const Prs3d_DatumParts aPart = Prs3d_DP_Origin;
      if (anAspect->DrawDatumPart (aPart))
      {
        Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart,
                                                                 mySelectionPriority.Find (aPart));
        Handle(Graphic3d_ArrayOfPrimitives) aPrimitives = arrayOfPrimitives(aPart);
        theSelection->Add (createSensitiveEntity (aPart, anOwner));
      }
      break;
    }
    case AIS_TrihedronSelectionMode_Axes:
    {
      for (int aPartIter = Prs3d_DP_XAxis; aPartIter <= Prs3d_DP_ZAxis; ++aPartIter)
      {
        const Prs3d_DatumParts aPart = (Prs3d_DatumParts )aPartIter;
        if (!anAspect->DrawDatumPart (aPart))
        {
          continue;
        }
        Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart,
                                                                        mySelectionPriority.Find (aPart));
        theSelection->Add (createSensitiveEntity (aPart, anOwner));
      }
      break;
    }
    case AIS_TrihedronSelectionMode_MainPlanes:
    {
      // create owner for each trihedron plane
      {
        for (int aPartIter = Prs3d_DP_XOYAxis; aPartIter <= Prs3d_DP_XOZAxis; ++aPartIter)
        {
          const Prs3d_DatumParts aPart = (Prs3d_DatumParts )aPartIter;
          if (!anAspect->DrawDatumPart (aPart))
          {
            continue;
          }
          Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart,
                                                                          mySelectionPriority.Find (aPart));
          theSelection->Add (createSensitiveEntity (aPart, anOwner));
        }
      }
      break;
    }
  }
}

//=======================================================================
//function : HilightOwnerWithColor
//purpose  :
//=======================================================================
void AIS_Trihedron::HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                           const Handle(Prs3d_Drawer)& theStyle,
                                           const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle(AIS_TrihedronOwner) anOwner = Handle(AIS_TrihedronOwner)::DownCast (theOwner);
  if (anOwner.IsNull())
  {
    /// default 0 selection mode
    Standard_Integer aHiMode = HasHilightMode() ? HilightMode() : 0;
    thePM->Color (this, theStyle, aHiMode, NULL, Graphic3d_ZLayerId_Top);
    return;
  }

  Handle(Prs3d_Presentation) aPresentation = GetHilightPresentation (thePM);
  if (aPresentation.IsNull())
  {
    return;
  }

  aPresentation->Clear();
  const Prs3d_DatumParts aPart = anOwner->DatumPart();
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (aPresentation);
  Handle(Prs3d_DatumAspect) anAspect = myDrawer->DatumAspect();
  if (aPart >= Prs3d_DP_XOYAxis && aPart <= Prs3d_DP_XOZAxis)
  {
    // planes selection is equal in both shading and wireframe mode
    aGroup->SetGroupPrimitivesAspect (getHighlightLineAspect()->Aspect());
  }
  else
  {
    if (myTrihDispMode == Prs3d_DM_Shaded)
    {
      aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect(aPart)->Aspect());
    }
    else
    {
      if (aPart == Prs3d_DP_Origin)
      {
        aGroup->SetGroupPrimitivesAspect (getHighlightPointAspect()->Aspect());
      }
      else
      {
        aGroup->SetGroupPrimitivesAspect (anAspect->LineAspect(aPart)->Aspect());
      }
    }
  }
  aGroup->AddPrimitiveArray (arrayOfPrimitives(aPart));

  aPresentation->Highlight (theStyle);
  thePM->AddToImmediateList (aPresentation);
}

//========================================================================
//function : HilightSelected
//purpose  :
//========================================================================
void AIS_Trihedron::HilightSelected (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                     const SelectMgr_SequenceOfOwner& theOwners)
{
  if (theOwners.IsEmpty() || !HasInteractiveContext())
  {
    return;
  }

  Handle(Prs3d_DatumAspect) anAspect = myDrawer->DatumAspect();
  const bool isShadingMode = myTrihDispMode == Prs3d_DM_Shaded;

  const Handle(Prs3d_Drawer)& aContextSelStyle = GetContext()->SelectionStyle();
  const Quantity_Color& aSelectionColor = aContextSelStyle->Color();
  for (SelectMgr_SequenceOfOwner::Iterator anIterator (theOwners); anIterator.More(); anIterator.Next())
  {
    const Handle(SelectMgr_EntityOwner)& anOwner = anIterator.Value();
    Handle(AIS_TrihedronOwner) aTrihedronOwner = Handle(AIS_TrihedronOwner)::DownCast(anOwner);
    if (aTrihedronOwner.IsNull())
    {
      thePM->Color (this, aContextSelStyle, 0);
      continue;
    }
      
    const Prs3d_DatumParts aPart = aTrihedronOwner->DatumPart();
    Handle(Graphic3d_Group) aGroup;
    if (mySelectedParts.Contains (aPart)
    || !myPartToGroup.Find (aPart, aGroup))
    {
      continue;
    }

    if (aPart >= Prs3d_DP_XOYAxis
     && aPart <= Prs3d_DP_XOZAxis)
    {
      getHighlightLineAspect()->SetColor (aSelectionColor);
      aGroup->SetGroupPrimitivesAspect (getHighlightLineAspect()->Aspect());
    }
    else
    {
      if (isShadingMode)
      {
        getHighlightAspect()->SetColor (aSelectionColor);
        aGroup->SetGroupPrimitivesAspect (getHighlightAspect()->Aspect());
      }
      else
      {
        if (aPart == Prs3d_DP_Origin)
        {
          getHighlightPointAspect()->SetColor (aSelectionColor);
          aGroup->SetGroupPrimitivesAspect (getHighlightPointAspect()->Aspect());
        }
        else
        {
          getHighlightLineAspect()->SetColor (aSelectionColor);
          aGroup->SetGroupPrimitivesAspect (getHighlightLineAspect()->Aspect());
        }
      }
    }
    mySelectedParts.Append (aPart);
  }
}

//=======================================================================
//function : ClearSelected
//purpose  :
//=======================================================================
void AIS_Trihedron::ClearSelected()
{
  Handle(Prs3d_DatumAspect) anAspect = myDrawer->DatumAspect();
  const bool isShadingMode = myTrihDispMode == Prs3d_DM_Shaded;
  for (NCollection_List<Prs3d_DatumParts>::Iterator anIterator (mySelectedParts); anIterator.More();
       anIterator.Next())
  {
    const Prs3d_DatumParts aPart = anIterator.Value();
    Handle(Graphic3d_Group) aGroup = myPartToGroup.Find (aPart);
    if (aPart >= Prs3d_DP_XOYAxis
     && aPart <= Prs3d_DP_XOZAxis)
    {
      aGroup->SetGroupPrimitivesAspect (anAspect->LineAspect (aPart)->Aspect());
    }
    if (isShadingMode)
    {
      aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect (aPart)->Aspect());
    }
    else
    {
      if (aPart == Prs3d_DP_Origin)
      {
        aGroup->SetGroupPrimitivesAspect (anAspect->PointAspect()->Aspect());
      }
      else
      {
        aGroup->SetGroupPrimitivesAspect (anAspect->LineAspect (aPart)->Aspect());
      }
    }
  }
  mySelectedParts.Clear();
}

//=======================================================================
//function : computePresentation
//purpose  :
//=======================================================================
void AIS_Trihedron::computePresentation (const Handle(PrsMgr_PresentationManager3d)& /*thePrsMgr*/,
                                         const Handle(Prs3d_Presentation)& thePrs)
{
  myPartToGroup.Clear();
  Handle(Prs3d_DatumAspect) anAspect = myDrawer->DatumAspect();
  const bool isShadingMode = myTrihDispMode == Prs3d_DM_Shaded;
  // display origin
  {
    // Origin is visualized only in shading mode
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePrs);
    const Prs3d_DatumParts aPart = Prs3d_DP_Origin;
    if (anAspect->DrawDatumPart(aPart))
    {
      myPartToGroup.Bind (aPart, aGroup);
      if (isShadingMode)
      {
        aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect (aPart)->Aspect());
      }
      else
      {
        aGroup->SetGroupPrimitivesAspect (anAspect->PointAspect()->Aspect());
      }
      aGroup->AddPrimitiveArray (arrayOfPrimitives (aPart));
    }
  }

  // display axes
  {
    for (Standard_Integer anAxisIter = Prs3d_DP_XAxis; anAxisIter <= Prs3d_DP_ZAxis; ++anAxisIter)
    {
      Prs3d_DatumParts aPart = (Prs3d_DatumParts )anAxisIter;
      if (!anAspect->DrawDatumPart (aPart))
      {
        continue;
      }

      Handle(Graphic3d_Group) anAxisGroup = Prs3d_Root::NewGroup (thePrs);
      myPartToGroup.Bind (aPart, anAxisGroup);
      if (isShadingMode)
      {
        anAxisGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect (aPart)->Aspect());
      }
      else
      {
        anAxisGroup->SetGroupPrimitivesAspect (anAspect->LineAspect (aPart)->Aspect());
      }
      anAxisGroup->AddPrimitiveArray (arrayOfPrimitives (aPart));

      // draw arrow
      Handle(Graphic3d_Group) anArrowGroup = Prs3d_Root::NewGroup (thePrs);
      anArrowGroup->SetPrimitivesAspect (anAspect->ArrowAspect()->Aspect());

      Prs3d_DatumParts anArrowPart = anAspect->ArrowPartForAxis (aPart);
      if (!anAspect->DrawDatumPart (anArrowPart))
      {
        continue;
      }
      anArrowGroup->AddPrimitiveArray (arrayOfPrimitives (anArrowPart));
    }
  }

  // display labels
  if (anAspect->ToDrawLabels())
  {
    Handle(Geom_Axis2Placement) aComponent = myComponent;
    const gp_Pnt anOrigin = aComponent->Location();
    for (Standard_Integer anAxisIter = Prs3d_DP_XAxis; anAxisIter <= Prs3d_DP_ZAxis; ++anAxisIter)
    {
      const Prs3d_DatumParts aPart = (Prs3d_DatumParts )anAxisIter;
      if (!anAspect->DrawDatumPart (aPart))
      {
        continue;
      }

      const Standard_Real anAxisLength = anAspect->AxisLength (aPart);
      const TCollection_ExtendedString& aLabel = myLabel.Find (aPart);
      gp_Dir aDir;
      switch (aPart)
      {
        case Prs3d_DP_XAxis: aDir = aComponent->XDirection(); break;
        case Prs3d_DP_YAxis: aDir = aComponent->YDirection(); break;
        case Prs3d_DP_ZAxis: aDir = aComponent->Direction();  break;
        default: break;
      }
      Handle(Graphic3d_Group) aLabelGroup = Prs3d_Root::NewGroup (thePrs);
      const gp_Pnt aPoint = anOrigin.XYZ() + aDir.XYZ() * anAxisLength;
      Prs3d_Text::Draw (aLabelGroup, anAspect->TextAspect(), aLabel, aPoint);
    }
  }

  // planes invisible group for planes selection
  for (Standard_Integer anAxisIter = Prs3d_DP_XOYAxis; anAxisIter <= Prs3d_DP_XOZAxis; ++anAxisIter)
  {
    Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
    if (!anAspect->DrawDatumPart(aPart))
    {
      continue;
    }

    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePrs);
    myPartToGroup.Bind (aPart, aGroup);

    const Handle(Graphic3d_AspectLine3d)& aLineAspect = anAspect->LineAspect (aPart)->Aspect();
    aLineAspect->SetType (Aspect_TOL_EMPTY);

    aGroup->AddPrimitiveArray (arrayOfPrimitives (aPart));
    aGroup->SetGroupPrimitivesAspect (aLineAspect);
  }
}

//=======================================================================
//function : LoadRecomputable
//purpose  :
//=======================================================================
void AIS_Trihedron::LoadRecomputable (const Standard_Integer theMode)
{
  myRecomputeEveryPrs = Standard_False;
  if (!myToRecomputeModes.Contains (theMode))
  {
    myToRecomputeModes.Append (theMode);
  }
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetDatumPartColor (const Prs3d_DatumParts thePart,
                                       const Quantity_Color&  theColor)
{
  setOwnDatumAspect();

  myDrawer->DatumAspect()->ShadingAspect (thePart)->SetColor (theColor);
  if (thePart != Prs3d_DP_Origin)
  {
    myDrawer->DatumAspect()->LineAspect (thePart)->SetColor (theColor);
  }
}

//=======================================================================
//function : SetTextColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetTextColor (const Quantity_Color& theColor)
{
  setOwnDatumAspect();
  myDrawer->DatumAspect()->TextAspect()->SetColor (theColor);
}

//=======================================================================
//function : Color
//purpose  :
//=======================================================================
Quantity_Color AIS_Trihedron::DatumPartColor (Prs3d_DatumParts thePart)
{
  if (myTrihDispMode == Prs3d_DM_Shaded)
  {
    return myDrawer->DatumAspect()->ShadingAspect (thePart)->Color();
  }
  else
  {
    return myDrawer->DatumAspect()->LineAspect (thePart)->Aspect()->Color();
  }
}

//=======================================================================
//function : SetOriginColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetOriginColor (const Quantity_Color& theColor)
{
  if (myTrihDispMode == Prs3d_DM_Shaded)
  {
    SetDatumPartColor (Prs3d_DP_Origin, theColor);
  }
}

//=======================================================================
//function : SetXAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetXAxisColor (const Quantity_Color& theColor)
{
  SetDatumPartColor (Prs3d_DP_XAxis, theColor);
}

//=======================================================================
//function : SetYAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetYAxisColor (const Quantity_Color& theColor)
{
  SetDatumPartColor (Prs3d_DP_YAxis, theColor);
}

//=======================================================================
//function : SetAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetAxisColor (const Quantity_Color& theColor)
{
  SetDatumPartColor (Prs3d_DP_ZAxis, theColor);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetColor (const Quantity_Color& theColor)
{
  hasOwnColor = Standard_True;
  myDrawer->SetColor (theColor);

  SetDatumPartColor (Prs3d_DP_Origin, theColor);
  SetDatumPartColor (Prs3d_DP_XAxis,  theColor);
  SetDatumPartColor (Prs3d_DP_YAxis,  theColor);
  SetDatumPartColor (Prs3d_DP_ZAxis,  theColor);
}

//=======================================================================
//function : SetArrowColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetArrowColor (const Quantity_Color& theColor)
{
  setOwnDatumAspect();

  myHasOwnArrowColor = Standard_True;
  myDrawer->DatumAspect()->ArrowAspect()->SetColor (theColor);
}

//=======================================================================
//function : TextColor
//purpose  :
//=======================================================================
Quantity_Color AIS_Trihedron::TextColor() const
{
  return myDrawer->DatumAspect()->TextAspect()->Aspect()->Color();
}

//=======================================================================
//function : ArrowColor
//purpose  :
//=======================================================================
Quantity_Color AIS_Trihedron::ArrowColor() const
{
  return myDrawer->DatumAspect()->ArrowAspect()->Aspect()->Color();
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void AIS_Trihedron::UnsetColor()
{
  hasOwnColor = Standard_False;
  Quantity_Color aDefaultColor (Quantity_NOC_LIGHTSTEELBLUE4);
  SetColor (aDefaultColor);
  if (HasTextColor())
  {
    SetTextColor (aDefaultColor);
    myHasOwnTextColor = Standard_False;
  }
  if (HasArrowColor())
  {
    SetArrowColor (aDefaultColor);
    myHasOwnArrowColor = Standard_False;
  }
}

//=======================================================================
//function : createSensitiveEntity
//purpose  :
//=======================================================================
Handle(SelectBasics_SensitiveEntity) AIS_Trihedron::createSensitiveEntity (const Prs3d_DatumParts thePart,
                                                   const Handle(SelectBasics_EntityOwner)& theOwner) const
{
  Handle(Prs3d_DatumAspect) anAspect = myDrawer->DatumAspect();
  Handle(Graphic3d_ArrayOfPrimitives) aPrimitives = arrayOfPrimitives (thePart);
  if (aPrimitives.IsNull())
  {
    return Handle(SelectBasics_SensitiveEntity)();
  }

  if (thePart >= Prs3d_DP_XOYAxis
   && thePart <= Prs3d_DP_XOZAxis)
  { // plane
    const gp_Pnt anXYZ1 = aPrimitives->Vertice (1);
    const gp_Pnt anXYZ2 = aPrimitives->Vertice (2);
    const gp_Pnt anXYZ3 = aPrimitives->Vertice (3);
    return new Select3D_SensitiveTriangle (theOwner, anXYZ1, anXYZ2, anXYZ3);
  }

  if (myTrihDispMode == Prs3d_DM_Shaded)
  {
    Handle(Select3D_SensitivePrimitiveArray) aSelArray = new Select3D_SensitivePrimitiveArray (theOwner);
    aSelArray->InitTriangulation (aPrimitives->Attributes(), aPrimitives->Indices(), TopLoc_Location());
    return aSelArray;
  }

  if (Handle(Graphic3d_ArrayOfPoints) aPoints = Handle(Graphic3d_ArrayOfPoints)::DownCast(aPrimitives))
  {
    const gp_Pnt anXYZ1 = aPoints->Vertice (1);
    return new Select3D_SensitivePoint (theOwner, anXYZ1);
  }
  else if (Handle(Graphic3d_ArrayOfSegments) aSegments = Handle(Graphic3d_ArrayOfSegments)::DownCast(aPrimitives))
  {
    const gp_Pnt anXYZ1 = aSegments->Vertice (1);
    const gp_Pnt anXYZ2 = aSegments->Vertice (2);
    return new Select3D_SensitiveSegment (theOwner, anXYZ1, anXYZ2);
  }
  return Handle(SelectBasics_SensitiveEntity)();
}

// =======================================================================
// function : arrayOfPrimitives
// purpose  :
// =======================================================================
Handle(Graphic3d_ArrayOfPrimitives) AIS_Trihedron::arrayOfPrimitives(
                                               Prs3d_DatumParts theDatumPart) const
{
  Handle(Graphic3d_ArrayOfPrimitives) anArray;
  myPrimitives.Find(theDatumPart, anArray);
  return anArray;
}

// =======================================================================
// function : updatePrimitives
// purpose  :
// =======================================================================
void AIS_Trihedron::updatePrimitives(const Handle(Prs3d_DatumAspect)& theAspect,
                                     Prs3d_DatumMode theMode,
                                     const gp_Pnt& theOrigin,
                                     const gp_Dir& theXDirection,
                                     const gp_Dir& theYDirection,
                                     const gp_Dir& theZDirection)
{
  myPrimitives.Clear();

  NCollection_DataMap<Prs3d_DatumParts, gp_Dir> anAxisDirs;
  anAxisDirs.Bind(Prs3d_DP_XAxis, theXDirection);
  anAxisDirs.Bind(Prs3d_DP_YAxis, theYDirection);
  anAxisDirs.Bind(Prs3d_DP_ZAxis, theZDirection);

  NCollection_DataMap<Prs3d_DatumParts, gp_Pnt> anAxisPoints;
  gp_XYZ anXYZOrigin = theOrigin.XYZ();
  for (int anAxisIter = Prs3d_DP_XAxis; anAxisIter <= Prs3d_DP_ZAxis; ++anAxisIter)
  {
    Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
    anAxisPoints.Bind(aPart, gp_Pnt(anXYZOrigin + anAxisDirs.Find(aPart).XYZ() *
                                                   theAspect->AxisLength(aPart)));
  }

  if (theMode == Prs3d_DM_WireFrame)
  {
    // origin
    if (theAspect->DrawDatumPart(Prs3d_DP_Origin))
    {
      Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPoints(1);
      aPrims->AddVertex(theOrigin);
      myPrimitives.Bind(Prs3d_DP_Origin, aPrims);
    }
    // axes
    for (int aPartIter = Prs3d_DP_XAxis; aPartIter <= Prs3d_DP_ZAxis; ++aPartIter)
    {
      const Prs3d_DatumParts aPart = (Prs3d_DatumParts)aPartIter;
      if (theAspect->DrawDatumPart(aPart))
      {
        Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfSegments(2);
        aPrims->AddVertex(theOrigin);
        aPrims->AddVertex(anAxisPoints.Find(aPart));
        myPrimitives.Bind(aPart, aPrims);
      }

      Prs3d_DatumParts anArrowPart = theAspect->ArrowPartForAxis(aPart);
      if (theAspect->DrawDatumPart(anArrowPart))
      {
        myPrimitives.Bind(anArrowPart,
            Prs3d_Arrow::DrawSegments(anAxisPoints.Find(aPart), anAxisDirs.Find(aPart),
            theAspect->ArrowAspect()->Angle(),
            theAspect->AxisLength(aPart) * theAspect->Attribute(Prs3d_DP_ShadingConeLengthPercent),
          (Standard_Integer)  theAspect->Attribute(Prs3d_DP_ShadingNumberOfFacettes)));
      }
    }
  }
  else
  {
    // shading mode
    // origin
    if (theAspect->DrawDatumPart(Prs3d_DP_Origin))
    {
      const Standard_Real aSphereRadius = theAspect->AxisLength(Prs3d_DP_XAxis) *
                                          theAspect->Attribute(Prs3d_DP_ShadingOriginRadiusPercent);
      const Standard_Integer aNbOfFacettes =
                           (Standard_Integer)theAspect->Attribute(Prs3d_DP_ShadingNumberOfFacettes);
      gp_Trsf aSphereTransform;
      aSphereTransform.SetTranslationPart(gp_Vec(gp::Origin(), theOrigin));
      myPrimitives.Bind(Prs3d_DP_Origin, Prs3d_ToolSphere::Create(aSphereRadius, aNbOfFacettes,
                                                                  aNbOfFacettes, aSphereTransform));
    }
    // axes
    {
      const Standard_Integer aNbOfFacettes = 
                               (Standard_Integer)theAspect->Attribute(Prs3d_DP_ShadingNumberOfFacettes);
      const Standard_Real aTubeRadiusPercent = theAspect->Attribute(Prs3d_DP_ShadingTubeRadiusPercent);
      const Standard_Real aConeLengthPercent = theAspect->Attribute(Prs3d_DP_ShadingConeLengthPercent);
      const Standard_Real aConeRadiusPercent = theAspect->Attribute(Prs3d_DP_ShadingConeRadiusPercent);
      for (Standard_Integer anAxisIter = Prs3d_DP_XAxis; anAxisIter <= Prs3d_DP_ZAxis; ++anAxisIter)
      {
        const Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
        const Prs3d_DatumParts anArrowPart = theAspect->ArrowPartForAxis(aPart);
        const bool aDrawArrow = theAspect->DrawDatumPart(anArrowPart);
        const Standard_Real anAxisLength = theAspect->AxisLength(aPart);
        const gp_Ax1 anAxis(theOrigin, anAxisDirs.Find(aPart));

        if (theAspect->DrawDatumPart(aPart))
        {
          // draw cylinder
          myPrimitives.Bind(aPart,
                            Prs3d_Arrow::DrawShaded(anAxis, anAxisLength * aTubeRadiusPercent,
                            aDrawArrow ? anAxisLength - anAxisLength * aConeLengthPercent : anAxisLength,
                            0.0, 0.0, aNbOfFacettes));
        }

        // draw arrow
        if (aDrawArrow)
        {
          myPrimitives.Bind(anArrowPart, Prs3d_Arrow::DrawShaded(anAxis, 0.0, anAxisLength,
                                                     anAxisLength * aConeRadiusPercent,
                                                     anAxisLength * aConeLengthPercent, aNbOfFacettes));
        }
      }
    }
  }
  // planes
  for (Standard_Integer aPlaneIter = Prs3d_DP_XOYAxis; aPlaneIter <= Prs3d_DP_XOZAxis; ++aPlaneIter)
  {
    const Prs3d_DatumParts aPart = (Prs3d_DatumParts)aPlaneIter;
    if (!theAspect->DrawDatumPart(aPart))
    {
      continue;
    }

    Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPolylines(4);
    aPrims->AddVertex(theOrigin);

    Prs3d_DatumParts aPart1 = Prs3d_DP_XAxis, aPart2 = Prs3d_DP_XAxis;
    switch(aPart)
    {
      case Prs3d_DP_XOYAxis:
      {
        aPart1 = Prs3d_DP_XAxis;
        aPart2 = Prs3d_DP_YAxis;
        break;
      }
      case Prs3d_DP_YOZAxis:
      {
        aPart1 = Prs3d_DP_YAxis;
        aPart2 = Prs3d_DP_ZAxis;
        break;
      }
      case Prs3d_DP_XOZAxis:
      {
        aPart1 = Prs3d_DP_XAxis;
        aPart2 = Prs3d_DP_ZAxis;
        break;
      }
      default: break;
    }
    aPrims->AddVertex(anAxisPoints.Find(aPart1));
    aPrims->AddVertex(anAxisPoints.Find(aPart2));

    aPrims->AddVertex(theOrigin);
    myPrimitives.Bind(aPart, aPrims);
  }
}

// =======================================================================
// function : getHighlightAspect
// purpose  :
// =======================================================================
Handle(Prs3d_ShadingAspect) AIS_Trihedron::getHighlightAspect()
{
  if (!myHighlightAspect.IsNull())
    return myHighlightAspect;

  Quantity_Color aHighlightColor = Quantity_NOC_GRAY80;
  if (!myHilightDrawer.IsNull())
    aHighlightColor = myHilightDrawer->Color();

  myHighlightAspect = new Prs3d_ShadingAspect();
  myHighlightAspect->Aspect()->SetInteriorStyle (Aspect_IS_SOLID);
  myHighlightAspect->SetColor (aHighlightColor);

  Graphic3d_MaterialAspect aHighlightMaterial;
  aHighlightMaterial.SetColor (aHighlightColor);
  myHighlightAspect->SetMaterial (aHighlightMaterial);

  return myHighlightAspect;
}

// =======================================================================
// function : getHighlightLineAspect
// purpose  :
// =======================================================================
Handle(Prs3d_LineAspect) AIS_Trihedron::getHighlightLineAspect()
{
  if (!myHighlightLineAspect.IsNull())
    return myHighlightLineAspect;

  Quantity_Color aHighlightColor = Quantity_NOC_GRAY80;
  if (!myHilightDrawer.IsNull())
    aHighlightColor = myHilightDrawer->Color();

  Handle(Prs3d_DatumAspect) aDatumAspect = Attributes()->DatumAspect();
  Handle(Prs3d_LineAspect) aLineAspect = aDatumAspect->LineAspect(Prs3d_DP_XAxis);
  myHighlightLineAspect = new Prs3d_LineAspect (aHighlightColor, aLineAspect->Aspect()->Type(),
                                                aLineAspect->Aspect()->Width());

  return myHighlightLineAspect;
}

// =======================================================================
// function : getHighlightPointAspect
// purpose  :
// =======================================================================
Handle(Prs3d_PointAspect) AIS_Trihedron::getHighlightPointAspect()
{
  if (!myHighlightPointAspect.IsNull())
    return myHighlightPointAspect;

  Quantity_Color aHighlightColor = Quantity_NOC_GRAY80;
  if (!myHilightDrawer.IsNull())
    aHighlightColor = myHilightDrawer->Color();
  myHighlightPointAspect = new Prs3d_PointAspect (Aspect_TOM_PLUS, aHighlightColor, 1.0);

  return myHighlightPointAspect;
}
