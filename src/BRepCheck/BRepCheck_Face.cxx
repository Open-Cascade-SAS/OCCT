// Created on: 1995-12-15
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Face.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <GProp_GProps.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <NCollection_DataMap.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopAbs_State.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_OrientedShapeMapHasher.hxx>
#include <TopTools_SequenceOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepCheck_Face,BRepCheck_Result)

//#include <BRepAdaptor_Curve2d.hxx>
//#include <Geom2dInt_GInter.hxx>
typedef NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_OrientedShapeMapHasher> DataMapOfShapeBox2d;

static Standard_Boolean Intersect(const TopoDS_Wire&,
				  const TopoDS_Wire&,
				  const TopoDS_Face&,
				  const DataMapOfShapeBox2d&);
				  

static Standard_Boolean IsInside(const TopoDS_Wire& wir,
				 const Standard_Boolean Inside,
				 const BRepTopAdaptor_FClass2d& FClass2d,
				 const TopoDS_Face& F);

static Standard_Boolean CheckThin(const TopoDS_Shape& w,
				  const TopoDS_Shape& f);

//=======================================================================
//function : BRepCheck_Face
//purpose  : 
//=======================================================================

BRepCheck_Face::BRepCheck_Face (const TopoDS_Face& F)
{
  Init(F);
  myIntdone = Standard_False;
  myImbdone = Standard_False;
  myOridone = Standard_False;
  myGctrl   = Standard_True;
}

//=======================================================================
//function : Minimum
//purpose  : 
//=======================================================================

void BRepCheck_Face::Minimum()
{
  if (!myMin) {
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(myShape, thelist);
    BRepCheck_ListOfStatus& lst = myMap(myShape);

    Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*) &myShape.TShape());
    if (TF->Surface().IsNull()) {
      BRepCheck::Add(lst,BRepCheck_NoSurface);
    }
    else {
      // Flag natural restriction???
    }
    if (lst.IsEmpty()) {
      lst.Append(BRepCheck_NoError);
    }
    myMin = Standard_True;
  }
}


//=======================================================================
//function : InContext
//purpose  : 
//=======================================================================

void BRepCheck_Face::InContext(const TopoDS_Shape& S)
{
  if (myMap.IsBound(S)) {
    return;
  }
  BRepCheck_ListOfStatus thelist;
  myMap.Bind(S, thelist);

  BRepCheck_ListOfStatus& lst = myMap(S);
  
  TopExp_Explorer exp(S,TopAbs_FACE);
  for (; exp.More(); exp.Next()) {
    if (exp.Current().IsSame(myShape)) {
      break;
    }
  }
  if (!exp.More()) {
    BRepCheck::Add(lst,BRepCheck_SubshapeNotInShape);
    return;
  }

  if (lst.IsEmpty()) {
    lst.Append(BRepCheck_NoError);
  }
}


//=======================================================================
//function : Blind
//purpose  : 
//=======================================================================

void BRepCheck_Face::Blind()
{
  if (!myBlind) {
    // nothing more than in the minimum
    myBlind = Standard_True;
  }
}


//=======================================================================
//function : IntersectWires
//purpose  : 
//=======================================================================

BRepCheck_Status BRepCheck_Face::IntersectWires(const Standard_Boolean Update)
{
  if (myIntdone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myIntres);
    }
    return myIntres;
  }

  myIntdone = Standard_True;
  myIntres = BRepCheck_NoError;
  // This method has to be called by an analyzer. It is assumed that
  // each edge has a correct 2d representation on the face.

  TopExp_Explorer exp1,exp2;

  // the wires are mapped
  exp1.Init(myShape.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
  TopTools_ListOfShape theListOfShape;
  while (exp1.More()) {
    if (!myMapImb.IsBound(exp1.Current())) {
      myMapImb.Bind(exp1.Current(), theListOfShape);
    }
    else { // the same wire is met twice...
      myIntres = BRepCheck_RedundantWire;
      if (Update) {
	BRepCheck::Add(myMap(myShape),myIntres);
      }
      return myIntres;
    }
    exp1.Next();
  }

  Geom2dAdaptor_Curve aC;
  Standard_Real aFirst, aLast;
  DataMapOfShapeBox2d aMapShapeBox2d;
  for (exp1.Init (myShape, TopAbs_WIRE); exp1.More(); exp1.Next()) 
  {
    const TopoDS_Wire& aWire = TopoDS::Wire (exp1.Current());
    // create 2d boxes for all edges from wire
    Bnd_Box2d aBoxW;
    for (exp2.Init (aWire, TopAbs_EDGE); exp2.More(); exp2.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (exp2.Current());
      aC.Load (BRep_Tool::CurveOnSurface (anEdge, TopoDS::Face (myShape), aFirst, aLast));
      // To avoid exeption in Segment if C1 is BSpline
      if (aC.FirstParameter() > aFirst)
      {
        aFirst = aC.FirstParameter();
      }
      if (aC.LastParameter() < aLast)
      {
        aLast = aC.LastParameter();
      }
      Bnd_Box2d aBoxE;
      BndLib_Add2dCurve::Add (aC, aFirst, aLast, 0., aBoxE);
      aMapShapeBox2d.Bind (anEdge, aBoxE);
      aBoxW.Add (aBoxE);
    }
    aMapShapeBox2d.Bind (aWire, aBoxW);
  }

  Standard_Integer Nbwire, Index,Indexbis;
  Nbwire = myMapImb.Extent();

  Index = 1;
  while (Index < Nbwire) {
    for (exp1.Init(myShape,TopAbs_WIRE),Indexbis = 0;
	 exp1.More();exp1.Next()) {
      Indexbis++;
      if (Indexbis == Index) {
	break;
      }
    }
    TopoDS_Wire wir1 = TopoDS::Wire(exp1.Current());
    // to reduce the number of calls Intersect(wir1,wir2)
    Bnd_Box2d aBox1, aBox2;
    if (aMapShapeBox2d.IsBound (wir1))
    {
      aBox1 = aMapShapeBox2d (wir1);
    }
    exp1.Next();
    for (; exp1.More(); exp1.Next()) {
      const TopoDS_Wire& wir2 = TopoDS::Wire(exp1.Current());
      if (aMapShapeBox2d.IsBound (wir2))
      {
        aBox2 = aMapShapeBox2d (wir2);
      }
      if (!aBox1.IsVoid() && !aBox2.IsVoid() && aBox1.IsOut (aBox2))
      {
        continue;
      }
      if (Intersect(wir1,wir2,TopoDS::Face(myShape), aMapShapeBox2d)) {
	myIntres = BRepCheck_IntersectingWires;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myIntres);
	}
	return myIntres;
      }
    }
    Index++;
  }
  if (Update) {
    BRepCheck::Add(myMap(myShape),myIntres);
  }
  return myIntres;
}


//=======================================================================
//function : ClassifyWires
//purpose  : 
//=======================================================================

BRepCheck_Status BRepCheck_Face::ClassifyWires(const Standard_Boolean Update)
{
  // It is assumed that each wire does not intersect any other one.
  if (myImbdone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myImbres);
    }
    return myImbres;
  }

  myImbdone = Standard_True;
  myImbres = IntersectWires();
  if (myImbres != BRepCheck_NoError) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myImbres);
    }
    return myImbres;
  }

  Standard_Integer Nbwire = myMapImb.Extent();
  if (Nbwire < 1) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myImbres);
    }
    return myImbres;
  }

  BRep_Builder B;
  TopExp_Explorer exp1,exp2;
  TopTools_ListOfShape theListOfShape;
  for (exp1.Init(myShape.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
       exp1.More();exp1.Next()) {

    const TopoDS_Wire& wir1 = TopoDS::Wire(exp1.Current());
    TopoDS_Shape aLocalShape = myShape.EmptyCopied();
    TopoDS_Face newFace = TopoDS::Face(aLocalShape);
//    TopoDS_Face newFace = TopoDS::Face(myShape.EmptyCopied());

    newFace.Orientation(TopAbs_FORWARD);
    B.Add(newFace,wir1);

    BRepTopAdaptor_FClass2d FClass2d(newFace,Precision::PConfusion());
    Standard_Boolean WireBienOriente = Standard_False;
    if(FClass2d.PerformInfinitePoint() != TopAbs_OUT) { 
      WireBienOriente=Standard_True;
      // the given wire defines a hole
      myMapImb.UnBind(wir1);
      myMapImb.Bind(wir1.Reversed(), theListOfShape);
    }

    for (exp2.Init(myShape.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
	 exp2.More();exp2.Next()) {
      const TopoDS_Wire& wir2 = TopoDS::Wire(exp2.Current());
      if (!wir2.IsSame(wir1)) {
	
	if (IsInside(wir2,WireBienOriente,FClass2d,newFace)) { 
	  myMapImb(wir1).Append(wir2);
	}
      }
    }
  }
  // It is required to have 1 wire that contains all others, and the others should not  
  // contain anything (case solid ended) or
  // the wires do not contain anything : in this case the wires should be
  // holes in an infinite face.
  TopoDS_Wire Wext;
  for (TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(myMapImb);
       itm.More();
       itm.Next()) {
    if (!itm.Value().IsEmpty()) {
      if (Wext.IsNull()) {
	Wext = TopoDS::Wire(itm.Key());
      }
      else {
	myImbres = BRepCheck_InvalidImbricationOfWires;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myImbres);
	}
	return myImbres;
      }
    }
  }

  if (!Wext.IsNull()) {
    // verifies that the list contains nbwire-1 elements
    if (myMapImb(Wext).Extent() != Nbwire-1) {
      myImbres = BRepCheck_InvalidImbricationOfWires;
      if (Update) {
	BRepCheck::Add(myMap(myShape),myImbres);
      }
      return myImbres;
    }
  }
  // quit without errors
  if (Update) {
    BRepCheck::Add(myMap(myShape),myImbres);
  }
  return myImbres;
  
}


//=======================================================================
//function : OrientationOfWires
//purpose  : 
//=======================================================================

BRepCheck_Status BRepCheck_Face::OrientationOfWires
   (const Standard_Boolean Update)
{
  // WARNING : it is assumed that the edges of a wire are correctly oriented


  Standard_Boolean Infinite = myShape.Infinite();

  if (myOridone) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myOrires);
    }
    return myOrires;
  }

  myOridone = Standard_True;
  myOrires = ClassifyWires();
  if (myOrires != BRepCheck_NoError) {
    if (Update) {
      BRepCheck::Add(myMap(myShape),myOrires);
    }
    return myOrires;
  }

  Standard_Integer Nbwire = myMapImb.Extent();
  TopoDS_Wire Wext;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(myMapImb);
  if (Nbwire == 1) {
    if (!Infinite) {
      Wext = TopoDS::Wire(itm.Key());
    }
  }
  else {
    for (;itm.More();itm.Next()) {
      if (!itm.Value().IsEmpty()) {
	Wext = TopoDS::Wire(itm.Key());
      }
    }
  }

  if (Wext.IsNull() && !Infinite) {
    if (Nbwire>0) myOrires = BRepCheck_InvalidImbricationOfWires;
    if (Update) {
      BRepCheck::Add(myMap(myShape),myOrires);
    }
    return myOrires;
  }

  // BRep_Builder B;
  TopExp_Explorer exp(myShape.Oriented(TopAbs_FORWARD),TopAbs_WIRE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Wire& wir = TopoDS::Wire(exp.Current());
    if (!Wext.IsNull() && wir.IsSame(Wext)) {
      if (wir.Orientation() != Wext.Orientation()) {
	//the exterior wire defines a hole 
	if( CheckThin(wir,myShape.Oriented(TopAbs_FORWARD)) )
	  return myOrires;
	myOrires = BRepCheck_BadOrientationOfSubshape;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myOrires);
	}
	return myOrires;
      }
    }
    else {
      for (itm.Reset(); itm.More(); itm.Next()) {
	if (itm.Key().IsSame(wir)) {
	  break;
	}
      }
      // No control on More()
      if (itm.Key().Orientation() == wir.Orientation()) {
	// the given wire does not define a hole
	myOrires = BRepCheck_BadOrientationOfSubshape;
	if (Update) {
	  BRepCheck::Add(myMap(myShape),myOrires);
	}
	return myOrires;
      }
    }
  }
  // quit withour error
  if (Update) {
    BRepCheck::Add(myMap(myShape),myOrires);
  }
  return myOrires;
}


//=======================================================================
//function : SetUnorientable
//purpose  : 
//=======================================================================

void BRepCheck_Face::SetUnorientable()
{
  BRepCheck::Add(myMap(myShape),BRepCheck_UnorientableShape);
}

//=======================================================================
//function :   SetStatus
//purpose  : 
//=======================================================================

void BRepCheck_Face::SetStatus(const BRepCheck_Status theStatus)
{
    BRepCheck::Add(myMap(myShape),theStatus);
}

//=======================================================================
//function : IsUnorientable
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Face::IsUnorientable() const
{
  if (myOridone) {
    return (myOrires != BRepCheck_NoError);
  }
  for (BRepCheck_ListIteratorOfListOfStatus itl(myMap(myShape));
       itl.More();
       itl.Next()) {
    if (itl.Value() == BRepCheck_UnorientableShape) {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================

void BRepCheck_Face::GeometricControls(const Standard_Boolean B)
{
  if (myGctrl != B) {
    if (B) {
      myIntdone = Standard_False; 
      myImbdone = Standard_False; 
      myOridone = Standard_False; 
    }
    myGctrl = B;
  }
}


//=======================================================================
//function : GeometricControls
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Face::GeometricControls() const
{
  return myGctrl;
}


//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

static Standard_Boolean Intersect(const TopoDS_Wire& wir1,
				  const TopoDS_Wire& wir2,
				  const TopoDS_Face& F,
				  const DataMapOfShapeBox2d& theMapEdgeBox)
{
  Standard_Real Inter2dTol = 1.e-10;
  TopExp_Explorer exp1,exp2;
//  BRepAdaptor_Curve2d cur1,cur2;

  //Find common vertices of two wires - non-manifold case
  TopTools_MapOfShape MapW1;
  TopTools_SequenceOfShape CommonVertices;
  for (exp1.Init( wir1, TopAbs_VERTEX ); exp1.More(); exp1.Next())
    MapW1.Add( exp1.Current() );
  for (exp2.Init( wir2, TopAbs_VERTEX ); exp2.More(); exp2.Next())
    {
      TopoDS_Shape V = exp2.Current();
      if (MapW1.Contains( V ))
	CommonVertices.Append( V );
    }

  // MSV 03.04.2002: create pure surface adaptor to avoid UVBounds computation
  //                 due to performance problem
  BRepAdaptor_Surface Surf(F,Standard_False);

  TColgp_SequenceOfPnt PntSeq;
  Standard_Integer i;
  for (i = 1; i <= CommonVertices.Length(); i++)
    {
      TopoDS_Vertex V = TopoDS::Vertex( CommonVertices(i) );
      gp_Pnt2d P2d = BRep_Tool::Parameters( V, F );
      gp_Pnt P = Surf.Value( P2d.X(), P2d.Y() );
      PntSeq.Append( P );
    }

  Geom2dAdaptor_Curve   C1,C2;
  gp_Pnt2d              pfirst1,plast1,pfirst2,plast2;
  Standard_Real         first1,last1,first2,last2;
  Geom2dInt_GInter      Inter;
  IntRes2d_Domain myDomain1,myDomain2;
  Bnd_Box2d Box1, Box2;

  for (exp1.Init(wir1,TopAbs_EDGE); exp1.More(); exp1.Next())
    {
      const TopoDS_Edge& edg1 = TopoDS::Edge(exp1.Current());
      //    cur1.Initialize(edg1,F);
      C1.Load( BRep_Tool::CurveOnSurface(edg1,F,first1,last1) );
      // To avoid exeption in Segment if C1 is BSpline - IFV
      if(C1.FirstParameter() > first1) first1 = C1.FirstParameter();
      if(C1.LastParameter()  < last1 ) last1  = C1.LastParameter();

      Box1.SetVoid();
      if (theMapEdgeBox.IsBound (edg1))
      {
        Box1 = theMapEdgeBox (edg1);
      }
      if (Box1.IsVoid())
      {
        BndLib_Add2dCurve::Add( C1, first1, last1, 0., Box1 );
      }
      for (exp2.Init(wir2,TopAbs_EDGE); exp2.More(); exp2.Next())
	{
	  const TopoDS_Edge& edg2 = TopoDS::Edge(exp2.Current());
	  if (!edg1.IsSame(edg2))
	    {
	      //cur2.Initialize(edg2,F);
	      C2.Load( BRep_Tool::CurveOnSurface(edg2,F,first2,last2) );
	      // To avoid exeption in Segment if C2 is BSpline - IFV
	      if(C2.FirstParameter() > first2) first2 = C2.FirstParameter();
	      if(C2.LastParameter()  < last2 ) last2  = C2.LastParameter();

	      Box2.SetVoid();
	      if (theMapEdgeBox.IsBound (edg2))
	      {
          Box2 = theMapEdgeBox (edg2);
	      }
	      if (Box2.IsVoid())
	      {
          BndLib_Add2dCurve::Add( C2, first2, last2, 0., Box2 );
	      }
	      if (! Box1.IsOut( Box2 ))
		{
		  BRep_Tool::UVPoints(edg1,F,pfirst1,plast1);
		  myDomain1.SetValues( pfirst1, first1, Inter2dTol, plast1, last1, Inter2dTol );
		  BRep_Tool::UVPoints(edg2,F,pfirst2,plast2);
		  myDomain2.SetValues( pfirst2, first2, Inter2dTol, plast2, last2, Inter2dTol );
		  Inter.Perform( C1, myDomain1, C2, myDomain2, Inter2dTol, Inter2dTol );
		  if (!Inter.IsDone())
		    return Standard_True;
		  if (Inter.NbSegments() > 0)
		    {
		      if (PntSeq.IsEmpty())
			return Standard_True;
		      else
			{
			  Standard_Integer NbCoinc = 0;
			  for (i = 1; i <= Inter.NbSegments(); i++)
			    {
			      if (!Inter.Segment(i).HasFirstPoint() || !Inter.Segment(i).HasLastPoint())
				return Standard_True;
			      gp_Pnt2d FirstP2d = Inter.Segment(i).FirstPoint().Value();
			      gp_Pnt2d LastP2d = Inter.Segment(i).LastPoint().Value();
			      gp_Pnt FirstP = Surf.Value( FirstP2d.X(), FirstP2d.Y() );
			      gp_Pnt LastP = Surf.Value( LastP2d.X(), LastP2d.Y() );
			      for (Standard_Integer j = 1; j <= PntSeq.Length(); j++)
				{
				  Standard_Real tolv = BRep_Tool::Tolerance( TopoDS::Vertex(CommonVertices(j)) );
				  if (FirstP.IsEqual( PntSeq(j), tolv ) || LastP.IsEqual( PntSeq(j), tolv ))
				    {
				      NbCoinc++;
				      break;
				    }
				}
			    }
			  if (NbCoinc == Inter.NbSegments())
			    return Standard_False;
			  return Standard_True;
			}
		    }
		  if (Inter.NbPoints() > 0)
		    {
		      if (PntSeq.IsEmpty())
			return Standard_True;
		      else
			{
			  Standard_Integer NbCoinc = 0;
			  for (i = 1; i <= Inter.NbPoints(); i++)
			    {
			      gp_Pnt2d P2d = Inter.Point(i).Value();
			      gp_Pnt P = Surf.Value( P2d.X(), P2d.Y() );
			      for (Standard_Integer j = 1; j <= PntSeq.Length(); j++)
				{
				  Standard_Real tolv = BRep_Tool::Tolerance( TopoDS::Vertex(CommonVertices(j)) );
				  if (P.IsEqual( PntSeq(j), tolv ))
				    {
				      NbCoinc++;
				      break;
				    }
				}
			    }
			  if (NbCoinc == Inter.NbPoints())
			    return Standard_False;
			  return Standard_True;
			}
		    }
		}
	    }
	}
    }
  return Standard_False;
}


//=======================================================================
//function : IsInside
//purpose  : 
//=======================================================================

static Standard_Boolean IsInside(const TopoDS_Wire& theWire,
				 const Standard_Boolean WireBienOriente,
				 const BRepTopAdaptor_FClass2d& FClass2d,
				 const TopoDS_Face& theFace)
{
  Standard_Real aParameter, aFirst, aLast;

  TopExp_Explorer anExplorer(theWire, TopAbs_EDGE);
  for( ; anExplorer.More(); anExplorer.Next() )
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge( anExplorer.Current() );
    Handle(Geom2d_Curve) aCurve2D =
      BRep_Tool::CurveOnSurface( anEdge, theFace, aFirst, aLast );

    // Selects the parameter of point on the curve
    if( !Precision::IsNegativeInfinite(aFirst) &&
        !Precision::IsPositiveInfinite(aLast) )
    {
      aParameter = (aFirst + aLast) * 0.5;

      // Edge is skipped if its parametric range is too small
      if( Abs(aParameter - aFirst) < Precision::PConfusion() )
      {
        continue;
      }

	  //Edge is skipped if its length is too small
	  Standard_Real aFirst3D, aLast3D;
	  Handle(Geom_Curve) aCurve = BRep_Tool::Curve( anEdge, aFirst3D, aLast3D );      
      if ( aCurve.IsNull() )
      {
        continue;
      }

      gp_Pnt aPoints[2];
      // Compute start point of edge
      aCurve->D0( aFirst, aPoints[0] );
      // Compute middle point of edge 
      aCurve->D0( (aFirst3D+aLast3D)/2., aPoints[1] );
      if( aPoints[0].Distance(aPoints[1]) < Precision::Confusion() )
      {
        continue;
      }
    }
    else
    {
      if( Precision::IsNegativeInfinite(aFirst) &&
          Precision::IsPositiveInfinite(aLast) )
      {
        aParameter = 0.;
      }
      else if( Precision::IsNegativeInfinite(aFirst) )
      {
        aParameter = aLast - 1.;
      }
      else
      {
        aParameter = aFirst + 1.;
      }
    }

    // Find point on curve (edge)
    gp_Pnt2d aPoint2D(aCurve2D->Value(aParameter));
    // Compute the topological position of a point relative to face
    TopAbs_State aState = FClass2d.Perform(aPoint2D, Standard_False);

    if( WireBienOriente )
    {
      return aState == TopAbs_OUT;
    }
    else
    {
      return aState == TopAbs_IN;
    }
  }
  return Standard_False;
}
Standard_Boolean CheckThin(const TopoDS_Shape& w, const TopoDS_Shape& f)
{
  TopoDS_Face aF = TopoDS::Face(f);
  TopoDS_Wire aW = TopoDS::Wire(w);

  Standard_Integer nbE = 0;
  TopTools_ListOfShape lE;
  TopExp_Explorer exp(aW,TopAbs_EDGE);
  for(; exp.More(); exp.Next()) {
    const TopoDS_Shape& s = exp.Current();
    lE.Append(s);
    nbE++;
  }

  if( nbE != 2 ) return Standard_False;
  TopoDS_Edge e1 = TopoDS::Edge(lE.First());
  TopoDS_Edge e2 = TopoDS::Edge(lE.Last());

  TopoDS_Vertex v1, v2, v3, v4;
  TopExp::Vertices(e1,v1,v2);
  TopExp::Vertices(e2,v3,v4);

  if( v1.IsNull() || v2.IsNull() || 
      v3.IsNull() || v4.IsNull() ) return Standard_False;

  if( v1.IsSame(v2) || v3.IsSame(v4) )
    return Standard_False;

  Standard_Boolean sF = Standard_False, sL = Standard_False;
  if( v1.IsSame(v3) || v1.IsSame(v4) ) sF = Standard_True;
  if( v2.IsSame(v3) || v2.IsSame(v4) ) sL = Standard_True;

  if( !sF || !sL ) return Standard_False;

  TopAbs_Orientation e1or = e1.Orientation();
  TopAbs_Orientation e2or = e2.Orientation();
 
  Standard_Real f1 = 0., l1 = 0., f2 = 0., l2 = 0.;
  Handle(Geom2d_Curve) pc1 = BRep_Tool::CurveOnSurface(e1,aF,f1,l1);
  Handle(Geom2d_Curve) pc2 = BRep_Tool::CurveOnSurface(e2,aF,f2,l2);
  
  if( pc1.IsNull() || pc2.IsNull() ) return Standard_False;

  Standard_Real d1 = Abs(l1-f1)/100.;
  Standard_Real d2 = Abs(l2-f2)/100.;
  Standard_Real m1 = (l1+f1)*0.5;
  Standard_Real m2 = (l2+f2)*0.5;

  gp_Pnt2d p1f(pc1->Value(m1-d1));
  gp_Pnt2d p1l(pc1->Value(m1+d1));
  gp_Pnt2d p2f(pc2->Value(m2-d2));
  gp_Pnt2d p2l(pc2->Value(m2+d2));

  gp_Vec2d vc1(p1f,p1l);
  gp_Vec2d vc2(p2f,p2l);
  
  if( (vc1*vc2) >= 0. && e1or == e2or ) return Standard_False;

  return Standard_True;
}
