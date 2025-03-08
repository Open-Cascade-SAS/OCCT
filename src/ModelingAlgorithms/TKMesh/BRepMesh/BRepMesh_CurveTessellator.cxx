// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_CurveTessellator.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_PCurve.hxx>
#include <IMeshTools_Parameters.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom_Plane.hxx>
#include <TopExp.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Standard_Failure.hxx>
#include <GCPnts_AbscissaPoint.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_CurveTessellator, IMeshTools_CurveTessellator)

//=================================================================================================

BRepMesh_CurveTessellator::BRepMesh_CurveTessellator(const IMeshData::IEdgeHandle& theEdge,
                                                     const IMeshTools_Parameters&  theParameters,
                                                     const Standard_Integer        theMinPointsNb)
    : myDEdge(theEdge),
      myParameters(theParameters),
      myEdge(theEdge->GetEdge()),
      myCurve(myEdge),
      myMinPointsNb(theMinPointsNb)
{
  init();
}

//=================================================================================================

BRepMesh_CurveTessellator::BRepMesh_CurveTessellator(const IMeshData::IEdgeHandle& theEdge,
                                                     const TopAbs_Orientation      theOrientation,
                                                     const IMeshData::IFaceHandle& theFace,
                                                     const IMeshTools_Parameters&  theParameters,
                                                     const Standard_Integer        theMinPointsNb)
    : myDEdge(theEdge),
      myParameters(theParameters),
      myEdge(TopoDS::Edge(theEdge->GetEdge().Oriented(theOrientation))),
      myCurve(myEdge, theFace->GetFace()),
      myMinPointsNb(theMinPointsNb)
{
  init();
}

//=================================================================================================

void BRepMesh_CurveTessellator::init()
{
  if (myParameters.MinSize <= 0.0)
  {
    Standard_Failure::Raise("The structure \"myParameters\" is not initialized");
  }

  TopExp::Vertices(myEdge, myFirstVertex, myLastVertex);

  Standard_Real aPreciseAngDef = 0.5 * myDEdge->GetAngularDeflection();
  Standard_Real aPreciseLinDef = 0.5 * myDEdge->GetDeflection();
  if (myEdge.Orientation() == TopAbs_INTERNAL)
  {
    aPreciseLinDef *= 0.5;
  }

  aPreciseLinDef = Max(aPreciseLinDef, Precision::Confusion());
  aPreciseAngDef = Max(aPreciseAngDef, Precision::Angular());

  Standard_Real aMinSize = myParameters.MinSize;
  if (myParameters.AdjustMinSize)
  {
    aMinSize = Min(aMinSize,
                   myParameters.RelMinSize()
                     * GCPnts_AbscissaPoint::Length(myCurve,
                                                    myCurve.FirstParameter(),
                                                    myCurve.LastParameter(),
                                                    aPreciseLinDef));
  }

  mySquareEdgeDef = aPreciseLinDef * aPreciseLinDef;
  mySquareMinSize = Max(mySquareEdgeDef, aMinSize * aMinSize);

  myEdgeSqTol = BRep_Tool::Tolerance(myEdge);
  myEdgeSqTol *= myEdgeSqTol;

  Standard_Integer aMinPntThreshold = 2;
  switch (myCurve.GetType())
  {
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
    case GeomAbs_Parabola:
    case GeomAbs_Hyperbola:
      aMinPntThreshold = 4;
      break;

    default:
      break;
  }

  const Standard_Integer aMinPntNb = Max(myMinPointsNb, aMinPntThreshold); // OCC287
  myDiscretTool.Initialize(myCurve,
                           myCurve.FirstParameter(),
                           myCurve.LastParameter(),
                           aPreciseAngDef,
                           aPreciseLinDef,
                           aMinPntNb,
                           Precision::PConfusion(),
                           aMinSize);

  if (myCurve.IsCurveOnSurface())
  {
    const Adaptor3d_CurveOnSurface&  aCurve   = myCurve.CurveOnSurface();
    const Handle(Adaptor3d_Surface)& aSurface = aCurve.GetSurface();

    constexpr Standard_Real aTol = Precision::Confusion();
    const Standard_Real     aDu  = aSurface->UResolution(aTol);
    const Standard_Real     aDv  = aSurface->VResolution(aTol);

    myFaceRangeU[0] = aSurface->FirstUParameter() - aDu;
    myFaceRangeU[1] = aSurface->LastUParameter() + aDu;

    myFaceRangeV[0] = aSurface->FirstVParameter() - aDv;
    myFaceRangeV[1] = aSurface->LastVParameter() + aDv;
  }

  addInternalVertices();
  splitByDeflection2d();
}

//=================================================================================================

BRepMesh_CurveTessellator::~BRepMesh_CurveTessellator() {}

//=================================================================================================

Standard_Integer BRepMesh_CurveTessellator::PointsNb() const
{
  return myDiscretTool.NbPoints();
}

//=================================================================================================

void BRepMesh_CurveTessellator::splitByDeflection2d()
{
  const Standard_Integer aNodesNb = myDiscretTool.NbPoints();
  if (!myDEdge->IsFree() && myDEdge->GetSameParam() && myDEdge->GetSameRange() && aNodesNb > 1)
  {
    for (Standard_Integer aPCurveIt = 0; aPCurveIt < myDEdge->PCurvesNb(); ++aPCurveIt)
    {
      TopLoc_Location                 aLoc;
      const IMeshData::IPCurveHandle& aPCurve  = myDEdge->GetPCurve(aPCurveIt);
      const TopoDS_Face&              aFace    = aPCurve->GetFace()->GetFace();
      const Handle(Geom_Surface)&     aSurface = BRep_Tool::Surface(aFace, aLoc);
      if (aSurface->IsInstance(STANDARD_TYPE(Geom_Plane)))
      {
        continue;
      }

      const TopoDS_Edge aCurrEdge = TopoDS::Edge(myEdge.Oriented(aPCurve->GetOrientation()));

      Standard_Real        aF, aL;
      Handle(Geom2d_Curve) aCurve2d = BRep_Tool::CurveOnSurface(aCurrEdge, aFace, aF, aL);
      TColStd_Array1OfReal aParamArray(1, aNodesNb);
      for (Standard_Integer i = 1; i <= aNodesNb; ++i)
        aParamArray.SetValue(i, myDiscretTool.Parameter(i));

      for (Standard_Integer i = 1; i < aNodesNb; ++i)
        splitSegment(aSurface, aCurve2d, aParamArray(i), aParamArray(i + 1), 1);
    }
  }
}

//=================================================================================================

void BRepMesh_CurveTessellator::addInternalVertices()
{
  // PTv, chl/922/G9, Take into account internal vertices
  // it is necessary for internal edges, which do not split other edges, by their vertex
  TopExp_Explorer aVertexIt(myEdge, TopAbs_VERTEX);
  for (; aVertexIt.More(); aVertexIt.Next())
  {
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(aVertexIt.Current());
    if (aVertex.Orientation() != TopAbs_INTERNAL)
    {
      continue;
    }

    myDiscretTool.AddPoint(BRep_Tool::Pnt(aVertex),
                           BRep_Tool::Parameter(aVertex, myEdge),
                           Standard_True);
  }
}

//=================================================================================================

Standard_Boolean BRepMesh_CurveTessellator::isInToleranceOfVertex(
  const gp_Pnt&        thePoint,
  const TopoDS_Vertex& theVertex) const
{
  const gp_Pnt        aPoint     = BRep_Tool::Pnt(theVertex);
  const Standard_Real aTolerance = BRep_Tool::Tolerance(theVertex);

  return (thePoint.SquareDistance(aPoint) < aTolerance * aTolerance);
}

//=================================================================================================

Standard_Boolean BRepMesh_CurveTessellator::Value(const Standard_Integer theIndex,
                                                  gp_Pnt&                thePoint,
                                                  Standard_Real&         theParameter) const
{
  thePoint     = myDiscretTool.Value(theIndex);
  theParameter = myDiscretTool.Parameter(theIndex);

  /*if (!isInToleranceOfVertex(thePoint, myFirstVertex) &&
      !isInToleranceOfVertex(thePoint, myLastVertex))
  {*/
  if (!myCurve.IsCurveOnSurface())
  {
    return Standard_True;
  }

  // If point coordinates are out of surface range,
  // it is necessary to re-project point.
  const Adaptor3d_CurveOnSurface&  aCurve   = myCurve.CurveOnSurface();
  const Handle(Adaptor3d_Surface)& aSurface = aCurve.GetSurface();
  if (aSurface->GetType() != GeomAbs_BSplineSurface && aSurface->GetType() != GeomAbs_BezierSurface
      && aSurface->GetType() != GeomAbs_OtherSurface)
  {
    return Standard_True;
  }

  // Let skip periodic case.
  if (aSurface->IsUPeriodic() || aSurface->IsVPeriodic())
  {
    return Standard_True;
  }

  gp_Pnt2d aUV;
  aCurve.GetCurve()->D0(theParameter, aUV);
  // Point lies within the surface range - nothing to do.
  if (aUV.X() > myFaceRangeU[0] && aUV.X() < myFaceRangeU[1] && aUV.Y() > myFaceRangeV[0]
      && aUV.Y() < myFaceRangeV[1])
  {
    return Standard_True;
  }

  gp_Pnt aPntOnSurf;
  aSurface->D0(aUV.X(), aUV.Y(), aPntOnSurf);

  return (thePoint.SquareDistance(aPntOnSurf) < myEdgeSqTol);
  /*}

  return Standard_False;*/
}

//=================================================================================================

void BRepMesh_CurveTessellator::splitSegment(const Handle(Geom_Surface)& theSurf,
                                             const Handle(Geom2d_Curve)& theCurve2d,
                                             const Standard_Real         theFirst,
                                             const Standard_Real         theLast,
                                             const Standard_Integer      theNbIter)
{
  // limit iteration depth
  if (theNbIter > 10)
  {
    return;
  }

  gp_Pnt2d      uvf, uvl, uvm;
  gp_Pnt        P3dF, P3dL, midP3d, midP3dFromSurf;
  Standard_Real midpar;

  if (Abs(theLast - theFirst) < 2 * Precision::PConfusion())
  {
    return;
  }

  if ((theCurve2d->FirstParameter() - theFirst > Precision::PConfusion())
      || (theLast - theCurve2d->LastParameter() > Precision::PConfusion()))
  {
    // E.g. test bugs moddata_3 bug30133
    return;
  }

  theCurve2d->D0(theFirst, uvf);
  theCurve2d->D0(theLast, uvl);

  P3dF = theSurf->Value(uvf.X(), uvf.Y());
  P3dL = theSurf->Value(uvl.X(), uvl.Y());

  if (P3dF.SquareDistance(P3dL) < mySquareMinSize)
  {
    return;
  }

  uvm            = gp_Pnt2d((uvf.XY() + uvl.XY()) * 0.5);
  midP3dFromSurf = theSurf->Value(uvm.X(), uvm.Y());

  gp_XYZ Vec1 = midP3dFromSurf.XYZ() - P3dF.XYZ();
  if (Vec1.SquareModulus() < mySquareMinSize)
  {
    return;
  }

  gp_XYZ aVec = P3dL.XYZ() - P3dF.XYZ();
  aVec.Normalize();

  Standard_Real aModulus = Vec1.Dot(aVec);
  gp_XYZ        aProj    = aVec * aModulus;
  gp_XYZ        aDist    = Vec1 - aProj;

  if (aDist.SquareModulus() < mySquareEdgeDef)
  {
    return;
  }

  midpar = (theFirst + theLast) * 0.5;
  myCurve.D0(midpar, midP3d);
  myDiscretTool.AddPoint(midP3d, midpar, Standard_False);

  splitSegment(theSurf, theCurve2d, theFirst, midpar, theNbIter + 1);
  splitSegment(theSurf, theCurve2d, midpar, theLast, theNbIter + 1);
}
