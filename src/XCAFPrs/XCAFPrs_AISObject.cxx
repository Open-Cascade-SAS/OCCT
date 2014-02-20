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

#include <XCAFPrs_AISObject.ixx>
#include <TCollection_ExtendedString.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <Precision.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRepTools.hxx>

#include <StdPrs_WFDeflectionShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <AIS_Drawer.hxx>
#include <Graphic3d_Group.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <XCAFPrs_Style.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFPrs_DataMapOfShapeStyle.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFPrs_DataMapOfStyleShape.hxx>
#include <TopoDS.hxx>
#include <XCAFPrs_DataMapIteratorOfDataMapOfStyleShape.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Aspect_InteriorStyle.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Prs3d_ShadingAspect.hxx>

#include <Prs3d_IsoAspect.hxx>
#include <XCAFPrs.hxx>

#include <TDataStd_Name.hxx>
#include <BRepBndLib.hxx>

#include <TPrsStd_AISPresentation.hxx>

#ifdef DEBUG
#include <DBRep.hxx>
#endif


//=======================================================================
//function : XCAFPrs_AISObject
//purpose  : 
//=======================================================================

XCAFPrs_AISObject::XCAFPrs_AISObject (const TDF_Label &lab) : AIS_Shape(TopoDS_Shape())
{
  myLabel = lab;

  TopoDS_Shape shape;
  if ( XCAFDoc_ShapeTool::GetShape ( myLabel, shape ) && ! shape.IsNull() ) 
    Set ( shape );
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void XCAFPrs_AISObject::SetColor(const Quantity_Color &aCol)
{
  AIS_Shape::SetColor(aCol);
  LoadRecomputable(1);
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================

void XCAFPrs_AISObject::UnsetColor()
{
  if (HasColor())
  {
    AIS_Shape::UnsetColor();
    LoadRecomputable(1);
  }
  else
  {
    myToRecomputeModes.Clear();
  }
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void XCAFPrs_AISObject::SetMaterial(const Graphic3d_NameOfMaterial aMat)
{
  AIS_Shape::SetMaterial(aMat);
  LoadRecomputable(1);
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void XCAFPrs_AISObject::SetMaterial(const Graphic3d_MaterialAspect& aMat)
{
  AIS_Shape::SetMaterial(aMat);
  LoadRecomputable(1);
}

//=======================================================================
//function : UnsetMaterial
//purpose  : 
//=======================================================================

void XCAFPrs_AISObject::UnsetMaterial()
{
  if (HasMaterial())
  {
    AIS_Shape::UnsetMaterial();
    LoadRecomputable(1);
  }
  else
  {
    myToRecomputeModes.Clear();
  }
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================

void XCAFPrs_AISObject::SetTransparency(const Standard_Real AValue)
{
  AIS_Shape::SetTransparency(AValue);
  LoadRecomputable(1);
}

//=======================================================================
//function : UnsetTransparency
//purpose  : 
//=======================================================================

void XCAFPrs_AISObject::UnsetTransparency()
{
  AIS_Shape::UnsetTransparency();
  LoadRecomputable(1);
}

//=======================================================================
//function : AddStyledItem
//purpose  : 
//=======================================================================

void XCAFPrs_AISObject::AddStyledItem (const XCAFPrs_Style &style, 
                                       const TopoDS_Shape &shape, 
                                       const Handle(PrsMgr_PresentationManager3d)&, // aPresentationManager,
                                       const Handle(Prs3d_Presentation)& aPrs,
                                       const Standard_Integer aMode)
{ 
  // remember current color settings
  Handle(Graphic3d_AspectFillArea3d) a4bis = myDrawer->ShadingAspect()->Aspect();
  Aspect_InteriorStyle aStyle;
  Quantity_Color aIntColor, aEdgeColor;
  Aspect_TypeOfLine aType;
  Standard_Real aWidth;
  a4bis->Values(aStyle,aIntColor,aEdgeColor,aType,aWidth);
  Graphic3d_MaterialAspect FMAT = a4bis->FrontMaterial();
  Quantity_Color aFColor = FMAT.Color();

  Quantity_Color aColor1, aColor2;
  Aspect_TypeOfLine aLine1, aLine2;
  Standard_Real aWigth1, aWigth2;
  Handle(Prs3d_LineAspect) waUFB = myDrawer->UnFreeBoundaryAspect();
  waUFB->Aspect()->Values(aColor1,aLine1,aWigth1);
  Handle(Prs3d_LineAspect) waFB = myDrawer->FreeBoundaryAspect();
  waFB->Aspect()->Values(aColor2,aLine2,aWigth2);

  Quantity_Color aColor;
  Aspect_TypeOfLine aLine;
  Standard_Real aWigth;
  Handle(Prs3d_LineAspect) wa = myDrawer->WireAspect();
  wa->Aspect()->Values(aColor,aLine,aWigth);

  Quantity_Color aColorU, aColorV;
  Aspect_TypeOfLine aLineU, aLineV;
  Standard_Real aWigthU, aWigthV;
  Handle(Prs3d_IsoAspect) UIso = myDrawer->UIsoAspect();
  Handle(Prs3d_IsoAspect) VIso = myDrawer->VIsoAspect();
  UIso->Aspect()->Values(aColorU,aLineU,aWigthU);
  VIso->Aspect()->Values(aColorV,aLineV,aWigthV);
  
  // Set colors etc. for current shape according to style
  if ( style.IsSetColorCurv() ) {
    Quantity_Color Color = style.GetColorCurv();
    waUFB->SetColor ( Color.Name() );
    waFB->SetColor ( Color.Name() );
    wa->SetColor ( Color.Name() );
  }
  if ( style.IsSetColorSurf() ) {
    Quantity_Color Color = style.GetColorSurf();
    a4bis->SetInteriorColor(Color);
    FMAT.SetColor(Color);
    a4bis->SetFrontMaterial(FMAT);
    UIso->SetColor ( Color.Name() );
    VIso->SetColor ( Color.Name() );
  }

  // force drawing isos on planes
  Standard_Boolean drawIsosPln = myDrawer->IsoOnPlane();
  myDrawer->SetIsoOnPlane (Standard_True);
  
  // add shape to presentation
  switch (aMode) {
  case 0:{
    try { OCC_CATCH_SIGNALS  StdPrs_WFDeflectionShape::Add(aPrs,shape,myDrawer); }
    catch (Standard_Failure) { 
#ifdef DEB
      cout << "AIS_Shape::Compute() failed: exception " <<
	      Standard_Failure::Caught()->DynamicType()->Name() << ": " <<
	      Standard_Failure::Caught()->GetMessageString() << endl;
#endif
//      cout << "a Shape should be incorrect: No Compute can be maked on it  "<< endl;     
// on calcule une presentation de la boite englobante
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
	  BRepTools::Clean(shape);
	}
    
      //shading seulement a partir de face...
      try {
	OCC_CATCH_SIGNALS
	if ((Standard_Integer) shape.ShapeType()>4)
	  StdPrs_WFDeflectionShape::Add(aPrs,shape,myDrawer);
	else {
	  myDrawer->SetShadingAspectGlobal(Standard_False);
	  if (IsInfinite()) 
	    StdPrs_WFDeflectionShape::Add(aPrs,shape,myDrawer);
	  else
	    StdPrs_ShadedShape::Add(aPrs,shape,myDrawer);
	}
      }
      catch (Standard_Failure) {
#ifdef DEB
	cout << "AIS_Shape::Compute() in ShadingMode failed: exception " <<
	        Standard_Failure::Caught()->DynamicType()->Name() << ": " <<
	        Standard_Failure::Caught()->GetMessageString() << endl;
#endif
	// last resort: try to display as wireframe
	try {
	  OCC_CATCH_SIGNALS
	  StdPrs_WFShape::Add(aPrs,shape,myDrawer);
	}
	catch (Standard_Failure) {
	}
      }
      break;
    }
  case 2:
    {
      // boite englobante
      if (IsInfinite()) StdPrs_WFDeflectionShape::Add(aPrs,shape,myDrawer);
      else DisplayBox(aPrs,BoundingBox(),myDrawer);
    }
  }

  // Restore initial settings
  if ( style.IsSetColorCurv() ) {
    waUFB->SetColor ( aColor1.Name() );
    waFB->SetColor ( aColor2.Name() );
    wa->SetColor ( aColor.Name() );
  }
  if ( style.IsSetColorSurf() ) {
    a4bis->SetInteriorColor(aIntColor);
    FMAT.SetColor(aFColor);
    a4bis->SetFrontMaterial(FMAT);
    UIso->SetColor ( aColorU );
    VIso->SetColor ( aColorV );
  }
  myDrawer->SetIsoOnPlane (drawIsosPln);
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
// The Compute() method is copied from AIS_Shape::Compute and enhanced to 
// support different color settings for different subshapes of a single shape
  
void XCAFPrs_AISObject::Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                                 const Handle(Prs3d_Presentation)& aPrs,
                                 const Standard_Integer aMode)
{  
#ifdef DEB
  //cout << "XCAFPrs_AISObject: Update called" << endl;
#endif
  aPrs->Clear();

  // abv: 06 Mar 00: to have good colors
  Handle(TPrsStd_AISPresentation) prs = Handle(TPrsStd_AISPresentation)::DownCast ( GetOwner() );
  if ( prs.IsNull() || !prs->HasOwnMaterial() )
    AIS_Shape::SetMaterial ( Graphic3d_NOM_PLASTIC );

  TopoDS_Shape shape;
  if ( ! XCAFDoc_ShapeTool::GetShape ( myLabel, shape ) || shape.IsNull() ) return;

  // wire,edge,vertex -> pas de HLR + priorite display superieure
  Standard_Integer TheType = (Standard_Integer)shape.ShapeType();
  if(TheType>4 && TheType<8) {
    aPrs->SetVisual(Graphic3d_TOS_ALL);
    aPrs->SetDisplayPriority(TheType+2);
  }
  // Shape vide -> Assemblage vide.
  if (shape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator anExplor (shape);
    if (!anExplor.More()) {
      return;
    }
  }
  if (IsInfinite()) aPrs->SetInfiniteState(Standard_True); //pas de prise en compte lors du FITALL
  
  // collect information on colored subshapes
  TopLoc_Location L;
  XCAFPrs_DataMapOfShapeStyle settings;
  XCAFPrs::CollectStyleSettings ( myLabel, L, settings );
#ifdef DEB
  //cout << "Styles collected" << endl;
#endif

  // dispatch (sub)shapes by their styles
  XCAFPrs_DataMapOfStyleShape items;
  XCAFPrs_Style DefStyle;
  DefaultStyle (DefStyle);
  XCAFPrs::DispatchStyles ( shape, settings, items, DefStyle );
#ifdef DEB
  //cout << "Dispatch done" << endl;
#endif

  // add subshapes to presentation (one shape per style)
  XCAFPrs_DataMapIteratorOfDataMapOfStyleShape it ( items );
#ifdef DEB
  //Standard_Integer i=1;
#endif
  for ( ; it.More(); it.Next() ) {
    XCAFPrs_Style s = it.Key();
#ifdef DEB
    //cout << "Style " << i << ": [" << 
    //  ( s.IsSetColorSurf() ? Quantity_Color::StringName ( s.GetColorSurf().Name() ) : "" ) << ", " <<
    //  ( s.IsSetColorCurv() ? Quantity_Color::StringName ( s.GetColorCurv().Name() ) : "" ) << "]" <<
	//" --> si_" << i << ( s.IsVisible() ? "" : " <invisible>" ) << endl;
    //i++;
#endif
    if (! s.IsVisible() ) continue;
    Prs3d_Root::NewGroup(aPrs);
    AddStyledItem ( s, it.Value(), aPresentationManager, aPrs, aMode );
  }
  
  if ( XCAFPrs::GetViewNameMode() ) {
  // Displaying Name attributes
#ifdef DEB
    //cout << "Now display name of shapes" << endl;
#endif
    aPrs->SetDisplayPriority(10);
    DisplayText (myLabel, aPrs, Attributes()->DimensionAspect()->TextAspect(), TopLoc_Location());//no location
  }
#ifdef DEB
  //cout << "Compute finished" << endl;
#endif
  
  aPrs->ReCompute(); // for hidden line recomputation if necessary...
}

//=======================================================================
//function : DefaultStyle
//purpose  : DefaultStyle() can be redefined by subclasses in order to set custom default style
//=======================================================================
void XCAFPrs_AISObject::DefaultStyle (XCAFPrs_Style& aStyle) const
{
  static const Quantity_Color White ( Quantity_NOC_WHITE );
  aStyle.SetColorSurf ( White );
  aStyle.SetColorCurv ( White );
}
