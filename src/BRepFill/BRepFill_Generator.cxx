// Created on: 1994-03-07
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepFill_Generator.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill.hxx>
#include <GeomFill_Generator.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

//=======================================================================
//function : DetectKPart
//purpose  : 
//=======================================================================
Standard_Integer DetectKPart(const TopoDS_Edge& Edge1,
			     const TopoDS_Edge& Edge2)
{
  // initializations
  Standard_Integer IType = 0;

  // characteristics of the first edge
  Standard_Real first1 = 0., last1 = 0., first2, last2, ff, ll;
  TopLoc_Location loc;
  TopoDS_Vertex V1, V2;
  Handle(Geom_Curve) curv1, curv;
  GeomAdaptor_Curve AdC1;
  Standard_Boolean degen1 = BRep_Tool::Degenerated(Edge1);

  // find the particular case
  gp_Pnt pos1, pos;
  Standard_Real  dist;
  Standard_Real dist1 =0.;
  gp_Ax1 axe1, axe;

  if (degen1) {
    IType = -2;
    TopExp::Vertices(Edge1,V1,V2);
    pos1 = BRep_Tool::Pnt(V1);
  }
  else {
    curv1 = BRep_Tool::Curve(Edge1, loc, first1, last1);
    if (curv1.IsNull())
      throw Standard_NullObject("Null 3D curve in edge");
    curv1 = 
      Handle(Geom_Curve)::DownCast(curv1->Transformed(loc.Transformation()));
    ff = first1;
    ll = last1;
    if (Edge1.Orientation() == TopAbs_REVERSED) {
      curv1->Reverse();
      first1 = curv1->ReversedParameter(ll);
      last1 = curv1->ReversedParameter(ff);
    }
    AdC1.Load(curv1);
    if (AdC1.GetType() == GeomAbs_Circle) {
      // first circular section 
      IType = 1;
      pos1 = AdC1.Circle().Location();
      dist1 = AdC1.Circle().Radius();
      axe1 = AdC1.Circle().Axis();
    }
    else if (AdC1.GetType() == GeomAbs_Line) {
      // first straight line section 
      IType = 4;
      pos1 = AdC1.Line().Location();
      dist1 = AdC1.Value(first1).Distance(AdC1.Value(last1));
      gp_Vec vec(AdC1.Value(first1),AdC1.Value(last1));
      gp_Dir dir(vec);
      axe1 = gp_Ax1(AdC1.Value(first1),dir);
    }
    else {
      // first section of any type
      IType = 0;
    }
  }

  if (IType!=0) {

    Standard_Boolean degen2 = BRep_Tool::Degenerated(Edge2);
    if (degen2) {
      TopExp::Vertices(Edge2,V1,V2);
      pos = BRep_Tool::Pnt(V1);
      if (IType==1) {
	// the only particular case with degenerated edge at end : the cone
	if (pos.IsEqual(pos1,Precision::Confusion())) {
	  // the top is mixed with the center of the circle
	  IType = 0;
	}
	else {
	  gp_Vec vec(pos1,pos);
	  gp_Dir dir(vec);
	  axe = gp_Ax1(pos1,dir);
	  if (axe.IsParallel(axe1,Precision::Angular())) {
	    // the top is on the axis of the circle
	    IType = 2;
	  }
	  else {
	    // incorrect top --> no particular case
	    IType = 0;
	  }
	}
      }
      else if (IType != 4) { //not a plane
	// no particular case
	IType = 0;
      }
    }
    else {
      curv = BRep_Tool::Curve(Edge2, loc, first2, last2);
      if (curv.IsNull())
        throw Standard_NullObject("Null 3D curve in edge");
      curv = 
	Handle(Geom_Curve)::DownCast(curv->Transformed(loc.Transformation()));
      ff = first2;
      ll = last2;
      if (Edge2.Orientation() == TopAbs_REVERSED) {
	curv->Reverse();
	first2 = curv->ReversedParameter(ll);
	last2 = curv->ReversedParameter(ff);
      }
      GeomAdaptor_Curve AdC(curv);
    
      if (IType>0 && IType<4) {
	if (AdC.GetType() != GeomAbs_Circle) {
	  // section not circular --> no particular case
	  IType = 0;
	}
	else {
	  if (AdC.Circle().Axis()
	      .IsCoaxial(axe1,Precision::Angular(),Precision::Confusion())) {
	    // same axis
	    if (Abs(AdC.Circle().Radius()-dist1)< Precision::Confusion()) {
	      // possibility of cylinder or a piece of cylinder
	      Standard_Real h1 = Abs(last1-first1), h2 = Abs(last2-first2);
	      Standard_Boolean Same, 
	       SameParametricLength = ( Abs(h1-h2) < Precision::PConfusion() );
	      Standard_Real m1=(first1+last1)/2., m2=(first2+last2)/2.;
	      gp_Pnt P1,P2;
	      gp_Vec DU;
	      AdC1.D1(m1,P1,DU);
	      AdC.D0(m2,P2);
	      Same = SameParametricLength 
		&& ( gp_Vec(P1,P2).IsNormal(DU,Precision::Angular()) ) ;
	      if (Same) {
		// cylinder or piece of cylinder
		IType = 1;
	      }
	      else {
		// the interval of definition is not correct
		IType = 0;
	      }
	    }
	    else {
	      // possibility of cone truncation
	      Standard_Real h1 = Abs(last1-first1), h2 = Abs(last2-first2);
	      Standard_Boolean Same, 
	       SameParametricLength = ( Abs(h1-h2) < Precision::PConfusion() );
	      Standard_Real m1=(first1+last1)/2., m2=(first2+last2)/2.;
	      gp_Pnt P1,P2;
	      gp_Vec DU;
	      AdC1.D1(m1,P1,DU);
	      AdC.D0(m2,P2);
	      Same = SameParametricLength 
		&& ( gp_Vec(P1,P2).IsNormal(DU,Precision::Angular()) ) ;
	      if (Same) {
		// truncation of cone
		IType = 2;
	      }
	      else {
		// the interval of definition is not correct
		IType = 0;
	      }
	    }
	    if (AdC.Circle().Location().IsEqual(pos1,Precision::Confusion())) {
	      // the centers are mixed
	      IType = 0;
	    }
	  }
	  else {
	    // different axis
	    if (AdC.Circle().Radius()==dist1) {
	      // torus ?
	      IType = 3;
	    }
	    else {
	      // different radius --> no particular case
	      IType = 0;
	    }
	  }
	}
      }
      else if (IType>=4) {
	if (AdC.GetType() != GeomAbs_Line) {
	  // not a straight line section --> no particular case
	  IType = 0;
	}
	else {
	  pos = AdC.Line().Location();
	  dist = AdC.Value(first2).Distance(AdC.Value(last2));
	  gp_Vec vec(AdC.Value(first2),AdC.Value(last2));
	  gp_Dir aDir(vec);
	  axe = gp_Ax1(AdC.Value(first2), aDir);
	  if (axe.IsParallel(axe1,Precision::Angular())) {
	    // parallel straight line
	    if (Abs(dist-dist1)<Precision::Confusion()) {
	      gp_Dir dir(gp_Vec(AdC1.Value(first1),AdC.Value(first2)));
	      if (dir.IsNormal(gp_Dir(vec),Precision::Angular())) {
		// plane
		IType = 4;
	      }
	      else {
		// extrusion ?
		IType = 5;
	      }
	    }
	    else {
	      // different length --> no particular case
	      IType = 0;
	    }
	  }
	  else {
	    // not parallel straight line --> no particular case
	    IType = 0;
	  }
	}
      }
      else if (IType==-2) {
	if (AdC.GetType() == GeomAbs_Line)
	  IType = 4; //plane
	else if (AdC.GetType() == GeomAbs_Circle)
	  {
	    // the only particular case with degenerated edge at the beginning the cone
	    pos = AdC.Circle().Location();
	    axe = AdC.Circle().Axis();
	    if (pos1.IsEqual(pos,Precision::Confusion())) {
	      // the top is mixed with the center of the circle
	      IType = 0;
	    }
	    else {
	      gp_Vec vec(pos1,pos);
	      gp_Dir dir(vec);
	      axe1 = gp_Ax1(pos1,dir);
	      if (axe.IsParallel(axe1,Precision::Angular())) {
		// the top is on the axis of the circle
		IType = -2;
	      }
	      else {
		// incorrect top --> no particular case
		IType = 0;
	      }
	    }
	  }
	else
	  IType = 0;
      }
    }
    
  }
  // torus and extrusion are not particular cases.
  if (IType == 3 || IType == 5) IType = 0;
  return IType;
}


//=======================================================================
//function : CreateKPart
//purpose  : 
//=======================================================================

void CreateKPart(const TopoDS_Edge& Edge1,const TopoDS_Edge& Edge2,
		 const Standard_Integer IType, 
		 Handle(Geom_Surface)& Surf)
{
  // find the dimension
  TopoDS_Vertex V1, V2;

  TopLoc_Location loc;
  Standard_Real a1, b1, aa =0., bb =0.;
  TopoDS_Vertex v1f,v1l,v2f,v2l;

  // find characteristics of the first edge
  Handle(Geom_Curve) C1;
  Standard_Boolean degen1 = BRep_Tool::Degenerated(Edge1);
  if(degen1) {
    // cone with degenerated edge at the top
    TopExp::Vertices(Edge1,v1f,v1l);
  }
  else {
    C1 = BRep_Tool::Curve(Edge1, loc, a1, b1);
    if (C1.IsNull())
      throw Standard_NullObject("Null 3D curve in edge");
    C1 = Handle(Geom_Curve)::DownCast(C1->Transformed(loc.Transformation()));
    aa = a1;
    bb = b1;
    if (Edge1.Orientation() == TopAbs_REVERSED) {
      C1->Reverse();
      aa = C1->ReversedParameter(b1);
      bb = C1->ReversedParameter(a1);
      TopExp::Vertices(Edge1,v1l,v1f);
    }
    else {
      TopExp::Vertices(Edge1,v1f,v1l);
    }
  }

  // find characteristics of the second edge
  Handle(Geom_Curve) C2;
  Standard_Boolean degen2 = BRep_Tool::Degenerated(Edge2);
  if(degen2) {
    // cone with degenerated edge at the top
    TopExp::Vertices(Edge2,v2f,v2l);
  }
  else {
    C2 = BRep_Tool::Curve(Edge2, loc, a1, b1);
    if (C2.IsNull())
      throw Standard_NullObject("Null 3D curve in edge");
    C2 = Handle(Geom_Curve)::DownCast(C2->Transformed(loc.Transformation()));
    if (Edge2.Orientation() == TopAbs_REVERSED) {
      C2->Reverse();
      if (degen1) {
	aa = a1;
	bb = b1;
      }
      TopExp::Vertices(Edge2,v2l,v2f);
    }
    else {
      if (degen1) {
	aa = a1; //C2->ReversedParameter(b1);
	bb = b1; //C2->ReversedParameter(a1);
      }
      TopExp::Vertices(Edge2,v2f,v2l);
    }
  }

  // create the new surface
  TopoDS_Face face;
  TopoDS_Wire W;
  TopoDS_Edge edge1, edge2, edge3, edge4, couture;

  TopoDS_Wire newW1, newW2;
  BRep_Builder BW1, BW2;
  BW1.MakeWire(newW1);
  BW2.MakeWire(newW2);

  GeomAdaptor_Curve aC1Adaptor;
  if (!C1.IsNull())
    aC1Adaptor.Load(C1);
  GeomAdaptor_Curve aC2Adaptor;
  if (!C2.IsNull())
    aC2Adaptor.Load(C2);

  // calculate the surface
  Handle(Geom_Surface) surface;
  Standard_Real V, Rad;
  if (IType==1) {
    // cylindrical surface
    gp_Circ c1 = aC1Adaptor.Circle();
    gp_Circ c2 = aC2Adaptor.Circle();
    gp_Ax3 Ac1 = c1.Position();
    V = gp_Vec( c1.Location(),c2.Location()).Dot(gp_Vec(Ac1.Direction()));
    if ( V < 0.) {
      Ac1.ZReverse();
      V = -V;
    }
    Handle(Geom_CylindricalSurface) Cyl = 
      new Geom_CylindricalSurface( Ac1, c1.Radius());
    surface = new Geom_RectangularTrimmedSurface
      ( Cyl, aa, bb, Min(0.,V), Max(0.,V) );
  }
  else if (IType==2) {
    // conical surface
    gp_Circ k1 = aC1Adaptor.Circle();
    gp_Ax3 Ak1 = k1.Position();
    if (degen2) {
      V = gp_Vec( k1.Location(),BRep_Tool::Pnt(v2f))
	.Dot(gp_Vec(Ak1.Direction()));
      Rad = - k1.Radius();
    }
    else {
      gp_Circ k2 = aC2Adaptor.Circle();
      V = gp_Vec( k1.Location(),k2.Location()).Dot(gp_Vec(Ak1.Direction()));
      Rad = k2.Radius() - k1.Radius();
    }
      
    if ( V < 0.) {
      Ak1.ZReverse();
      V = -V;
    }
    Standard_Real Ang = ATan( Rad / V);
    Handle(Geom_ConicalSurface) Cone = 
      new Geom_ConicalSurface( Ak1, Ang, k1.Radius());
    V /= Cos(Ang);
    surface = new Geom_RectangularTrimmedSurface
      ( Cone, aa, bb, Min(0.,V), Max(0.,V) );
  }
  else if (IType==-2) {
    // conical surface with the top at the beginning (degen1 is true)
    gp_Circ k2 = aC2Adaptor.Circle();
    gp_Ax3 Ak2 = k2.Position();
    Ak2.SetLocation(BRep_Tool::Pnt(v1f));
    V = gp_Vec(BRep_Tool::Pnt(v1f),k2.Location())
	                        .Dot(gp_Vec(Ak2.Direction()));
    Rad = k2.Radius(); // - k2.Radius();      
    if ( V < 0.) {
      Ak2.ZReverse();
      V = -V;
    }
    Standard_Real Ang = ATan( Rad / V);
    Handle(Geom_ConicalSurface) Cone = 
      new Geom_ConicalSurface( Ak2, Ang, 0.);
    V /= Cos(Ang);
    surface = new Geom_RectangularTrimmedSurface
      ( Cone, aa, bb, Min(0.,V), Max(0.,V) );
  }
  else if (IType==3) {
    // torus surface ?
  }
  else if (IType==4) {
    // surface plane
    gp_Lin L1, L2, aLine;
    if (!degen1)
    {
      L1 = aC1Adaptor.Line();
      aLine = L1;
    }
    if (!degen2)
    {
      L2 = aC2Adaptor.Line();
      aLine = L2;
    }

    gp_Pnt P1 = (degen1)? BRep_Tool::Pnt(v1f) : L1.Location();
    gp_Pnt P2 = (degen2)? BRep_Tool::Pnt(v2f) : L2.Location();

    gp_Vec P1P2( P1, P2 ); 
    gp_Dir D1 = aLine.Direction();
    gp_Ax3 Ax( aLine.Location(), gp_Dir(D1.Crossed(P1P2)), D1 );
    Handle(Geom_Plane) Plan = new Geom_Plane(Ax);
    V = P1P2.Dot( Ax.YDirection());
    surface = Plan;
    //surface = new Geom_RectangularTrimmedSurface
      //( Plan, aa, bb, Min(0.,V), Max(0.,V) );
  }
  else if (IType==5) {
    // surface of extrusion ?
  }
  else {
    // IType incorrect
  }
  Surf = surface;
}


//=======================================================================
//function : BRepFill_Generator
//purpose  : 
//=======================================================================

BRepFill_Generator::BRepFill_Generator()
{
}


//=======================================================================
//function : AddWire
//purpose  : 
//=======================================================================

void BRepFill_Generator::AddWire(const TopoDS_Wire& Wire)
{
  myWires.Append( Wire);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFill_Generator::Perform()
{
  TopoDS_Shell Shell;
  TopoDS_Face  Face;
  TopoDS_Shape S1, S2;
  TopoDS_Edge  Edge1, Edge2, Edge3, Edge4, Couture;

  BRep_Builder B;
  B.MakeShell(myShell);

  Standard_Integer Nb = myWires.Length();

  BRepTools_WireExplorer ex1,ex2;

  Standard_Boolean wPoint1, wPoint2, uClosed = Standard_False, DegenFirst = Standard_False, DegenLast = Standard_False;
  
  for ( Standard_Integer i = 1; i <= Nb-1; i++) {

    TopoDS_Wire Wire1 = TopoDS::Wire(myWires( i ));
    TopoDS_Wire Wire2 = TopoDS::Wire(myWires(i+1));

    wPoint1 = Standard_False;
    if (i==1) {
      wPoint1 = Standard_True;
      for(ex1.Init(Wire1); ex1.More(); ex1.Next()) {
	wPoint1 = wPoint1 && (BRep_Tool::Degenerated(ex1.Current()));
      }
      DegenFirst = wPoint1;

      TopoDS_Vertex V1, V2;
      TopExp::Vertices(Wire1, V1, V2);
      uClosed = V1.IsSame(V2);
    }

    wPoint2 = Standard_False;
    if (i==Nb-1) {
      wPoint2 = Standard_True;
      for(ex2.Init(Wire2); ex2.More(); ex2.Next()) {
	wPoint2 = wPoint2 && (BRep_Tool::Degenerated(ex2.Current()));
      }
      DegenLast = wPoint2;
    }

    ex1.Init(Wire1);
    ex2.Init(Wire2);

    TopTools_DataMapOfShapeShape Map;
    
    Standard_Boolean tantque = ex1.More() && ex2.More();

    while ( tantque ) { 

      TopoDS_Vertex V1f,V1l,V2f,V2l, Vf_toMap, Vl_toMap;

      Standard_Boolean degen1 
	= BRep_Tool::Degenerated(TopoDS::Edge(ex1.Current()));
      Standard_Boolean degen2
	= BRep_Tool::Degenerated(TopoDS::Edge(ex2.Current()));

      if ( degen1 ) {
	TopoDS_Shape aLocalShape = ex1.Current().EmptyCopied();
	Edge1 = TopoDS::Edge(aLocalShape);
//	Edge1 = TopoDS::Edge(ex1.Current().EmptyCopied());
//	aLocalShape = ex1.Current();
//	TopExp::Vertices(TopoDS::Edge(aLocalShape),V1f,V1l);
	TopExp::Vertices(TopoDS::Edge(ex1.Current()),V1f,V1l);
	V1f.Orientation(TopAbs_FORWARD);
	B.Add(Edge1,V1f);
	V1l.Orientation(TopAbs_REVERSED);
	B.Add(Edge1,V1l);
	B.Range(Edge1,0,1);
      }
      else {
	TopoDS_Shape aLocalShape = ex1.Current();
	Edge1 = TopoDS::Edge(aLocalShape);
//	Edge1 = TopoDS::Edge(ex1.Current());
      }

      if ( degen2 ) {
	TopoDS_Shape aLocalShape = ex2.Current().EmptyCopied();
	Edge2 = TopoDS::Edge(aLocalShape);
//	Edge2 = TopoDS::Edge(ex2.Current().EmptyCopied());
	TopExp::Vertices(TopoDS::Edge(ex2.Current()),V2f,V2l);
	V2f.Orientation(TopAbs_FORWARD);
	B.Add(Edge2,V2f);
	V2l.Orientation(TopAbs_REVERSED);
	B.Add(Edge2,V2l);
	B.Range(Edge2,0,1);
      }
      else {
	Edge2 = TopoDS::Edge(ex2.Current());
      }

      Standard_Boolean Periodic = (BRep_Tool::IsClosed(Edge1) || degen1) &&
                                  (BRep_Tool::IsClosed(Edge2) || degen2);
      // ATTENTION : a non-punctual wire should not 
      //             contain a punctual edge
      if (!wPoint1) ex1.Next();
      if (!wPoint2) ex2.Next();

      // initialization of vertices
      Handle(Geom_Surface) Surf;
      Standard_Real f1=0, l1=1, f2=0, l2=1;
      if (Edge1.Orientation() == TopAbs_REVERSED)
	TopExp::Vertices(Edge1,V1l,V1f);
      else
	TopExp::Vertices(Edge1,V1f,V1l);
      if (Edge2.Orientation() == TopAbs_REVERSED)
	TopExp::Vertices(Edge2,V2l,V2f);
      else
	TopExp::Vertices(Edge2,V2f,V2l);
      if (degen1)
	{
	  Vf_toMap = V2f;
	  Vl_toMap = V2l;
	}
      else
	{
	  Vf_toMap = V1f;
	  Vl_toMap = V1l;
	}

      // processing of KPart
      Standard_Integer IType = DetectKPart(Edge1,Edge2);
      if (IType==0) {
	// no part cases
	TopLoc_Location L,L1,L2;

	Handle(Geom_Curve) C1, C2;
	TColgp_Array1OfPnt Extremities(1,2);
      
	if (degen1) {
	  Extremities(1) = BRep_Tool::Pnt(V1f);
	  Extremities(2) = BRep_Tool::Pnt(V1l);
	  C1 = new Geom_BezierCurve(Extremities);
	}
	else {
	  C1 = BRep_Tool::Curve(Edge1,L1,f1,l1);
          if (C1.IsNull())
            throw Standard_NullObject("Null 3D curve in edge");
	}
	if (degen2) {
	  Extremities(1) = BRep_Tool::Pnt(V2l);
	  Extremities(2) = BRep_Tool::Pnt(V2f);
	  C2 = new Geom_BezierCurve(Extremities);
	}
	else {
	  C2 = BRep_Tool::Curve(Edge2,L2,f2,l2);
          if (C2.IsNull())
            throw Standard_NullObject("Null 3D curve in edge");
	}
	
	// compute the location
	Standard_Boolean SameLoc = Standard_False;
	
	// transform and trim the curves
      
	if (Abs(f1 - C1->FirstParameter()) > Precision::PConfusion() ||
	    Abs(l1 - C1->LastParameter())  > Precision::PConfusion()   ) {
	  C1 = new Geom_TrimmedCurve(C1,f1,l1);
	}
	else {
	  C1 = Handle(Geom_Curve)::DownCast(C1->Copy());
	}
	if (!SameLoc) C1->Transform(L1.Transformation());
	if (Edge1.Orientation() == TopAbs_REVERSED) {
	  C1->Reverse();
	}
	
	if (Abs(f2 - C2->FirstParameter()) > Precision::PConfusion() ||
	    Abs(l2 - C2->LastParameter())  > Precision::PConfusion()   ) {
	  C2 = new Geom_TrimmedCurve(C2,f2,l2);
	}
	else {
	  C2 = Handle(Geom_Curve)::DownCast(C2->Copy());
	}
	if (!SameLoc) C2->Transform(L2.Transformation());
	if (Edge2.Orientation() == TopAbs_REVERSED) {
	  C2->Reverse();
	}
	
	GeomFill_Generator Generator;
	Generator.AddCurve( C1);
	Generator.AddCurve( C2);
	Generator.Perform( Precision::PConfusion());
	
	Surf = Generator.Surface();
	B.MakeFace(Face,Surf,Precision::Confusion());
      }
      else {
	// particular case
	CreateKPart(Edge1,Edge2,IType,Surf);
	B.MakeFace(Face,Surf,Precision::Confusion());
      }
      
      // make the missing edges
      Standard_Real first,last;
      Surf->Bounds(f1,l1,f2,l2);
      if (IType == 0)
      {
        first = f2;
        last = l2;
      }
      else
      {
        first = 0.;
        last = 1.;
      }

      if ( Map.IsBound(Vf_toMap)) {
	TopoDS_Shape aLocalShape = Map(Vf_toMap).Reversed();
	Edge3 = TopoDS::Edge(aLocalShape);
//	Edge3 = TopoDS::Edge(Map(V1f).Reversed());
      }
      else
      {
        if (V1f.IsSame(V2f))
        {
          B.MakeEdge(Edge3);
          B.Degenerated(Edge3, Standard_True);
        }
        else
        {
          Handle(Geom_Curve) CC;
          TColgp_Array1OfPnt Extremities(1, 2);
          if (IType == 0) {
            // general case : Edge3 corresponds to iso U=f1
            CC = Surf->UIso(f1);
          }
          else {
            // particular case : it is required to calculate the curve 3d
            Extremities(1) = BRep_Tool::Pnt(V1f);
            Extremities(2) = BRep_Tool::Pnt(V2f);
            CC = new Geom_BezierCurve(Extremities);
          }
          B.MakeEdge(Edge3, CC, Precision::Confusion());
        }
        V1f.Orientation(TopAbs_FORWARD);
        B.Add(Edge3, V1f);
        V2f.Orientation(TopAbs_REVERSED);
        B.Add(Edge3, V2f);
        B.Range(Edge3, first, last);
        Edge3.Reverse();
        Map.Bind(Vf_toMap, Edge3);
      }

      Standard_Boolean CommonEdge = Standard_False;
      if ( Map.IsBound(Vl_toMap)  ) {
	TopoDS_Shape aLocalShape = Map(Vl_toMap).Reversed();
	const TopoDS_Edge CommonE = TopoDS::Edge(aLocalShape);
//	const TopoDS_Edge CommonE = TopoDS::Edge(Map(V1l).Reversed());
	TopoDS_Vertex V1, V2;
	TopExp::Vertices(CommonE,V1,V2);
	CommonEdge = V1.IsSame(V1l) && V2.IsSame(V2l);
      }
      if ( CommonEdge ) {
	TopoDS_Shape aLocalShape = Map(Vl_toMap).Reversed();
	Edge4 = TopoDS::Edge(aLocalShape);
//	Edge4 = TopoDS::Edge(Map(V1l).Reversed());
      }
      else
      {
        if (V1l.IsSame(V2l))
        {
          B.MakeEdge(Edge4);
          B.Degenerated(Edge4, Standard_True);
        }
        else
        {
          Handle(Geom_Curve) CC;
          TColgp_Array1OfPnt Extremities(1, 2);
          if (IType == 0) {
            // general case : Edge4 corresponds to iso U=l1
            CC = Surf->UIso(l1);
          }
          else {
            // particular case : it is required to calculate the curve 3d
            Extremities(1) = BRep_Tool::Pnt(V1l);
            Extremities(2) = BRep_Tool::Pnt(V2l);
            CC = new Geom_BezierCurve(Extremities);
          }
          B.MakeEdge(Edge4, CC, Precision::Confusion());
        }
        V1l.Orientation(TopAbs_FORWARD);
        B.Add(Edge4, V1l);
        V2l.Orientation(TopAbs_REVERSED);
        B.Add(Edge4, V2l);
        B.Range(Edge4, first, last);
        Map.Bind(Vl_toMap, Edge4);
      }

      // make the wire
      
      TopoDS_Wire W;
      B.MakeWire(W);
      
      if (! (degen1 && IType == 4))
	B.Add(W,Edge1);
      B.Add(W,Edge4);
      if (! (degen2 && IType == 4))
	B.Add(W,Edge2.Reversed());
      B.Add(W,Edge3);
      
      B.Add(Face,W);
      
      B.Add(myShell,Face);

    // complete myMap for edge1
      if (! (degen1 && IType == 4))
	{
	  TopTools_ListOfShape Empty;
	  if (!myMap.IsBound(Edge1)) myMap.Bind(Edge1,Empty);
	  myMap(Edge1).Append(Face);
	}
      
      // set the pcurves
      
      Standard_Real T = Precision::Confusion();

      if (IType != 4) //not plane
	{
	  if ( Edge1.Orientation() == TopAbs_REVERSED ) {
	    B.UpdateEdge(Edge1,
			 new Geom2d_Line(gp_Pnt2d(0,f2),gp_Dir2d(-1,0)),
			 Face,T);
	    B.Range(Edge1,Face,-l1,-f1);
	  }
	  else {
	    B.UpdateEdge(Edge1,
			 new Geom2d_Line(gp_Pnt2d(0,f2),gp_Dir2d(1,0)),
			 Face,T);
	    B.Range(Edge1,Face,f1,l1);
	  }
	  
	  if ( Edge2.Orientation() == TopAbs_REVERSED ) {
	    B.UpdateEdge(Edge2,
			 new Geom2d_Line(gp_Pnt2d(0,l2),gp_Dir2d(-1,0)),
			 Face,T);
	    B.Range(Edge2,Face,-l1,-f1);
	  }
	  else {
	    B.UpdateEdge(Edge2,
			 new Geom2d_Line(gp_Pnt2d(0,l2),gp_Dir2d(1,0)),
			 Face,T);
	    B.Range(Edge2,Face,f1,l1);
	  }
	}

      if (IType==0) {
	if ( Periodic) {
	  B.UpdateEdge(Edge3,
		       new Geom2d_Line(gp_Pnt2d(l1,0),gp_Dir2d(0,1)),
		       new Geom2d_Line(gp_Pnt2d(f1,0),gp_Dir2d(0,1)),
		       Face,T);
	}
	else {
	  B.UpdateEdge(Edge3,
		       new Geom2d_Line(gp_Pnt2d(f1,0),gp_Dir2d(0,1)),
		       Face,T);
	  B.UpdateEdge(Edge4,
		       new Geom2d_Line(gp_Pnt2d(l1,0),gp_Dir2d(0,1)),
		       Face,T);
	}
      }
      else {
	// KPart
	if ( Periodic) {
	  TColgp_Array1OfPnt2d Extrem1(1,2);
	  Extrem1(1).SetCoord(l1,f2);
	  Extrem1(2).SetCoord(l1,l2);
	  TColgp_Array1OfPnt2d Extrem2(1,2);
	  Extrem2(1).SetCoord(f1,f2);
	  Extrem2(2).SetCoord(f1,l2);
	  B.UpdateEdge(Edge3,
		       new Geom2d_BezierCurve(Extrem1),
		       new Geom2d_BezierCurve(Extrem2),
		       Face,T);
	}
	else if (IType != 4) { //not plane
	  TColgp_Array1OfPnt2d Extrem2(1,2);
	  Extrem2(1).SetCoord(f1,f2);
	  Extrem2(2).SetCoord(f1,l2);
	  B.UpdateEdge(Edge3,
		       new Geom2d_BezierCurve(Extrem2),
		       Face,T);
	  TColgp_Array1OfPnt2d Extrem1(1,2);
	  Extrem1(1).SetCoord(l1,f2);
	  Extrem1(2).SetCoord(l1,l2);
	  B.UpdateEdge(Edge4,
		       new Geom2d_BezierCurve(Extrem1),
		       Face,T);
	}
      }
      // Set the non parameter flag;
      B.SameParameter(Edge1,Standard_False);
      B.SameParameter(Edge2,Standard_False);
      B.SameParameter(Edge3,Standard_False);
      B.SameParameter(Edge4,Standard_False);
      B.SameRange(Edge1,Standard_False);
      B.SameRange(Edge2,Standard_False);
      B.SameRange(Edge3,Standard_False);
      B.SameRange(Edge4,Standard_False);

      tantque = ex1.More() && ex2.More();
      if (wPoint1) tantque = ex2.More();
      if (wPoint2) tantque = ex1.More();
    }
  }
  BRepLib::SameParameter(myShell);

  if (uClosed && DegenFirst && DegenLast)
    myShell.Closed(Standard_True);
}


//=======================================================================
//function : GeneratedShapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& 
 BRepFill_Generator::GeneratedShapes (const TopoDS_Shape& SSection) const 
{
  if (myMap.IsBound(SSection)) {
    return myMap(SSection);
  }
  else {
    static TopTools_ListOfShape Empty;
    return Empty;
  }
}

//=======================================================================
//function : Generated
//purpose  : 
//=================================================================== ====

const TopTools_DataMapOfShapeListOfShape& BRepFill_Generator::Generated() const
{
  return myMap;
}


