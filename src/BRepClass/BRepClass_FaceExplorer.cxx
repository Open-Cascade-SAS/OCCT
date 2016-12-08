// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

//  Modified by skv - Thu Jul 13 17:42:58 2006 OCC12627
//  Total rewriting of the method Segment; add the method OtherSegment.

#include <BRep_Tool.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

static const Standard_Real Probing_Start = 0.123;
static const Standard_Real Probing_End = 0.7;
static const Standard_Real Probing_Step = 0.2111;

//=======================================================================
//function : BRepClass_FaceExplorer
//purpose  : 
//=======================================================================

BRepClass_FaceExplorer::BRepClass_FaceExplorer(const TopoDS_Face& F) :
       myFace(F),
       myCurEdgeInd(1),
       myCurEdgePar(Probing_Start)
{
  myFace.Orientation(TopAbs_FORWARD);
}

//=======================================================================
//function : CheckPoint
//purpose  : 
//=======================================================================

Standard_Boolean  BRepClass_FaceExplorer::CheckPoint(gp_Pnt2d& thePoint)
{
  Standard_Real anUMin = 0.0, anUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  TopLoc_Location aLocation;
  const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(myFace, aLocation);
  aSurface->Bounds(anUMin, anUMax, aVMin, aVMax);
  if (Precision::IsInfinite(anUMin) || Precision::IsInfinite(anUMax) ||
      Precision::IsInfinite(aVMin) || Precision::IsInfinite(aVMax))
  {
    BRepTools::UVBounds(myFace, anUMin, anUMax, aVMin, aVMax);
    if (Precision::IsInfinite(anUMin) || Precision::IsInfinite(anUMax) ||
        Precision::IsInfinite(aVMin) || Precision::IsInfinite(aVMax))
    {
      return Standard_True;
    }
  }

  gp_Pnt2d aCenterPnt(( anUMin + anUMax ) / 2, ( aVMin + aVMax ) / 2);
  Standard_Real aDistance = aCenterPnt.Distance(thePoint);
  if (Precision::IsInfinite(aDistance))
  {
    thePoint.SetCoord(anUMin - ( anUMax - anUMin ),
                       aVMin - ( aVMax - aVMin ));
    return Standard_False;
  }
  else
  {
    Standard_Real anEpsilon = Epsilon(aDistance);
    if (anEpsilon > Max(anUMax - anUMin, aVMax - aVMin))
    {
      gp_Vec2d aLinVec(aCenterPnt, thePoint);
      gp_Dir2d aLinDir(aLinVec);
      thePoint = aCenterPnt.XY() + aLinDir.XY() * ( 2. * anEpsilon );
      return Standard_False;
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================

Standard_Boolean  BRepClass_FaceExplorer::Reject(const gp_Pnt2d&)const 
{
  return Standard_False;
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass_FaceExplorer::Segment(const gp_Pnt2d& P, 
						 gp_Lin2d& L,
						 Standard_Real& Par)
{
  myCurEdgeInd = 1;
  myCurEdgePar = Probing_Start;

  return OtherSegment(P, L, Par);
}

//=======================================================================
//function : OtherSegment
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass_FaceExplorer::OtherSegment(const gp_Pnt2d& P, 
						      gp_Lin2d& L,
						      Standard_Real& Par)
{
  TopExp_Explorer      anExpF(myFace,TopAbs_EDGE);
  Standard_Integer     i;
  Standard_Real        aFPar;
  Standard_Real        aLPar;
  Handle(Geom2d_Curve) aC2d;
  Standard_Real        aTolParConf2 = Precision::PConfusion() * Precision::PConfusion();
  gp_Pnt2d             aPOnC;
  Standard_Real        aParamIn;

  for (i = 1; anExpF.More(); anExpF.Next(), i++) {
    if (i != myCurEdgeInd)
      continue;

    const TopoDS_Shape       &aLocalShape   = anExpF.Current();
    const TopAbs_Orientation  anOrientation = aLocalShape.Orientation();

    if (anOrientation == TopAbs_FORWARD || anOrientation == TopAbs_REVERSED) {
      const TopoDS_Edge &anEdge = TopoDS::Edge(aLocalShape);

      aC2d = BRep_Tool::CurveOnSurface(anEdge, myFace, aFPar, aLPar);

      if (!aC2d.IsNull()) {
	// Treatment of infinite cases.
	if (Precision::IsNegativeInfinite(aFPar)) {
	  if (Precision::IsPositiveInfinite(aLPar)) {
	    aFPar = -1.;
	    aLPar =  1.;
	  } else {
	    aFPar = aLPar - 1.;
	  }
	} else if (Precision::IsPositiveInfinite(aLPar))
	  aLPar = aFPar + 1.;

	for (; myCurEdgePar < Probing_End ;myCurEdgePar += Probing_Step) {
	  aParamIn = myCurEdgePar*aFPar + (1. - myCurEdgePar)*aLPar;

          gp_Vec2d aTanVec;
	  aC2d->D1(aParamIn, aPOnC, aTanVec);
	  Par = aPOnC.SquareDistance(P);

	  if (Par > aTolParConf2) {
	    gp_Vec2d aLinVec(P, aPOnC);
	    gp_Dir2d aLinDir(aLinVec);

            Standard_Real aTanMod = aTanVec.SquareMagnitude();
            if (aTanMod < aTolParConf2)
              continue;
            aTanVec /= Sqrt(aTanMod);
            Standard_Real aSinA = aTanVec.Crossed(aLinDir.XY());
            const Standard_Real SmallAngle = 0.001;
            if (Abs(aSinA) < SmallAngle)
            {
              // The line from the input point P to the current point on edge
              // is tangent to the edge curve. This condition is bad for classification.
              // Therefore try to go to another point in the hope that there will be 
              // no tangent. If there tangent is preserved then leave the last point in 
              // order to get this edge chanse to participate in classification.
              if (myCurEdgePar + Probing_Step < Probing_End)
                continue;
            }

	    L = gp_Lin2d(P, aLinDir);

	    // Check if ends of a curve lie on a line.
	    aC2d->D0(aFPar, aPOnC);

	    if (L.SquareDistance(aPOnC) > aTolParConf2) {
	      aC2d->D0(aLPar, aPOnC);

	      if (L.SquareDistance(aPOnC) > aTolParConf2) {
		myCurEdgePar += Probing_Step;

		if (myCurEdgePar >= Probing_End) {
		  myCurEdgeInd++;
		  myCurEdgePar = Probing_Start;
		}

		Par = Sqrt(Par);
		return Standard_True;
	      }
	    }
	  }
	}
      } // if (!aC2d.IsNull()) {
    } // if (anOrientation == TopAbs_FORWARD ...

    // This curve is not valid for line construction. Go to another edge.
    myCurEdgeInd++;
    myCurEdgePar = Probing_Start;
  }

  // nothing found, return an horizontal line
  Par = RealLast();
  L   = gp_Lin2d(P,gp_Dir2d(1,0));

  return Standard_False;
}

//=======================================================================
//function : InitWires
//purpose  : 
//=======================================================================

void  BRepClass_FaceExplorer::InitWires()
{
  myWExplorer.Init(myFace,TopAbs_WIRE);
}

//=======================================================================
//function : RejectWire NYI
//purpose  : 
//=======================================================================

Standard_Boolean  BRepClass_FaceExplorer::RejectWire
  (const gp_Lin2d& , 
   const Standard_Real)const 
{
  return Standard_False;
}

//=======================================================================
//function : InitEdges
//purpose  : 
//=======================================================================

void  BRepClass_FaceExplorer::InitEdges()
{
  myEExplorer.Init(myWExplorer.Current(),TopAbs_EDGE);
}

//=======================================================================
//function : RejectEdge NYI
//purpose  : 
//=======================================================================

Standard_Boolean  BRepClass_FaceExplorer::RejectEdge
  (const gp_Lin2d& , 
   const Standard_Real )const 
{
  return Standard_False;
}


//=======================================================================
//function : CurrentEdge
//purpose  : 
//=======================================================================

void  BRepClass_FaceExplorer::CurrentEdge(BRepClass_Edge& E, 
					  TopAbs_Orientation& Or) const 
{
  E.Edge() = TopoDS::Edge(myEExplorer.Current());
  E.Face() = myFace;
  Or = E.Edge().Orientation();
}

