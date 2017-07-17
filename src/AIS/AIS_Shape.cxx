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
#include <Aspect_TypeOfLine.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <Geom_Transformation.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_SequenceOfGroup.hxx>
#include <Graphic3d_Structure.hxx>
#include <HLRBRep.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <StdPrs_BndBox.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <StdPrs_HLRShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdSelect.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <StdSelect_DisplayMode.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Shape,AIS_InteractiveObject)

static Standard_Boolean IsInList(const TColStd_ListOfInteger& LL, const Standard_Integer aMode)
{
  TColStd_ListIteratorOfListOfInteger It(LL);
  for(;It.More();It.Next()){
    if(It.Value()==aMode) 
      return Standard_True;}
  return Standard_False;
}

//==================================================
// Function: AIS_Shape
// Purpose :
//==================================================
AIS_Shape::AIS_Shape(const TopoDS_Shape& theShape)
: AIS_InteractiveObject (PrsMgr_TOP_ProjectorDependant),
  myshape (theShape),
  myUVOrigin(0.0, 0.0),
  myUVRepeat(1.0, 1.0),
  myUVScale (1.0, 1.0),
  myInitAng (0.0),
  myCompBB (Standard_True)
{
  //
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
AIS_KindOfInteractive AIS_Shape::Type() const 
{return AIS_KOI_Shape;}


//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================
Standard_Integer AIS_Shape::Signature() const 
{return 0;}

//=======================================================================
//function : AcceptShapeDecomposition
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Shape::AcceptShapeDecomposition() const 
{return Standard_True;}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_Shape::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                        const Handle(Prs3d_Presentation)& aPrs,
                        const Standard_Integer theMode)
{  
  if(myshape.IsNull()) return;

  // wire,edge,vertex -> pas de HLR + priorite display superieure
  Standard_Integer TheType = (Standard_Integer) myshape.ShapeType();
  if(TheType>4 && TheType<8) {
    aPrs->SetVisual(Graphic3d_TOS_ALL);
    aPrs->SetDisplayPriority(TheType+2);
  }
  // Shape vide -> Assemblage vide.
  if (myshape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator anExplor (myshape);

    if (!anExplor.More()) {
      return;
    }
  }

  if (IsInfinite())
  {
    aPrs->SetInfiniteState (Standard_True); //not taken in account during FITALL
  }

  switch (theMode)
  {
    case AIS_WireFrame:
    {
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange (myshape, myDrawer, Standard_True);
      try
      {
        OCC_CATCH_SIGNALS
        StdPrs_WFShape::Add (aPrs, myshape, myDrawer);
      }
      catch (Standard_Failure)
      {
      #ifdef OCCT_DEBUG
        cout << "AIS_Shape::Compute() failed" << endl;
        cout << "a Shape should be incorrect : No Compute can be maked on it " << endl;
      #endif
        // presentation of the bounding box is calculated
        //      Compute(aPresentationManager,aPrs,2);
      }
      break;
    }
    case AIS_Shaded:
    {
      StdPrs_ToolTriangulatedShape::ClearOnOwnDeflectionChange (myshape, myDrawer, Standard_True);
      if ((Standard_Integer) myshape.ShapeType() > 4)
      {
        StdPrs_WFShape::Add (aPrs, myshape, myDrawer);
      }
      else
      {
        if (IsInfinite())
        {
          StdPrs_WFShape::Add (aPrs, myshape, myDrawer);
        }
        else
        {
          try
          {
            OCC_CATCH_SIGNALS
            StdPrs_ShadedShape::Add (aPrs, myshape, myDrawer,
                                     myDrawer->ShadingAspect()->Aspect()->ToMapTexture()
                                 && !myDrawer->ShadingAspect()->Aspect()->TextureMap().IsNull(),
                                     myUVOrigin, myUVRepeat, myUVScale);
          }
          catch (Standard_Failure)
          {
          #ifdef OCCT_DEBUG
            cout << "AIS_Shape::Compute() in ShadingMode failed" << endl;
          #endif
            StdPrs_WFShape::Add (aPrs, myshape, myDrawer);
          }
        }
      }
      Standard_Real aTransparency = Transparency() ;
      if (aTransparency > 0.0)
      {
        SetTransparency (aTransparency);
      }
      break;
    }

    // Bounding box.
    case 2:
    {
      if (IsInfinite())
      {
        StdPrs_WFShape::Add (aPrs, myshape, myDrawer);
      }
      else
      {
        StdPrs_BndBox::Add (aPrs, BoundingBox(), myDrawer);
      }
    }
  }

  // Recompute hidden line presentation (if necessary).
  aPrs->ReCompute();
}

//=======================================================================
//function : Compute
//purpose  : Hidden Line Removal
//=======================================================================
void AIS_Shape::Compute(const Handle(Prs3d_Projector)& aProjector,
                        const Handle(Prs3d_Presentation)& aPresentation)
{
  Compute(aProjector,aPresentation,myshape);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_Shape::Compute(const Handle(Prs3d_Projector)&     aProjector,
                        const Handle(Geom_Transformation)& TheTrsf,
                        const Handle(Prs3d_Presentation)&  aPresentation)
{
  const TopLoc_Location& loc = myshape.Location();
  TopoDS_Shape shbis = myshape.Located(TopLoc_Location(TheTrsf->Trsf())*loc);
  Compute(aProjector,aPresentation,shbis);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_Shape::Compute(const Handle(Prs3d_Projector)& aProjector,
                        const Handle(Prs3d_Presentation)& aPresentation,
                        const TopoDS_Shape& SH)
{
  if (SH.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator anExplor (SH);

    if (!anExplor.More()) // Shape vide -> Assemblage vide.
      return;
  }

  Handle (Prs3d_Drawer) defdrawer = GetContext()->DefaultDrawer();
  if (defdrawer->DrawHiddenLine())
    {myDrawer->EnableDrawHiddenLine();}
  else {myDrawer->DisableDrawHiddenLine();}

  Aspect_TypeOfDeflection prevdef = defdrawer->TypeOfDeflection();
  defdrawer->SetTypeOfDeflection(Aspect_TOD_RELATIVE);

  if (myDrawer->IsAutoTriangulation())
  {
    // coefficients for calculation
    Standard_Real aPrevAngle, aNewAngle, aPrevCoeff, aNewCoeff;
    Standard_Boolean isOwnHLRDeviationAngle = OwnHLRDeviationAngle (aNewAngle, aPrevAngle);
    Standard_Boolean isOwnHLRDeviationCoefficient = OwnHLRDeviationCoefficient (aNewCoeff, aPrevCoeff);
    if (((Abs (aNewAngle - aPrevAngle) > Precision::Angular()) && isOwnHLRDeviationAngle) ||
        ((Abs (aNewCoeff - aPrevCoeff) > Precision::Confusion()) && isOwnHLRDeviationCoefficient))
    {
      BRepTools::Clean(SH);
    }
  }
  
  {
    try {
      OCC_CATCH_SIGNALS
      switch (TypeOfHLR()) {
        case Prs3d_TOH_Algo:
          StdPrs_HLRShape::Add (aPresentation, SH, myDrawer, aProjector);
          break;
        case Prs3d_TOH_PolyAlgo:
        default:
          StdPrs_HLRPolyShape::Add (aPresentation, SH, myDrawer, aProjector);
          break;
      }
    }
    catch (Standard_Failure) {
#ifdef OCCT_DEBUG
      cout <<"AIS_Shape::Compute(Proj) HLR Algorithm failed" << endl;
#endif
      StdPrs_WFShape::Add(aPresentation,SH,myDrawer);
    }
  }

  defdrawer->SetTypeOfDeflection (prevdef);
}

//=======================================================================
//function : SelectionType
//purpose  : gives the type according to the Index of Selection Mode
//=======================================================================

TopAbs_ShapeEnum AIS_Shape::SelectionType(const Standard_Integer aMode)
{
  switch(aMode){
  case 1:
    return TopAbs_VERTEX;
  case 2:
    return TopAbs_EDGE;
  case 3:
    return TopAbs_WIRE;
  case 4:
    return TopAbs_FACE;
  case 5:
    return TopAbs_SHELL;
  case 6:
    return TopAbs_SOLID;
  case 7:
    return TopAbs_COMPSOLID;
  case 8:
    return TopAbs_COMPOUND;
  case 0:
  default:
    return TopAbs_SHAPE;
  }
  
}
//=======================================================================
//function : SelectionType
//purpose  : gives the SelectionMode according to the Type od Decomposition...
//=======================================================================
Standard_Integer AIS_Shape::SelectionMode(const TopAbs_ShapeEnum aType)
{
  switch(aType){
  case TopAbs_VERTEX:
    return 1;
  case TopAbs_EDGE:
    return 2;
  case TopAbs_WIRE:
    return 3;
  case  TopAbs_FACE:
    return 4;
  case TopAbs_SHELL:
    return 5;
  case TopAbs_SOLID:
    return 6;
  case TopAbs_COMPSOLID:
    return 7;
  case TopAbs_COMPOUND:
    return 8;
  case TopAbs_SHAPE:
  default:
    return 0;
  }
}


//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================

void AIS_Shape::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                              const Standard_Integer aMode)
{
  if(myshape.IsNull()) return;
  if (myshape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator anExplor (myshape);

    if (!anExplor.More()) // empty Shape -> empty Assembly.
      return;
  }

  TopAbs_ShapeEnum TypOfSel = AIS_Shape::SelectionType(aMode);
  TopoDS_Shape shape = myshape;

// POP protection against crash in low layers

  Standard_Real aDeflection = Prs3d::GetDeflection(shape, myDrawer);
  try {  
    OCC_CATCH_SIGNALS
    StdSelect_BRepSelectionTool::Load(aSelection,
                                      this,
                                      shape,
                                      TypOfSel,
                                      aDeflection,
                                      myDrawer->HLRAngle(),
                                      myDrawer->IsAutoTriangulation());
  } catch ( Standard_Failure ) {
//    cout << "a Shape should be incorrect : A Selection on the Bnd  is activated   "<<endl;
    if ( aMode == 0 ) {
      aSelection->Clear();
      Bnd_Box B = BoundingBox();
      Handle(StdSelect_BRepOwner) aOwner = new StdSelect_BRepOwner(shape,this);
      Handle(Select3D_SensitiveBox) aSensitiveBox = new Select3D_SensitiveBox(aOwner,B);
      aSelection->Add(aSensitiveBox);
    }
  }

  // insert the drawer in the BrepOwners for hilight...
  StdSelect::SetDrawerForBRepOwner(aSelection,myDrawer);
}

void AIS_Shape::Color( Quantity_Color& aColor ) const {
  aColor = myDrawer->ShadingAspect()->Color(myCurrentFacingModel);
}

Graphic3d_NameOfMaterial AIS_Shape::Material() const {
  return (myDrawer->ShadingAspect()->Material(myCurrentFacingModel)).Name();
}

Standard_Real AIS_Shape::Transparency() const {
  return myDrawer->ShadingAspect()->Transparency(myCurrentFacingModel);
}

//=======================================================================
//function : setColor
//purpose  :
//=======================================================================

void AIS_Shape::setColor (const Handle(Prs3d_Drawer)& theDrawer,
                          const Quantity_Color&       theColor) const
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
  if (!theDrawer->HasOwnPointAspect())
  {
    theDrawer->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_BLACK, 1.0));
    if (theDrawer->HasLink())
    {
      *theDrawer->PointAspect()->Aspect() = *theDrawer->Link()->PointAspect()->Aspect();
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
  if (!theDrawer->HasOwnSeenLineAspect())
  {
    theDrawer->SetSeenLineAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    if (theDrawer->HasLink())
    {
      *theDrawer->SeenLineAspect()->Aspect() = *theDrawer->Link()->SeenLineAspect()->Aspect();
    }
  }

  // override color
  theDrawer->ShadingAspect()->SetColor (theColor, myCurrentFacingModel);
  theDrawer->LineAspect()->SetColor (theColor);
  theDrawer->WireAspect()->SetColor (theColor);
  theDrawer->PointAspect()->SetColor (theColor);
  theDrawer->FreeBoundaryAspect()->SetColor (theColor);
  theDrawer->UnFreeBoundaryAspect()->SetColor (theColor);
  theDrawer->SeenLineAspect()->SetColor (theColor);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================

void AIS_Shape::SetColor (const Quantity_Color& theColor)
{
  setColor (myDrawer, theColor);
  myDrawer->SetColor (theColor);
  hasOwnColor = Standard_True;

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList     = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAspect = myDrawer->ShadingAspect()->Aspect();
  Handle(Graphic3d_AspectLine3d)     aLineAspect  = myDrawer->LineAspect()->Aspect();
  Handle(Graphic3d_AspectMarker3d)   aPointAspect = myDrawer->PointAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_Shaded)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();

      // Check if aspect of given type is set for the group, 
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAspect);
      }
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_LINE))
      {
        aGroup->SetGroupPrimitivesAspect (aLineAspect);
      }
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_MARKER))
      {
        aGroup->SetGroupPrimitivesAspect (aPointAspect);
      }
    }
  }

  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (2);
}

//=======================================================================
//function : UnsetColor
//purpose  :
//=======================================================================

void AIS_Shape::UnsetColor()
{
  if (!HasColor())
  {
    myToRecomputeModes.Clear();
    return;
  }
  hasOwnColor = Standard_False;
  myDrawer->SetColor (myDrawer->HasLink() ? myDrawer->Link()->Color() : Quantity_Color (Quantity_NOC_WHITE));

  if (!HasWidth())
  {
    Handle(Prs3d_LineAspect) anEmptyAsp;
    myDrawer->SetLineAspect          (anEmptyAsp);
    myDrawer->SetWireAspect          (anEmptyAsp);
    myDrawer->SetFreeBoundaryAspect  (anEmptyAsp);
    myDrawer->SetUnFreeBoundaryAspect(anEmptyAsp);
    myDrawer->SetSeenLineAspect      (anEmptyAsp);
  }
  else
  {
    Quantity_Color aColor = Quantity_NOC_YELLOW;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor (myDrawer->Link(), AIS_TOA_Line,   aColor);
    }
    myDrawer->LineAspect()->SetColor (aColor);
    aColor = Quantity_NOC_RED;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor (myDrawer->Link(), AIS_TOA_Wire,   aColor);
    }
    myDrawer->WireAspect()->SetColor (aColor);
    aColor = Quantity_NOC_GREEN;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor (myDrawer->Link(), AIS_TOA_Free,   aColor);
    }
    myDrawer->FreeBoundaryAspect()->SetColor (aColor);
    aColor = Quantity_NOC_YELLOW;
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor (myDrawer->Link(), AIS_TOA_UnFree, aColor);
    }
    myDrawer->UnFreeBoundaryAspect()->SetColor (aColor);
    if (myDrawer->HasLink())
    {
      AIS_GraphicTool::GetLineColor (myDrawer->Link(), AIS_TOA_Seen,   aColor);
    }
    myDrawer->SeenLineAspect()->SetColor (aColor);
  }

  if (!myDrawer->HasOwnShadingAspect())
  {
    //
  }
  else if (HasMaterial()
        || IsTransparent()
        || myDrawer->ShadingAspect()->Aspect()->ToMapTexture())
  {
    const Graphic3d_MaterialAspect aDefaultMat (Graphic3d_NOM_BRASS);
    Graphic3d_MaterialAspect mat = aDefaultMat;
    Quantity_Color anInteriorColors[2] = {Quantity_NOC_CYAN1, Quantity_NOC_CYAN1};
    if (myDrawer->HasLink())
    {
      anInteriorColors[0] = myDrawer->Link()->ShadingAspect()->Aspect()->InteriorColor();
      anInteriorColors[1] = myDrawer->Link()->ShadingAspect()->Aspect()->BackInteriorColor();
    }
    if (HasMaterial() || myDrawer->HasLink())
    {
      const Handle(Graphic3d_AspectFillArea3d)& aSrcAspect = (HasMaterial() ? myDrawer : myDrawer->Link())->ShadingAspect()->Aspect();
      mat = myCurrentFacingModel != Aspect_TOFM_BACK_SIDE
          ? aSrcAspect->FrontMaterial()
          : aSrcAspect->BackMaterial();
    }
    if (HasMaterial())
    {
      const Quantity_Color aColor = myDrawer->HasLink()
                                  ? myDrawer->Link()->ShadingAspect()->Color (myCurrentFacingModel)
                                  : aDefaultMat.AmbientColor();
      mat.SetColor (aColor);
    }
    if (IsTransparent())
    {
      Standard_Real aTransp = myDrawer->ShadingAspect()->Transparency (myCurrentFacingModel);
      mat.SetTransparency (Standard_ShortReal(aTransp));
    }
    myDrawer->ShadingAspect()->SetMaterial (mat, myCurrentFacingModel);
    myDrawer->ShadingAspect()->Aspect()->SetInteriorColor    (anInteriorColors[0]);
    myDrawer->ShadingAspect()->Aspect()->SetBackInteriorColor(anInteriorColors[1]);
  }
  else
  {
    myDrawer->SetShadingAspect (Handle(Prs3d_ShadingAspect)());
  }
  myDrawer->SetPointAspect (Handle(Prs3d_PointAspect)());

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList  = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
  Handle(Graphic3d_AspectLine3d)     aLineAsp  = myDrawer->LineAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_Shaded)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();

      // Check if aspect of given type is set for the group,
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAsp);
      }
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_LINE))
      {
        aGroup->SetGroupPrimitivesAspect (aLineAsp);
      }
    }
  }

  LoadRecomputable (AIS_WireFrame);
  LoadRecomputable (2);
}

//=======================================================================
//function : setWidth
//purpose  :
//=======================================================================

void AIS_Shape::setWidth (const Handle(Prs3d_Drawer)& theDrawer,
                          const Standard_Real         theLineWidth) const
{
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
  if (!theDrawer->HasOwnSeenLineAspect())
  {
    theDrawer->SetSeenLineAspect (new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
    if (theDrawer->HasLink())
    {
      *theDrawer->SeenLineAspect()->Aspect() = *theDrawer->Link()->SeenLineAspect()->Aspect();
    }
  }

  // override width
  theDrawer->LineAspect()->SetWidth (theLineWidth);
  theDrawer->WireAspect()->SetWidth (theLineWidth);
  theDrawer->FreeBoundaryAspect()->SetWidth (theLineWidth);
  theDrawer->UnFreeBoundaryAspect()->SetWidth (theLineWidth);
  theDrawer->SeenLineAspect()->SetWidth (theLineWidth);
}

//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================

void AIS_Shape::SetWidth (const Standard_Real theLineWidth)
{
  setWidth (myDrawer, theLineWidth);
  myOwnWidth = theLineWidth;
  LoadRecomputable (AIS_WireFrame); // means that it is necessary to recompute only the wireframe....
  LoadRecomputable (2);             // and the bounding box...
}

//=======================================================================
//function : UnsetWidth
//purpose  :
//=======================================================================

void AIS_Shape::UnsetWidth()
{
  if (myOwnWidth == 0.0)
  {
    myToRecomputeModes.Clear();
    return;
  }

  myOwnWidth = 0.0;

  Handle(Prs3d_LineAspect) anEmptyAsp;

  if (!HasColor())
  {
    myDrawer->SetLineAspect          (anEmptyAsp);
    myDrawer->SetWireAspect          (anEmptyAsp);
    myDrawer->SetFreeBoundaryAspect  (anEmptyAsp);
    myDrawer->SetUnFreeBoundaryAspect(anEmptyAsp);
    myDrawer->SetSeenLineAspect      (anEmptyAsp);
  }
  else
  {
    myDrawer->LineAspect()          ->SetWidth (myDrawer->HasLink() ?
      AIS_GraphicTool::GetLineWidth (myDrawer->Link(), AIS_TOA_Line) : 1.);
    myDrawer->WireAspect()          ->SetWidth (myDrawer->HasLink() ?
      AIS_GraphicTool::GetLineWidth (myDrawer->Link(), AIS_TOA_Wire) : 1.);
    myDrawer->FreeBoundaryAspect()  ->SetWidth (myDrawer->HasLink() ?
      AIS_GraphicTool::GetLineWidth (myDrawer->Link(), AIS_TOA_Free) : 1.);
    myDrawer->UnFreeBoundaryAspect()->SetWidth (myDrawer->HasLink() ?
      AIS_GraphicTool::GetLineWidth (myDrawer->Link(), AIS_TOA_UnFree) : 1.);
    myDrawer->SeenLineAspect()      ->SetWidth (myDrawer->HasLink() ?
      AIS_GraphicTool::GetLineWidth (myDrawer->Link(), AIS_TOA_Seen) : 1.);
  }
  LoadRecomputable (AIS_WireFrame);
}

//=======================================================================
//function : setMaterial
//purpose  :
//=======================================================================

void AIS_Shape::setMaterial (const Handle(Prs3d_Drawer)&     theDrawer,
                             const Graphic3d_MaterialAspect& theMaterial,
                             const Standard_Boolean          theToKeepColor,
                             const Standard_Boolean          theToKeepTransp) const
{
  const Quantity_Color aColor  = theDrawer->ShadingAspect()->Material     (myCurrentFacingModel).Color();
  const Standard_Real  aTransp = theDrawer->ShadingAspect()->Transparency (myCurrentFacingModel);
  if (!theDrawer->HasOwnShadingAspect())
  {
    theDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
    if (theDrawer->HasLink())
    {
      *theDrawer->ShadingAspect()->Aspect() = *theDrawer->Link()->ShadingAspect()->Aspect();
    }
  }
  theDrawer->ShadingAspect()->SetMaterial (theMaterial, myCurrentFacingModel);

  if (theToKeepColor)
  {
    theDrawer->ShadingAspect()->SetColor (aColor, myCurrentFacingModel);
  }
  if (theToKeepTransp)
  {
    theDrawer->ShadingAspect()->SetTransparency (aTransp, myCurrentFacingModel);
  }
}

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================

void AIS_Shape::SetMaterial (const Graphic3d_MaterialAspect& theMat)
{
  setMaterial (myDrawer, theMat, HasColor(), IsTransparent());
  hasOwnMaterial = Standard_True;

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList  = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_Shaded)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();

      // Check if aspect of given type is set for the group, 
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAsp);
      }
    }
  }

  myRecomputeEveryPrs = Standard_False; // no mode to recalculate  :only viewer update
  myToRecomputeModes.Clear();
}

//=======================================================================
//function : UnsetMaterial
//purpose  :
//=======================================================================

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
  else if (HasColor()
        || IsTransparent()
        || myDrawer->ShadingAspect()->Aspect()->ToMapTexture())
  {
    if(myDrawer->HasLink())
    {
      myDrawer->ShadingAspect()->SetMaterial (myDrawer->Link()->ShadingAspect()->Material (myCurrentFacingModel),
                                              myCurrentFacingModel);
    }
    if (HasColor())
    {
      myDrawer->ShadingAspect()->SetColor        (myDrawer->Color(),        myCurrentFacingModel);
      myDrawer->ShadingAspect()->SetTransparency (myDrawer->Transparency(), myCurrentFacingModel);
    }
  }
  else
  {
    myDrawer->SetShadingAspect (Handle(Prs3d_ShadingAspect)());
  }
  hasOwnMaterial = Standard_False;

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList  = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_Shaded)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAsp);
      }
    }
  }

  myRecomputeEveryPrs = Standard_False; // no mode to recalculate :only viewer update
  myToRecomputeModes.Clear();  
}

//=======================================================================
//function : setTransparency
//purpose  :
//=======================================================================

void AIS_Shape::setTransparency (const Handle(Prs3d_Drawer)& theDrawer,
                                 const Standard_Real         theValue) const
{
  if (!theDrawer->HasOwnShadingAspect())
  {
    theDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
    if (theDrawer->HasLink())
    {
      *theDrawer->ShadingAspect()->Aspect() = *theDrawer->Link()->ShadingAspect()->Aspect();
    }
  }

  // override transparency
  theDrawer->ShadingAspect()->SetTransparency (theValue, myCurrentFacingModel);
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================

void AIS_Shape::SetTransparency (const Standard_Real theValue)
{
  setTransparency (myDrawer, theValue);
  myDrawer->SetTransparency ((Standard_ShortReal )theValue);

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList  = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_Shaded)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAsp);
      }
    }
  }

  myRecomputeEveryPrs = Standard_False; // no mode to recalculate - only viewer update
  myToRecomputeModes.Clear();
}

//=======================================================================
//function : UnsetTransparency
//purpose  :
//=======================================================================

void AIS_Shape::UnsetTransparency()
{
  myDrawer->SetTransparency (0.0f);
  if (!myDrawer->HasOwnShadingAspect())
  {
    return;
  }
  else if (HasColor()
        || HasMaterial()
        || myDrawer->ShadingAspect()->Aspect()->ToMapTexture())
  {
    myDrawer->ShadingAspect()->SetTransparency (0.0, myCurrentFacingModel);
  }
  else
  {
    myDrawer->SetShadingAspect (Handle(Prs3d_ShadingAspect)());
  }

  // modify shading presentation without re-computation
  const PrsMgr_Presentations&        aPrsList  = Presentations();
  Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
  for (Standard_Integer aPrsIt = 1; aPrsIt <= aPrsList.Length(); ++aPrsIt)
  {
    const PrsMgr_ModedPresentation& aPrsModed = aPrsList.Value (aPrsIt);
    if (aPrsModed.Mode() != AIS_Shaded)
    {
      continue;
    }

    const Handle(Prs3d_Presentation)& aPrs = aPrsModed.Presentation()->Presentation();
    for (Graphic3d_SequenceOfGroup::Iterator aGroupIt (aPrs->Groups()); aGroupIt.More(); aGroupIt.Next())
    {
      const Handle(Graphic3d_Group)& aGroup = aGroupIt.Value();
      if (aGroup->IsGroupPrimitivesAspectSet (Graphic3d_ASPECT_FILL_AREA))
      {
        aGroup->SetGroupPrimitivesAspect (anAreaAsp);
      }
    }
  }

  myRecomputeEveryPrs = Standard_False; // no mode to recalculate :only viewer update
  myToRecomputeModes.Clear();
}

//=======================================================================
//function : LoadRecomputable
//purpose  : 
//=======================================================================

void AIS_Shape::LoadRecomputable(const Standard_Integer TheMode)
{
  myRecomputeEveryPrs = Standard_False;
  if(!IsInList(myToRecomputeModes,TheMode))
    myToRecomputeModes.Append(TheMode);
}

//=======================================================================
//function : BoundingBox
//purpose  : 
//=======================================================================

const Bnd_Box& AIS_Shape::BoundingBox()  
{
  if (myshape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator anExplor (myshape);

    if (!anExplor.More()) { // empty Shape  -> empty Assembly.
      myBB.SetVoid();
      return myBB;
    }
  }

  if(myCompBB) {
    BRepBndLib::AddClose(myshape, myBB);
    myCompBB = Standard_False;
  }
  return myBB;
}

//*****
//***** Reset
//=======================================================================
//function : SetOwnDeviationCoefficient
//purpose  : resets myhasOwnDeviationCoefficient to Standard_False and
//           returns Standard_True if it change
//=======================================================================

Standard_Boolean AIS_Shape::SetOwnDeviationCoefficient ()
{
  Standard_Boolean itSet = myDrawer->HasOwnDeviationCoefficient();
  if(itSet)  myDrawer->SetDeviationCoefficient();
  return itSet;
}

//=======================================================================
//function : SetHLROwnDeviationCoefficient
//purpose  : resets myhasOwnHLRDeviationCoefficient to Standard_False and
//           returns Standard_True if it change
//=======================================================================

Standard_Boolean AIS_Shape::SetOwnHLRDeviationCoefficient ()
{
  Standard_Boolean itSet = myDrawer->HasOwnHLRDeviationCoefficient();
  if(itSet)  myDrawer->SetHLRDeviationCoefficient();
  return itSet;

}

//=======================================================================
//function : SetOwnDeviationAngle
//purpose  : resets myhasOwnDeviationAngle to Standard_False and
//           returns Standard_True if it change
//=======================================================================

Standard_Boolean AIS_Shape::SetOwnDeviationAngle ()
{
  Standard_Boolean itSet = myDrawer->HasOwnDeviationAngle();
  if(itSet)  myDrawer->SetDeviationAngle();
  return itSet;

}

//=======================================================================
//function : SetOwnHLRDeviationAngle
//purpose  : resets myhasOwnHLRDeviationAngle to Standard_False and
//           returns Standard_True if it change
//=======================================================================

Standard_Boolean AIS_Shape::SetOwnHLRDeviationAngle ()
{
  Standard_Boolean itSet = myDrawer->HasOwnHLRDeviationAngle();
  if(itSet)  myDrawer->SetHLRAngle();
  return itSet;

}
//***** SetOwn
//=======================================================================
//function : SetOwnDeviationCoefficient
//purpose  : 
//=======================================================================

void AIS_Shape::SetOwnDeviationCoefficient ( const Standard_Real  aCoefficient )
{
  myDrawer->SetDeviationCoefficient( aCoefficient );
  SetToUpdate(0) ; // WireFrame
  SetToUpdate(1) ; // Shadding
}

//=======================================================================
//function : SetOwnHLRDeviationCoefficient
//purpose  : 
//=======================================================================

void AIS_Shape::SetOwnHLRDeviationCoefficient ( const Standard_Real  aCoefficient )
{
  myDrawer->SetHLRDeviationCoefficient( aCoefficient );
  
}

//=======================================================================
//function : SetOwnDeviationAngle
//purpose  : 
//=======================================================================

void AIS_Shape::SetOwnDeviationAngle ( const Standard_Real  anAngle )
{

  myDrawer->SetDeviationAngle(anAngle );
  SetToUpdate(0) ;   // WireFrame
}
//=======================================================================
//function : SetOwnDeviationAngle
//purpose  : 
//=======================================================================

void AIS_Shape::SetAngleAndDeviation ( const Standard_Real  anAngle )
{
  Standard_Real OutAngl,OutDefl;
  HLRBRep::PolyHLRAngleAndDeflection(anAngle,OutAngl,OutDefl);
  SetOwnDeviationAngle(anAngle) ;
  SetOwnDeviationCoefficient(OutDefl) ;
  myInitAng = anAngle;
  SetToUpdate(0);
  SetToUpdate(1);
}

//=======================================================================
//function : UserAngle
//purpose  : 
//=======================================================================

Standard_Real AIS_Shape::UserAngle() const
{
  return myInitAng ==0. ? GetContext()->DeviationAngle(): myInitAng;
}


//=======================================================================
//function : SetHLRAngleAndDeviation
//purpose  : 
//=======================================================================

void AIS_Shape::SetHLRAngleAndDeviation ( const Standard_Real  anAngle )
{
  Standard_Real OutAngl,OutDefl;
  HLRBRep::PolyHLRAngleAndDeflection(anAngle,OutAngl,OutDefl);
  SetOwnHLRDeviationAngle( OutAngl );
  SetOwnHLRDeviationCoefficient(OutDefl);

}
//=======================================================================
//function : SetOwnHLRDeviationAngle
//purpose  : 
//=======================================================================

void AIS_Shape::SetOwnHLRDeviationAngle ( const Standard_Real  anAngle )
{
  myDrawer->SetHLRAngle( anAngle );
}

//***** GetOwn
//=======================================================================
//function : OwnDeviationCoefficient
//purpose  : 
//=======================================================================

Standard_Boolean AIS_Shape::OwnDeviationCoefficient ( Standard_Real &  aCoefficient,
                                                      Standard_Real & aPreviousCoefficient ) const
{
  aCoefficient = myDrawer->DeviationCoefficient();
  aPreviousCoefficient = myDrawer->PreviousDeviationCoefficient ();
  return myDrawer->HasOwnDeviationCoefficient() ;
}

//=======================================================================
//function : OwnHLRDeviationCoefficient
//purpose  : 
//=======================================================================

Standard_Boolean AIS_Shape::OwnHLRDeviationCoefficient ( Standard_Real & aCoefficient,
                                                         Standard_Real & aPreviousCoefficient ) const
{
  aCoefficient = myDrawer->HLRDeviationCoefficient();
  aPreviousCoefficient = myDrawer->PreviousHLRDeviationCoefficient ();
  return myDrawer->HasOwnHLRDeviationCoefficient();

}

//=======================================================================
//function : OwnDeviationAngle
//purpose  : 
//=======================================================================

Standard_Boolean AIS_Shape::OwnDeviationAngle ( Standard_Real &  anAngle,
                                                Standard_Real & aPreviousAngle ) const
{
  anAngle = myDrawer->DeviationAngle();
  aPreviousAngle = myDrawer->PreviousDeviationAngle (); 
  return myDrawer->HasOwnDeviationAngle();
}

//=======================================================================
//function : OwnHLRDeviationAngle
//purpose  : 
//=======================================================================

Standard_Boolean AIS_Shape::OwnHLRDeviationAngle ( Standard_Real &  anAngle,
                                                   Standard_Real & aPreviousAngle ) const
{
  anAngle = myDrawer->HLRAngle();
  aPreviousAngle = myDrawer->PreviousHLRDeviationAngle (); 
  return myDrawer->HasOwnHLRDeviationAngle();
}
