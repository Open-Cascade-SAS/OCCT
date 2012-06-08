// Created on: 1994-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BRepLib_FindSurface.ixx>

#include <Precision.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <math_Gauss.hxx>

#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_Plane.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>

#include <GeomLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Standard_ErrorHandler.hxx>

//=======================================================================
//function : Controle
//purpose  : 
//=======================================================================
static Standard_Real Controle(const TColgp_SequenceOfPnt& thePoints,
			      const Handle(Geom_Plane)& thePlane)
{
  Standard_Real dfMaxDist=0.;
  Standard_Real a,b,c,d, dist;
  Standard_Integer ii;
  thePlane->Coefficients(a,b,c,d);
  for (ii=1; ii<=thePoints.Length(); ii++) {
    const gp_XYZ& xyz = thePoints(ii).XYZ();
    dist = Abs(a*xyz.X() + b*xyz.Y() + c*xyz.Z() + d);
    if (dist > dfMaxDist)
      dfMaxDist = dist;
  }

  return dfMaxDist;
}

//=======================================================================
//function : Is2DConnected
//purpose  : Return true if the last vertex of theEdge1 coincides with
//           the first vertex of theEdge2 in parametric space of theFace
//=======================================================================

inline static Standard_Boolean Is2DConnected( const TopoDS_Edge& theEdge1,
                                              const TopoDS_Edge& theEdge2,
                                              const TopoDS_Face& theFace )
{
  Standard_Real f,l;
  Handle(Geom2d_Curve) aCurve;
  gp_Pnt2d p1, p2;

  // get 2D points
  aCurve   = BRep_Tool::CurveOnSurface( theEdge1, theFace,f,l );
  p1       = aCurve->Value( theEdge1.Orientation() == TopAbs_FORWARD ? l : f );
  aCurve   = BRep_Tool::CurveOnSurface( theEdge2,  theFace,f,l );
  p2       = aCurve->Value(  theEdge2.Orientation() == TopAbs_FORWARD ? f : l );

  // compare 2D points
  BRepAdaptor_Surface aSurface( theFace );
  TopoDS_Vertex    aV = TopExp::FirstVertex( theEdge2, /*CumOri=*/Standard_True );
  Standard_Real tol3D = BRep_Tool::Tolerance( aV );
  Standard_Real tol2D = aSurface.UResolution( tol3D ) + aSurface.VResolution( tol3D );
  Standard_Real dist2 = p1.SquareDistance( p2 );
  return dist2 < tol2D * tol2D;
}

//=======================================================================
//function : Is2DClosed
//purpose  : Return true if edges of theShape form a closed wire in
//           parametric space of theSurface
//=======================================================================

static Standard_Boolean Is2DClosed( const TopoDS_Shape&         theShape,
                                    const Handle(Geom_Surface)& theSurface)
{
  try
  {
    // get a wire theShape 
    TopExp_Explorer aWireExp( theShape, TopAbs_WIRE );
    if ( !aWireExp.More() )
      return Standard_False;
    TopoDS_Wire aWire = TopoDS::Wire( aWireExp.Current() );
    // a tmp face
    TopoDS_Face aTmpFace = BRepLib_MakeFace( theSurface, Precision::PConfusion() );

    // check topological closeness using wire explorer, if the wire is not closed
    // the 1st and the last vertices of wire are different
    BRepTools_WireExplorer aWireExplorer( aWire, aTmpFace );
    if ( !aWireExplorer.More())
      return Standard_False;
    // remember the 1st and the last edges of aWire
    TopoDS_Edge aFisrtEdge = aWireExplorer.Current(), aLastEdge = aFisrtEdge;
    // check if edges connected topologically (that is assured by BRepTools_WireExplorer)
    // are connected in 2D
    TopoDS_Edge aPrevEdge = aFisrtEdge;
    for ( aWireExplorer.Next(); aWireExplorer.More(); aWireExplorer.Next() )
    {
      aLastEdge = aWireExplorer.Current();
      if ( !Is2DConnected( aPrevEdge, aLastEdge, aTmpFace ))
        return false;
      aPrevEdge = aLastEdge;
    }
    // wire is closed if ( 1st vertex of aFisrtEdge ) ==
    // ( last vertex of aLastEdge ) in 2D
    TopoDS_Vertex aV1 = TopExp::FirstVertex( aFisrtEdge, /*CumOri=*/Standard_True );
    TopoDS_Vertex aV2 = TopExp::LastVertex( aLastEdge, /*CumOri=*/Standard_True );
    return ( aV1.IsSame( aV2 ) && Is2DConnected( aLastEdge, aFisrtEdge, aTmpFace ));
  }
  catch ( Standard_Failure )
  {
    return Standard_False;
  }
}

//=======================================================================
//function : BRepLib_FindSurface
//purpose  : 
//=======================================================================
BRepLib_FindSurface::BRepLib_FindSurface() 
{
}
//=======================================================================
//function : BRepLib_FindSurface
//purpose  : 
//=======================================================================
BRepLib_FindSurface::BRepLib_FindSurface(const TopoDS_Shape&    S, 
					 const Standard_Real    Tol,
					 const Standard_Boolean OnlyPlane,
                                         const Standard_Boolean OnlyClosed)
{
  Init(S,Tol,OnlyPlane,OnlyClosed);
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepLib_FindSurface::Init(const TopoDS_Shape&    S, 
			       const Standard_Real    Tol,
			       const Standard_Boolean OnlyPlane,
                               const Standard_Boolean OnlyClosed)
{
  myTolerance = Tol;
  myTolReached = 0.;
  isExisted = Standard_False;
  myLocation.Identity();
  mySurface.Nullify();

  // compute the tolerance
  TopExp_Explorer ex;

  for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next()) {
    Standard_Real t = BRep_Tool::Tolerance(TopoDS::Edge(ex.Current()));
    if (t > myTolerance) myTolerance = t;
  }

  // search an existing surface
  ex.Init(S,TopAbs_EDGE);
  if (!ex.More()) return;    // no edges ....

  TopoDS_Edge E = TopoDS::Edge(ex.Current());
  Standard_Real f,l,ff,ll;
  Handle(Geom2d_Curve) PC,PPC;
  Handle(Geom_Surface) SS;
  TopLoc_Location L;
  Standard_Integer i = 0,j;

  // iterate on the surfaces of the first edge
  while ( Standard_True) {
    i++;
    BRep_Tool::CurveOnSurface(E,PC,mySurface,myLocation,f,l,i);
    if (mySurface.IsNull()) {
      break;
    }
    // check the other edges
    for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next()) {
      if (!E.IsSame(ex.Current())) {
	j = 0;
	while (Standard_True) {
	  j++;
	  BRep_Tool::CurveOnSurface(TopoDS::Edge(ex.Current()),
				    PPC,SS,L,ff,ll,j);
	  if (SS.IsNull()) {
	    break;
	  }
	  if (SS == mySurface) {
	    break;
	  }
	  SS.Nullify();
	}

	if (SS.IsNull()) {
	  mySurface.Nullify();
	  break;
	}
      }
    }

    // if OnlyPlane, eval if mySurface is a plane.
    if ( OnlyPlane && !mySurface.IsNull() ) 
    {
      if ( mySurface->IsKind( STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
        mySurface = Handle(Geom_RectangularTrimmedSurface)::DownCast(mySurface)->BasisSurface();
      mySurface = Handle(Geom_Plane)::DownCast(mySurface);
    }

    if (!mySurface.IsNull())
      // if S is e.g. the bottom face of a cylinder, mySurface can be the
      // lateral (cylindrical) face of the cylinder; reject an improper mySurface
      if ( !OnlyClosed || Is2DClosed( S, mySurface ))
        break;
  }

  if (!mySurface.IsNull()) {
    isExisted = Standard_True;
    return;
  }
  //
  // no existing surface, search a plane
  // 07/02/02 akm vvv : (OCC157) changed algorithm
  //                    1. Collect the points along all edges of the shape
  //                       For each point calculate the WEIGHT = sum of
  //                       distances from neighboring points (_only_ same edge)
  //                    2. Minimizing the weighed sum of squared deviations
  //                       compute coefficients of the sought plane.
  
  TColgp_SequenceOfPnt aPoints;
  TColStd_SequenceOfReal aWeight;

  // ======================= Step #1
  for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next()) 
  {
    BRepAdaptor_Curve c(TopoDS::Edge(ex.Current()));

    Standard_Real dfUf = c.FirstParameter();
    Standard_Real dfUl = c.LastParameter();
    if (IsEqual(dfUf,dfUl)) {
      // Degenerate
      continue;
    }
    Standard_Integer iNbPoints=0;

    // Add the points with weights to the sequences
    switch (c.GetType()) 
    {
    case GeomAbs_BezierCurve:
      {
	// Put all poles for bezier
	Handle(Geom_BezierCurve) GC = c.Bezier();
	Standard_Integer iNbPol = GC->NbPoles();
	if ( iNbPol < 2)
	  // Degenerate
	  continue;
	else
	{
	  Handle(TColgp_HArray1OfPnt) aPoles = new (TColgp_HArray1OfPnt) (1, iNbPol);
	  GC->Poles(aPoles->ChangeArray1());
	  gp_Pnt aPolePrev = aPoles->Value(1), aPoleNext;
	  Standard_Real dfDistPrev = 0., dfDistNext;
	  for (Standard_Integer iPol=1; iPol<=iNbPol; iPol++)
	  {
	    if (iPol<iNbPol)
	    {
	      aPoleNext = aPoles->Value(iPol+1);
	      dfDistNext = aPolePrev.Distance(aPoleNext);
	    }
	    else
	      dfDistNext = 0.;
	    aPoints.Append (aPolePrev);
	    aWeight.Append (dfDistPrev+dfDistNext);
	    dfDistPrev = dfDistNext;
	    aPolePrev = aPoleNext;
	  }
	}
      }
      break;
    case GeomAbs_BSplineCurve:
      {
	// Put all poles for bspline
	Handle(Geom_BSplineCurve) GC = c.BSpline();
	Standard_Integer iNbPol = GC->NbPoles();
	if ( iNbPol < 2)
	  // Degenerate
	  continue;
	else
	{
	  Handle(TColgp_HArray1OfPnt) aPoles = new (TColgp_HArray1OfPnt) (1, iNbPol);
	  GC->Poles(aPoles->ChangeArray1());
	  gp_Pnt aPolePrev = aPoles->Value(1), aPoleNext;
	  Standard_Real dfDistPrev = 0., dfDistNext;
	  for (Standard_Integer iPol=1; iPol<=iNbPol; iPol++)
	  {
	    if (iPol<iNbPol)
	    {
	      aPoleNext = aPoles->Value(iPol+1);
	      dfDistNext = aPolePrev.Distance(aPoleNext);
	    }
	    else
	      dfDistNext = 0.;
	    aPoints.Append (aPolePrev);
	    aWeight.Append (dfDistPrev+dfDistNext);
	    dfDistPrev = dfDistNext;
	    aPolePrev = aPoleNext;
	  }
	}
      }
      break;

    case GeomAbs_Line:
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
      if (c.GetType() == GeomAbs_Line)
	// Two points on straight segment
	iNbPoints=2;
      else
	// Four points on otheranalitical curves
	iNbPoints=4;
    default:
      { 
	// Put some points on other curves
	if (iNbPoints==0)
	  iNbPoints = 15 + c.NbIntervals(GeomAbs_C3);
	Standard_Real dfDelta = (dfUl-dfUf)/(iNbPoints-1);
	Standard_Integer iPoint;
	Standard_Real dfU;
	gp_Pnt aPointPrev = c.Value(dfUf), aPointNext;
	Standard_Real dfDistPrev = 0., dfDistNext;
	for (iPoint=1, dfU=dfUf+dfDelta; 
	     iPoint<=iNbPoints; 
	     iPoint++, dfU+=dfDelta) 
	{
	  if (iPoint<iNbPoints)
	  {
	    aPointNext = c.Value(dfU);
	    dfDistNext = aPointPrev.Distance(aPointNext);
	  }
	  else
	    dfDistNext = 0.;
	  aPoints.Append (aPointPrev);
	  aWeight.Append (dfDistPrev+dfDistNext);
	  dfDistPrev = dfDistNext;
	  aPointPrev = aPointNext;
	}
      } // default:
    } // switch (c.GetType()) ...
  } // for (ex.Init(S,TopAbs_EDGE); ex.More() && control; ex.Next()) ...
  
  if (aPoints.Length() < 3) {
    return;
  }

  // ======================= Step #2
  myLocation.Identity();
  Standard_Integer iPoint;
  math_Matrix aMat (1,3,1,3, 0.);
  math_Vector aVec (1,3, 0.);
  // Find the barycenter and normalize weights 
  Standard_Real dfMaxWeight=0.;
  gp_XYZ aBaryCenter(0.,0.,0.);
  Standard_Real dfSumWeight=0.;
  for (iPoint=1; iPoint<=aPoints.Length(); iPoint++)  {
    Standard_Real dfW = aWeight(iPoint);
    aBaryCenter += dfW*aPoints(iPoint).XYZ();
    dfSumWeight += dfW;
    if (dfW > dfMaxWeight) {
      dfMaxWeight = dfW;
    }
  }
  aBaryCenter /= dfSumWeight;

  // Fill the matrix and the right vector
  for (iPoint=1; iPoint<=aPoints.Length(); iPoint++)  {
    gp_XYZ p=aPoints(iPoint).XYZ()-aBaryCenter;
    Standard_Real w=aWeight(iPoint)/dfMaxWeight;
    aMat(1,1)+=w*p.X()*p.X(); 
        aMat(1,2)+=w*p.X()*p.Y(); 
            aMat(1,3)+=w*p.X()*p.Z();
    aMat(2,1)+=w*p.Y()*p.X();  
        aMat(2,2)+=w*p.Y()*p.Y();  
            aMat(2,3)+=w*p.Y()*p.Z();
    aMat(3,1)+=w*p.Z()*p.X();  
        aMat(3,2)+=w*p.Z()*p.Y(); 
            aMat(3,3)+=w*p.Z()*p.Z();
    aVec(1) -= w*p.X();
    aVec(2) -= w*p.Y();
    aVec(3) -= w*p.Z();
  }

  // Solve the system of equations to get plane coefficients
  math_Gauss aSolver(aMat);
  Standard_Boolean isSolved = aSolver.IsDone();
  //
  //  let us be more tolerant (occ415)
  Standard_Real dfDist = RealLast();
  Handle(Geom_Plane) aPlane;
  //
  if (isSolved)  {
    aSolver.Solve(aVec);
    if (aVec.Norm2()<gp::Resolution()) {
      isSolved = Standard_False;
    }
  }
  //
  if (isSolved)  {
    aPlane = new Geom_Plane(aBaryCenter,gp_Dir(aVec(1),aVec(2),aVec(3)));
    dfDist = Controle (aPoints, aPlane);
  }
  //
  if (!isSolved || myTolerance < dfDist)  {
    gp_Pnt aFirstPnt=aPoints(1);
    for (iPoint=2; iPoint<=aPoints.Length(); iPoint++)  {
      gp_Vec aDir(aFirstPnt,aPoints(iPoint));
      Standard_Real dfSide=aDir.Magnitude();
      if (dfSide<myTolerance) {
	continue; // degeneration
      }
      for (Standard_Integer iP1=iPoint+1; iP1<=aPoints.Length(); iP1++) {
	gp_Vec aCross = gp_Vec(aFirstPnt,aPoints(iP1)) ^ aDir ;
	if (aCross.Magnitude() > dfSide*myTolerance) {
	  Handle(Geom_Plane) aPlane2 = new Geom_Plane(aFirstPnt, aCross);
	  Standard_Real dfDist2 = Controle (aPoints, aPlane2);
	  if (dfDist2 < myTolerance)  {
	    myTolReached = dfDist2;
	    mySurface = aPlane2;
	    return;
	  }
	  if (dfDist2 < dfDist)  {
	    dfDist = dfDist2;
	    aPlane = aPlane2;
	  }
	}
      }
    }
  }
  //
  //XXf
  //static Standard_Real weakness = 5.0;
  Standard_Real weakness = 5.0;
  //XXf
  if(dfDist <= myTolerance || dfDist < myTolerance*weakness && Tol<0) { 
    //XXf 
    //myTolReached = dfDist;
    //XXt
    mySurface = aPlane;
  }
  //XXf
  myTolReached = dfDist;
  //XXt
}
//=======================================================================
//function : Found
//purpose  : 
//=======================================================================
Standard_Boolean BRepLib_FindSurface::Found() const 
{
  return !mySurface.IsNull();
}
//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================
Handle(Geom_Surface) BRepLib_FindSurface::Surface() const 
{
  return mySurface;
}
//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================
Standard_Real BRepLib_FindSurface::Tolerance() const 
{
  return myTolerance;
}
//=======================================================================
//function : ToleranceReached
//purpose  : 
//=======================================================================
Standard_Real BRepLib_FindSurface::ToleranceReached() const 
{
  return myTolReached;
}
//=======================================================================
//function : Existed
//purpose  : 
//=======================================================================
Standard_Boolean BRepLib_FindSurface::Existed() const 
{
  return isExisted;
}
//=======================================================================
//function : Location
//purpose  : 
//=======================================================================
TopLoc_Location BRepLib_FindSurface::Location() const
{
  return myLocation;
}
