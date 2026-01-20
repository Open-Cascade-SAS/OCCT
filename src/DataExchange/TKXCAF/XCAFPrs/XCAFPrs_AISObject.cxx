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

#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <TDataStd_Name.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TopoDS_Iterator.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_Style.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <XCAFPrs_Style.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFPrs_AISObject, AIS_ColoredShape)

//=================================================================================================

XCAFPrs_AISObject::XCAFPrs_AISObject(const TDF_Label& theLabel)
    : AIS_ColoredShape(TopoDS_Shape()),
      myToSyncStyles(true)
{
  // define plastic material by default for proper color reproduction
  setMaterial(myDrawer, Graphic3d_NameOfMaterial_Plastified, false, false);
  hasOwnMaterial = true;

  myLabel = theLabel;
}

//=================================================================================================

static void DisplayText(const TDF_Label&                  aLabel,
                        const occ::handle<Prs3d_Presentation>& aPrs,
                        const occ::handle<Prs3d_TextAspect>&   anAspect,
                        const TopLoc_Location&            aLocation)
{
  // first label itself
  occ::handle<TDataStd_Name> aName;
  if (aLabel.FindAttribute(TDataStd_Name::GetID(), aName))
  {
    TopoDS_Shape aShape;
    if (XCAFDoc_ShapeTool::GetShape(aLabel, aShape))
    {
      // find the position to display as middle of the bounding box
      aShape.Move(aLocation);
      Bnd_Box aBox;
      BRepBndLib::Add(aShape, aBox);
      if (!aBox.IsVoid())
      {
        double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
        aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
        gp_Pnt aPnt(0.5 * (aXmin + aXmax), 0.5 * (aYmin + aYmax), 0.5 * (aZmin + aZmax));
        Prs3d_Text::Draw(aPrs->CurrentGroup(), anAspect, aName->Get(), aPnt);
      }
    }
  }

  NCollection_Sequence<TDF_Label> seq;

  // attributes of subshapes
  if (XCAFDoc_ShapeTool::GetSubShapes(aLabel, seq))
  {
    int i = 1;
    for (i = 1; i <= seq.Length(); i++)
    {
      TDF_Label aL = seq.Value(i);
      // clang-format off
      DisplayText (aL, aPrs, anAspect, aLocation); //suppose that subshapes do not contain locations
      // clang-format on
    }
  }

  // attributes of components
  seq.Clear();
  if (XCAFDoc_ShapeTool::GetComponents(aLabel, seq))
  {
    int i = 1;
    for (i = 1; i <= seq.Length(); i++)
    {
      TDF_Label aL = seq.Value(i);
      DisplayText(aL, aPrs, anAspect, aLocation);
      TDF_Label aRefLabel;

      // attributes of references
      TopLoc_Location aLoc = XCAFDoc_ShapeTool::GetLocation(aL);
      if (XCAFDoc_ShapeTool::GetReferredShape(aL, aRefLabel))
      {
        DisplayText(aRefLabel, aPrs, anAspect, aLoc);
      }
    }
  }
}

//=================================================================================================

void XCAFPrs_AISObject::DispatchStyles(const bool theToSyncStyles)
{
  myToSyncStyles = theToSyncStyles;
  myShapeColors.Clear();

  TopoDS_Shape aShape;
  if (!XCAFDoc_ShapeTool::GetShape(myLabel, aShape) || aShape.IsNull())
  {
    Set(TopoDS_Shape());
    return;
  }
  Set(aShape);

  // Collecting information on colored subshapes
  TopLoc_Location                    aLoc;
  NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher> aSettings;
  XCAFPrs::CollectStyleSettings(myLabel, aLoc, aSettings);

  // Getting default colors
  XCAFPrs_Style aDefStyle;
  DefaultStyle(aDefStyle);
  setStyleToDrawer(myDrawer,
                   aDefStyle,
                   aDefStyle,
                   myDrawer->ShadingAspect()->Aspect()->FrontMaterial());

  // collect sub-shapes with the same style into compounds
  BRep_Builder                                               aBuilder;
  NCollection_IndexedDataMap<XCAFPrs_Style, TopoDS_Compound> aStyleGroups;
  for (NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher>::Iterator aStyledShapeIter(aSettings);
       aStyledShapeIter.More();
       aStyledShapeIter.Next())
  {
    TopoDS_Compound aComp;
    if (aStyleGroups.FindFromKey(aStyledShapeIter.Value(), aComp))
    {
      aBuilder.Add(aComp, aStyledShapeIter.Key());
      continue;
    }

    aBuilder.MakeCompound(aComp);
    aBuilder.Add(aComp, aStyledShapeIter.Key());
    TopoDS_Compound* aMapShape = aStyleGroups.ChangeSeek(aStyledShapeIter.Value());
    if (aMapShape == NULL)
      aStyleGroups.Add(aStyledShapeIter.Value(), aComp);
    else
      *aMapShape = aComp;
  }
  aSettings.Clear();

  // assign custom aspects
  for (NCollection_IndexedDataMap<XCAFPrs_Style, TopoDS_Compound>::Iterator aStyleGroupIter(
         aStyleGroups);
       aStyleGroupIter.More();
       aStyleGroupIter.Next())
  {
    const TopoDS_Compound& aComp = aStyleGroupIter.Value();
    TopoDS_Iterator        aShapeIter(aComp);
    TopoDS_Shape           aShapeCur = aShapeIter.Value();
    aShapeIter.Next();
    if (aShapeIter.More())
    {
      aShapeCur = aComp;
    }

    occ::handle<AIS_ColoredDrawer> aDrawer = new AIS_ColoredDrawer(myDrawer);
    myShapeColors.Bind(aShapeCur, aDrawer);
    const XCAFPrs_Style& aStyle = aStyleGroupIter.Key();
    aDrawer->SetHidden(!aStyle.IsVisible());
    if (!aStyle.Material().IsNull() && !aStyle.Material()->IsEmpty())
    {
      aDrawer->SetOwnMaterial();
    }
    if (aStyle.IsSetColorSurf() || aStyle.IsSetColorCurv())
    {
      aDrawer->SetOwnColor(Quantity_Color());
    }
    setStyleToDrawer(aDrawer,
                     aStyle,
                     aDefStyle,
                     myDrawer->ShadingAspect()->Aspect()->FrontMaterial());
  }
  aStyleGroups.Clear();
}

//=================================================================================================

void XCAFPrs_AISObject::Compute(const occ::handle<PrsMgr_PresentationManager>& thePresentationManager,
                                const occ::handle<Prs3d_Presentation>&         thePrs,
                                const int                    theMode)
{
  // update shape and sub-shapes styles only on first compute, or on first recompute
  if (myToSyncStyles)
  {
    bool toMapStyles = myToSyncStyles;
    for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(myPresentations); aPrsIter.More(); aPrsIter.Next())
    {
      if (aPrsIter.Value() != thePrs && !aPrsIter.Value()->MustBeUpdated())
      {
        toMapStyles = false;
        break;
      }
    }
    if (toMapStyles)
    {
      DispatchStyles(true);
    }
  }
  if (myshape.IsNull())
  {
    return;
  }

  if (myshape.ShapeType() == TopAbs_COMPOUND)
  {
    if (myshape.NbChildren() == 0)
    {
      return;
    }
  }

  AIS_ColoredShape::Compute(thePresentationManager, thePrs, theMode);

  if (XCAFPrs::GetViewNameMode())
  {
    // Displaying Name attributes
    thePrs->SetDisplayPriority(Graphic3d_DisplayPriority_Topmost);
    // clang-format off
    DisplayText (myLabel, thePrs, Attributes()->DimensionAspect()->TextAspect(), TopLoc_Location());//no location
    // clang-format on
  }
}

//=================================================================================================

void XCAFPrs_AISObject::setStyleToDrawer(const occ::handle<Prs3d_Drawer>&     theDrawer,
                                         const XCAFPrs_Style&            theStyle,
                                         const XCAFPrs_Style&            theDefStyle,
                                         const Graphic3d_MaterialAspect& theDefMaterial)
{
  theDrawer->SetupOwnShadingAspect();
  theDrawer->SetOwnLineAspects();

  Quantity_ColorRGBA                 aSurfColor = theDefStyle.GetColorSurfRGBA();
  Quantity_Color                     aCurvColor = theDefStyle.GetColorCurv();
  Graphic3d_MaterialAspect           aMaterial  = theDefMaterial;
  const occ::handle<XCAFDoc_VisMaterial>& anXMat =
    !theStyle.Material().IsNull() ? theStyle.Material() : theDefStyle.Material();
  if (!anXMat.IsNull() && !anXMat->IsEmpty())
  {
    anXMat->FillAspect(theDrawer->ShadingAspect()->Aspect());
    aMaterial  = theDrawer->ShadingAspect()->Aspect()->FrontMaterial();
    aSurfColor = Quantity_ColorRGBA(aMaterial.Color(), aMaterial.Alpha());
    aCurvColor = aMaterial.Color();
  }
  if (theStyle.IsSetColorSurf())
  {
    aSurfColor = theStyle.GetColorSurfRGBA();
    aMaterial.SetColor(aSurfColor.GetRGB());
    aMaterial.SetAlpha(aSurfColor.Alpha());
  }
  if (theStyle.IsSetColorCurv())
  {
    aCurvColor = theStyle.GetColorCurv();
  }

  theDrawer->UnFreeBoundaryAspect()->SetColor(aCurvColor);
  theDrawer->FreeBoundaryAspect()->SetColor(aCurvColor);
  theDrawer->WireAspect()->SetColor(aCurvColor);

  theDrawer->ShadingAspect()->Aspect()->SetInteriorColor(aSurfColor);
  theDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(aMaterial);
  theDrawer->UIsoAspect()->SetColor(aSurfColor.GetRGB());
  theDrawer->VIsoAspect()->SetColor(aSurfColor.GetRGB());
}

//=======================================================================
// function : DefaultStyle
// purpose  : DefaultStyle() can be redefined by subclasses in order to set custom default style
//=======================================================================
void XCAFPrs_AISObject::DefaultStyle(XCAFPrs_Style& theStyle) const
{
  theStyle.SetColorSurf(Quantity_NOC_WHITE);
  theStyle.SetColorCurv(Quantity_NOC_WHITE);
}

//=================================================================================================

void XCAFPrs_AISObject::SetMaterial(const Graphic3d_MaterialAspect& theMaterial)
{
  XCAFPrs_Style aDefStyle;
  DefaultStyle(aDefStyle);
  setMaterial(myDrawer, theMaterial, HasColor(), IsTransparent());
  setStyleToDrawer(myDrawer,
                   aDefStyle,
                   aDefStyle,
                   myDrawer->ShadingAspect()->Aspect()->FrontMaterial());
  for (NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>::Iterator anIter(myShapeColors); anIter.More(); anIter.Next())
  {
    const occ::handle<AIS_ColoredDrawer>& aDrawer = anIter.Value();
    if (aDrawer->HasOwnMaterial())
    {
      continue;
    }

    if (aDrawer->HasOwnShadingAspect())
    {
      // take current color
      const Quantity_ColorRGBA aSurfColor = aDrawer->ShadingAspect()->Aspect()->InteriorColorRGBA();
      Graphic3d_MaterialAspect aMaterial  = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
      aMaterial.SetColor(aSurfColor.GetRGB());
      aMaterial.SetAlpha(aSurfColor.Alpha());
      aDrawer->ShadingAspect()->Aspect()->SetInteriorColor(aSurfColor);
      aDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(aMaterial);
    }
  }
  SynchronizeAspects();
}
