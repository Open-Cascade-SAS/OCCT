// Created on: 2000-08-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <XCAFPrs_AISObject.hxx>

#include <AIS_DisplayMode.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Handle_AIS_Drawer.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_LabelSequence.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TopoDS_Iterator.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_DataMapOfShapeStyle.hxx>
#include <XCAFPrs_DataMapIteratorOfDataMapOfShapeStyle.hxx>
#include <XCAFPrs_Style.hxx>

IMPLEMENT_STANDARD_HANDLE (XCAFPrs_AISObject, AIS_ColoredShape)
IMPLEMENT_STANDARD_RTTIEXT(XCAFPrs_AISObject, AIS_ColoredShape)

//=======================================================================
//function : XCAFPrs_AISObject
//purpose  : 
//=======================================================================

XCAFPrs_AISObject::XCAFPrs_AISObject (const TDF_Label& theLabel)
: AIS_ColoredShape(TopoDS_Shape())
{
  myLabel = theLabel;
}

//=======================================================================
//function : DisplayText
//purpose  : 
//=======================================================================

static void DisplayText (const TDF_Label& aLabel,
			 const Handle(Prs3d_Presentation)& aPrs,
			 const Handle(Prs3d_TextAspect)& anAspect,
			 const TopLoc_Location& aLocation)
{
  // first label itself
  Handle (TDataStd_Name) aName;
  if (aLabel.FindAttribute (TDataStd_Name::GetID(), aName)) {
    TopoDS_Shape aShape;
    if (XCAFDoc_ShapeTool::GetShape (aLabel, aShape)) {
      // find the position to display as middle of the bounding box
      aShape.Move (aLocation);
      Bnd_Box aBox;
      BRepBndLib::Add (aShape, aBox);
      if ( ! aBox.IsVoid() ) 
      {
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	aBox.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	gp_Pnt aPnt (0.5 * (aXmin + aXmax), 0.5 * (aYmin + aYmax), 0.5 * (aZmin + aZmax));
	Prs3d_Text::Draw( aPrs, anAspect, aName->Get(), aPnt);
      }
    }
  }

  TDF_LabelSequence seq;
  
  // attibutes of subshapes
  if (XCAFDoc_ShapeTool::GetSubShapes (aLabel, seq)) {
    Standard_Integer i = 1;
    for (i = 1; i <= seq.Length(); i++) {
      TDF_Label aL = seq.Value (i);
      DisplayText (aL, aPrs, anAspect, aLocation); //suppose that subshapes do not contain locations
    }
  }
  
  // attibutes of components
  seq.Clear();
  if (XCAFDoc_ShapeTool::GetComponents (aLabel, seq)) {
    Standard_Integer i = 1;
    for (i = 1; i <= seq.Length(); i++) {
      TDF_Label aL = seq.Value (i);
      DisplayText (aL, aPrs, anAspect, aLocation);
      TDF_Label aRefLabel;
      
      // attributes of referrences
      TopLoc_Location aLoc = XCAFDoc_ShapeTool::GetLocation (aL);
      if (XCAFDoc_ShapeTool::GetReferredShape (aL, aRefLabel)) {
	DisplayText (aRefLabel, aPrs, anAspect, aLoc);
      }
    }
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void XCAFPrs_AISObject::Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                 const Handle(Prs3d_Presentation)&           thePrs,
                                 const Standard_Integer                      theMode)
{
  thePrs->Clear();

  // abv: 06 Mar 00: to have good colors
  Handle(TPrsStd_AISPresentation) aPrs = Handle(TPrsStd_AISPresentation)::DownCast (GetOwner());
  if (aPrs.IsNull() || !aPrs->HasOwnMaterial()) SetMaterial (Graphic3d_NOM_PLASTIC);

  TopoDS_Shape aShape;
  if (!XCAFDoc_ShapeTool::GetShape (myLabel, aShape) || aShape.IsNull()) return;

  // Shape vide -> Assemblage vide.
  if (aShape.ShapeType() == TopAbs_COMPOUND)
  {
    TopoDS_Iterator anExplor (aShape);
    if (!anExplor.More())
    {
      return;
    }
  }

  Set (aShape);
  ClearCustomAspects();

  // Collecting information on colored subshapes
  TopLoc_Location aLoc;
  XCAFPrs_DataMapOfShapeStyle aSettings;
  XCAFPrs::CollectStyleSettings ( myLabel, aLoc, aSettings );

  // Getting default colors
  XCAFPrs_Style aDefStyle;
  DefaultStyle (aDefStyle);
  Quantity_Color aColorCurv = aDefStyle.GetColorCurv();
  Quantity_Color aColorSurf = aDefStyle.GetColorSurf();

  SetColors (myDrawer, aColorCurv, aColorSurf);

  // Set colors etc. for current shape according to style
  for (XCAFPrs_DataMapIteratorOfDataMapOfShapeStyle anIter( aSettings ); anIter.More(); anIter.Next())
  {
    Handle(AIS_ColoredDrawer) aDrawer = CustomAspects (anIter.Key());
    const XCAFPrs_Style& aStyle = anIter.Value();
    aDrawer->SetHidden (!aStyle.IsVisible());

    aColorCurv = aStyle.IsSetColorCurv() ? aStyle.GetColorCurv() : aDefStyle.GetColorCurv();
    aColorSurf = aStyle.IsSetColorSurf() ? aStyle.GetColorSurf() : aDefStyle.GetColorSurf();

    SetColors (aDrawer, aColorCurv, aColorSurf);
  }

  AIS_ColoredShape::Compute (thePresentationManager, thePrs, theMode);

  if (XCAFPrs::GetViewNameMode())
  {
    // Displaying Name attributes
    thePrs->SetDisplayPriority (10);
    DisplayText (myLabel, thePrs, Attributes()->DimensionAspect()->TextAspect(), TopLoc_Location());//no location
  }
}

//=======================================================================
//function : SetColors
//purpose  :
//=======================================================================
void XCAFPrs_AISObject::SetColors (const Handle(AIS_Drawer)& theDrawer,
                                   const Quantity_Color&     theColorCurv,
                                   const Quantity_Color&     theColorSurf)
{
  if (!theDrawer->HasShadingAspect())
  {
    theDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
    *theDrawer->ShadingAspect()->Aspect() = *theDrawer->Link()->ShadingAspect()->Aspect();
  }
  if (!theDrawer->HasLineAspect())
  {
    theDrawer->SetLineAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    *theDrawer->LineAspect()->Aspect() = *theDrawer->Link()->LineAspect()->Aspect();
  }
  if (!theDrawer->HasWireAspect())
  {
    theDrawer->SetWireAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    *theDrawer->WireAspect()->Aspect() = *theDrawer->Link()->WireAspect()->Aspect();
  }
  if (!theDrawer->HasUIsoAspect())
  {
    theDrawer->SetUIsoAspect (new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5, 1));
    *theDrawer->UIsoAspect()->Aspect() = *theDrawer->Link()->UIsoAspect()->Aspect();
    theDrawer->UIsoAspect()->SetNumber (theDrawer->Link()->UIsoAspect()->Number());
  }
  if (!theDrawer->HasVIsoAspect())
  {
    theDrawer->SetVIsoAspect (new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5, 1));
    *theDrawer->VIsoAspect()->Aspect() = *theDrawer->Link()->VIsoAspect()->Aspect();
    theDrawer->VIsoAspect()->SetNumber (theDrawer->Link()->VIsoAspect()->Number());
  }
  if (!theDrawer->HasFreeBoundaryAspect())
  {
    theDrawer->SetFreeBoundaryAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    *theDrawer->FreeBoundaryAspect()->Aspect() = *theDrawer->Link()->FreeBoundaryAspect()->Aspect();
  }
  if (!theDrawer->HasUnFreeBoundaryAspect())
  {
    theDrawer->SetUnFreeBoundaryAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    *theDrawer->UnFreeBoundaryAspect()->Aspect() = *theDrawer->Link()->UnFreeBoundaryAspect()->Aspect();
  }

  theDrawer->UnFreeBoundaryAspect()->SetColor (theColorCurv);
  theDrawer->FreeBoundaryAspect()->SetColor (theColorCurv);
  theDrawer->WireAspect()->SetColor (theColorCurv);

  Graphic3d_MaterialAspect aMaterial = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
  aMaterial.SetColor (theColorSurf);
  theDrawer->ShadingAspect()->Aspect()->SetInteriorColor (theColorSurf);
  theDrawer->ShadingAspect()->Aspect()->SetFrontMaterial (aMaterial);
  theDrawer->UIsoAspect()->SetColor (theColorSurf);
  theDrawer->VIsoAspect()->SetColor (theColorSurf);
}

//=======================================================================
//function : DefaultStyle
//purpose  : DefaultStyle() can be redefined by subclasses in order to set custom default style
//=======================================================================
void XCAFPrs_AISObject::DefaultStyle (XCAFPrs_Style& theStyle) const
{
  theStyle.SetColorSurf (Quantity_NOC_WHITE);
  theStyle.SetColorCurv (Quantity_NOC_WHITE);
}
