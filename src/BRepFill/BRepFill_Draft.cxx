// Created on: 1998-06-08
// Created by: Stephanie HUMEAU
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BndLib_AddSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_DSAccess.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepFill_DataMapOfShapeHArray2OfShape.hxx>
#include <BRepFill_Draft.hxx>
#include <BRepFill_DraftLaw.hxx>
#include <BRepFill_SectionLaw.hxx>
#include <BRepFill_ShapeLaw.hxx>
#include <BRepFill_Sweep.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomFill_LocationDraft.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#ifdef DRAW
#include <Geom_Circle.hxx>
#include <gp.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
static Standard_Boolean Affich = 0;
#endif

//=======================================================================
//function : Trsf
//purpose  : 
//======================================================================
static void ComputeTrsf(const TopoDS_Wire& W,
			const gp_Dir& D,
			Bnd_Box& Box,
			gp_Trsf& Tf)
{
  // Calculate approximate barycenter
  BRepTools_WireExplorer Exp(W);
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  gp_XYZ Bary(0.,0.,0.);
  Standard_Integer nb;

  for (nb=0; Exp.More(); Exp.Next()) {
//    Bary += BT.Pnt(Exp.CurrentVertex()).XYZ();
    Bary += BRep_Tool::Pnt(Exp.CurrentVertex()).XYZ();
    nb++;
  }
  Bary /= nb;

  // Calculate the Transformation  
  gp_Ax3 N(Bary, D);
  Tf.SetTransformation(N);
  BRepAdaptor_Curve AC;
//  BndLib_Add3dCurve BC;  

  // transformation to the wire
  TopoDS_Wire TheW = W;
  TopLoc_Location Loc(Tf);
  TheW.Location(Loc);


  // Calculate the box
  Box.SetVoid();
  for (Exp.Init(TheW); Exp.More(); Exp.Next()) {
    AC.Initialize(Exp.Current());
//    BC.Add(AC, 0.1, Box);
    BndLib_Add3dCurve::Add(AC, 0.1, Box);
  }
}

//=======================================================================
//function : Length
//purpose  : 
//======================================================================
static Standard_Real Longueur(const Bnd_Box& WBox,
			      const Bnd_Box& SBox,
			      gp_Dir& D,
			      gp_Pnt& P)
{
  // face of the box most remoted from the face input in 
  // the direction of skin
  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax,WZmin,WZmax,L;

  //"coord" of the box
  WBox.Get(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax);
  WZmin = Zmin;
  WZmax = Zmax;   

  SBox.Get(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax);
  P.SetCoord( (Xmin+Xmax)/2, (Ymin+Ymax)/2, Zmax);

  if (Zmax < WZmin) {
    // Skin in the wrong direction. Invert...
    D.Reverse();
    L = WZmax - Zmin;
    P.SetZ(Zmin);
  }
  else {
    L = Zmax - WZmin;
  }
  return L;
}

//=======================================================================
//function : GoodOrientation
//purpose  : Check if the law is oriented to have an exterior skin
//======================================================================
static Standard_Boolean GoodOrientation(const Bnd_Box& B,
					const Handle(BRepFill_LocationLaw)& Law,
					const gp_Dir& D)
{
  Standard_Real f, l, r, t;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;

  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  gp_Pnt P1(aXmin, aYmin, aZmin), P2(aXmax, aYmax, aZmax);
  gp_Vec V(P1, P2);

  Law->CurvilinearBounds(Law->NbLaw(), f, l);
  r = V.Magnitude()/l;
 
  Standard_Integer ii, Ind;
//#ifndef OCCT_DEBUG
  Standard_Integer Nb = (Standard_Integer) (4+(10*r));
//#else
//  Standard_Integer Nb = 4+(10*r);
//#endif
  r = l/Nb;

  Nb++; // Number of points

  TColgp_Array1OfPnt Pnts(1, Nb);
  Handle(Adaptor3d_HCurve) AC;
  gp_XYZ Bary(0.,0.,0.);
  
  for (ii=1; ii<=Nb; ii++) {
    Law->Parameter((ii-1)*r, Ind, t);
    AC = Law->Law(Ind)->GetCurve();
    AC->D0(t,  Pnts(ii));
    Bary+= Pnts(ii).XYZ();
  }

  Bary /= Nb;
  gp_Pnt Centre(Bary);
  gp_Vec Normal(D.XYZ());
  Standard_Real Angle = 0;
  gp_Vec Ref(Centre,  Pnts(1));

  for (ii=2; ii<=Nb; ii++) {
    gp_Vec R(Centre, Pnts(ii));
    Angle += Ref.AngleWithRef(R, Normal);
    Ref = R;
  }

  return (Angle >= 0);
}

//=======================================================================
//function : Constructeur
//purpose  : 
//======================================================================
 BRepFill_Draft::BRepFill_Draft(const TopoDS_Shape& S,
				const gp_Dir& Dir,
				const Standard_Real Angle)
{
  myLoc.Nullify();
  mySec.Nullify();
  myFaces.Nullify();
  mySections.Nullify(); 
   
  switch (S.ShapeType()) {
  case TopAbs_WIRE :
    {
      myWire = TopoDS::Wire(S);
      break;
    }
  case TopAbs_FACE :
    {  
      TopoDS_Iterator Exp (S);
      myWire = TopoDS::Wire(Exp.Value());
      break;
    }
  case TopAbs_SHELL :
    {  
      TopTools_ListOfShape List;
      TopTools_IndexedDataMapOfShapeListOfShape edgemap;
      TopExp::MapShapesAndAncestors(S,TopAbs_EDGE,TopAbs_FACE,edgemap);
      Standard_Integer iedge, nbf;
      for (iedge = 1; iedge <= edgemap.Extent(); iedge++) {   
	const TopoDS_Edge& theEdge = TopoDS::Edge(edgemap.FindKey(iedge));    
	// skip degenerated edges
	if (!BRep_Tool::Degenerated(theEdge)) {
	  nbf = edgemap(iedge).Extent();
	  if (nbf==1) List.Append(theEdge);
	}
      }

      if( List.Extent()>0) {
	BRepLib_MakeWire MW;
	MW.Add(List);
	BRepLib_WireError Err = MW.Error();
	if (Err == BRepLib_WireDone) {
	  myWire = MW.Wire();
	}
	else {
#ifdef OCCT_DEBUG
	  cout << "Error in MakeWire" << endl;
#endif 
	  throw Standard_ConstructionError("BRepFill_Draft");
	}
      }
      else {
#ifdef OCCT_DEBUG
	  cout << "No Free Borders !" << endl;
#endif 
	  throw Standard_ConstructionError("BRepFill_Draft");
      }
      break;
    }
    default :
      throw Standard_ConstructionError("BRepFill_Draft");
  }

  // Attention to closed non declared wires !
  if (!myWire.Closed()) {
    TopoDS_Vertex Vf, Vl;
    TopExp::Vertices(myWire, Vf, Vl);
    if (Vf.IsSame(Vl)) myWire.Closed(Standard_True);
  }
#if DRAW
  if (Affich) {
    DBRep::Set("TheWire", myWire);
  }
#endif

  myAngle = Abs(Angle);
  myDir = Dir;
  myTop = S;
  myDone = Standard_False;
  myTol = 1.e-4;
  myCont = GeomAbs_C1;
  SetOptions();
  SetDraft();
}

//=======================================================================
//function :SetOptions
//purpose  : Defines the style
//======================================================================
 void BRepFill_Draft::SetOptions(const BRepFill_TransitionStyle Style,
				 const Standard_Real Min,
				 const Standard_Real Max)
{
  myStyle =  Style;
  angmin = Min; 
  angmax = Max;
}

//=======================================================================
//function :SetDraft
//purpose  :
//======================================================================
 void BRepFill_Draft::SetDraft(const Standard_Boolean Internal)
{
  IsInternal = Internal;
}


//=======================================================================
//function :Perform
//purpose  : calculate a surface of skinning
//======================================================================
 void BRepFill_Draft::Perform(const Standard_Real LengthMax)
{
  Handle(Geom_Surface) S;
  S.Nullify();
  Bnd_Box WBox;//, SBox;
  gp_Trsf Trsf;


  ComputeTrsf(myWire, myDir, WBox, Trsf); 
  Init(S, LengthMax, WBox);
  BuildShell(S); 
  Sewing(); 
}

//=======================================================================
//function :Perform
//purpose  : calculate a surface of skinning
//======================================================================
 void BRepFill_Draft::Perform(const Handle(Geom_Surface)& Surface,
			      const Standard_Boolean KeepInsideSurface)
{
  Bnd_Box WBox, SBox;
  gp_Trsf Trsf;
  gp_Pnt Pt;
  Standard_Real L;

  ComputeTrsf(myWire, myDir, WBox, Trsf);

  // box with bounds of the stop surface  
  Handle(Geom_Surface) Surf;
  Surf =   Handle(Geom_Surface)::DownCast(Surface->Transformed(Trsf));
  GeomAdaptor_Surface S1 (Surf);   
//  BndLib_AddSurface AS; 
//  AS.Add(S1, 0.1, SBox);
  BndLib_AddSurface::Add(S1, 0.1, SBox);

  // calculate the maximum length of the rule.
  L = Longueur(WBox, SBox, myDir, Pt);
  L /= Abs(Cos(myAngle));

  // Construction
  Init(Surface, L, WBox);
  BuildShell(Surface, !KeepInsideSurface);
  Sewing(); 
}

//================================================================
//function :Perform
//purpose  : calculate the surface of skinning, stopped by a shape
//================================================================
 void BRepFill_Draft::Perform(const TopoDS_Shape& StopShape,
			      const Standard_Boolean KeepOutSide)
{
  Bnd_Box WBox, SBox;
  gp_Trsf Trsf;
  gp_Pnt Pt;
  Standard_Real L;

  ComputeTrsf(myWire, myDir, WBox, Trsf); 

// bounding box of the stop shape
  Bnd_Box BSurf;//, TheBox;
  Standard_Real Umin, Umax, Vmin, Vmax;
//  BRepTools B;
//  BRep_Tool BT;
  Handle(Geom_Surface) Surf;
 
//  BndLib_AddSurface AS;
  
  TopExp_Explorer Ex (StopShape, TopAbs_FACE);

  SBox.SetVoid();
  while (Ex.More()) { // parse faces of the stop shape
//    B.UVBounds(TopoDS::Face(Ex.Current()), Umin,Umax,Vmin,Vmax); 
    BRepTools::UVBounds(TopoDS::Face(Ex.Current()), Umin,Umax,Vmin,Vmax); 
    Surf = Handle(Geom_Surface)::DownCast(
//     BT.Surface(TopoDS::Face(Ex.Current()))->Transformed(Trsf) ); 
     BRep_Tool::Surface(TopoDS::Face(Ex.Current()))->Transformed(Trsf) ); 
    GeomAdaptor_Surface S1 (Surf);
// bounding box of the current face
//    AS.Add(S1, Umin, Umax, Vmin, Vmax, 0.1, BSurf); 
    BndLib_AddSurface::Add(S1, Umin, Umax, Vmin, Vmax, 0.1, BSurf);
    SBox.Add(BSurf);	// group boxes
    Ex.Next();
  }// while_Ex

  // calculate the maximum length of the rule.
  L = Longueur(WBox, SBox, myDir, Pt);
  L /= Abs(Cos(myAngle));

// surface of stop
  gp_Trsf Inv;
  Inv = Trsf.Inverted(); // inverted transformation
  Pt.Transform(Inv); // coordinate in the absolute reference
  Handle(Geom_Plane) Plan = new (Geom_Plane)(Pt, myDir);
  Surf = new (Geom_RectangularTrimmedSurface) (Plan,-L, L, -L, L); 

#if DRAW
  if (Affich) {
    char* Temp = "ThePlan" ;
    DrawTrSurf::Set(Temp, Surf);
//    DrawTrSurf::Set("ThePlan", Surf);
  }
#endif

// Sweeping and restriction
  Init(Plan,  L*1.01, WBox);
  BuildShell(Surf, Standard_False);
  Fuse(StopShape,  KeepOutSide);
  Sewing();
}

//=======================================================================
//function : Init
//purpose  : Construction of laws.
//======================================================================
 void BRepFill_Draft::Init(const Handle(Geom_Surface)& ,
                           const Standard_Real Length,
			   const Bnd_Box&  Box)
{
  Standard_Boolean B;

// law of positioning   
  Handle(GeomFill_LocationDraft) Loc
    = new (GeomFill_LocationDraft) (myDir, myAngle); 
  myLoc = new (BRepFill_DraftLaw) (myWire, Loc);

  B = GoodOrientation(Box, myLoc, myDir);

  if (IsInternal ^ (!B) )  {
    myAngle = - myAngle;
    Loc->SetAngle(myAngle);
    myLoc = new (BRepFill_DraftLaw) (myWire, Loc);
  }

  myLoc->CleanLaw(angmin); // Clean small discontinuities.

// law of section
// generating line is straight and parallel to binormal.
  gp_Pnt P(0, 0, 0);
  gp_Vec D (0., 1., 0.);

// Control of the orientation
  Standard_Real f,l;
  myLoc->Law(1)->GetDomain(f,l);
  gp_Mat M;

  gp_Vec Bid; 
  myLoc->Law(1)->D0( (f+l)/2, M, Bid);
  gp_Dir BN(M.Column(2));
  
  Standard_Real ang = myDir.Angle(BN);
  if (ang > M_PI/2) D.Reverse();    
  Handle(Geom_Line) L = new (Geom_Line) (P, D);

  Handle(Geom_Curve) TC = new (Geom_TrimmedCurve) (L, 0, Length);


#if DRAW
  if (Affich > 2) {
     TC = new (Geom_Circle) (gp::XOY(), Length);
  }
#endif 

  BRepLib_MakeEdge ME(TC);
  TopoDS_Edge EG = ME.Edge();
  
  BRepLib_MakeWire MW(EG);
  TopoDS_Wire G = MW.Wire();

  mySec = new (BRepFill_ShapeLaw) (G, Standard_True);
}


//=======================================================================
//function : BuildShell
//purpose  : Construction of the skinning surface 
//======================================================================
 void BRepFill_Draft::BuildShell(const Handle(Geom_Surface)& Surf,
				 const Standard_Boolean  KeepOutSide) 
{
// construction of the surface
  BRepFill_Sweep Sweep(mySec, myLoc, Standard_True);
  Sweep.SetTolerance(myTol);
  Sweep.SetAngularControl(angmin, angmax);
  TopTools_MapOfShape Dummy;
  BRepFill_DataMapOfShapeHArray2OfShape Dummy2;
  BRepFill_DataMapOfShapeHArray2OfShape Dummy3;
  Sweep.Build(Dummy, Dummy2, Dummy3, myStyle, myCont);
  if (Sweep.IsDone()) {
    myShape = Sweep.Shape();
    myShell = TopoDS::Shell(myShape);
    myFaces    = Sweep.SubShape();
    mySections = Sweep.Sections();    
    myDone = Standard_True;
    // Control of the orientation
    Standard_Boolean out=Standard_True;
    TopExp_Explorer ex(myShell,TopAbs_FACE);
    TopoDS_Face F;
    F = TopoDS::Face(ex.Current());
    BRepAdaptor_Surface SF(F);
    Standard_Real u, v;
    gp_Pnt P;
    gp_Vec V1, V2, V;
    u = SF.FirstUParameter();
    v = SF.FirstVParameter();
    SF.D1(u,v,P,V1,V2);
    V = V1.Crossed(V2);
    if (F.Orientation() == TopAbs_REVERSED) V.Reverse();
    if (V.Magnitude() > 1.e-10) {
      out = myDir.Angle(V) > M_PI/2;
    }
    if (out == IsInternal) {
      myShell.Reverse();
      myShape.Reverse();
    }    
  }
  else {
    myDone = Standard_False;
    return;
  }

  if (!Surf.IsNull()) { // Add the face at end

  // Waiting the use of traces & retriction in BRepFill_Sweep
  // Make Fuse.
    BRepLib_MakeFace MkF;
    MkF.Init(Surf, Standard_True, Precision::Confusion());
    Fuse(MkF.Face(), KeepOutSide);
  }
}


//=======================================================================
//function : Fuse
//purpose  : Boolean operation between the skin and the  
//           stop shape 
//======================================================================
 Standard_Boolean BRepFill_Draft::Fuse(const TopoDS_Shape& StopShape,
			   const Standard_Boolean KeepOutSide)
{
  BRep_Builder B;
  Standard_Boolean issolid = Standard_False;
  TopoDS_Solid Sol1, Sol2;
  TopAbs_State State1 = TopAbs_OUT,  State2 = TopAbs_OUT;


  if (myShape.ShapeType()==TopAbs_SOLID) {
    Sol1 = TopoDS::Solid(myShape);
    issolid = Standard_True;
  }
  else {
    B.MakeSolid(Sol1);
    B.Add(Sol1, myShape); // shell => solid (for fusion)    
  }


  switch (StopShape.ShapeType()) {
  case TopAbs_COMPOUND :
    {
      TopoDS_Iterator It(StopShape);
      return Fuse(It.Value(), KeepOutSide);
    }
  case TopAbs_SOLID : 
    {
      Sol2 = TopoDS::Solid(StopShape);
      break;
    }
 case TopAbs_SHELL :
    {
      B.MakeSolid(Sol2);
      B.Add(Sol2, StopShape); // shell => solid (for fusion) 
      break;
    }

 case TopAbs_FACE :
    {
      TopoDS_Shell S;
      B.MakeShell(S);
      B.Add(S, StopShape);
      S.Closed (BRep_Tool::IsClosed (S));
      B.MakeSolid(Sol2);
      B.Add(Sol2, S); // shell => solid (for fusion)
      break;
    }

  default :
    {
      return Standard_False; // Impossible to do
    }
  }
      
  BRepAlgo_DSAccess DSA;
  DSA.Load(Sol1, Sol2);
  DSA.Intersect(Sol1, Sol2); // intersection of 2 solids
 
// removal of edges corresponding to "unused" intersections
  Standard_Integer NbPaquet;
// gp_Pnt P1,P2;
  TopoDS_Vertex V,V1;
  TopTools_ListOfShape List;
  List  = DSA.GetSectionEdgeSet();// list of edges
  
  NbPaquet = List.Extent();

  if (NbPaquet == 0) {
#if DRAW
    cout << "No fusion" << endl;
    DBRep::Set("DepPart", Sol1);
    DBRep::Set("StopPart", Sol2);
#endif
    return Standard_False;
  }

  if (NbPaquet > 1) {
    // It is required to select packs.
    TColStd_Array1OfReal Dist(1, NbPaquet);
    TopTools_ListIteratorOfListOfShape it(List);
    Standard_Real D, Dmin = 1.e10;
    Standard_Integer ii;
 
    //Classify the packs by distance.
    BRepExtrema_DistShapeShape Dist2;
    Dist2.LoadS1( myWire );
    for (ii=1; it.More();it.Next(),ii++){
      Dist2.LoadS2( it.Value() );
      Dist2.Perform();
      if (Dist2.IsDone()) {
	D = Dist2.Value();
	Dist(ii) = D;
	if (D < Dmin) Dmin = D;
      }
      else
	Dist(ii) = 1.e10;
    }

    // remove edges "farther" than Dmin
    for (ii=1, it.Initialize(List); it.More();it.Next(), ii++){
      if (Dist(ii) > Dmin) {
	DSA.SuppressEdgeSet(it.Value());
      }
#if DRAW
     else if (Affich) {
	DBRep::Set("KeepEdges", it.Value());
      }
#endif
    }
  }

  if (StopShape.ShapeType() != TopAbs_SOLID) {
    // It is required to choose the state by the geometry

    //(1) Return an edge of section
    List  = DSA.GetSectionEdgeSet();// list of edges
    TopTools_ListIteratorOfListOfShape it(List);
    TopoDS_Iterator iter(it.Value());
    TopoDS_Edge E = TopoDS::Edge(iter.Value());

    //(2) Return geometry on StopShape
// Class BRep_Tool without fields and without Constructor :
//    BRep_Tool BT;
    Handle(Geom_Surface) S;
    Handle(Geom2d_Curve) C2d;
    gp_Pnt2d P2d;
    Standard_Real f,l;  
    TopLoc_Location L;
//    BT.CurveOnSurface(E, C2d, S, L, f, l, 2);
    BRep_Tool::CurveOnSurface(E, C2d, S, L, f, l, 2);

    // Find a normal.
    C2d->D0((f+l)/2,P2d); 
    GeomLProp_SLProps SP(S, P2d.X(), P2d.Y(), 1, 1.e-12);
    if (! SP.IsNormalDefined()) {
      C2d->D0((3*f+l)/4,P2d);
      SP.SetParameters(P2d.X(), P2d.Y());
      if ( !SP.IsNormalDefined()) {
	C2d->D0((f+3*l)/4,P2d);
	SP.SetParameters(P2d.X(), P2d.Y());
      }
    }

    // Subtract State1
    if (myDir.Angle(SP.Normal()) < M_PI/2)  State1 = TopAbs_IN;
    else  State1 = TopAbs_OUT;
  }

  if (! KeepOutSide) { // Invert State2;
    if (State2 == TopAbs_IN) State2 = TopAbs_OUT;
    else State2 = TopAbs_IN;
  }
 
//recalculate the final shape
  TopoDS_Shape result = DSA.Merge(State1, State2); 

  if (issolid) myShape =  result;
  else {
    TopExp_Explorer Exp;
    Exp.Init(result, TopAbs_SHELL);
    if (Exp.More()) myShape = Exp.Current();
  }

// Update the History 
  Standard_Integer ii;
  for (ii=1; ii<=myLoc->NbLaw(); ii++) {
    const TopTools_ListOfShape& L = DSA.Modified(myFaces->Value(1,ii));
    if (L.Extent()>0) 
      myFaces->SetValue(1, ii, L.First());
  }
  for (ii=1; ii<=myLoc->NbLaw()+1; ii++) {
    const TopTools_ListOfShape& L = DSA.Modified(mySections->Value(1,ii));
    if (L.Extent()>0) 
      mySections->SetValue(1, ii, L.First());
  } 
 
  return Standard_True;
}

//=======================================================================
//function : Sewing 
//purpose  : Assemble the skin with the above face
//======================================================================
 Standard_Boolean BRepFill_Draft::Sewing()
{
  Standard_Boolean ToAss;
  Standard_Boolean Ok = Standard_False;
  ToAss = (myTop.ShapeType() != TopAbs_WIRE);

  if ((!ToAss) || (!myDone)) return Standard_False;

   // Assembly make a shell from the faces of the shape + the input shape
  Handle(BRepBuilderAPI_Sewing) Ass =  new BRepBuilderAPI_Sewing(5*myTol, Standard_True, 
   		                                     Standard_True, Standard_False);
  Ass->Add(myShape);
  Ass->Add(myTop);
  ToAss = Standard_True;
 

  Standard_Integer NbCE;
  
  Ass->Perform();
  // Check if the assembly is real.
  NbCE = Ass->NbContigousEdges();
  
  if (NbCE > 0) {
    TopoDS_Shape res;
    res = Ass->SewedShape();   
    if ((res.ShapeType() == TopAbs_SHELL)||
	(res.ShapeType() == TopAbs_SOLID))  {
      myShape = res;
      Ok = Standard_True;
    }
    else if (res.ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Iterator It(res);
      res = It.Value();
      It.Next();
      if (!It.More()) {//Only one part => this is correct
	myShape = res;
	Ok = Standard_True;
      }
    }
  }

  if (Ok) {
    // Update the History
    Standard_Integer ii;
    for (ii=1; ii<=myLoc->NbLaw(); ii++) {
      if (Ass->IsModified(myFaces->Value(1,ii)))     
	myFaces->SetValue(1, ii, 
			  Ass->Modified(myFaces->Value(1,ii)));
    }
    for (ii=1; ii<=myLoc->NbLaw()+1; ii++) {
      if (Ass->IsModified(mySections->Value(1,ii))) 
	mySections->SetValue(1, ii, 
			     Ass->Modified(mySections->Value(1,ii)));
    }   

    if (myShape.Closed()) { // Make a Solid
      TopoDS_Solid solid;
      BRep_Builder BS;
      BS.MakeSolid(solid); 
      BS.Add(solid,TopoDS::Shell(myShape));
      
      BRepClass3d_SolidClassifier SC(solid);
      SC.PerformInfinitePoint(Precision::Confusion());
      if ( SC.State() == TopAbs_IN) {
	BS.MakeSolid(solid);
	myShape.Reverse();
	BS.Add(solid,TopoDS::Shell(myShape));
      }
      myShape = solid;
    }    
  }
#ifdef OCCT_DEBUG
  else cout << "Draft : No assembly !" << endl;
#endif
  return Ok;
}

//=======================================================================
//function : Generated
//purpose  : return a sub-part generated by sweeping
//======================================================================
 const TopTools_ListOfShape& 
 BRepFill_Draft::Generated(const TopoDS_Shape& S) 
{
  myGenerated.Clear();
  TopoDS_Edge E;
  Standard_Integer ii;
  E = TopoDS::Edge(S);
  if (E.IsNull()) {
   for (ii=0; ii<=myLoc->NbLaw(); ii++)
      if (E.IsSame(myLoc->Vertex(ii))) {
	myGenerated.Append(mySections->Value(1, ii+1));
	break;
      } 
  }
  else {
    for (ii=1; ii<=myLoc->NbLaw(); ii++)
      if (E.IsSame(myLoc->Edge(ii))) {
	myGenerated.Append(myFaces->Value(1, ii));
	break;
      }
  }
  
  return myGenerated;
}

//=======================================================================
//function : Shape
//purpose  : return the complete shape
//======================================================================
 TopoDS_Shape BRepFill_Draft::Shape()const
{
  return myShape;
}

//=====================================================================
//function : Shell
//purpose  : surface of skinning with the input face (=>shell)
//=====================================================================
 TopoDS_Shell BRepFill_Draft::Shell()const
{
  return myShell;
}

//=======================================================================
//function : IsDone
//purpose  : 
//======================================================================
 Standard_Boolean BRepFill_Draft::IsDone()const
{
  return myDone;
}
