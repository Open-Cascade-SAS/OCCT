// File:	XCAFPrs_AISObject.cxx
// Created:	Fri Aug 11 16:49:09 2000
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

#include <XCAFPrs_AISObject.ixx>
#include <TCollection_ExtendedString.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_LengthAspect.hxx>
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
#include <Graphic3d_Array1OfVertex.hxx>
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
//function : Compute
//purpose  : 
//=======================================================================

void DisplayBox(const Handle(Prs3d_Presentation)& aPrs,
		       const Bnd_Box& B,
		       const Handle(Prs3d_Drawer)& aDrawer)
{
  Standard_Real X[2],Y[2],Z[2];
  Standard_Integer Indx [16] ;
  if ( B.IsVoid() )
    return;
  
#ifdef BUC60577
  Indx [0]=1;Indx [1]=2;Indx [2]=4;Indx [3]=3;
  Indx [4]=5;Indx [5]=6;Indx [6]=8;Indx [7]=7;
  Indx [8]=1;Indx [9]=3;Indx [10]=7;Indx [11]=5;
  Indx [12]=2;Indx [13]=4;Indx [14]=8;Indx [15]=6;
  B.Get(X[0], Y[0], Z[0], X[1], Y[1], Z[1]);
#else
  Indx [0]=1;Indx [1]=2;Indx [2]=3;Indx [3]=4;Indx [4]=5;Indx [5]=6;Indx [6]=7;
  Indx [7]=8;Indx [8]=1;Indx [9]=2;Indx [10]=6;Indx [10]=5;Indx [10]=3;
  Indx [10]=4;Indx [10]=8;Indx [10]=7;
  B.Get(X[1], Y[1], Z[1], X[2], Y[2], Z[2]);
#endif

  Graphic3d_Array1OfVertex V(1,8);
  Standard_Integer Rank(0);
  for(Standard_Integer k=0;k<=1;k++)
    for(Standard_Integer j=0;j<=1;j++)
      for(Standard_Integer i=0;i<=1;i++)
	V(++Rank) = Graphic3d_Vertex(X[i],Y[j],Z[k]);
  
  
  Handle(Graphic3d_Group) G = Prs3d_Root::CurrentGroup(aPrs);
  Quantity_Color Q;
  Aspect_TypeOfLine A;
  Standard_Real W;
  aDrawer->LineAspect()->Aspect()->Values(Q,A,W);
  

  G->SetGroupPrimitivesAspect(new Graphic3d_AspectLine3d(Q,Aspect_TOL_DOTDASH,W));
  
  G->BeginPrimitives();Standard_Integer I,J;
  Graphic3d_Array1OfVertex VVV (1,5);
  for(I=1;I<=4;I++){
    for(J=1;J<=4;J++){
      VVV.SetValue(J,V(Indx[J+4*I-5]));
    }
    VVV.SetValue(5,VVV(1));
    G->Polyline(VVV);
  }
  G->EndPrimitives();
}

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
      
      if (OwnDeviationAngle(newangle,prevangle) ||
	  OwnDeviationCoefficient(newcoeff,prevcoeff))
	if (Abs (newangle - prevangle) > Precision::Angular() ||
	    Abs (newcoeff - prevcoeff) > Precision::Confusion()  ) { 
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
  cout << "XCAFPrs_AISObject: Update called" << endl;
#endif
  aPrs->Clear();

  // abv: 06 Mar 00: to have good colors
  Handle(TPrsStd_AISPresentation) prs = Handle(TPrsStd_AISPresentation)::DownCast ( GetOwner() );
  Graphic3d_NameOfMaterial material = ( prs.IsNull() ? Graphic3d_NOM_PLASTIC : prs->Material() );
//  Graphic3d_NameOfMaterial material = Material();
  SetMaterial ( material );
  
//  SetMaterial ( Graphic3d_NOM_PLASTIC );

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
  cout << "Styles collected" << endl;
#endif

  // dispatch (sub)shapes by their styles
  XCAFPrs_DataMapOfStyleShape items;
  XCAFPrs_Style DefStyle;
  Quantity_Color White ( Quantity_NOC_WHITE );
  DefStyle.SetColorSurf ( White );
  DefStyle.SetColorCurv ( White );
  XCAFPrs::DispatchStyles ( shape, settings, items, DefStyle );
#ifdef DEB
  cout << "Dispatch done" << endl;
#endif

  // add subshapes to presentation (one shape per style)
  XCAFPrs_DataMapIteratorOfDataMapOfStyleShape it ( items );
#ifdef DEB
  Standard_Integer i=1;
#endif
  for ( ; it.More(); it.Next() ) {
    XCAFPrs_Style s = it.Key();
#ifdef DEB
    cout << "Style " << i << ": [" << 
      ( s.IsSetColorSurf() ? Quantity_Color::StringName ( s.GetColorSurf().Name() ) : "" ) << ", " <<
      ( s.IsSetColorCurv() ? Quantity_Color::StringName ( s.GetColorCurv().Name() ) : "" ) << "]" <<
	" --> si_" << i << ( s.IsVisible() ? "" : " <invisible>" ) << endl;
#ifdef DEBUG
    char str[200];
    sprintf ( str, "si_%d", i );
    DBRep::Set ( str, it.Value() );
    try { OCC_CATCH_SIGNALS  ; } // to handle all till the end of for
    catch (Standard_Failure) {
      cout << "Exception in AddStyledItem!" << endl;
      continue;
    }
#endif
    i++;
#endif
    if (! s.IsVisible() ) continue;
    AddStyledItem ( s, it.Value(), aPresentationManager, aPrs, aMode );
  }
  
  if ( XCAFPrs::GetViewNameMode() ) {
  // Displaying Name attributes
#ifdef DEB
    cout << "Now display name of shapes" << endl;
#endif
    aPrs->SetDisplayPriority(10);
    DisplayText (myLabel, aPrs, Attributes()->LengthAspect()->TextAspect(), TopLoc_Location());//no location
  }
#ifdef DEB
  cout << "Compute finished" << endl;
#endif
  
  aPrs->ReCompute(); // for hidden line recomputation if necessary...
}
