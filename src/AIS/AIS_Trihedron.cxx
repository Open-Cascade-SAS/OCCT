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
#include <AIS_TrihedronSelectionMode.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>

#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_ToolSphere.hxx>

#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Trihedron, AIS_InteractiveObject)

//=======================================================================
//function : AIS_Trihedron
//purpose  :
//=======================================================================
AIS_Trihedron::AIS_Trihedron (const Handle(Geom_Axis2Placement)& theComponent)
: myComponent (theComponent),
  myTrihDispMode (Prs3d_DM_WireFrame),
  myHasOwnSize (Standard_False),
  myHasOwnTextColor (Standard_False),
  myHasOwnArrowColor (Standard_False)
{
  myAutoHilight = Standard_False;

  // selection priorities
  memset (mySelectionPriority, 0, sizeof(mySelectionPriority));
  mySelectionPriority[Prs3d_DatumParts_None] =    5; // complete trihedron: priority 5 (same as faces)
  mySelectionPriority[Prs3d_DatumParts_Origin] =  8; // origin: priority 8
  for (int aPartIter = Prs3d_DatumParts_XAxis; aPartIter <= Prs3d_DatumParts_ZAxis; ++aPartIter)
  {
    mySelectionPriority[aPartIter] = 7; // axes: priority: 7
  }
  for (int aPartIter = Prs3d_DatumParts_XOYAxis; aPartIter <= Prs3d_DatumParts_XOZAxis; ++aPartIter)
  {
    mySelectionPriority[aPartIter] = 5; // planes: priority: 5
  }
  myHiddenLineAspect = new Graphic3d_AspectLine3d (Quantity_NOC_WHITE, Aspect_TOL_EMPTY, 1.0f);

  // trihedron labels
  myLabels[Prs3d_DatumParts_XAxis] = "X";
  myLabels[Prs3d_DatumParts_YAxis] = "Y";
  myLabels[Prs3d_DatumParts_ZAxis] = "Z";
}

//=======================================================================
//function : SetComponent
//purpose  :
//=======================================================================
void AIS_Trihedron::SetComponent (const Handle(Geom_Axis2Placement)& theComponent)
{
  myComponent = theComponent;
  SetToUpdate();
}

//=======================================================================
//function : setOwnDatumAspect
//purpose  :
//=======================================================================
void AIS_Trihedron::setOwnDatumAspect()
{
  if (myDrawer->HasOwnDatumAspect())
  {
    return;
  }

  Handle(Prs3d_DatumAspect) aNewAspect = new Prs3d_DatumAspect();
  myDrawer->SetDatumAspect (aNewAspect);
  if (!myDrawer->Link().IsNull())
  {
    aNewAspect->CopyAspectsFrom (myDrawer->Link()->DatumAspect());
  }
}

//=======================================================================
//function : SetSize
//purpose  :
//=======================================================================
void AIS_Trihedron::SetSize(const Standard_Real theValue)
{
  myHasOwnSize = Standard_True;

  setOwnDatumAspect();
  myDrawer->DatumAspect()->SetAxisLength (theValue, theValue, theValue);

  SetToUpdate();
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
    myDrawer->DatumAspect()->SetAxisLength (DA->AxisLength (Prs3d_DatumParts_XAxis),
                                            DA->AxisLength (Prs3d_DatumParts_YAxis),
                                            DA->AxisLength (Prs3d_DatumParts_ZAxis));
  }
  else
  {
    SetToUpdate();
  }
  UpdateSelection();
}

//=======================================================================
//function : Size
//purpose  :
//=======================================================================
Standard_Real AIS_Trihedron::Size() const 
{
  return myDrawer->DatumAspect()->AxisLength(Prs3d_DatumParts_XAxis);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_Trihedron::Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
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
      Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this, mySelectionPriority[Prs3d_DatumParts_None]);
      const bool isShadingMode = myTrihDispMode == Prs3d_DM_Shaded;
      for (int aPartIter = isShadingMode ? Prs3d_DatumParts_Origin : Prs3d_DatumParts_XAxis; aPartIter <= Prs3d_DatumParts_ZAxis;
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
      const Prs3d_DatumParts aPart = Prs3d_DatumParts_Origin;
      if (anAspect->DrawDatumPart (aPart))
      {
        Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart, mySelectionPriority[aPart]);
        Handle(Graphic3d_ArrayOfPrimitives) aPrimitives = arrayOfPrimitives(aPart);
        theSelection->Add (createSensitiveEntity (aPart, anOwner));
      }
      break;
    }
    case AIS_TrihedronSelectionMode_Axes:
    {
      for (int aPartIter = Prs3d_DatumParts_XAxis; aPartIter <= Prs3d_DatumParts_ZAxis; ++aPartIter)
      {
        const Prs3d_DatumParts aPart = (Prs3d_DatumParts )aPartIter;
        if (!anAspect->DrawDatumPart (aPart))
        {
          continue;
        }
        Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart, mySelectionPriority[aPart]);
        theSelection->Add (createSensitiveEntity (aPart, anOwner));
      }
      break;
    }
    case AIS_TrihedronSelectionMode_MainPlanes:
    {
      // create owner for each trihedron plane
      {
        for (int aPartIter = Prs3d_DatumParts_XOYAxis; aPartIter <= Prs3d_DatumParts_XOZAxis; ++aPartIter)
        {
          const Prs3d_DatumParts aPart = (Prs3d_DatumParts )aPartIter;
          if (!anAspect->DrawDatumPart (aPart))
          {
            continue;
          }
          Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart, mySelectionPriority[aPart]);
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
void AIS_Trihedron::HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager)& thePM,
                                           const Handle(Prs3d_Drawer)& theStyle,
                                           const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle(AIS_TrihedronOwner) anOwner = Handle(AIS_TrihedronOwner)::DownCast (theOwner);
  if (anOwner.IsNull())
  {
    // default 0 selection mode
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
  Handle(Graphic3d_Group) aGroup = aPresentation->CurrentGroup();
  if (aPart >= Prs3d_DatumParts_XOYAxis && aPart <= Prs3d_DatumParts_XOZAxis)
  {
    // planes selection is equal in both shading and wireframe mode
    aGroup->SetGroupPrimitivesAspect (theStyle->LineAspect()->Aspect());
  }
  else
  {
    if (myTrihDispMode == Prs3d_DM_Shaded)
    {
      aGroup->SetGroupPrimitivesAspect (theStyle->ShadingAspect()->Aspect());
    }
    else
    {
      if (aPart == Prs3d_DatumParts_Origin)
      {
        aGroup->SetGroupPrimitivesAspect (theStyle->PointAspect()->Aspect());
      }
      else
      {
        aGroup->SetGroupPrimitivesAspect(theStyle->LineAspect()->Aspect());
      }
    }
  }
  aGroup->AddPrimitiveArray (arrayOfPrimitives(aPart));

  const Graphic3d_ZLayerId aLayer = theStyle->ZLayer() != Graphic3d_ZLayerId_UNKNOWN ? theStyle->ZLayer() : myDrawer->ZLayer();
  if (aPresentation->GetZLayer() != aLayer)
  {
    aPresentation->SetZLayer (aLayer);
  }

  aPresentation->Highlight (theStyle);
  thePM->AddToImmediateList (aPresentation);
}

//========================================================================
//function : HilightSelected
//purpose  :
//========================================================================
void AIS_Trihedron::HilightSelected (const Handle(PrsMgr_PresentationManager)& thePM,
                                     const SelectMgr_SequenceOfOwner& theOwners)
{
  if (theOwners.IsEmpty() || !HasInteractiveContext())
  {
    return;
  }

  const bool isShadingMode = myTrihDispMode == Prs3d_DM_Shaded;

  Handle(Prs3d_Drawer) anAspect = !myHilightDrawer.IsNull() ? myHilightDrawer : GetContext()->SelectionStyle();
  for (SelectMgr_SequenceOfOwner::Iterator anIterator (theOwners); anIterator.More(); anIterator.Next())
  {
    const Handle(SelectMgr_EntityOwner)& anOwner = anIterator.Value();
    Handle(AIS_TrihedronOwner) aTrihedronOwner = Handle(AIS_TrihedronOwner)::DownCast(anOwner);
    if (aTrihedronOwner.IsNull())
    {
      thePM->Color (this, anAspect, 0);
      continue;
    }
      
    const Prs3d_DatumParts aPart = aTrihedronOwner->DatumPart();
    if (myPartToGroup[aPart].IsNull()
     || mySelectedParts.Contains (aPart))
    {
      continue;
    }

    const Handle(Graphic3d_Group)& aGroup = myPartToGroup[aPart];
    if (aPart >= Prs3d_DatumParts_XOYAxis
     && aPart <= Prs3d_DatumParts_XOZAxis)
    {
      aGroup->SetGroupPrimitivesAspect (anAspect->LineAspect()->Aspect());
    }
    else
    {
      if (isShadingMode)
      {
        aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect()->Aspect());
      }
      else
      {
        if (aPart == Prs3d_DatumParts_Origin)
        {
          aGroup->SetGroupPrimitivesAspect (anAspect->PointAspect()->Aspect());
        }
        else
        {
          aGroup->SetGroupPrimitivesAspect (anAspect->LineAspect()->Aspect());
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
    const Handle(Graphic3d_Group)& aGroup = myPartToGroup[aPart];
    if (aPart >= Prs3d_DatumParts_XOYAxis
     && aPart <= Prs3d_DatumParts_XOZAxis)
    {
      aGroup->SetGroupPrimitivesAspect (myHiddenLineAspect);
    }
    else if (isShadingMode)
    {
      aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect (aPart)->Aspect());
    }
    else
    {
      if (aPart == Prs3d_DatumParts_Origin)
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
void AIS_Trihedron::computePresentation (const Handle(PrsMgr_PresentationManager)& /*thePrsMgr*/,
                                         const Handle(Prs3d_Presentation)& thePrs)
{
  for (Standard_Integer aPartIter = 0; aPartIter < Prs3d_DatumParts_NB; ++aPartIter)
  {
    myPartToGroup[aPartIter].Nullify();
  }

  Handle(Prs3d_DatumAspect) anAspect = myDrawer->DatumAspect();
  const bool isShadingMode = myTrihDispMode == Prs3d_DM_Shaded;
  // display origin
  {
    // Origin is visualized only in shading mode
    Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
    const Prs3d_DatumParts aPart = Prs3d_DatumParts_Origin;
    if (anAspect->DrawDatumPart(aPart))
    {
      myPartToGroup[aPart] = aGroup;
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
    for (Standard_Integer anAxisIter = Prs3d_DatumParts_XAxis; anAxisIter <= Prs3d_DatumParts_ZAxis; ++anAxisIter)
    {
      Prs3d_DatumParts aPart = (Prs3d_DatumParts )anAxisIter;
      if (!anAspect->DrawDatumPart (aPart))
      {
        continue;
      }

      Handle(Graphic3d_Group) anAxisGroup = thePrs->NewGroup();
      myPartToGroup[aPart] = anAxisGroup;
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
      const Prs3d_DatumParts anArrowPart = Prs3d_DatumAspect::ArrowPartForAxis (aPart);
      if (!anAspect->DrawDatumPart (anArrowPart))
      {
        continue;
      }

      Handle(Graphic3d_Group) anArrowGroup = thePrs->NewGroup();
      if (isShadingMode)
      {
        anArrowGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect (anArrowPart)->Aspect());
      }
      else
      {
        anArrowGroup->SetGroupPrimitivesAspect (anAspect->LineAspect (anArrowPart)->Aspect());
      }
      anArrowGroup->AddPrimitiveArray (arrayOfPrimitives (anArrowPart));
    }
  }

  // display labels
  if (anAspect->ToDrawLabels())
  {
    Handle(Geom_Axis2Placement) aComponent = myComponent;
    const gp_Pnt anOrigin = aComponent->Location();
    for (Standard_Integer anAxisIter = Prs3d_DatumParts_XAxis; anAxisIter <= Prs3d_DatumParts_ZAxis; ++anAxisIter)
    {
      const Prs3d_DatumParts aPart = (Prs3d_DatumParts )anAxisIter;
      if (!anAspect->DrawDatumPart (aPart))
      {
        continue;
      }

      const Standard_Real anAxisLength = anAspect->AxisLength (aPart);
      const TCollection_ExtendedString& aLabel = myLabels[aPart];
      gp_Dir aDir;
      switch (aPart)
      {
        case Prs3d_DatumParts_XAxis: aDir = aComponent->XDirection(); break;
        case Prs3d_DatumParts_YAxis: aDir = aComponent->YDirection(); break;
        case Prs3d_DatumParts_ZAxis: aDir = aComponent->Direction();  break;
        default: break;
      }
      Handle(Graphic3d_Group) aLabelGroup = thePrs->NewGroup();
      const gp_Pnt aPoint = anOrigin.XYZ() + aDir.XYZ() * anAxisLength;
      Prs3d_Text::Draw (aLabelGroup, anAspect->TextAspect (aPart), aLabel, aPoint);
    }
  }

  // planes invisible group for planes selection
  for (Standard_Integer anAxisIter = Prs3d_DatumParts_XOYAxis; anAxisIter <= Prs3d_DatumParts_XOZAxis; ++anAxisIter)
  {
    Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
    if (!anAspect->DrawDatumPart(aPart))
    {
      continue;
    }

    Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
    myPartToGroup[aPart] = aGroup;

    aGroup->AddPrimitiveArray (arrayOfPrimitives (aPart));
    aGroup->SetGroupPrimitivesAspect (myHiddenLineAspect);
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
  if (thePart != Prs3d_DatumParts_Origin)
  {
    myDrawer->DatumAspect()->LineAspect (thePart)->SetColor (theColor);
  }
}

//=======================================================================
//function : SetTextColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetTextColor (const Prs3d_DatumParts thePart,
                                  const Quantity_Color& theColor)
{
  setOwnDatumAspect();
  myDrawer->DatumAspect()->TextAspect (thePart)->SetColor (theColor);
}

//=======================================================================
//function : SetTextColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetTextColor (const Quantity_Color& theColor)
{
  setOwnDatumAspect();
  myDrawer->DatumAspect()->TextAspect (Prs3d_DatumParts_XAxis)->SetColor (theColor);
  myDrawer->DatumAspect()->TextAspect (Prs3d_DatumParts_YAxis)->SetColor (theColor);
  myDrawer->DatumAspect()->TextAspect (Prs3d_DatumParts_ZAxis)->SetColor (theColor);
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
    SetDatumPartColor (Prs3d_DatumParts_Origin, theColor);
  }
}

//=======================================================================
//function : SetXAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetXAxisColor (const Quantity_Color& theColor)
{
  SetDatumPartColor (Prs3d_DatumParts_XAxis, theColor);
}

//=======================================================================
//function : SetYAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetYAxisColor (const Quantity_Color& theColor)
{
  SetDatumPartColor (Prs3d_DatumParts_YAxis, theColor);
}

//=======================================================================
//function : SetAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetAxisColor (const Quantity_Color& theColor)
{
  SetDatumPartColor (Prs3d_DatumParts_ZAxis, theColor);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetColor (const Quantity_Color& theColor)
{
  hasOwnColor = Standard_True;
  myDrawer->SetColor (theColor);

  SetDatumPartColor (Prs3d_DatumParts_Origin, theColor);
  SetDatumPartColor (Prs3d_DatumParts_XAxis,  theColor);
  SetDatumPartColor (Prs3d_DatumParts_YAxis,  theColor);
  SetDatumPartColor (Prs3d_DatumParts_ZAxis,  theColor);
}

//=======================================================================
//function : SetArrowColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetArrowColor (const Prs3d_DatumParts thePart,
                                   const Quantity_Color& theColor)
{
  setOwnDatumAspect();
  myHasOwnArrowColor = Standard_True;
  const Prs3d_DatumParts anArrowPart = Prs3d_DatumAspect::ArrowPartForAxis (thePart);
  myDrawer->DatumAspect()->ShadingAspect(anArrowPart)->SetColor (theColor);
  myDrawer->DatumAspect()->LineAspect   (anArrowPart)->SetColor (theColor);
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
  for (Standard_Integer anAxisIter = Prs3d_DatumParts_XArrow; anAxisIter <= Prs3d_DatumParts_ZArrow; ++anAxisIter)
  {
    myDrawer->DatumAspect()->ShadingAspect((Prs3d_DatumParts )anAxisIter)->SetColor (theColor);
    myDrawer->DatumAspect()->LineAspect   ((Prs3d_DatumParts )anAxisIter)->SetColor (theColor);
  }
}

//=======================================================================
//function : TextColor
//purpose  :
//=======================================================================
Quantity_Color AIS_Trihedron::TextColor() const
{
  return myDrawer->DatumAspect()->TextAspect (Prs3d_DatumParts_XAxis)->Aspect()->Color();
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
//function : ToDrawArrows
//purpose  :
//=======================================================================
Standard_Boolean AIS_Trihedron::ToDrawArrows() const
{
  return myDrawer->DatumAspect()->ToDrawArrows();
}

//=======================================================================
//function : SetDrawArrows
//purpose  :
//=======================================================================
void AIS_Trihedron::SetDrawArrows (const Standard_Boolean theToDraw)
{
  setOwnDatumAspect();
  myDrawer->DatumAspect()->SetDrawArrows (theToDraw);
}

//=======================================================================
//function : createSensitiveEntity
//purpose  :
//=======================================================================
Handle(Select3D_SensitiveEntity) AIS_Trihedron::createSensitiveEntity (const Prs3d_DatumParts thePart,
                                                   const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  Handle(Prs3d_DatumAspect) anAspect = myDrawer->DatumAspect();
  Handle(Graphic3d_ArrayOfPrimitives) aPrimitives = arrayOfPrimitives (thePart);
  if (aPrimitives.IsNull())
  {
    return Handle(Select3D_SensitiveEntity)();
  }

  if (thePart >= Prs3d_DatumParts_XOYAxis
   && thePart <= Prs3d_DatumParts_XOZAxis)
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
  return Handle(Select3D_SensitiveEntity)();
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
  for (Standard_Integer aPartIter = 0; aPartIter < Prs3d_DatumParts_NB; ++aPartIter)
  {
    myPrimitives[aPartIter].Nullify();
  }

  NCollection_DataMap<Prs3d_DatumParts, gp_Dir> anAxisDirs;
  anAxisDirs.Bind(Prs3d_DatumParts_XAxis, theXDirection);
  anAxisDirs.Bind(Prs3d_DatumParts_YAxis, theYDirection);
  anAxisDirs.Bind(Prs3d_DatumParts_ZAxis, theZDirection);

  NCollection_DataMap<Prs3d_DatumParts, gp_Pnt> anAxisPoints;
  gp_XYZ anXYZOrigin = theOrigin.XYZ();
  for (int anAxisIter = Prs3d_DatumParts_XAxis; anAxisIter <= Prs3d_DatumParts_ZAxis; ++anAxisIter)
  {
    Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
    anAxisPoints.Bind(aPart, gp_Pnt(anXYZOrigin + anAxisDirs.Find(aPart).XYZ() *
                                                   theAspect->AxisLength(aPart)));
  }

  if (theMode == Prs3d_DM_WireFrame)
  {
    // origin
    if (theAspect->DrawDatumPart(Prs3d_DatumParts_Origin))
    {
      Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPoints(1);
      aPrims->AddVertex(theOrigin);
      myPrimitives[Prs3d_DatumParts_Origin] = aPrims;
    }
    // axes
    for (int aPartIter = Prs3d_DatumParts_XAxis; aPartIter <= Prs3d_DatumParts_ZAxis; ++aPartIter)
    {
      const Prs3d_DatumParts aPart = (Prs3d_DatumParts)aPartIter;
      if (theAspect->DrawDatumPart(aPart))
      {
        Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfSegments(2);
        aPrims->AddVertex(theOrigin);
        aPrims->AddVertex(anAxisPoints.Find(aPart));
        myPrimitives[aPart] = aPrims;
      }

      const Prs3d_DatumParts anArrowPart = Prs3d_DatumAspect::ArrowPartForAxis (aPart);
      if (theAspect->DrawDatumPart(anArrowPart))
      {
        myPrimitives[anArrowPart] = Prs3d_Arrow::DrawSegments (anAxisPoints.Find(aPart), anAxisDirs.Find(aPart),
                                                               theAspect->ArrowAspect()->Angle(),
                                                               theAspect->AxisLength(aPart) * theAspect->Attribute(Prs3d_DatumAttribute_ShadingConeLengthPercent),
                                                               (Standard_Integer) theAspect->Attribute(Prs3d_DatumAttribute_ShadingNumberOfFacettes));
      }
    }
  }
  else
  {
    // shading mode
    // origin
    if (theAspect->DrawDatumPart(Prs3d_DatumParts_Origin))
    {
      const Standard_Real aSphereRadius = theAspect->AxisLength(Prs3d_DatumParts_XAxis) *
                                          theAspect->Attribute(Prs3d_DatumAttribute_ShadingOriginRadiusPercent);
      const Standard_Integer aNbOfFacettes =
                           (Standard_Integer)theAspect->Attribute(Prs3d_DatumAttribute_ShadingNumberOfFacettes);
      gp_Trsf aSphereTransform;
      aSphereTransform.SetTranslationPart(gp_Vec(gp::Origin(), theOrigin));
      myPrimitives[Prs3d_DatumParts_Origin] = Prs3d_ToolSphere::Create (aSphereRadius, aNbOfFacettes, aNbOfFacettes, aSphereTransform);
    }
    // axes
    {
      const Standard_Integer aNbOfFacettes = 
                               (Standard_Integer)theAspect->Attribute(Prs3d_DatumAttribute_ShadingNumberOfFacettes);
      const Standard_Real aTubeRadiusPercent = theAspect->Attribute(Prs3d_DatumAttribute_ShadingTubeRadiusPercent);
      const Standard_Real aConeLengthPercent = theAspect->Attribute(Prs3d_DatumAttribute_ShadingConeLengthPercent);
      const Standard_Real aConeRadiusPercent = theAspect->Attribute(Prs3d_DatumAttribute_ShadingConeRadiusPercent);
      for (Standard_Integer anAxisIter = Prs3d_DatumParts_XAxis; anAxisIter <= Prs3d_DatumParts_ZAxis; ++anAxisIter)
      {
        const Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
        const Prs3d_DatumParts anArrowPart = Prs3d_DatumAspect::ArrowPartForAxis (aPart);
        const bool aDrawArrow = theAspect->DrawDatumPart(anArrowPart);
        const Standard_Real anAxisLength = theAspect->AxisLength(aPart);
        const gp_Ax1 anAxis(theOrigin, anAxisDirs.Find(aPart));

        if (theAspect->DrawDatumPart(aPart))
        {
          // draw cylinder
          myPrimitives[aPart] = Prs3d_Arrow::DrawShaded (anAxis, anAxisLength * aTubeRadiusPercent,
                                                         aDrawArrow ? (anAxisLength - anAxisLength * aConeLengthPercent) : anAxisLength,
                                                         0.0, 0.0, aNbOfFacettes);
        }

        // draw arrow
        if (aDrawArrow)
        {
          myPrimitives[anArrowPart] = Prs3d_Arrow::DrawShaded (anAxis, 0.0, anAxisLength,
                                                               anAxisLength * aConeRadiusPercent,
                                                               anAxisLength * aConeLengthPercent, aNbOfFacettes);
        }
      }
    }
  }
  // planes
  for (Standard_Integer aPlaneIter = Prs3d_DatumParts_XOYAxis; aPlaneIter <= Prs3d_DatumParts_XOZAxis; ++aPlaneIter)
  {
    const Prs3d_DatumParts aPart = (Prs3d_DatumParts)aPlaneIter;
    if (!theAspect->DrawDatumPart(aPart))
    {
      continue;
    }

    Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPolylines(4);
    aPrims->AddVertex(theOrigin);

    Prs3d_DatumParts aPart1 = Prs3d_DatumParts_XAxis, aPart2 = Prs3d_DatumParts_XAxis;
    switch(aPart)
    {
      case Prs3d_DatumParts_XOYAxis:
      {
        aPart1 = Prs3d_DatumParts_XAxis;
        aPart2 = Prs3d_DatumParts_YAxis;
        break;
      }
      case Prs3d_DatumParts_YOZAxis:
      {
        aPart1 = Prs3d_DatumParts_YAxis;
        aPart2 = Prs3d_DatumParts_ZAxis;
        break;
      }
      case Prs3d_DatumParts_XOZAxis:
      {
        aPart1 = Prs3d_DatumParts_XAxis;
        aPart2 = Prs3d_DatumParts_ZAxis;
        break;
      }
      default: break;
    }
    aPrims->AddVertex(anAxisPoints.Find(aPart1));
    aPrims->AddVertex(anAxisPoints.Find(aPart2));

    aPrims->AddVertex(theOrigin);
    myPrimitives[aPart] = aPrims;
  }
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void AIS_Trihedron::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, AIS_InteractiveObject)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnTextColor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnArrowColor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTrihDispMode)
}
