// Created on: 1993-09-29
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepMesh_GeomTool.hxx>

#include <BRepMesh_DefaultRangeSplitter.hxx>

#include <TopAbs_Orientation.hxx>
#include <CSLib.hxx>
#include <Precision.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <Adaptor3d_Curve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>

namespace
{
  void ComputeErrFactors (const Standard_Real               theDeflection,
                          const Handle(Adaptor3d_Surface)& theFace,
                          Standard_Real&                    theErrFactorU,
                          Standard_Real&                    theErrFactorV)
  {
    theErrFactorU = theDeflection * 10.;
    theErrFactorV = theDeflection * 10.;

    switch (theFace->GetType ())
    {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
      break;

    case GeomAbs_SurfaceOfExtrusion:
    case GeomAbs_SurfaceOfRevolution:
      {
        Handle(Adaptor3d_Curve) aCurve = theFace->BasisCurve ();
        if (aCurve->GetType () == GeomAbs_BSplineCurve && aCurve->Degree () > 2)
        {
          theErrFactorV /= (aCurve->Degree () * aCurve->NbKnots ());
        }
        break;
      }
    case GeomAbs_BezierSurface:
      {
        if (theFace->UDegree () > 2)
        {
          theErrFactorU /= (theFace->UDegree ());
        }
        if (theFace->VDegree () > 2)
        {
          theErrFactorV /= (theFace->VDegree ());
        }
        break;
      }
    case GeomAbs_BSplineSurface:
      {
        if (theFace->UDegree () > 2)
        {
          theErrFactorU /= (theFace->UDegree () * theFace->NbUKnots ());
        }
        if (theFace->VDegree () > 2)
        {
          theErrFactorV /= (theFace->VDegree () *  theFace->NbVKnots ());
        }
        break;
      }

    case GeomAbs_Plane:
    default:
      theErrFactorU = theErrFactorV = 1.;
    }
  }

  void AdjustCellsCounts (const Handle(Adaptor3d_Surface)& theFace,
                          const Standard_Integer            theNbVertices,
                          Standard_Integer&                 theCellsCountU,
                          Standard_Integer&                 theCellsCountV)
  {
    const GeomAbs_SurfaceType aType = theFace->GetType ();
    if (aType == GeomAbs_OtherSurface)
    {
      // fallback to the default behavior
      theCellsCountU = theCellsCountV = -1;
      return;
    }

    Standard_Real aSqNbVert = theNbVertices;
    if (aType == GeomAbs_Plane)
    {
      theCellsCountU = theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
    }
    else if (aType == GeomAbs_Cylinder || aType == GeomAbs_Cone)
    {
      theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
    }
    else if (aType == GeomAbs_SurfaceOfExtrusion || aType == GeomAbs_SurfaceOfRevolution)
    {
      Handle (Adaptor3d_Curve) aCurve = theFace->BasisCurve ();
      if (aCurve->GetType () == GeomAbs_Line ||
         (aCurve->GetType () == GeomAbs_BSplineCurve && aCurve->Degree () < 2))
      {
        // planar, cylindrical, conical cases
        if (aType == GeomAbs_SurfaceOfExtrusion)
          theCellsCountU = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
        else
          theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
      }
      if (aType == GeomAbs_SurfaceOfExtrusion)
      {
        // V is always a line
        theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
      }
    }
    else if (aType == GeomAbs_BezierSurface || aType == GeomAbs_BSplineSurface)
    {
      if (theFace->UDegree () < 2)
      {
        theCellsCountU = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
      }
      if (theFace->VDegree () < 2)
      {
        theCellsCountV = (Standard_Integer)Ceiling (Pow (2, Log10 (aSqNbVert)));
      }
    }

    theCellsCountU = Max (theCellsCountU, 2);
    theCellsCountV = Max (theCellsCountV, 2);
  }
}

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
BRepMesh_GeomTool::BRepMesh_GeomTool(
  const BRepAdaptor_Curve& theCurve,
  const Standard_Real      theFirstParam,
  const Standard_Real      theLastParam,
  const Standard_Real      theLinDeflection,
  const Standard_Real      theAngDeflection,
  const Standard_Integer   theMinPointsNb,
  const Standard_Real      theMinSize)
  : myEdge(&theCurve.Edge()),
    myIsoType(GeomAbs_NoneIso)
{
  myDiscretTool.Initialize(theCurve, theFirstParam, theLastParam,
    theAngDeflection, theLinDeflection, theMinPointsNb, 
    Precision::PConfusion(), theMinSize);
}

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
BRepMesh_GeomTool::BRepMesh_GeomTool(
  const Handle(BRepAdaptor_Surface)& theSurface,
  const GeomAbs_IsoType               theIsoType,
  const Standard_Real                 theParamIso,
  const Standard_Real                 theFirstParam,
  const Standard_Real                 theLastParam,
  const Standard_Real                 theLinDeflection,
  const Standard_Real                 theAngDeflection,
  const Standard_Integer              theMinPointsNb,
  const Standard_Real                 theMinSize)
  : myEdge(NULL),
    myIsoType(theIsoType)
{
  Adaptor3d_IsoCurve aIso(theSurface, theIsoType, theParamIso,
    theFirstParam, theLastParam);

  myDiscretTool.Initialize(aIso, theFirstParam, theLastParam,
    theAngDeflection, theLinDeflection, theMinPointsNb,
    Precision::PConfusion(), theMinSize);
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_GeomTool::Value(
  const Standard_Integer              theIndex,
  const Handle(BRepAdaptor_Surface)& theSurface,
  Standard_Real&                      theParam,
  gp_Pnt&                             thePoint,
  gp_Pnt2d&                           theUV) const
{
  if (theIndex < 1 || theIndex > NbPoints())
    return Standard_False;

  if (myEdge == NULL)
    return Standard_False;

  thePoint = myDiscretTool.Value(theIndex);
  theParam = myDiscretTool.Parameter(theIndex);

  const TopoDS_Face& aFace = theSurface->Face();

  Standard_Real aFirst, aLast;
  Handle(Geom2d_Curve) aCurve = 
    BRep_Tool::CurveOnSurface(*myEdge, aFace, aFirst, aLast);

  aCurve->D0(theParam, theUV);

  return Standard_True;
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_GeomTool::Value(
  const Standard_Integer theIndex,
  const Standard_Real    theIsoParam,
  Standard_Real&         theParam,
  gp_Pnt&                thePoint,
  gp_Pnt2d&              theUV) const
{
  if (theIndex < 1 || theIndex > NbPoints())
    return Standard_False;

  thePoint = myDiscretTool.Value(theIndex);
  theParam = myDiscretTool.Parameter(theIndex);

  if (myIsoType == GeomAbs_IsoU)
    theUV.SetCoord(theIsoParam, theParam);
  else
    theUV.SetCoord(theParam, theIsoParam);

  return Standard_True;
}

//=======================================================================
//function : Normal
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_GeomTool::Normal(
  const Handle(BRepAdaptor_Surface)& theSurface,
  const Standard_Real                 theParamU,
  const Standard_Real                 theParamV,
  gp_Pnt&                             thePoint,
  gp_Dir&                             theNormal)
{
  Standard_Boolean isOK = Standard_True;
  gp_Vec aD1U, aD1V;

  theSurface->D1(theParamU, theParamV, thePoint, aD1U, aD1V);

  CSLib_DerivativeStatus aStatus;
  CSLib::Normal(aD1U, aD1V, Precision::Angular(), aStatus, theNormal);
  if (aStatus != CSLib_Done)
  {
    gp_Vec aD2U,aD2V,aD2UV;
    theSurface->D2(theParamU, theParamV, thePoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
    CSLib_NormalStatus aNormalStatus;
    CSLib::Normal(aD1U, aD1V, aD2U, aD2V, aD2UV, Precision::Angular(), 
      isOK, aNormalStatus, theNormal);
  }

  if (!isOK)
    return Standard_False;

  const TopoDS_Face& aFace = theSurface->Face();
  TopAbs_Orientation aOri = aFace.Orientation();
  if (aOri == TopAbs_REVERSED)
    theNormal.Reverse();

  return Standard_True;
}

//=============================================================================
//function : IntLinLin
//purpose  : 
//=============================================================================
BRepMesh_GeomTool::IntFlag BRepMesh_GeomTool::IntLinLin(
  const gp_XY&  theStartPnt1,
  const gp_XY&  theEndPnt1,
  const gp_XY&  theStartPnt2,
  const gp_XY&  theEndPnt2,
  gp_XY&        theIntPnt,
  Standard_Real (&theParamOnSegment)[2])
{
  gp_XY aVec1    = theEndPnt1   - theStartPnt1;
  gp_XY aVec2    = theEndPnt2   - theStartPnt2;
  gp_XY aVecO1O2 = theStartPnt2 - theStartPnt1;
    
  Standard_Real aCrossD1D2 = aVec1    ^ aVec2;
  Standard_Real aCrossD1D3 = aVecO1O2 ^ aVec2;

  const Standard_Real aPrec = gp::Resolution();
  // Are edgegs codirectional
  if ( Abs( aCrossD1D2 ) < aPrec )
  {
    // Just a parallel case?
    if( Abs( aCrossD1D3 ) < aPrec )
      return BRepMesh_GeomTool::Same;
    else
      return BRepMesh_GeomTool::NoIntersection;
  }

  theParamOnSegment[0] = aCrossD1D3 / aCrossD1D2;
  theIntPnt = theStartPnt1 + theParamOnSegment[0] * aVec1;

  Standard_Real aCrossD2D3 = aVecO1O2 ^ aVec1;
  theParamOnSegment[1] = aCrossD2D3 / aCrossD1D2;

  return BRepMesh_GeomTool::Cross;
}

//=============================================================================
//function : IntSegSeg
//purpose  : 
//=============================================================================
BRepMesh_GeomTool::IntFlag BRepMesh_GeomTool::IntSegSeg(
  const gp_XY&           theStartPnt1,
  const gp_XY&           theEndPnt1,
  const gp_XY&           theStartPnt2,
  const gp_XY&           theEndPnt2,
  const Standard_Boolean isConsiderEndPointTouch,
  const Standard_Boolean isConsiderPointOnSegment,
  gp_Pnt2d&              theIntPnt)
{
  Standard_Integer aPointHash[] = {
    classifyPoint(theStartPnt1, theEndPnt1, theStartPnt2),
    classifyPoint(theStartPnt1, theEndPnt1, theEndPnt2  ),
    classifyPoint(theStartPnt2, theEndPnt2, theStartPnt1),
    classifyPoint(theStartPnt2, theEndPnt2, theEndPnt1  )
  };

  Standard_Integer aPosHash =
    aPointHash[0] + aPointHash[1] + aPointHash[2] + aPointHash[3];

  // Consider case when edges have shared vertex
  if ( aPointHash[0] < 0 || aPointHash[1] < 0 )
  {
    if (aPosHash == -1)
    {
      // -1 means, that 2 points are equal, and 1 point is on another curve
      return BRepMesh_GeomTool::Glued;
    }
    else
    {
      if (isConsiderEndPointTouch)
        return BRepMesh_GeomTool::EndPointTouch;

      return BRepMesh_GeomTool::NoIntersection;
    }
  }

  /*=========================================*/
  /*  1) hash code == 1:

                    0+
                    /
           0      1/         0
           +======+==========+
  
      2) hash code == 2:

           0    1        1   0
        a) +----+========+---+

           0       1   1     0
        b) +-------+===+=====+

                                             */
  /*=========================================*/
  if ( aPosHash == 1 )
  {
    if (isConsiderPointOnSegment)
    {
      if (aPointHash[0] == 1)
        theIntPnt = theStartPnt1;
      else if (aPointHash[1] == 1)
        theIntPnt = theEndPnt1;
      else if (aPointHash[2] == 1)
        theIntPnt = theStartPnt2;
      else
        theIntPnt = theEndPnt2;

      return BRepMesh_GeomTool::PointOnSegment;
    }

    return BRepMesh_GeomTool::NoIntersection;
  }
  else if ( aPosHash == 2 )
    return BRepMesh_GeomTool::Glued;

  Standard_Real aParam[2];
  IntFlag aIntFlag = IntLinLin(theStartPnt1, theEndPnt1, 
    theStartPnt2, theEndPnt2, theIntPnt.ChangeCoord(), aParam);

  if (aIntFlag == BRepMesh_GeomTool::NoIntersection)
    return BRepMesh_GeomTool::NoIntersection;

  if (aIntFlag == BRepMesh_GeomTool::Same)
  {
    if ( aPosHash < -2 )
      return BRepMesh_GeomTool::Same;
    else if ( aPosHash == -1 )
      return BRepMesh_GeomTool::Glued;

    return BRepMesh_GeomTool::NoIntersection;
  }

  // Cross
  // Intersection is out of segments ranges
  constexpr Standard_Real aPrec = Precision::PConfusion();
  const Standard_Real aEndPrec = 1 - aPrec;
  for (Standard_Integer i = 0; i < 2; ++i)
  {
    if(aParam[i] < aPrec || aParam[i] > aEndPrec )
      return BRepMesh_GeomTool::NoIntersection;
  }
 
  return BRepMesh_GeomTool::Cross;
}

//=============================================================================
//function : CellsCount
//purpose  :
//=============================================================================
std::pair<Standard_Integer, Standard_Integer> BRepMesh_GeomTool::CellsCount (
  const Handle (Adaptor3d_Surface)&   theSurface,
  const Standard_Integer               theVerticesNb,
  const Standard_Real                  theDeflection,
  const BRepMesh_DefaultRangeSplitter* theRangeSplitter)
{
  if (theRangeSplitter == NULL)
    return std::pair<Standard_Integer, Standard_Integer>(-1, -1);

  const GeomAbs_SurfaceType aType = theSurface->GetType ();

  Standard_Real anErrFactorU, anErrFactorV;
  ComputeErrFactors(theDeflection, theSurface, anErrFactorU, anErrFactorV);

  const std::pair<Standard_Real, Standard_Real>& aRangeU = theRangeSplitter->GetRangeU();
  const std::pair<Standard_Real, Standard_Real>& aRangeV = theRangeSplitter->GetRangeV();
  const std::pair<Standard_Real, Standard_Real>& aDelta  = theRangeSplitter->GetDelta ();

  Standard_Integer aCellsCountU, aCellsCountV;
  if (aType == GeomAbs_Torus)
  {
    aCellsCountU = (Standard_Integer)Ceiling(Pow(2, Log10(
      (aRangeU.second - aRangeU.first) / aDelta.first)));
    aCellsCountV = (Standard_Integer)Ceiling(Pow(2, Log10(
      (aRangeV.second - aRangeV.first) / aDelta.second)));
  }
  else if (aType == GeomAbs_Cylinder)
  {
    aCellsCountU = (Standard_Integer)Ceiling(Pow(2, Log10(
      (aRangeU.second - aRangeU.first) / aDelta.first /
      (aRangeV.second - aRangeV.first))));
    aCellsCountV = (Standard_Integer)Ceiling(Pow(2, Log10(
      (aRangeV.second - aRangeV.first) / anErrFactorV)));
  }
  else
  {
    aCellsCountU = (Standard_Integer)Ceiling(Pow(2, Log10(
      (aRangeU.second - aRangeU.first) / aDelta.first  / anErrFactorU)));
    aCellsCountV = (Standard_Integer)Ceiling(Pow(2, Log10(
      (aRangeV.second - aRangeV.first) / aDelta.second / anErrFactorV)));
  }

  AdjustCellsCounts(theSurface, theVerticesNb, aCellsCountU, aCellsCountV);
  return std::pair<Standard_Integer, Standard_Integer>(aCellsCountU, aCellsCountV);
}

//=============================================================================
//function : classifyPoint
//purpose  : 
//=============================================================================
Standard_Integer BRepMesh_GeomTool::classifyPoint(
  const gp_XY& thePoint1,
  const gp_XY& thePoint2,
  const gp_XY& thePointToCheck)
{
  gp_XY aP1 = thePoint2       - thePoint1;
  gp_XY aP2 = thePointToCheck - thePoint1;
  
  constexpr Standard_Real aPrec = Precision::PConfusion();
  const Standard_Real aSqPrec = aPrec * aPrec;
  Standard_Real aDist = Abs(aP1 ^ aP2);
  if (aDist > aPrec)
  {
    aDist = (aDist * aDist) / aP1.SquareModulus();
    if (aDist > aSqPrec)
      return 0; //out
  }
    
  gp_XY aMult = aP1.Multiplied(aP2);
  if ( aMult.X() < 0.0 || aMult.Y() < 0.0 )
    return 0; //out
    
  if (aP1.SquareModulus() < aP2.SquareModulus())
    return 0; //out
    
  if (thePointToCheck.IsEqual(thePoint1, aPrec) || 
      thePointToCheck.IsEqual(thePoint2, aPrec))
  {
    return -1; //coincides with an end point
  }
    
  return 1;
}
