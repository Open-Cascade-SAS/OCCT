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

#include <BRepMesh_ShapeTool.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_PCurve.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Precision.hxx>
#include <Bnd_Box.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_ShapeTool, Standard_Transient)

namespace
{
//! Auxiliary struct to take a tolerance of edge.
struct EdgeTolerance
{
  static double Get(const TopoDS_Shape& theEdge)
  {
    return BRep_Tool::Tolerance(TopoDS::Edge(theEdge));
  }
};

//! Auxiliary struct to take a tolerance of vertex.
struct VertexTolerance
{
  static double Get(const TopoDS_Shape& theVertex)
  {
    return BRep_Tool::Tolerance(TopoDS::Vertex(theVertex));
  }
};

//! Returns maximum tolerance of face element of the specified type.
template <TopAbs_ShapeEnum ShapeType, class ToleranceExtractor>
double MaxTolerance(const TopoDS_Face& theFace)
{
  double          aMaxTolerance = RealFirst();
  TopExp_Explorer aExplorer(theFace, ShapeType);
  for (; aExplorer.More(); aExplorer.Next())
  {
    double aTolerance = ToleranceExtractor::Get(aExplorer.Current());
    if (aTolerance > aMaxTolerance)
      aMaxTolerance = aTolerance;
  }

  return aMaxTolerance;
}
} // namespace

//=================================================================================================

double BRepMesh_ShapeTool::MaxFaceTolerance(const TopoDS_Face& theFace)
{
  double aMaxTolerance = BRep_Tool::Tolerance(theFace);

  double aTolerance = std::max(MaxTolerance<TopAbs_EDGE, EdgeTolerance>(theFace),
                               MaxTolerance<TopAbs_VERTEX, VertexTolerance>(theFace));

  return std::max(aMaxTolerance, aTolerance);
}

//=================================================================================================

void BRepMesh_ShapeTool::BoxMaxDimension(const Bnd_Box& theBox, double& theMaxDimension)
{
  if (theBox.IsVoid())
    return;

  double aMinX, aMinY, aMinZ, aMaxX, aMaxY, aMaxZ;
  theBox.Get(aMinX, aMinY, aMinZ, aMaxX, aMaxY, aMaxZ);

  theMaxDimension = std::max(aMaxX - aMinX, std::max(aMaxY - aMinY, aMaxZ - aMinZ));
}

//=================================================================================================

void BRepMesh_ShapeTool::CheckAndUpdateFlags(const IMeshData::IEdgeHandle&   theEdge,
                                             const IMeshData::IPCurveHandle& thePCurve)
{
  if (!theEdge->GetSameParam() && !theEdge->GetSameRange() && theEdge->GetDegenerated())
  {
    // Nothing to do worse.
    return;
  }

  const TopoDS_Edge& aEdge = theEdge->GetEdge();
  const TopoDS_Face& aFace = thePCurve->GetFace()->GetFace();

  occ::handle<Geom_Curve> aCurve;
  double                  aFirstParam, aLastParam;
  Range(aEdge, aCurve, aFirstParam, aLastParam);
  if (aCurve.IsNull())
  {
    theEdge->SetDegenerated(true);
    return;
  }

  BRepAdaptor_Curve aCurveOnSurf(aEdge, aFace);
  if (theEdge->GetSameParam() || theEdge->GetSameRange())
  {
    if (theEdge->GetSameRange())
    {
      const double aDiffFirst = aCurveOnSurf.FirstParameter() - aFirstParam;
      const double aDiffLast  = aCurveOnSurf.LastParameter() - aLastParam;
      theEdge->SetSameRange(std::abs(aDiffFirst) < Precision::PConfusion()
                            && std::abs(aDiffLast) < Precision::PConfusion());

      if (!theEdge->GetSameRange())
      {
        theEdge->SetSameParam(false);
      }
    }
  }

  if (!theEdge->GetDegenerated() /* || theEdge->GetSameParam ()*/)
  {
    TopoDS_Vertex aStartVertex, aEndVertex;
    TopExp::Vertices(aEdge, aStartVertex, aEndVertex);
    if (aStartVertex.IsNull() || aEndVertex.IsNull())
    {
      theEdge->SetDegenerated(true);
      return;
    }

    if (aStartVertex.IsSame(aEndVertex))
    {
      const int    aPointsNb        = 20;
      const double aVertexTolerance = BRep_Tool::Tolerance(aStartVertex);
      const double aDu              = (aLastParam - aFirstParam) / aPointsNb;
      // const double    aEdgeTolerance     = BRep_Tool::Tolerance (aEdge);
      // const double    aSqEdgeTolerance   = aEdgeTolerance * aEdgeTolerance;

      gp_Pnt aPrevPnt;
      aCurve->D0(aFirstParam, aPrevPnt);

      double aLength = 0.0;
      for (int i = 1; i <= aPointsNb; ++i)
      {
        const double aParameter = aFirstParam + i * aDu;
        // Calculation of the length of the edge in 3D
        // in order to check degenerativity
        gp_Pnt aPnt;
        aCurve->D0(aParameter, aPnt);
        aLength += aPrevPnt.Distance(aPnt);

        // if (theEdge->GetSameParam ())
        //{
        //   // Check that points taken at the 3d and pcurve using
        //   // same parameter are within tolerance of an edge.
        //   gp_Pnt aPntOnSurf;
        //   aCurveOnSurf.D0 (aParameter, aPntOnSurf);
        //   theEdge->SetSameParam (aPnt.SquareDistance (aPntOnSurf) < aSqEdgeTolerance);
        // }

        if (aLength > aVertexTolerance /*&& !theEdge->GetSameParam()*/)
        {
          break;
        }

        aPrevPnt = aPnt;
      }

      theEdge->SetDegenerated(aLength < aVertexTolerance);
    }
  }
}

//=================================================================================================

void BRepMesh_ShapeTool::AddInFace(const TopoDS_Face&               theFace,
                                   occ::handle<Poly_Triangulation>& theTriangulation)
{
  const TopLoc_Location& aLoc = theFace.Location();
  if (!aLoc.IsIdentity())
  {
    gp_Trsf aTrsf = aLoc.Transformation();
    aTrsf.Invert();
    for (int aNodeIter = 1; aNodeIter <= theTriangulation->NbNodes(); ++aNodeIter)
    {
      gp_Pnt aNode = theTriangulation->Node(aNodeIter);
      aNode.Transform(aTrsf);
      theTriangulation->SetNode(aNodeIter, aNode);
    }
  }

  BRep_Builder aBuilder;
  aBuilder.UpdateFace(theFace, theTriangulation);
}

//=================================================================================================

void BRepMesh_ShapeTool::NullifyFace(const TopoDS_Face& theFace)
{
  BRep_Builder aBuilder;
  aBuilder.UpdateFace(theFace, occ::handle<Poly_Triangulation>());
}

//=================================================================================================

void BRepMesh_ShapeTool::NullifyEdge(const TopoDS_Edge&                     theEdge,
                                     const occ::handle<Poly_Triangulation>& theTriangulation,
                                     const TopLoc_Location&                 theLocation)
{
  UpdateEdge(theEdge, occ::handle<Poly_PolygonOnTriangulation>(), theTriangulation, theLocation);
}

//=================================================================================================

void BRepMesh_ShapeTool::NullifyEdge(const TopoDS_Edge& theEdge, const TopLoc_Location& theLocation)
{
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(theEdge, occ::handle<Poly_Polygon3D>(), theLocation);
}

//=================================================================================================

void BRepMesh_ShapeTool::UpdateEdge(const TopoDS_Edge&                              theEdge,
                                    const occ::handle<Poly_PolygonOnTriangulation>& thePolygon,
                                    const occ::handle<Poly_Triangulation>& theTriangulation,
                                    const TopLoc_Location&                 theLocation)
{
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(theEdge, thePolygon, theTriangulation, theLocation);
}

//=================================================================================================

void BRepMesh_ShapeTool::UpdateEdge(const TopoDS_Edge&                              theEdge,
                                    const occ::handle<Poly_PolygonOnTriangulation>& thePolygon1,
                                    const occ::handle<Poly_PolygonOnTriangulation>& thePolygon2,
                                    const occ::handle<Poly_Triangulation>& theTriangulation,
                                    const TopLoc_Location&                 theLocation)
{
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(theEdge, thePolygon1, thePolygon2, theTriangulation, theLocation);
}

//=================================================================================================

void BRepMesh_ShapeTool::UpdateEdge(const TopoDS_Edge&                 theEdge,
                                    const occ::handle<Poly_Polygon3D>& thePolygon)
{
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(theEdge, thePolygon);
}

//=================================================================================================

gp_Pnt BRepMesh_ShapeTool::UseLocation(const gp_Pnt& thePnt, const TopLoc_Location& theLoc)
{
  if (theLoc.IsIdentity())
  {
    return thePnt;
  }

  return thePnt.Transformed(theLoc.Transformation());
}

//=================================================================================================

bool BRepMesh_ShapeTool::UVPoints(const TopoDS_Edge& theEdge,
                                  const TopoDS_Face& theFace,
                                  gp_Pnt2d&          theFirstPoint2d,
                                  gp_Pnt2d&          theLastPoint2d,
                                  const bool         isConsiderOrientation)
{

  occ::handle<Geom2d_Curve> aCurve2d;
  double                    aFirstParam, aLastParam;
  if (!Range(theEdge, theFace, aCurve2d, aFirstParam, aLastParam, isConsiderOrientation))
  {
    return false;
  }

  aCurve2d->D0(aFirstParam, theFirstPoint2d);
  aCurve2d->D0(aLastParam, theLastPoint2d);
  return true;
}

//=================================================================================================

bool BRepMesh_ShapeTool::Range(const TopoDS_Edge&         theEdge,
                               const TopoDS_Face&         theFace,
                               occ::handle<Geom2d_Curve>& thePCurve,
                               double&                    theFirstParam,
                               double&                    theLastParam,
                               const bool                 isConsiderOrientation)
{

  ShapeAnalysis_Edge aEdge;
  return aEdge
    .PCurve(theEdge, theFace, thePCurve, theFirstParam, theLastParam, isConsiderOrientation);
}

//=================================================================================================

bool BRepMesh_ShapeTool::Range(const TopoDS_Edge&       theEdge,
                               occ::handle<Geom_Curve>& theCurve,
                               double&                  theFirstParam,
                               double&                  theLastParam,
                               const bool               isConsiderOrientation)
{

  ShapeAnalysis_Edge aEdge;
  return aEdge.Curve3d(theEdge, theCurve, theFirstParam, theLastParam, isConsiderOrientation);
}
