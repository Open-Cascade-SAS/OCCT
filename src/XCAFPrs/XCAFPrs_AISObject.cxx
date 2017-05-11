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
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Drawer.hxx>
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


IMPLEMENT_STANDARD_RTTIEXT(XCAFPrs_AISObject,AIS_ColoredShape)

//=======================================================================
//function : XCAFPrs_AISObject
//purpose  : 
//=======================================================================

XCAFPrs_AISObject::XCAFPrs_AISObject (const TDF_Label& theLabel)
: AIS_ColoredShape(TopoDS_Shape()),
  myToSyncStyles (Standard_True)
{
  // define plastic material by default for proper color reproduction
  setMaterial (myDrawer, Graphic3d_NOM_PLASTIC, Standard_False, Standard_False);
  hasOwnMaterial = Standard_True;

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
	Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPrs), anAspect, aName->Get(), aPnt);
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
//function : DispatchStyles
//purpose  :
//=======================================================================
void XCAFPrs_AISObject::DispatchStyles (const Standard_Boolean theToSyncStyles)
{
  myToSyncStyles = theToSyncStyles;
  myShapeColors.Clear();

  TopoDS_Shape aShape;
  if (!XCAFDoc_ShapeTool::GetShape (myLabel, aShape) || aShape.IsNull())
  {
    Set (TopoDS_Shape());
    return;
  }
  Set (aShape);

  // Collecting information on colored subshapes
  TopLoc_Location aLoc;
  XCAFPrs_DataMapOfShapeStyle aSettings;
  XCAFPrs::CollectStyleSettings (myLabel, aLoc, aSettings);

  // Getting default colors
  XCAFPrs_Style aDefStyle;
  DefaultStyle (aDefStyle);
  Quantity_Color aColorCurv = aDefStyle.GetColorCurv();
  Quantity_ColorRGBA aColorSurf = aDefStyle.GetColorSurfRGBA();

  SetColors (myDrawer, aColorCurv, aColorSurf);

  // collect sub-shapes with the same style into compounds
  BRep_Builder aBuilder;
  NCollection_DataMap<XCAFPrs_Style, TopoDS_Compound, XCAFPrs_Style> aStyleGroups;
  for (XCAFPrs_DataMapIteratorOfDataMapOfShapeStyle aStyledShapeIter (aSettings);
       aStyledShapeIter.More(); aStyledShapeIter.Next())
  {
    TopoDS_Compound aComp;
    if (aStyleGroups.Find (aStyledShapeIter.Value(), aComp))
    {
      aBuilder.Add (aComp, aStyledShapeIter.Key());
      continue;
    }

    aBuilder.MakeCompound (aComp);
    aBuilder.Add (aComp, aStyledShapeIter.Key());
    aStyleGroups.Bind (aStyledShapeIter.Value(), aComp);
  }
  aSettings.Clear();

  // assign custom aspects
  for (NCollection_DataMap<XCAFPrs_Style, TopoDS_Compound, XCAFPrs_Style>::Iterator aStyleGroupIter (aStyleGroups);
       aStyleGroupIter.More(); aStyleGroupIter.Next())
  {
    const TopoDS_Compound& aComp = aStyleGroupIter.Value();
    TopoDS_Iterator aShapeIter (aComp);
    TopoDS_Shape aShapeCur = aShapeIter.Value();
    aShapeIter.Next();
    if (aShapeIter.More())
    {
      aShapeCur = aComp;
    }

    Handle(AIS_ColoredDrawer) aDrawer = CustomAspects (aShapeCur);
    const XCAFPrs_Style& aStyle = aStyleGroupIter.Key();
    aDrawer->SetHidden (!aStyle.IsVisible());

    aColorCurv = aStyle.IsSetColorCurv() ? aStyle.GetColorCurv()     : aDefStyle.GetColorCurv();
    aColorSurf = aStyle.IsSetColorSurf() ? aStyle.GetColorSurfRGBA() : aDefStyle.GetColorSurfRGBA();

    SetColors (aDrawer, aColorCurv, aColorSurf);
  }
  aStyleGroups.Clear();
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void XCAFPrs_AISObject::Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                 const Handle(Prs3d_Presentation)&           thePrs,
                                 const Standard_Integer                      theMode)
{
  // update shape and sub-shapes styles only on first compute, or on first recompute
  if (myToSyncStyles)
  {
    Standard_Boolean toMapStyles = myToSyncStyles;
    for (PrsMgr_Presentations::Iterator aPrsIter (myPresentations); aPrsIter.More(); aPrsIter.Next())
    {
      if (aPrsIter.Value().Presentation()->Presentation() != thePrs
      && !aPrsIter.Value().Presentation()->MustBeUpdated())
      {
        toMapStyles = Standard_False;
        break;
      }
    }
    if (toMapStyles)
    {
      DispatchStyles (Standard_True);
    }
  }
  if (myshape.IsNull())
  {
    return;
  }

  if (myshape.ShapeType() == TopAbs_COMPOUND)
  {
    TopoDS_Iterator anExplor (myshape);
    if (!anExplor.More())
    {
      return;
    }
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
void XCAFPrs_AISObject::SetColors (const Handle(Prs3d_Drawer)& theDrawer,
                                   const Quantity_Color&       theColorCurv,
                                   const Quantity_ColorRGBA&   theColorSurf)
{
  if (!theDrawer->HasOwnShadingAspect())
  {
    theDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
    if (theDrawer->HasLink())
    {
      *theDrawer->ShadingAspect()->Aspect() = *theDrawer->Link()->ShadingAspect()->Aspect();
    }
  }
  if (!theDrawer->HasOwnLineAspect())
  {
    theDrawer->SetLineAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    if (theDrawer->HasLink())
    {
      *theDrawer->LineAspect()->Aspect() = *theDrawer->Link()->LineAspect()->Aspect();
    }
  }
  if (!theDrawer->HasOwnWireAspect())
  {
    theDrawer->SetWireAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    if (theDrawer->HasLink())
    {
      *theDrawer->WireAspect()->Aspect() = *theDrawer->Link()->WireAspect()->Aspect();
    }
  }
  if (!theDrawer->HasOwnUIsoAspect())
  {
    theDrawer->SetUIsoAspect (new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5, 1));
    if (theDrawer->HasLink())
    {
      *theDrawer->UIsoAspect()->Aspect() = *theDrawer->Link()->UIsoAspect()->Aspect();
      theDrawer->UIsoAspect()->SetNumber (theDrawer->Link()->UIsoAspect()->Number());
    }
  }
  if (!theDrawer->HasOwnVIsoAspect())
  {
    theDrawer->SetVIsoAspect (new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5, 1));
    if (theDrawer->HasLink())
    {
      *theDrawer->VIsoAspect()->Aspect() = *theDrawer->Link()->VIsoAspect()->Aspect();
      theDrawer->VIsoAspect()->SetNumber (theDrawer->Link()->VIsoAspect()->Number());
    }
  }
  if (!theDrawer->HasOwnFreeBoundaryAspect())
  {
    theDrawer->SetFreeBoundaryAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    if (theDrawer->HasLink())
    {
      *theDrawer->FreeBoundaryAspect()->Aspect() = *theDrawer->Link()->FreeBoundaryAspect()->Aspect();
    }
  }
  if (!theDrawer->HasOwnUnFreeBoundaryAspect())
  {
    theDrawer->SetUnFreeBoundaryAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    if (theDrawer->HasLink())
    {
      *theDrawer->UnFreeBoundaryAspect()->Aspect() = *theDrawer->Link()->UnFreeBoundaryAspect()->Aspect();
    }
  }

  theDrawer->UnFreeBoundaryAspect()->SetColor (theColorCurv);
  theDrawer->FreeBoundaryAspect()->SetColor (theColorCurv);
  theDrawer->WireAspect()->SetColor (theColorCurv);

  Graphic3d_MaterialAspect aMaterial = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
  aMaterial.SetColor (theColorSurf.GetRGB());
  aMaterial.SetAlpha (theColorSurf.Alpha());
  theDrawer->ShadingAspect()->Aspect()->SetInteriorColor (theColorSurf);
  theDrawer->ShadingAspect()->Aspect()->SetFrontMaterial (aMaterial);
  theDrawer->UIsoAspect()->SetColor (theColorSurf.GetRGB());
  theDrawer->VIsoAspect()->SetColor (theColorSurf.GetRGB());
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

// =======================================================================
// function : SetMaterial
// purpose  :
// =======================================================================
void XCAFPrs_AISObject::SetMaterial (const Graphic3d_MaterialAspect& theMaterial)
{
  XCAFPrs_Style aDefStyle;
  DefaultStyle (aDefStyle);
  setMaterial (myDrawer, theMaterial, HasColor(), IsTransparent());
  SetColors (myDrawer, aDefStyle.GetColorCurv(), aDefStyle.GetColorSurf());
  for (AIS_DataMapOfShapeDrawer::Iterator anIter (myShapeColors); anIter.More(); anIter.Next())
  {
    const Handle(AIS_ColoredDrawer)& aDrawer = anIter.Value();

    // take current color
    const Quantity_Color     aColorCurv = aDrawer->WireAspect()->Aspect()->Color();
    const Quantity_ColorRGBA aSurfColor = aDrawer->ShadingAspect()->Aspect()->InteriorColorRGBA();

    // SetColors() will take the material from myDrawer
    SetColors (aDrawer, aColorCurv, aSurfColor);
  }
  SynchronizeAspects();
}
