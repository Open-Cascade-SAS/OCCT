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
#include <AIS_InteractiveObject.hxx>
#include <AIS_TrihedronOwner.hxx>

#include <Geom_Axis2Placement.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>

#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>

#include <Quantity_Color.hxx>

#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <UnitsAPI.hxx>

//#define POINT_SELECTION_MODE

#define PATCH_TO_OCCT_710

IMPLEMENT_STANDARD_RTTIEXT(AIS_Trihedron, AIS_InteractiveObject)

static Standard_Boolean IsInList(const TColStd_ListOfInteger& LL, const Standard_Integer aMode)
{
  TColStd_ListIteratorOfListOfInteger It(LL);
  for (;It.More(); It.Next()){
    if (It.Value() == aMode)
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : AIS_Trihedron
//purpose  : 
//=======================================================================
AIS_Trihedron::AIS_Trihedron(const Handle(Geom_Axis2Placement)& theComponent)
: myHasOwnSize(Standard_False),
  myHasOwnTextColor(Standard_False),
  myHasOwnArrowColor(Standard_False),
  myComponent(theComponent),
  myDisplayMode(Prs3d_DM_WireFrame)
{
  // default presentation parameters
  SetDisplayMode (0);
  SetHilightMode (0);

  // datum creation
  Handle (Prs3d_DatumAspect) aDatumAspect = new Prs3d_DatumAspect();
  myDrawer->SetDatumAspect(aDatumAspect);

  // entities to provide custom selection
  Quantity_Color aHighlightColor = Quantity_NOC_GRAY80;
  myHighlightAspect = new Prs3d_ShadingAspect();
  myHighlightAspect->Aspect()->SetInteriorStyle (Aspect_IS_SOLID);
  myHighlightAspect->SetColor(aHighlightColor);
  Graphic3d_MaterialAspect aHighlightMaterial;
  aHighlightMaterial.SetColor (aHighlightColor);
  myHighlightAspect->SetMaterial (aHighlightMaterial);

  Handle(Prs3d_LineAspect) aLineAspect = aDatumAspect->LineAspect(Prs3d_DP_XAxis);
  myHighlightLineAspect = new Prs3d_LineAspect(aHighlightColor, aLineAspect->Aspect()->Type(),
                                               aLineAspect->Aspect()->Width());
  myHighlightPointAspect = new Prs3d_PointAspect(Aspect_TOM_PLUS, aHighlightColor, 1.0);

  // selection priorities
  mySelectionPriority.Bind(Prs3d_DP_None, 5); // complete triedron: priority 5 (same as faces)
  mySelectionPriority.Bind(Prs3d_DP_Origin, 8); // origin: priority 8
  for (int i = Prs3d_DP_XAxis; i <= Prs3d_DP_ZAxis; i++)
    mySelectionPriority.Bind((Prs3d_DatumParts)i, 7); // axes: priority: 7
  for (int i = Prs3d_DP_XOYAxis; i <= Prs3d_DP_XOZAxis; i++)
    mySelectionPriority.Bind((Prs3d_DatumParts)i, 5); // planes: priority: 5

  // trihedron labels
  myLabel.Bind(Prs3d_DP_XAxis, "X");
  myLabel.Bind(Prs3d_DP_YAxis, "Y");
  myLabel.Bind(Prs3d_DP_ZAxis, "Z");

  Aspect_TypeOfMarker aPointMarker = Aspect_TOM_POINT;
  myPointAspect = new Prs3d_PointAspect(aPointMarker, aHighlightColor, 0.1);
}

//=======================================================================
//function : SetComponent
//purpose  : 
//=======================================================================
void AIS_Trihedron::SetComponent(const Handle(Geom_Axis2Placement)& theComponent)
{
  myComponent = theComponent;
  LoadRecomputable (AIS_WireFrame);
}

//=======================================================================
//function : SetSize
//purpose  : 
//=======================================================================
void AIS_Trihedron::SetSize(const Standard_Real aValue)
{
  myHasOwnSize = Standard_True;
  myDrawer->DatumAspect()->SetAxisLength(aValue, aValue, aValue);

  Update();
  UpdateSelection();
}

//=======================================================================
//function : UnsetSize
//purpose  : if the object has 1 color, the default size of the 
//           drawer is reproduced, otherwise DatumAspect becomes null
//=======================================================================
void AIS_Trihedron::UnsetSize()
{
  if(!myHasOwnSize) return;
  
  myHasOwnSize = Standard_False;
  if(hasOwnColor){
    const Handle(Prs3d_DatumAspect) DA =
      myDrawer->HasLink() ? myDrawer->Link()->DatumAspect() : new Prs3d_DatumAspect();
    myDrawer->DatumAspect()->SetAxisLength(DA->AxisLength(Prs3d_DP_XAxis),
                                           DA->AxisLength(Prs3d_DP_YAxis),
                                           DA->AxisLength(Prs3d_DP_ZAxis));
  }
  else
  Update();
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
                             const Standard_Integer /*theMode*/)
{
  thePrs->SetInfiniteState (Standard_True);

  gp_Ax2 anAxis(myComponent->Ax2());
  myDrawer->DatumAspect()->UpdatePrimitives(myDisplayMode, anAxis.Location(),
                                            anAxis.XDirection(), anAxis.YDirection(),
                                            anAxis.Direction());
  ComputePresentation (thePrsMgr, thePrs);
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
  theSelection->Clear();

  Handle(Prs3d_DatumAspect) anAspect = Attributes()->DatumAspect();
  switch (theMode)
  {
    case 0: // complete triedron: only 1 owner
    {
      Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this,
                                                    mySelectionPriority.Find(Prs3d_DP_None));
      bool isShadingMode = DatumDisplayMode() == Prs3d_DM_Shaded;
      for (int i = isShadingMode ? Prs3d_DP_Origin : Prs3d_DP_XAxis; i <= Prs3d_DP_ZAxis; i++)
      {
        Prs3d_DatumParts aPart = (Prs3d_DatumParts)i;
        if (!anAspect->DrawDatumPart(aPart))
          continue;
        theSelection->Add (CreateSensitiveEntity(aPart, anOwner));
      }
    }
    break;
    case 1: // origin
    {
      Prs3d_DatumParts aPart = Prs3d_DP_Origin;
      if (anAspect->DrawDatumPart(aPart))
      {
        Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart,
                                                              mySelectionPriority.Find(aPart));
        Handle(Graphic3d_ArrayOfPrimitives) aPrimitives = anAspect->ArrayOfPrimitives(aPart);
        theSelection->Add (CreateSensitiveEntity(aPart, anOwner));
      }
    }
    break;
    case 2: // axes
    {
      for (int i = Prs3d_DP_XAxis; i <= Prs3d_DP_ZAxis; i++)
      {
        Prs3d_DatumParts aPart = (Prs3d_DatumParts)i;
        if (!anAspect->DrawDatumPart(aPart))
          continue;
        Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart,
                                                            mySelectionPriority.Find(aPart));
        theSelection->Add (CreateSensitiveEntity(aPart, anOwner));
      }
    }
    break;
    case 3: // main planes
    {
      /// Creates Trihedron owner for each trihedron plane
      {
#ifndef PATCH_TO_OCCT_710
        for (int i = Prs3d_DP_XOYAxis; i <= Prs3d_DP_XOZAxis; i++)
        {
          Prs3d_DatumParts aPart = (Prs3d_DatumParts)i;
          if (!anAspect->DrawDatumPart(aPart))
            continue;
          Handle(SelectMgr_EntityOwner) anOwner = new AIS_TrihedronOwner (this, aPart,
                                                              mySelectionPriority.Find(aPart));
          theSelection->Add (CreateSensitiveEntity(aPart, anOwner));
        }
#endif
      }
    }
    break;
    default:
      break;
  }
}

//=======================================================================
//function : IsAutoHilight
//purpose  :
//=======================================================================
Standard_Boolean AIS_Trihedron::IsAutoHilight() const
{
  // highlight is processed in the current presentation
  return false;
}

//=======================================================================
//function : HilightOwnerWithColor
//purpose  :
//=======================================================================
void AIS_Trihedron::HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                           const Handle(Graphic3d_HighlightStyle)& theStyle,
                                           const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle(AIS_TrihedronOwner) anOwner = Handle(AIS_TrihedronOwner)::DownCast (theOwner);
  if (anOwner.IsNull())
  {
    /// default 0 selectin mode
    Standard_Integer aHiMode = HasHilightMode() ? HilightMode() : 0;
    thePM->Color (this, theStyle, aHiMode, NULL, Graphic3d_ZLayerId_Top);
  }
  else {
    Handle(Prs3d_Presentation) aPresentation = GetHilightPresentation(thePM);
    if (!aPresentation.IsNull())
    {
      aPresentation->Clear();
      const Prs3d_DatumParts& aPart = anOwner->DatumPart();
      Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (aPresentation);
      Handle(Prs3d_DatumAspect) anAspect = Attributes()->DatumAspect();
      if (aPart >= Prs3d_DP_XOYAxis && aPart <= Prs3d_DP_XOZAxis)
      {
        // planes selection is equal in both shading and wireframe mode
        aGroup->SetGroupPrimitivesAspect (myHighlightLineAspect->Aspect());
      }
      else {
        if (DatumDisplayMode() == Prs3d_DM_Shaded)
          aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect(aPart)->Aspect());
        else {
          if (aPart == Prs3d_DP_Origin)
            aGroup->SetGroupPrimitivesAspect (myHighlightPointAspect->Aspect());
          else
            aGroup->SetGroupPrimitivesAspect (anAspect->LineAspect(aPart)->Aspect());
        }
      }
      aGroup->AddPrimitiveArray (anAspect->ArrayOfPrimitives(aPart));

      aPresentation->Highlight (theStyle);
      thePM->AddToImmediateList (aPresentation);
    }
  }
}

//========================================================================
//function : HilightSelected
//purpose  :
//========================================================================
void AIS_Trihedron::HilightSelected (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                     const SelectMgr_SequenceOfOwner& theOwners)
{
  if (theOwners.IsEmpty() || GetContext().IsNull())
  {
    return;
  }

  Handle(Prs3d_DatumAspect) anAspect = Attributes()->DatumAspect();
  bool isShadingMode = DatumDisplayMode() == Prs3d_DM_Shaded;

  SelectMgr_SequenceOfOwner::Iterator anIterator(theOwners);

#ifndef PATCH_TO_OCCT_710
  const Handle(Prs3d_Drawer)& aContextSelStyle = GetContext()->SelectionStyle();
  const Quantity_Color& aSelectionColor = aContextSelStyle->Color();
#else
  const Quantity_Color& aSelectionColor = GetContext()->SelectionStyle()->Color();
#endif
  for (; anIterator.More(); anIterator.Next())
  {
    const Handle(SelectMgr_EntityOwner)& anOwner = anIterator.Value();
    if (!anOwner->IsKind (STANDARD_TYPE (AIS_TrihedronOwner)))
    {
#ifdef PATCH_TO_OCCT_710
      thePM->Color (this, GetContext()->SelectionStyle(), 0);
#else
      thePM->Color (this, aContextSelStyle, 0);
#endif
    }
    else
    {
      Handle(AIS_TrihedronOwner) aTrihedronOwner = Handle(AIS_TrihedronOwner)::DownCast
                                                                             (anOwner);
      const Prs3d_DatumParts& aPart = aTrihedronOwner->DatumPart();
      if (mySelectedParts.Contains(aPart))
      {
        // the owner has been already selected
        continue;
      }
      Handle(Graphic3d_Group) aGroup;
      if (!myPartToGroup.Find(aTrihedronOwner->DatumPart(), aGroup) )
      {
        // graphical group for this owner was not created
        continue;
      }

      if (aPart >= Prs3d_DP_XOYAxis && aPart <= Prs3d_DP_XOZAxis) {
        Handle(Prs3d_LineAspect) aLineAspect = myHighlightLineAspect;
        aLineAspect->SetColor(aSelectionColor);
        aGroup->SetGroupPrimitivesAspect (aLineAspect->Aspect());
      }
      else {
        if (isShadingMode) {
          Handle(Prs3d_ShadingAspect) aShadingAspect = myHighlightAspect;
          aShadingAspect->SetColor(aSelectionColor);
          aGroup->SetGroupPrimitivesAspect (aShadingAspect->Aspect());
        }
        else {
          if (aPart == Prs3d_DP_Origin) {
#ifndef POINT_SELECTION_MODE
#ifdef PATCH_TO_OCCT_710
            myPointAspect->SetColor(aSelectionColor);
            myPointAspect->SetTypeOfMarker(Aspect_TOM_PLUS);
#else
            Handle(Prs3d_PointAspect) aPointAspect = myHighlightPointAspect;
            aPointAspect->SetColor(aSelectionColor);
            aGroup->SetGroupPrimitivesAspect (aPointAspect->Aspect());
#endif
#else
            anAspect->PointAspect()->SetTypeOfMarker(Aspect_TOM_PLUS);
#endif
          }
          else {
            Handle(Prs3d_LineAspect) aLineAspect = myHighlightLineAspect;
            aLineAspect->SetColor(aSelectionColor);
            aGroup->SetGroupPrimitivesAspect (aLineAspect->Aspect());
          }
        }
      }
      mySelectedParts.Append(aPart);
    }
  }
  SynchronizeAspects();
}

//=======================================================================
//function : ClearSelected
//purpose  :
//=======================================================================
void AIS_Trihedron::ClearSelected()
{
  NCollection_List<Prs3d_DatumParts>::Iterator anIterator(mySelectedParts);
  Handle(Prs3d_DatumAspect) anAspect = Attributes()->DatumAspect();
  bool isShadingMode = DatumDisplayMode() == Prs3d_DM_Shaded;
  for (; anIterator.More(); anIterator.Next())
  {
    Prs3d_DatumParts aPart = anIterator.Value();
    Handle(Graphic3d_Group) aGroup = myPartToGroup.Find(aPart);
    if (aPart >= Prs3d_DP_XOYAxis && aPart <= Prs3d_DP_XOZAxis) {
      aGroup->SetGroupPrimitivesAspect (anAspect->LineAspect(aPart)->Aspect());
    }
    if (isShadingMode)
      aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect(aPart)->Aspect());
    else {
      if (aPart == Prs3d_DP_Origin) {
#ifndef POINT_SELECTION_MODE
#ifdef PATCH_TO_OCCT_710
        myPointAspect->SetTypeOfMarker(Aspect_TOM_POINT);
        aGroup->SetGroupPrimitivesAspect(myPointAspect->Aspect());
#else
        aGroup->SetGroupPrimitivesAspect (anAspect->PointAspect()->Aspect());
#endif
#else
        anAspect->PointAspect()->SetTypeOfMarker(Aspect_TOM_EMPTY);
        SynchronizeAspects();
#endif
      }
      else
        aGroup->SetGroupPrimitivesAspect (anAspect->LineAspect(aPart)->Aspect());
    }
  }
  mySelectedParts.Clear();
}

//=======================================================================
//function : ComputePresentation
//purpose  :
//=======================================================================
void AIS_Trihedron::ComputePresentation (const Handle(PrsMgr_PresentationManager3d)& /*thePrsMgr*/,
                                         const Handle(Prs3d_Presentation)& thePrs)
{
  myPartToGroup.Clear();
  Handle(Prs3d_DatumAspect) anAspect = Attributes()->DatumAspect();
  bool isShadingMode = DatumDisplayMode() == Prs3d_DM_Shaded;
  // display origin
  {
    // Origin is visualized only in shading mode
    Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePrs);
    Prs3d_DatumParts aPart = Prs3d_DP_Origin;
#ifdef PATCH_TO_OCCT_710
      // Origin is visualized only in shading mode
      //Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePrs);
      //Prs3d_DatumParts aPart = Prs3d_DP_Origin;
      myPartToGroup.Bind(aPart, aGroup);
      if (isShadingMode)
        aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect(aPart)->Aspect());
      else
        aGroup->SetGroupPrimitivesAspect(myPointAspect->Aspect());
      aGroup->AddPrimitiveArray (anAspect->ArrayOfPrimitives(aPart));
#else
    if (anAspect->DrawDatumPart(aPart)) {
      myPartToGroup.Bind(aPart, aGroup);
      if (isShadingMode)
        aGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect(aPart)->Aspect());
      else {
        aGroup->SetGroupPrimitivesAspect (anAspect->PointAspect()->Aspect());
      }
      aGroup->AddPrimitiveArray (anAspect->ArrayOfPrimitives(aPart));
    }
#endif
  }

  // display axes
  {
    for (Standard_Integer anAxisIter = Prs3d_DP_XAxis; anAxisIter <= Prs3d_DP_ZAxis; ++anAxisIter)
    {
      Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
      if (!anAspect->DrawDatumPart(aPart))
        continue;
      {
        Handle(Graphic3d_Group) anAxisGroup = Prs3d_Root::NewGroup (thePrs);
        myPartToGroup.Bind(aPart, anAxisGroup);
        if (isShadingMode)
          anAxisGroup->SetGroupPrimitivesAspect (anAspect->ShadingAspect(aPart)->Aspect());
        else
          anAxisGroup->SetGroupPrimitivesAspect (anAspect->LineAspect(aPart)->Aspect());
        anAxisGroup->AddPrimitiveArray (anAspect->ArrayOfPrimitives(aPart));

        // draw arrow
        Handle(Graphic3d_Group) anArrowGroup = Prs3d_Root::NewGroup (thePrs);
        anArrowGroup->SetPrimitivesAspect(anAspect->ArrowAspect()->Aspect());

        Prs3d_DatumParts anArrowPart = anAspect->ArrowPartForAxis(aPart);
        if (!anAspect->DrawDatumPart(anArrowPart))
          continue;
        anArrowGroup->AddPrimitiveArray (anAspect->ArrayOfPrimitives(anArrowPart));
      }
    }
  }
  // display labels
  if (anAspect->ToDrawLabels())
  {
    Handle(Geom_Axis2Placement) aComponent = myComponent;
    gp_Pnt anOrigin = aComponent->Location();
    for (Standard_Integer anAxisIter = Prs3d_DP_XAxis; anAxisIter <= Prs3d_DP_ZAxis; ++anAxisIter)
    {
      Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
      if (!anAspect->DrawDatumPart(aPart))
        continue;
      Standard_Real anAxisLength = anAspect->AxisLength(aPart);
      TCollection_ExtendedString aLabel = myLabel.Find(aPart);
      gp_Dir aDir;
      switch (aPart)
      {
        case Prs3d_DP_XAxis: aDir = aComponent->XDirection(); break;
        case Prs3d_DP_YAxis: aDir = aComponent->YDirection(); break;
        case Prs3d_DP_ZAxis: aDir = aComponent->Direction(); break;
        default: break;
      }
      Handle(Graphic3d_Group) aLabelGroup = Prs3d_Root::NewGroup (thePrs);
      gp_Pnt aPoint = anOrigin.XYZ() + aDir.XYZ()*anAxisLength;
      Prs3d_Text::Draw (aLabelGroup, anAspect->TextAspect(), aLabel, aPoint);
    }
  }
  // planes invisible group for planes selection
  /// plane selection mode is available only in wireframe mode
#ifndef PATCH_TO_OCCT_710
  for (Standard_Integer anAxisIter = Prs3d_DP_XOYAxis; anAxisIter <= Prs3d_DP_XOZAxis; ++anAxisIter)
  {
    Prs3d_DatumParts aPart = (Prs3d_DatumParts)anAxisIter;
    if (!anAspect->DrawDatumPart(aPart))
      continue;
    {
      Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePrs);
      myPartToGroup.Bind(aPart, aGroup);

      const Handle(Graphic3d_AspectLine3d)& aLineAspect = anAspect->LineAspect(aPart)->Aspect();
      aLineAspect->SetType(Aspect_TOL_EMPTY);

      aGroup->AddPrimitiveArray (anAspect->ArrayOfPrimitives(aPart));
      aGroup->SetGroupPrimitivesAspect (aLineAspect);
    }
  }
#endif
}

//=======================================================================
//function : LoadRecomputable
//purpose  : 
//=======================================================================

void AIS_Trihedron::LoadRecomputable(const Standard_Integer TheMode)
{
  myRecomputeEveryPrs = Standard_False;
  if(!IsInList(myToRecomputeModes,TheMode))
    myToRecomputeModes.Append(TheMode);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================

void AIS_Trihedron::SetColor (const Prs3d_DatumParts& thePart,
                              const Quantity_Color& theColor)
{
  myDrawer->DatumAspect()->ShadingAspect(thePart)->SetColor(theColor);
  if (thePart != Prs3d_DP_Origin)
    myDrawer->DatumAspect()->LineAspect(thePart)->SetColor(theColor);
}

//=======================================================================
//function : SetTextColor
//purpose  :
//=======================================================================

void AIS_Trihedron::SetTextColor (const Quantity_NameOfColor theColor)
{
  SetTextColor(Quantity_Color(theColor));
}

//=======================================================================
//function : SetTextColor
//purpose  :
//=======================================================================

void AIS_Trihedron::SetTextColor (const Quantity_Color& theColor)
{
  myDrawer->DatumAspect()->TextAspect()->SetColor(theColor);
}

Quantity_Color AIS_Trihedron::Color (const Prs3d_DatumParts& thePart)
{
  Quantity_Color aColor;
  if (DatumDisplayMode() == Prs3d_DM_Shaded)
  {
    aColor = myDrawer->DatumAspect()->ShadingAspect(thePart)->Color();
  }
  else
  {
    aColor = myDrawer->DatumAspect()->LineAspect(thePart)->Aspect()->Color();
  }
  return aColor;
}

//=======================================================================
//function : SetOriginColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetOriginColor (const Quantity_Color& theColor)
{
  if (DatumDisplayMode() == Prs3d_DM_Shaded)
  {
    SetColor(Prs3d_DP_Origin, theColor);
  }
}

//=======================================================================
//function : SetXAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetXAxisColor (const Quantity_Color& theColor)
{
  SetColor(Prs3d_DP_XAxis, theColor);
}

//=======================================================================
//function : SetYAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetYAxisColor (const Quantity_Color& theColor)
{
  SetColor(Prs3d_DP_YAxis, theColor);
}

//=======================================================================
//function : SetAxisColor
//purpose  :
//=======================================================================
void AIS_Trihedron::SetAxisColor (const Quantity_Color& theColor)
{
  SetColor(Prs3d_DP_ZAxis, theColor);
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void AIS_Trihedron::SetColor (const Quantity_NameOfColor theColor)
{
  SetColor(Quantity_Color(theColor));
}

void AIS_Trihedron::SetColor (const Quantity_Color& theColor)
{
  hasOwnColor = Standard_True;
#ifndef PATCH_TO_OCCT_710
  myDrawer->SetColor(theColor);
#endif
  SetColor(Prs3d_DP_Origin, theColor);
  SetColor(Prs3d_DP_XAxis, theColor);
  SetColor(Prs3d_DP_YAxis, theColor);
  SetColor(Prs3d_DP_ZAxis, theColor);
}

void AIS_Trihedron::SetArrowColor (const Quantity_NameOfColor theColor)
{
  SetArrowColor(Quantity_Color(theColor));
}

void AIS_Trihedron::SetArrowColor (const Quantity_Color& theColor)
{
  myHasOwnArrowColor = Standard_True;
  myDrawer->DatumAspect()->ArrowAspect()->SetColor(theColor);
}

//=======================================================================
Standard_Boolean AIS_Trihedron::HasTextColor() const
{
  return myHasOwnTextColor;
}

//=======================================================================
Quantity_Color AIS_Trihedron::TextColor() const
{
  return myDrawer->DatumAspect()->TextAspect()->Aspect()->Color();
}

//=======================================================================
Standard_Boolean AIS_Trihedron::HasArrowColor () const
{
  return myHasOwnArrowColor;
}

//=======================================================================
Quantity_Color AIS_Trihedron::ArrowColor () const
{
  return myDrawer->DatumAspect()->ArrowAspect()->Aspect()->Color();
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
AIS_KindOfInteractive AIS_Trihedron::Type () const 
{
  return AIS_KOI_Datum;
}


//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
 Standard_Integer AIS_Trihedron::Signature () const 
{
  return 3;
 }

//=======================================================================
//function : AcceptDisplayMode
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Trihedron::AcceptDisplayMode (const Standard_Integer theMode) const
{
  return theMode == AIS_WireFrame || theMode == AIS_Shaded;
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void AIS_Trihedron::UnsetColor()
{
  hasOwnColor=Standard_False;
  Quantity_Color aDefaultColor = Quantity_NOC_LIGHTSTEELBLUE4;
  SetColor(aDefaultColor);
  if( HasTextColor() )
  {
    SetTextColor(aDefaultColor.Name());
    myHasOwnTextColor = Standard_False;
  }
  if( HasArrowColor() )
  {
    SetArrowColor(aDefaultColor.Name());
    myHasOwnArrowColor = Standard_False;
  }
}

Handle(SelectBasics_SensitiveEntity) AIS_Trihedron::CreateSensitiveEntity(
                                       const Prs3d_DatumParts& thePart,
                                       const Handle(SelectBasics_EntityOwner)& theOwner) const
{
  Handle(SelectBasics_SensitiveEntity) aSelectEntity;

  Handle(Prs3d_DatumAspect) anAspect = Attributes()->DatumAspect();
  Handle(Graphic3d_ArrayOfPrimitives) aPrimitives = anAspect->ArrayOfPrimitives(thePart);
  if (aPrimitives.IsNull())
    return aSelectEntity;

#ifndef PATCH_TO_OCCT_710
  if (thePart >= Prs3d_DP_XOYAxis && thePart <= Prs3d_DP_XOZAxis)
  { // plane
    Standard_Real aX1, anY1, aZ1, aX2, anY2, aZ2, aX3, anY3, aZ3;
    aPrimitives->Vertice(1, aX1, anY1, aZ1);
    aPrimitives->Vertice(2, aX2, anY2, aZ2);
    aPrimitives->Vertice(3, aX3, anY3, aZ3);
    aSelectEntity = new Select3D_SensitiveTriangle(theOwner, gp_Pnt(aX1, anY1, aZ1),
                                      gp_Pnt(aX2, anY2, aZ2), gp_Pnt(aX3, anY3, aZ3));
  }
  else if (DatumDisplayMode() == Prs3d_DM_Shaded)
#else
  if (DatumDisplayMode() == Prs3d_DM_Shaded)
#endif
  {
    Handle(Select3D_SensitivePrimitiveArray) aSelArray = new Select3D_SensitivePrimitiveArray
                                                                                  (theOwner);
    aSelArray->InitTriangulation (aPrimitives->Attributes(), aPrimitives->Indices(),
                                  TopLoc_Location());
    aSelectEntity = aSelArray;
  }
  else {
    if (!Handle(Graphic3d_ArrayOfPoints)::DownCast(aPrimitives).IsNull())
    {
      Standard_Real aX, anY, aZ;
      aPrimitives->Vertice(1, aX, anY, aZ);
      aSelectEntity = new Select3D_SensitivePoint(theOwner, gp_Pnt(aX, anY, aZ));
    }
    else if (!Handle(Graphic3d_ArrayOfSegments)::DownCast(aPrimitives).IsNull())
    {
      Standard_Real aX1, anY1, aZ1, aX2, anY2, aZ2;
      aPrimitives->Vertice(1, aX1, anY1, aZ1);
      aPrimitives->Vertice(2, aX2, anY2, aZ2);
      aSelectEntity = new Select3D_SensitiveSegment(theOwner, gp_Pnt(aX1, anY1, aZ1),
                                                    gp_Pnt(aX2, anY2, aZ2));
    }
  }
  return aSelectEntity;
}
