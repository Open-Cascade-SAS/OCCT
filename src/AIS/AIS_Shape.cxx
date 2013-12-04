// Created on: 1996-12-20
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <AIS_Shape.ixx>


#include <Standard_ErrorHandler.hxx>
#include <OSD_Timer.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <Quantity_Color.hxx>

#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_MaterialAspect.hxx>

#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>

#include <StdPrs_WFShape.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_HLRShape.hxx>
#include <StdPrs_HLRPolyShape.hxx>

#include <PrsMgr_ModedPresentation.hxx>

#include <Select3D_SensitiveEntity.hxx>
#include <StdSelect.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_DisplayMode.hxx>

#include <AIS_GraphicTool.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Drawer.hxx>
#include <HLRBRep.hxx>
#include <Precision.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <TopoDS_Iterator.hxx>

static Standard_Boolean myFirstCompute;

Standard_Real AIS_Shape::GetDeflection(const TopoDS_Shape& aShape,
                                       const Handle(Prs3d_Drawer)& aDrawer)
{
  // WARNING: this same piece of code appears several times in Prs3d classes
  Standard_Real aDeflection = aDrawer->MaximalChordialDeviation();
  if (aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE) {
    Bnd_Box B;
    BRepBndLib::Add(aShape, B, Standard_False);
    if ( ! B.IsVoid() )
    {
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      aDeflection = Max( aXmax-aXmin, Max(aYmax-aYmin, aZmax-aZmin)) *
                    aDrawer->DeviationCoefficient() * 4;
    }
  }
  return aDeflection;
}

void AIS_Shape::DisplayBox(const Handle(Prs3d_Presentation)& aPrs,
                           const Bnd_Box& B,
                           const Handle(Prs3d_Drawer)& aDrawer)
{
  static const Standard_Integer Indx[][3] =
  { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 },
    { 0, 1, 1 }, { 1, 1, 1 }, { 1, 1, 0 }, { 0, 1, 0 },
    { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 },
    { 0, 1, 1 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } };

  if ( B.IsVoid() )
    return; // nothing to show

  Standard_Real X[2],Y[2],Z[2];
  B.Get(X[0], Y[0], Z[0], X[1], Y[1], Z[1]);

  Handle(Graphic3d_Group) G = Prs3d_Root::CurrentGroup(aPrs);
  Quantity_Color Q;
  Aspect_TypeOfLine A;
  Standard_Real W;
  aDrawer->LineAspect()->Aspect()->Values(Q,A,W);

  G->SetGroupPrimitivesAspect(new Graphic3d_AspectLine3d(Q,Aspect_TOL_DOTDASH,W));

  Handle(Graphic3d_ArrayOfPolylines) aPolyline = new Graphic3d_ArrayOfPolylines(16);
  Standard_Integer i(0);
  for(;i<16;i++)
    aPolyline->AddVertex(X[Indx[i][0]],Y[Indx[i][1]],Z[Indx[i][2]]);
  G->AddPrimitiveArray(aPolyline);
}

static Standard_Boolean IsInList(const TColStd_ListOfInteger& LL, const Standard_Integer aMode)
{
  TColStd_ListIteratorOfListOfInteger It(LL);
  for(;It.More();It.Next()){
    if(It.Value()==aMode) 
      return Standard_True;}
  return Standard_False;
}

//==================================================
// Function: 
// Purpose :
//==================================================

AIS_Shape::
AIS_Shape(const TopoDS_Shape& shap):
AIS_InteractiveObject(PrsMgr_TOP_ProjectorDependant),
myshape(shap),
myCompBB(Standard_True),
myInitAng(0.)
{
  myFirstCompute = Standard_True;
  SetHilightMode(0);
  myDrawer->SetShadingAspectGlobal(Standard_False);
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
                        const Standard_Integer aMode)
{  
  aPrs->Clear();
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

  if (IsInfinite()) aPrs->SetInfiniteState(Standard_True); //not taken in account duting FITALL
  switch (aMode) {
  case 0:{
    try { OCC_CATCH_SIGNALS  StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer); }
    catch (Standard_Failure) { 
#ifdef DEB
      cout << "AIS_Shape::Compute()  failed"<< endl;
#endif
      cout << "a Shape should be incorrect : No Compute can be maked on it  "<< endl;     
// presentation of the bounding box is calculated
//      Compute(aPresentationManager,aPrs,2);
    }
    break;
  }
  case 1:
    {
      Standard_Real prevangle ;
      Standard_Real newangle  ; 
      Standard_Real prevcoeff ;
      Standard_Real newcoeff  ; 
      
      Standard_Boolean isOwnDeviationAngle = OwnDeviationAngle(newangle,prevangle);
      Standard_Boolean isOwnDeviationCoefficient = OwnDeviationCoefficient(newcoeff,prevcoeff);
      if (((Abs (newangle - prevangle) > Precision::Angular()) && isOwnDeviationAngle) ||
          ((Abs (newcoeff - prevcoeff) > Precision::Confusion()) && isOwnDeviationCoefficient)) { 
#ifdef DEB
        cout << "AIS_Shape : compute"<<endl;
        cout << "newangl   : " << newangle << " # de " << "prevangl  : " << prevangle << " OU "<<endl;
        cout << "newcoeff  : " << newcoeff << " # de " << "prevcoeff : " << prevcoeff << endl;
#endif
        BRepTools::Clean(myshape);
      }
      
      //shading only on face...
      if ((Standard_Integer) myshape.ShapeType()>4)
        StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);
      else {
        myDrawer->SetShadingAspectGlobal(Standard_False);
        if (IsInfinite()) StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);
        else {
          {
            try {
              OCC_CATCH_SIGNALS
              StdPrs_ShadedShape::Add(aPrs,myshape,myDrawer);
            }
            catch (Standard_Failure) {
#ifdef DEB
              cout << "AIS_Shape::Compute() in ShadingMode failed"<< endl;
#endif
              StdPrs_WFShape::Add(aPrs,myshape,myDrawer);
            }
          }
        }
      }
      Standard_Real value = Transparency() ;
      if( value > 0. ) {
        SetTransparency( value );
      }
      break;
    }
  case 2:
    {
      // bounding box
      if (IsInfinite()) StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);
      else DisplayBox(aPrs,BoundingBox(),myDrawer);
    }
  } // end switch
  aPrs->ReCompute(); // for hidden line recomputation if necessary...
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

// coefficients for calculation

  Standard_Real prevangle, newangle ,prevcoeff,newcoeff ; 
  Standard_Boolean isOwnHLRDeviationAngle = OwnHLRDeviationAngle(newangle,prevangle);
  Standard_Boolean isOwnHLRDeviationCoefficient = OwnHLRDeviationCoefficient(newcoeff,prevcoeff);
  if (((Abs (newangle - prevangle) > Precision::Angular()) && isOwnHLRDeviationAngle) ||
      ((Abs (newcoeff - prevcoeff) > Precision::Confusion()) && isOwnHLRDeviationCoefficient)) {
#ifdef DEB
      cout << "AIS_Shape : compute"<<endl;
      cout << "newangle  : " << newangle << " # de " << "prevangl  : " << prevangle << " OU "<<endl;
      cout << "newcoeff  : " << newcoeff << " # de " << "prevcoeff : " << prevcoeff << endl;
#endif
      BRepTools::Clean(SH);
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
#ifdef DEB
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

  static TopAbs_ShapeEnum TypOfSel;
  TypOfSel = AIS_Shape::SelectionType(aMode);
  TopoDS_Shape shape = myshape;
  if( HasTransformation() ) {
    Handle(Geom_Transformation) trsf = Transformation();
    shape = shape.Located(TopLoc_Location(trsf->Trsf())*shape.Location());
  }

// POP protection against crash in low layers

  Standard_Real aDeflection = GetDeflection(shape, myDrawer);
  Standard_Boolean autoTriangulation = Standard_True;
  try {  
    OCC_CATCH_SIGNALS
    StdSelect_BRepSelectionTool::Load(aSelection,
                                      this,
                                      shape,
                                      TypOfSel,
                                      aDeflection,
                                      myDrawer->HLRAngle(),
                                      autoTriangulation); 
  } catch ( Standard_Failure ) {
//    cout << "a Shape should be incorrect : A Selection on the Bnd  is activated   "<<endl;
    if ( aMode == 0 ) {
      Bnd_Box B = BoundingBox();
      Handle(StdSelect_BRepOwner) aOwner = new StdSelect_BRepOwner(shape,this);
      Handle(Select3D_SensitiveBox) aSensitiveBox = new Select3D_SensitiveBox(aOwner,B);
      aSelection->Add(aSensitiveBox);
    }
  }

  // insert the drawer in the BrepOwners for hilight...
  StdSelect::SetDrawerForBRepOwner(aSelection,myDrawer);
}

Quantity_NameOfColor AIS_Shape::Color() const {
Quantity_Color aColor;
  Color(aColor);
  return aColor.Name();
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
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_Shape::SetColor(const Quantity_NameOfColor aCol)
{
  SetColor(Quantity_Color(aCol));
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_Shape::SetColor(const Quantity_Color &aCol)
{
  if( !HasColor() && !IsTransparent() && !HasMaterial() ) {
    myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  }
  hasOwnColor = Standard_True;

  myDrawer->ShadingAspect()->SetColor(aCol,myCurrentFacingModel);
  myDrawer->ShadingAspect()->SetTransparency(myTransparency,myCurrentFacingModel);
  myDrawer->SetShadingAspectGlobal(Standard_False);


  const Standard_Real WW = HasWidth()? Width():AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Line);

  myDrawer->SetLineAspect(new Prs3d_LineAspect(aCol,Aspect_TOL_SOLID,WW));
  myDrawer->SetWireAspect(new Prs3d_LineAspect(aCol,Aspect_TOL_SOLID,WW));
  myDrawer->SetFreeBoundaryAspect(new Prs3d_LineAspect(aCol,Aspect_TOL_SOLID,WW));
  myDrawer->SetUnFreeBoundaryAspect(new Prs3d_LineAspect(aCol,Aspect_TOL_SOLID,WW));
  myDrawer->SetSeenLineAspect(new Prs3d_LineAspect(aCol,Aspect_TOL_SOLID,WW));

  // fast shading modification...
  if(!GetContext().IsNull()){
    if( GetContext()->MainPrsMgr()->HasPresentation(this,1)){
      Handle(Prs3d_Presentation) aPresentation = 
        GetContext()->MainPrsMgr()->CastPresentation(this,1)->Presentation();
      Handle(Graphic3d_Group) aCurGroup = Prs3d_Root::CurrentGroup(aPresentation);
      Handle(Graphic3d_AspectFillArea3d) anAreaAspect = myDrawer->ShadingAspect()->Aspect();
      Handle(Graphic3d_AspectLine3d) aLineAspect = myDrawer->LineAspect()->Aspect();

      // Set aspects for presentation and for group
      aPresentation->SetPrimitivesAspect(anAreaAspect);
      aPresentation->SetPrimitivesAspect(aLineAspect);
      // Check if aspect of given type is set for the group, 
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aCurGroup->IsGroupPrimitivesAspectSet(Graphic3d_ASPECT_FILL_AREA))
        aCurGroup->SetGroupPrimitivesAspect(anAreaAspect);
      if (aCurGroup->IsGroupPrimitivesAspectSet(Graphic3d_ASPECT_LINE))
        aCurGroup->SetGroupPrimitivesAspect(aLineAspect);
    }
  }

  LoadRecomputable(0);
  LoadRecomputable(2);
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================

void AIS_Shape::UnsetColor()
{
  if ( !HasColor() )
  {
    myToRecomputeModes.Clear();
    return;
  }
  hasOwnColor = Standard_False;

  Handle(Prs3d_LineAspect) NullAsp;
  Handle(Prs3d_ShadingAspect) NullShA;
  
  if(!HasWidth()) {
    myDrawer->SetLineAspect(NullAsp);
    myDrawer->SetWireAspect(NullAsp);
    myDrawer->SetFreeBoundaryAspect(NullAsp);
    myDrawer->SetUnFreeBoundaryAspect(NullAsp);
    myDrawer->SetSeenLineAspect(NullAsp);
  }
  else {
    Quantity_Color CC;
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line,CC);
    myDrawer->LineAspect()->SetColor(CC);
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Wire,CC);
    myDrawer->WireAspect()->SetColor(CC);
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Free,CC);
    myDrawer->FreeBoundaryAspect()->SetColor(CC);
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_UnFree,CC);
    myDrawer->UnFreeBoundaryAspect()->SetColor(CC);
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Seen,CC);
    myDrawer->SeenLineAspect()->SetColor(CC);
  }

  if( HasMaterial() || IsTransparent()) {
    Graphic3d_MaterialAspect mat = AIS_GraphicTool::GetMaterial(HasMaterial()? myDrawer : myDrawer->Link());
    if( HasMaterial() ) {
      Quantity_Color color = myDrawer->Link()->ShadingAspect()->Color(myCurrentFacingModel);
      mat.SetColor(color);
    }
    if( IsTransparent() ) {
      Standard_Real trans = myDrawer->ShadingAspect()->Transparency(myCurrentFacingModel);
      mat.SetTransparency(trans);
    }
    myDrawer->ShadingAspect()->SetMaterial(mat,myCurrentFacingModel);
  }
  else {
    myDrawer->SetShadingAspect(NullShA);
  }

  if(!GetContext().IsNull()){
    if(GetContext()->MainPrsMgr()->HasPresentation(this,1)){
      Handle(Prs3d_Presentation) aPresentation = 
        GetContext()->MainPrsMgr()->CastPresentation(this,1)->Presentation();
      Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(aPresentation);

      Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->Link()->ShadingAspect()->Aspect();
      Handle(Graphic3d_AspectLine3d) aLineAsp = myDrawer->Link()->LineAspect()->Aspect();
      Quantity_Color CC;
      AIS_GraphicTool::GetInteriorColor(myDrawer->Link(),CC);
      anAreaAsp->SetInteriorColor(CC);
      aPresentation->SetPrimitivesAspect(anAreaAsp);
      aPresentation->SetPrimitivesAspect(aLineAsp);
      // Check if aspect of given type is set for the group, 
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet(Graphic3d_ASPECT_FILL_AREA))
        aGroup->SetGroupPrimitivesAspect(anAreaAsp);
      if (aGroup->IsGroupPrimitivesAspectSet(Graphic3d_ASPECT_LINE))
        aGroup->SetGroupPrimitivesAspect(aLineAsp);
    }
  }
  LoadRecomputable(0);
  LoadRecomputable(2);
}

//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================

void AIS_Shape::SetWidth(const Standard_Real W)
{
  if(HasColor() || HasWidth()){
    myDrawer->LineAspect()->SetWidth(W);
    myDrawer->WireAspect()->SetWidth(W);
    myDrawer->FreeBoundaryAspect()->SetWidth(W);
    myDrawer->UnFreeBoundaryAspect()->SetWidth(W);
    myDrawer->SeenLineAspect()->SetWidth(W);
  }
  else{
    Quantity_Color CC;
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Line,CC);
    myDrawer->SetLineAspect(new Prs3d_LineAspect(CC,Aspect_TOL_SOLID,W));
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Wire,CC);
    myDrawer->SetWireAspect(new Prs3d_LineAspect(CC,Aspect_TOL_SOLID,W));
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Free,CC);
    myDrawer->SetFreeBoundaryAspect(new Prs3d_LineAspect(CC,Aspect_TOL_SOLID,W));
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_UnFree,CC);
    myDrawer->SetUnFreeBoundaryAspect(new Prs3d_LineAspect(CC,Aspect_TOL_SOLID,W));
    AIS_GraphicTool::GetLineColor(myDrawer->Link(),AIS_TOA_Seen,CC);
    myDrawer->SetSeenLineAspect(new Prs3d_LineAspect(CC,Aspect_TOL_SOLID,W));
  }
  myOwnWidth = W;
  LoadRecomputable(0); // means that it is necessary to recompute only the wireframe....
  LoadRecomputable(2); // and the bounding box...
}

//=======================================================================
//function : UnsetWidth
//purpose  : 
//=======================================================================

void AIS_Shape::UnsetWidth()
{
  if(myOwnWidth == 0.0)
  {
    myToRecomputeModes.Clear();
    return;
  }
  myOwnWidth=0.0;

  Handle(Prs3d_LineAspect) NullAsp;

  if(!HasColor()){
    myDrawer->SetLineAspect(NullAsp);
    myDrawer->SetWireAspect(NullAsp);
    myDrawer->SetFreeBoundaryAspect(NullAsp);
    myDrawer->SetUnFreeBoundaryAspect(NullAsp);
    myDrawer->SetSeenLineAspect(NullAsp);
  }
  else{
    myDrawer->LineAspect()->SetWidth(AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Line));
    myDrawer->WireAspect()->SetWidth(AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Wire));
    myDrawer->FreeBoundaryAspect()->SetWidth(AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Free));
    myDrawer->UnFreeBoundaryAspect()->SetWidth(AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_UnFree));
    myDrawer->SeenLineAspect()->SetWidth(AIS_GraphicTool::GetLineWidth(myDrawer->Link(),AIS_TOA_Seen));
  }
  LoadRecomputable(0);
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void AIS_Shape::SetMaterial(const Graphic3d_NameOfMaterial aMat)
{
  SetMaterial(Graphic3d_MaterialAspect(aMat));
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void AIS_Shape::SetMaterial(const Graphic3d_MaterialAspect& aMat)
{
  if( !HasColor() && !IsTransparent() && !HasMaterial() ) {
    myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  }
  hasOwnMaterial = Standard_True;

  myDrawer->ShadingAspect()->SetMaterial(aMat,myCurrentFacingModel);
  myDrawer->ShadingAspect()->SetTransparency(myTransparency,myCurrentFacingModel);

  if(!GetContext().IsNull()){
    if(GetContext()->MainPrsMgr()->HasPresentation(this,1)){
      Handle(Prs3d_Presentation) aPresentation = 
        GetContext()->MainPrsMgr()->CastPresentation(this,1)->Presentation();
      Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(aPresentation);
    
      Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
      aPresentation->SetPrimitivesAspect(anAreaAsp);
      // Check if aspect of given type is set for the group, 
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet(Graphic3d_ASPECT_FILL_AREA))
        aGroup->SetGroupPrimitivesAspect(anAreaAsp);
    }
    myRecomputeEveryPrs =Standard_False; // no mode to recalculate  :only viewer update
    myToRecomputeModes.Clear();
  }
}
//=======================================================================
//function : UnsetMaterial
//purpose  : 
//=======================================================================

void AIS_Shape::UnsetMaterial()
{
  if( !HasMaterial() ) return;

  if( HasColor() || IsTransparent()) {
    Graphic3d_MaterialAspect mat = AIS_GraphicTool::GetMaterial(myDrawer->Link()); 
    if( HasColor() ) {
      Quantity_Color color = myDrawer->ShadingAspect()->Color(myCurrentFacingModel);
      mat.SetColor(color);
    }
    if( IsTransparent() ) {
      Standard_Real trans = myDrawer->ShadingAspect()->Transparency(myCurrentFacingModel);
      mat.SetTransparency(trans);
    }
    myDrawer->ShadingAspect()->SetMaterial(mat,myCurrentFacingModel);
  } else {
    Handle(Prs3d_ShadingAspect) SA;
    myDrawer->SetShadingAspect(SA);
  }
  hasOwnMaterial = Standard_False;
  if(!GetContext().IsNull()){
    if(GetContext()->MainPrsMgr()->HasPresentation(this,1)){
      Handle(Prs3d_Presentation) aPresentation = 
        GetContext()->MainPrsMgr()->CastPresentation(this,1)->Presentation();
      Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(aPresentation);
      Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
      aPresentation->SetPrimitivesAspect(anAreaAsp);
      // Check if aspect of given type is set for the group, 
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet(Graphic3d_ASPECT_FILL_AREA))
        aGroup->SetGroupPrimitivesAspect(anAreaAsp);
    }
  }
  myRecomputeEveryPrs =Standard_False; // no mode to recalculate :only viewer update
  myToRecomputeModes.Clear();  
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================

void AIS_Shape::SetTransparency(const Standard_Real AValue)
{
  if ( !HasColor() && !HasMaterial() ) {
    myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  }
  myDrawer->ShadingAspect()->SetTransparency(AValue,myCurrentFacingModel);
  myTransparency = AValue;

  if(!GetContext().IsNull()){
    if(GetContext()->MainPrsMgr()->HasPresentation(this,1)){
      Handle(Prs3d_Presentation) aPresentation = 
        GetContext()->MainPrsMgr()->CastPresentation(this,1)->Presentation();
      Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(aPresentation);
      Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
      aPresentation->SetPrimitivesAspect(anAreaAsp);
      //force highest priority for transparent objects
      aPresentation->SetDisplayPriority(10);
      // Check if aspect of given type is set for the group, 
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet(Graphic3d_ASPECT_FILL_AREA))
        aGroup->SetGroupPrimitivesAspect(anAreaAsp);
    }
  }
  myRecomputeEveryPrs =Standard_False; // no mode to recalculate :only viewer update
  myToRecomputeModes.Clear();
}

//=======================================================================
//function : UnsetTransparency
//purpose  : 
//=======================================================================

void AIS_Shape::UnsetTransparency()
{
  if( HasColor() || HasMaterial() ) {
    myDrawer->ShadingAspect()->SetTransparency(0.0,myCurrentFacingModel);
  } else {
    Handle(Prs3d_ShadingAspect) SA;
    myDrawer->SetShadingAspect(SA);
  }

  myTransparency = 0.0;

  if(!GetContext().IsNull()){
    if(GetContext()->MainPrsMgr()->HasPresentation(this,1)){
      Handle(Prs3d_Presentation) aPresentation = 
        GetContext()->MainPrsMgr()->CastPresentation(this,1)->Presentation();
      Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(aPresentation);
      Handle(Graphic3d_AspectFillArea3d) anAreaAsp = myDrawer->ShadingAspect()->Aspect();
      aPresentation->SetPrimitivesAspect(anAreaAsp);
      // Check if aspect of given type is set for the group, 
      // because setting aspect for group with no already set aspect
      // can lead to loss of presentation data
      if (aGroup->IsGroupPrimitivesAspectSet(Graphic3d_ASPECT_FILL_AREA))
        aGroup->SetGroupPrimitivesAspect(anAreaAsp);

      aPresentation->ResetDisplayPriority();
    }
  }
  myRecomputeEveryPrs =Standard_False; // no mode to recalculate :only viewer update
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
  Standard_Boolean itSet = myDrawer->IsOwnDeviationCoefficient();
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
  Standard_Boolean itSet = myDrawer->IsOwnHLRDeviationCoefficient();
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
  Standard_Boolean itSet = myDrawer->IsOwnDeviationAngle();
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
  Standard_Boolean itSet = myDrawer->IsOwnHLRDeviationAngle();
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
  return myDrawer->IsOwnDeviationCoefficient() ;
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
  return myDrawer->IsOwnHLRDeviationCoefficient();

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
  return myDrawer->IsOwnDeviationAngle();
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
  return myDrawer->IsOwnHLRDeviationAngle();
}
