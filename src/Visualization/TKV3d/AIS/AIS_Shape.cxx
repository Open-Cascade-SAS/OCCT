// Created on: 1996-12-20
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#include <AIS_Shape.hxx>

#include <AIS_GraphicTool.hxx>
#include <AIS_InteractiveContext.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_Structure.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <HLRBRep.hxx>
#include <OSD_Timer.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_BndBox.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <StdPrs_HLRShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdSelect.hxx>
#include <StdSelect_BRepSelectionTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Shape, AIS_InteractiveObject)

// Auxiliary macros
#define replaceAspectWithDef(theMap, theAspect)                                                    \
  if (myDrawer->Link()->theAspect()->Aspect() != myDrawer->theAspect()->Aspect())                  \
  {                                                                                                \
    theMap.Bind(myDrawer->theAspect()->Aspect(), myDrawer->Link()->theAspect()->Aspect());         \
  }

// Auxiliary macros for replaceWithNewOwnAspects()
#define replaceAspectWithOwn(theMap, theAspect)                                                    \
  if (myDrawer->Link()->theAspect()->Aspect() != myDrawer->theAspect()->Aspect())                  \
  {                                                                                                \
    theMap.Bind(myDrawer->Link()->theAspect()->Aspect(), myDrawer->theAspect()->Aspect());         \
  }

//=================================================================================================

void AIS_Shape::replaceWithNewOwnAspects()
{
  NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>> aReplaceMap;

  replaceAspectWithOwn(aReplaceMap, ShadingAspect);
  replaceAspectWithOwn(aReplaceMap, LineAspect);
  replaceAspectWithOwn(aReplaceMap, WireAspect);
  replaceAspectWithOwn(aReplaceMap, FreeBoundaryAspect);
  replaceAspectWithOwn(aReplaceMap, UnFreeBoundaryAspect);
  replaceAspectWithOwn(aReplaceMap, SeenLineAspect);
  replaceAspectWithOwn(aReplaceMap, FaceBoundaryAspect);
  replaceAspectWithOwn(aReplaceMap, PointAspect);

  replaceAspects(aReplaceMap);
}

//=================================================================================================

AIS_Shape::AIS_Shape(const TopoDS_Shape& theShape)
    : AIS_InteractiveObject(PrsMgr_TOP_ProjectorDependent),
      myshape(theShape),
      myUVOrigin(0.0, 0.0),
      myUVRepeat(1.0, 1.0),
      myUVScale(1.0, 1.0),
      myInitAng(0.0),
      myCompBB(true)
{}

//=================================================================================================

void AIS_Shape::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                        const occ::handle<Prs3d_Presentation>& thePrs,
                        const int                              theMode)
{
  if (myshape.IsNull() || (myshape.ShapeType() == TopAbs_COMPOUND && myshape.NbChildren() == 0))
  {
    return;
  }

  // wire,edge,vertex -> pas de HLR + priorite display superieure
  if (myshape.ShapeType() >= TopAbs_WIRE && myshape.ShapeType() <= TopAbs_VERTEX)
  {
    // TopAbs_WIRE -> 7, TopAbs_EDGE -> 8, TopAbs_VERTEX -> 9 (Graphic3d_DisplayPriority_Highlight)
    const int aPrior =
      (int)Graphic3d_DisplayPriority_Above1 + (int)myshape.ShapeType() - TopAbs_WIRE;
    thePrs->SetVisual(Graphic3d_TOS_ALL);
    thePrs->SetDisplayPriority((Graphic3d_DisplayPriority)aPrior);
  }

  if (IsInfinite())
  {
    thePrs->SetInfiniteState(true); // not taken in account during FITALL
  }

  switch (theMode)
  {
    case AIS_WireFrame: {
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange(myshape, myDrawer, true);
      try
      {
        OCC_CATCH_SIGNALS
        StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
      }
      catch (Standard_Failure const& anException)
      {
        Message::SendFail(
          TCollection_AsciiString(
            "Error: AIS_Shape::Compute() wireframe presentation builder has failed (")
          + anException.GetMessageString() + ")");
      }
      break;
    }
    case AIS_Shaded: {
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange(myshape, myDrawer, true);
      if ((int)myshape.ShapeType() > 4)
      {
        StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
      }
      else
      {
        if (IsInfinite())
        {
          StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
        }
        else
        {
          try
          {
            OCC_CATCH_SIGNALS
            StdPrs_ShadedShape::Add(
              thePrs,
              myshape,
              myDrawer,
              myDrawer->ShadingAspect()->Aspect()->ToMapTexture()
                && !myDrawer->ShadingAspect()->Aspect()->TextureMap().IsNull(),
              myUVOrigin,
              myUVRepeat,
              myUVScale);
          }
          catch (Standard_Failure const& anException)
          {
            Message::SendFail(
              TCollection_AsciiString(
                "Error: AIS_Shape::Compute() shaded presentation builder has failed (")
              + anException.GetMessageString() + ")");
            StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
          }
        }
      }
      double aTransparency = Transparency();
      if (aTransparency > 0.0)
      {
        SetTransparency(aTransparency);
      }
      break;
    }

    // Bounding box.
    case 2: {
      if (IsInfinite())
      {
        StdPrs_WFShape::Add(thePrs, myshape, myDrawer);
      }
      else
      {
        Prs3d_BndBox::Add(thePrs, BoundingBox(), myDrawer);
      }
    }
  }

  // Recompute hidden line presentation (if necessary).
  thePrs->ReCompute();
}

//=================================================================================================

void AIS_Shape::computeHlrPresentation(const occ::handle<Graphic3d_Camera>&   theProjector,
                                       const occ::handle<Prs3d_Presentation>& thePrs,
                                       const TopoDS_Shape&                    theShape,
                                       const occ::handle<Prs3d_Drawer>&       theDrawer)
{
  if (theShape.IsNull())
  {
    return;
  }

  switch (theShape.ShapeType())
  {
    case TopAbs_VERTEX:
    case TopAbs_EDGE:
    case TopAbs_WIRE: {
      thePrs->SetDisplayPriority(Graphic3d_DisplayPriority_Below);
      StdPrs_WFShape::Add(thePrs, theShape, theDrawer);
      return;
    }
    case TopAbs_COMPOUND: {
      if (theShape.NbChildren() == 0)
      {
        return;
      }
      break;
    }
    default: {
      break;
    }
  }

  const occ::handle<Prs3d_Drawer>& aDefDrawer = theDrawer->Link();
  if (aDefDrawer->DrawHiddenLine())
  {
    theDrawer->EnableDrawHiddenLine();
  }
  else
  {
    theDrawer->DisableDrawHiddenLine();
  }

  const Aspect_TypeOfDeflection aPrevDef = aDefDrawer->TypeOfDeflection();
  aDefDrawer->SetTypeOfDeflection(Aspect_TOD_RELATIVE);
  if (theDrawer->IsAutoTriangulation())
  {
    StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange(theShape, theDrawer, true);
  }

  {
    try
    {
      OCC_CATCH_SIGNALS
      switch (theDrawer->TypeOfHLR())
      {
        case Prs3d_TOH_Algo: {
          StdPrs_HLRShape aBuilder;
          aBuilder.ComputeHLR(thePrs, theShape, theDrawer, theProjector);
          break;
        }
        case Prs3d_TOH_PolyAlgo:
        case Prs3d_TOH_NotSet: {
          StdPrs_HLRPolyShape aBuilder;
          aBuilder.ComputeHLR(thePrs, theShape, theDrawer, theProjector);
          break;
        }
      }
    }
    catch (Standard_Failure const& anException)
    {
      Message::SendFail(
        TCollection_AsciiString("Error: AIS_Shape::Compute() HLR Algorithm has failed (")
        + anException.GetMessageString() + ")");
      StdPrs_WFShape::Add(thePrs, theShape, theDrawer);
    }
  }

  aDefDrawer->SetTypeOfDeflection(aPrevDef);
}

//=================================================================================================

void AIS_Shape::ComputeSelection(const occ::handle<SelectMgr_Selection>& aSelection,
                                 const int                               aMode)
{
  if (myshape.IsNull())
    return;
  if (myshape.ShapeType() == TopAbs_COMPOUND && myshape.NbChildren() == 0)
  {
    // empty Shape -> empty Assembly.
    return;
  }

  TopAbs_ShapeEnum TypOfSel = AIS_Shape::SelectionType(aMode);
  TopoDS_Shape     shape    = myshape;

  // POP protection against crash in low layers

  double aDeflection = StdPrs_ToolTriangulatedShape::GetDeflection(shape, myDrawer);
  try
  {
    OCC_CATCH_SIGNALS
    StdSelect_BRepSelectionTool::Load(aSelection,
                                      this,
                                      shape,
                                      TypOfSel,
                                      aDeflection,
                                      myDrawer->DeviationAngle(),
                                      myDrawer->IsAutoTriangulation());
  }
  catch (Standard_Failure const& anException)
  {
    Message::SendFail(TCollection_AsciiString("Error: AIS_Shape::ComputeSelection(") + aMode
                      + ") has failed (" + anException.GetMessageString() + ")");
    if (aMode == 0)
    {
      aSelection->Clear();
      Bnd_Box                            B             = BoundingBox();
      occ::handle<StdSelect_BRepOwner>   aOwner        = new StdSelect_BRepOwner(shape, this);
      occ::handle<Select3D_SensitiveBox> aSensitiveBox = new Select3D_SensitiveBox(aOwner, B);
      aSelection->Add(aSensitiveBox);
    }
  }

  // insert the drawer in the BrepOwners for hilight...
  StdSelect::SetDrawerForBRepOwner(aSelection, myDrawer);
}

void AIS_Shape::Color(Quantity_Color& theColor) const
{
  if (const occ::handle<Prs3d_ShadingAspect>& aShading = myDrawer->ShadingAspect())
  {
    theColor = aShading->Color(myCurrentFacingModel);
  }
}

Graphic3d_NameOfMaterial AIS_Shape::Material() const
{
  const occ::handle<Prs3d_ShadingAspect>& aShading = myDrawer->ShadingAspect();
  return !aShading.IsNull() ? aShading->Material(myCurrentFacingModel).Name()
                            : Graphic3d_NameOfMaterial_DEFAULT;
}

double AIS_Shape::Transparency() const
{
  const occ::handle<Prs3d_ShadingAspect>& aShading = myDrawer->ShadingAspect();
  return !aShading.IsNull() ? aShading->Transparency(myCurrentFacingModel) : 0.0;
}

//=================================================================================================

bool AIS_Shape::setColor(const occ::handle<Prs3d_Drawer>& theDrawer,
                         const Quantity_Color&            theColor) const
{
  bool toRecompute = false;
  toRecompute      = theDrawer->SetupOwnShadingAspect() || toRecompute;
  toRecompute      = theDrawer->SetOwnLineAspects() || toRecompute;
  toRecompute      = theDrawer->SetupOwnPointAspect() || toRecompute;

  // override color
  theDrawer->ShadingAspect()->SetColor(theColor, myCurrentFacingModel);
  theDrawer->LineAspect()->SetColor(theColor);
  theDrawer->WireAspect()->SetColor(theColor);
  theDrawer->PointAspect()->SetColor(theColor);
  theDrawer->FreeBoundaryAspect()->SetColor(theColor);
  theDrawer->UnFreeBoundaryAspect()->SetColor(theColor);
  theDrawer->SeenLineAspect()->SetColor(theColor);
  theDrawer->FaceBoundaryAspect()->SetColor(theColor);
  return toRecompute;
}

//=================================================================================================

void AIS_Shape::SetColor(const Quantity_Color& theColor)
{
  const bool toRecompute = setColor(myDrawer, theColor);
  myDrawer->SetColor(theColor);
  hasOwnColor = true;

  if (!toRecompute || !myDrawer->HasLink())
  {
    SynchronizeAspects();
  }
  else
  {
    replaceWithNewOwnAspects();
  }
  recomputeComputed();
}

//=================================================================================================

void AIS_Shape::UnsetColor()
{
  if (!HasColor())
  {
    return;
  }

  hasOwnColor = false;
  myDrawer->SetColor(myDrawer->HasLink() ? myDrawer->Link()->Color()
                                         : Quantity_Color(Quantity_NOC_WHITE));

  NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>> aReplaceMap;
  if (!HasWidth())
  {
    replaceAspectWithDef(aReplaceMap, LineAspect);
    replaceAspectWithDef(aReplaceMap, WireAspect);
    replaceAspectWithDef(aReplaceMap, FreeBoundaryAspect);
    replaceAspectWithDef(aReplaceMap, UnFreeBoundaryAspect);
    replaceAspectWithDef(aReplaceMap, SeenLineAspect);
    replaceAspectWithDef(aReplaceMap, FaceBoundaryAspect);
    myDrawer->SetLineAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetWireAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetFreeBoundaryAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetUnFreeBoundaryAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetSeenLineAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetFaceBoundaryAspect(occ::handle<Prs3d_LineAspect>());
  }
  else
  {
    Quantity_Color aColor = Quantity_NOC_YELLOW;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor(myDrawer->Link(), AIS_TOA_Line, aColor);
    }
    myDrawer->LineAspect()->SetColor(aColor);
    aColor = Quantity_NOC_RED;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor(myDrawer->Link(), AIS_TOA_Wire, aColor);
    }
    myDrawer->WireAspect()->SetColor(aColor);
    aColor = Quantity_NOC_GREEN;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor(myDrawer->Link(), AIS_TOA_Free, aColor);
    }
    myDrawer->FreeBoundaryAspect()->SetColor(aColor);
    aColor = Quantity_NOC_YELLOW;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor(myDrawer->Link(), AIS_TOA_UnFree, aColor);
    }
    myDrawer->UnFreeBoundaryAspect()->SetColor(aColor);
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor(myDrawer->Link(), AIS_TOA_Seen, aColor);
    }
    myDrawer->SeenLineAspect()->SetColor(aColor);
    aColor = Quantity_NOC_BLACK;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor(myDrawer->Link(), AIS_TOA_FaceBoundary, aColor);
    }
    myDrawer->FaceBoundaryAspect()->SetColor(aColor);
  }

  if (!myDrawer->HasOwnShadingAspect())
  {
    //
  }
  else if (HasMaterial() || IsTransparent() || myDrawer->ShadingAspect()->Aspect()->ToMapTexture())
  {
    const Graphic3d_MaterialAspect aDefaultMat(Graphic3d_NameOfMaterial_Brass);
    Graphic3d_MaterialAspect       mat                 = aDefaultMat;
    Quantity_Color                 anInteriorColors[2] = {Quantity_NOC_CYAN1, Quantity_NOC_CYAN1};
    if (myDrawer->HasLink())
    {
      anInteriorColors[0] = myDrawer->Link()->ShadingAspect()->Aspect()->InteriorColor();
      anInteriorColors[1] = myDrawer->Link()->ShadingAspect()->Aspect()->BackInteriorColor();
    }
    if (HasMaterial() || myDrawer->HasLink())
    {
      const occ::handle<Graphic3d_AspectFillArea3d>& aSrcAspect =
        (HasMaterial() ? myDrawer : myDrawer->Link())->ShadingAspect()->Aspect();
      mat = myCurrentFacingModel != Aspect_TOFM_BACK_SIDE ? aSrcAspect->FrontMaterial()
                                                          : aSrcAspect->BackMaterial();
    }
    if (HasMaterial())
    {
      const Quantity_Color aColor =
        myDrawer->HasLink() ? myDrawer->Link()->ShadingAspect()->Color(myCurrentFacingModel)
                            : aDefaultMat.AmbientColor();
      mat.SetColor(aColor);
    }
    if (IsTransparent())
    {
      double aTransp = myDrawer->ShadingAspect()->Transparency(myCurrentFacingModel);
      mat.SetTransparency(float(aTransp));
    }
    myDrawer->ShadingAspect()->SetMaterial(mat, myCurrentFacingModel);
    myDrawer->ShadingAspect()->Aspect()->SetInteriorColor(anInteriorColors[0]);
    myDrawer->ShadingAspect()->Aspect()->SetBackInteriorColor(anInteriorColors[1]);
  }
  else
  {
    replaceAspectWithDef(aReplaceMap, ShadingAspect);
    myDrawer->SetShadingAspect(occ::handle<Prs3d_ShadingAspect>());
  }
  if (myDrawer->HasOwnPointAspect())
  {
    replaceAspectWithDef(aReplaceMap, PointAspect);
    myDrawer->SetPointAspect(occ::handle<Prs3d_PointAspect>());
  }
  replaceAspects(aReplaceMap);
  SynchronizeAspects();
  recomputeComputed();
}

//=================================================================================================

bool AIS_Shape::setWidth(const occ::handle<Prs3d_Drawer>& theDrawer,
                         const double                     theLineWidth) const
{
  bool toRecompute = theDrawer->SetOwnLineAspects();

  // override width
  theDrawer->LineAspect()->SetWidth(theLineWidth);
  theDrawer->WireAspect()->SetWidth(theLineWidth);
  theDrawer->FreeBoundaryAspect()->SetWidth(theLineWidth);
  theDrawer->UnFreeBoundaryAspect()->SetWidth(theLineWidth);
  theDrawer->SeenLineAspect()->SetWidth(theLineWidth);
  theDrawer->FaceBoundaryAspect()->SetWidth(theLineWidth);
  return toRecompute;
}

//=================================================================================================

void AIS_Shape::SetWidth(const double theLineWidth)
{
  myOwnWidth = (float)theLineWidth;

  if (!setWidth(myDrawer, theLineWidth) || !myDrawer->HasLink())
  {
    SynchronizeAspects();
  }
  else
  {
    replaceWithNewOwnAspects();
  }
  recomputeComputed();
}

//=================================================================================================

void AIS_Shape::UnsetWidth()
{
  if (myOwnWidth == 0.0f)
  {
    return;
  }

  myOwnWidth = 0.0f;
  if (!HasColor())
  {
    NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>> aReplaceMap;
    replaceAspectWithDef(aReplaceMap, LineAspect);
    replaceAspectWithDef(aReplaceMap, WireAspect);
    replaceAspectWithDef(aReplaceMap, FreeBoundaryAspect);
    replaceAspectWithDef(aReplaceMap, UnFreeBoundaryAspect);
    replaceAspectWithDef(aReplaceMap, SeenLineAspect);
    replaceAspectWithDef(aReplaceMap, FaceBoundaryAspect);
    myDrawer->SetLineAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetWireAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetFreeBoundaryAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetUnFreeBoundaryAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetSeenLineAspect(occ::handle<Prs3d_LineAspect>());
    myDrawer->SetFaceBoundaryAspect(occ::handle<Prs3d_LineAspect>());
    replaceAspects(aReplaceMap);
  }
  else
  {
    myDrawer->LineAspect()->SetWidth(
      myDrawer->HasLink() ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_Line) : 1.);
    myDrawer->WireAspect()->SetWidth(
      myDrawer->HasLink() ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_Wire) : 1.);
    myDrawer->FreeBoundaryAspect()->SetWidth(
      myDrawer->HasLink() ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_Free) : 1.);
    myDrawer->UnFreeBoundaryAspect()->SetWidth(
      myDrawer->HasLink() ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_UnFree) : 1.);
    myDrawer->SeenLineAspect()->SetWidth(
      myDrawer->HasLink() ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_Seen) : 1.);
    myDrawer->FaceBoundaryAspect()->SetWidth(
      myDrawer->HasLink() ? AIS_GraphicTool::GetLineWidth(myDrawer->Link(), AIS_TOA_FaceBoundary)
                          : 1.);
    SynchronizeAspects();
  }
  recomputeComputed();
}

//=================================================================================================

void AIS_Shape::setMaterial(const occ::handle<Prs3d_Drawer>& theDrawer,
                            const Graphic3d_MaterialAspect&  theMaterial,
                            const bool                       theToKeepColor,
                            const bool                       theToKeepTransp) const
{
  theDrawer->SetupOwnShadingAspect();

  const Quantity_Color aColor  = theDrawer->ShadingAspect()->Color(myCurrentFacingModel);
  const double         aTransp = theDrawer->ShadingAspect()->Transparency(myCurrentFacingModel);
  theDrawer->ShadingAspect()->SetMaterial(theMaterial, myCurrentFacingModel);

  if (theToKeepColor)
  {
    theDrawer->ShadingAspect()->SetColor(aColor, myCurrentFacingModel);
  }
  if (theToKeepTransp)
  {
    theDrawer->ShadingAspect()->SetTransparency(aTransp, myCurrentFacingModel);
  }
}

//=================================================================================================

void AIS_Shape::SetMaterial(const Graphic3d_MaterialAspect& theMat)
{
  const bool toRecompute = !myDrawer->HasOwnShadingAspect();
  setMaterial(myDrawer, theMat, HasColor(), IsTransparent());
  hasOwnMaterial = true;

  if (!toRecompute || !myDrawer->HasLink())
  {
    SynchronizeAspects();
  }
  else
  {
    replaceWithNewOwnAspects();
  }
}

//=================================================================================================

void AIS_Shape::UnsetMaterial()
{
  if (!HasMaterial())
  {
    return;
  }

  if (!myDrawer->HasOwnShadingAspect())
  {
    //
  }
  else if (HasColor() || IsTransparent() || myDrawer->ShadingAspect()->Aspect()->ToMapTexture())
  {
    if (myDrawer->HasLink())
    {
      myDrawer->ShadingAspect()->SetMaterial(
        myDrawer->Link()->ShadingAspect()->Material(myCurrentFacingModel),
        myCurrentFacingModel);
    }
    if (HasColor())
    {
      myDrawer->ShadingAspect()->SetColor(myDrawer->Color(), myCurrentFacingModel);
      myDrawer->ShadingAspect()->SetTransparency(myDrawer->Transparency(), myCurrentFacingModel);
    }
    SynchronizeAspects();
  }
  else
  {
    NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>> aReplaceMap;
    replaceAspectWithDef(aReplaceMap, ShadingAspect);
    myDrawer->SetShadingAspect(occ::handle<Prs3d_ShadingAspect>());
    replaceAspects(aReplaceMap);
  }
}

//=================================================================================================

void AIS_Shape::setTransparency(const occ::handle<Prs3d_Drawer>& theDrawer,
                                const double                     theValue) const
{
  theDrawer->SetupOwnShadingAspect();
  // override transparency
  theDrawer->ShadingAspect()->SetTransparency(theValue, myCurrentFacingModel);
}

//=================================================================================================

void AIS_Shape::SetTransparency(const double theValue)
{
  const bool toRecompute = !myDrawer->HasOwnShadingAspect();
  setTransparency(myDrawer, theValue);
  myDrawer->SetTransparency((float)theValue);

  if (!toRecompute || !myDrawer->HasLink())
  {
    SynchronizeAspects();
  }
  else
  {
    replaceWithNewOwnAspects();
  }
}

//=================================================================================================

void AIS_Shape::UnsetTransparency()
{
  myDrawer->SetTransparency(0.0f);
  if (!myDrawer->HasOwnShadingAspect())
  {
    return;
  }
  else if (HasColor() || HasMaterial() || myDrawer->ShadingAspect()->Aspect()->ToMapTexture())
  {
    myDrawer->ShadingAspect()->SetTransparency(0.0, myCurrentFacingModel);
    SynchronizeAspects();
  }
  else
  {
    NCollection_DataMap<occ::handle<Graphic3d_Aspects>, occ::handle<Graphic3d_Aspects>> aReplaceMap;
    replaceAspectWithDef(aReplaceMap, ShadingAspect);
    myDrawer->SetShadingAspect(occ::handle<Prs3d_ShadingAspect>());
    replaceAspects(aReplaceMap);
  }
}

//=================================================================================================

const Bnd_Box& AIS_Shape::BoundingBox()
{
  if (myshape.ShapeType() == TopAbs_COMPOUND && myshape.NbChildren() == 0)
  {
    // empty Shape  -> empty Assembly.
    myBB.SetVoid();
    return myBB;
  }

  if (myCompBB)
  {
    // Clear the bounding box to re-compute it.
    myBB.SetVoid();
    BRepBndLib::Add(myshape, myBB, false);
    myCompBB = false;
  }
  return myBB;
}

//*****
//***** Reset
//=======================================================================
// function : SetOwnDeviationCoefficient
// purpose  : resets myhasOwnDeviationCoefficient to false and
//           returns true if it change
//=======================================================================

bool AIS_Shape::SetOwnDeviationCoefficient()
{
  bool itSet = myDrawer->HasOwnDeviationCoefficient();
  if (itSet)
    myDrawer->SetDeviationCoefficient();
  return itSet;
}

//=======================================================================
// function : SetOwnDeviationAngle
// purpose  : resets myhasOwnDeviationAngle to false and
//           returns true if it change
//=======================================================================

bool AIS_Shape::SetOwnDeviationAngle()
{
  bool itSet = myDrawer->HasOwnDeviationAngle();
  if (itSet)
    myDrawer->SetDeviationAngle();
  return itSet;
}

//=================================================================================================

void AIS_Shape::SetOwnDeviationCoefficient(const double aCoefficient)
{
  myDrawer->SetDeviationCoefficient(aCoefficient);
  SetToUpdate();
}

//=================================================================================================

void AIS_Shape::SetOwnDeviationAngle(const double theAngle)
{
  myDrawer->SetDeviationAngle(theAngle);
  SetToUpdate(AIS_WireFrame);
}

//=================================================================================================

void AIS_Shape::SetAngleAndDeviation(const double anAngle)
{
  double OutAngl, OutDefl;
  HLRBRep::PolyHLRAngleAndDeflection(anAngle, OutAngl, OutDefl);
  SetOwnDeviationAngle(anAngle);
  SetOwnDeviationCoefficient(OutDefl);
  myInitAng = anAngle;
  SetToUpdate();
}

//=================================================================================================

double AIS_Shape::UserAngle() const
{
  return myInitAng == 0. ? GetContext()->DeviationAngle() : myInitAng;
}

//=================================================================================================

bool AIS_Shape::OwnDeviationCoefficient(double& aCoefficient, double& aPreviousCoefficient) const
{
  aCoefficient         = myDrawer->DeviationCoefficient();
  aPreviousCoefficient = myDrawer->PreviousDeviationCoefficient();
  return myDrawer->HasOwnDeviationCoefficient();
}

//=================================================================================================

bool AIS_Shape::OwnDeviationAngle(double& anAngle, double& aPreviousAngle) const
{
  anAngle        = myDrawer->DeviationAngle();
  aPreviousAngle = myDrawer->PreviousDeviationAngle();
  return myDrawer->HasOwnDeviationAngle();
}

//=================================================================================================

void AIS_Shape::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, AIS_InteractiveObject)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myshape)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBB)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myInitAng)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCompBB)
}
