// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraphAlgo_BndLib.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <BRepGProp.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_OBB.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <gp_Ax3.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>

namespace
{

//! Returns true if the curve is line-like.
static bool isLinear(const Adaptor3d_Curve& theC)
{
  const GeomAbs_CurveType aCT = theC.GetType();
  if (aCT == GeomAbs_OffsetCurve)
  {
    return isLinear(GeomAdaptor_Curve(theC.OffsetCurve()->BasisCurve()));
  }
  if ((aCT == GeomAbs_BSplineCurve) || (aCT == GeomAbs_BezierCurve))
  {
    return ((theC.Degree() == 1) && (theC.Continuity() != GeomAbs_C0));
  }
  if (aCT == GeomAbs_Line)
  {
    return true;
  }
  return false;
}

//! Returns true if the surface is plane-like.
static bool isPlanar(const Adaptor3d_Surface& theS)
{
  const GeomAbs_SurfaceType aST = theS.GetType();
  if (aST == GeomAbs_OffsetSurface)
  {
    return isPlanar(*theS.BasisSurface());
  }
  if (aST == GeomAbs_SurfaceOfExtrusion)
  {
    return isLinear(*theS.BasisCurve());
  }
  if ((aST == GeomAbs_BSplineSurface) || (aST == GeomAbs_BezierSurface))
  {
    if ((theS.UDegree() != 1) || (theS.VDegree() != 1))
    {
      return false;
    }
    return ((theS.UContinuity() != GeomAbs_C0) && (theS.VContinuity() != GeomAbs_C0));
  }
  if (aST == GeomAbs_Plane)
  {
    return true;
  }
  return false;
}

//! Collect points for OBB construction from graph data.
//! Returns number of points. If thePts is not null, fills the arrays.
static int pointsForOBB(const BRepGraph&            theGraph,
                        bool                        theIsTriangulationUsed,
                        NCollection_Array1<gp_Pnt>* thePts        = nullptr,
                        NCollection_Array1<double>* theArrOfToler = nullptr)
{
  int aRetVal = 0;

  // Collect all vertex points.
  const int aNbVerts = theGraph.Defs().NbVertices();
  for (int i = 0; i < aNbVerts; ++i)
  {
    if (thePts != nullptr)
    {
      (*thePts)(aRetVal) = BRepGraph_Tool::Vertex::Pnt(theGraph, i);
    }
    if (theArrOfToler != nullptr)
    {
      (*theArrOfToler)(aRetVal) = BRepGraph_Tool::Vertex::Tolerance(theGraph, i);
    }
    ++aRetVal;
  }

  if (aRetVal == 0)
  {
    return 0;
  }

  // Analyze faces for planarity and triangulation.
  const int aNbFaces = theGraph.Defs().NbFaces();
  for (int aFaceIdx = 0; aFaceIdx < aNbFaces; ++aFaceIdx)
  {
    if (!BRepGraph_Tool::Face::HasSurface(theGraph, aFaceIdx))
    {
      continue;
    }

    const GeomAdaptor_TransformedSurface aGAS =
      BRepGraph_Tool::Face::SurfaceAdaptor(theGraph, aFaceIdx);
    if (!isPlanar(aGAS))
    {
      if (!theIsTriangulationUsed)
      {
        return 0;
      }
    }
    else
    {
      // Planar face: check if all edges are linear.
      bool                               hasNonLinearEdge = false;
      const BRepGraph_TopoNode::FaceDef& aFaceDef         = theGraph.Defs().Face(aFaceIdx);
      for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length() && !hasNonLinearEdge;
           ++aWireRefIdx)
      {
        const BRepGraphInc::WireRef&       aWR      = aFaceDef.WireRefs.Value(aWireRefIdx);
        const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.Defs().Wire(aWR.WireIdx);
        for (int anIdx = 0; anIdx < aWireDef.CoEdgeRefs.Length(); ++anIdx)
        {
          const BRepGraphInc::CoEdgeRef&       aCR       = aWireDef.CoEdgeRefs.Value(anIdx);
          const BRepGraph_TopoNode::CoEdgeDef& aCoEdge   = theGraph.Defs().CoEdge(aCR.CoEdgeIdx);
          const int                            anEdgeIdx = aCoEdge.EdgeIdx;
          if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeIdx)
              || !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeIdx))
          {
            continue;
          }
          const GeomAdaptor_TransformedCurve aCurveAdaptor =
            BRepGraph_Tool::Edge::CurveAdaptor(theGraph, anEdgeIdx);
          if (!isLinear(aCurveAdaptor))
          {
            hasNonLinearEdge = true;
            break;
          }
        }
      }

      if (!hasNonLinearEdge)
      {
        // Skip planar face with linear edges - vertices already added.
        continue;
      }

      if (!theIsTriangulationUsed)
      {
        return 0;
      }
    }

    // Use active triangulation of the face.
    if (!BRepGraph_Tool::Face::HasTriangulation(theGraph, aFaceIdx))
    {
      return 0;
    }
    const occ::handle<Poly_Triangulation>& aTriangulation =
      BRepGraph_Tool::Face::Triangulation(theGraph, aFaceIdx);
    if (aTriangulation.IsNull())
    {
      return 0;
    }

    const int aCNode = aTriangulation->NbNodes();
    for (int i = 1; i <= aCNode; ++i)
    {
      if (thePts != nullptr)
      {
        (*thePts)(aRetVal) = aTriangulation->Node(i);
      }
      if (theArrOfToler != nullptr)
      {
        (*theArrOfToler)(aRetVal) = aTriangulation->Deflection();
      }
      ++aRetVal;
    }
  }

  // Consider free edges (not in any face).
  const int aNbEdges = theGraph.Defs().NbEdges();
  for (int i = 0; i < aNbEdges; ++i)
  {
    if (theGraph.Defs().FaceCountOfEdge(i) > 0)
    {
      continue; // Edge is in a face, already handled.
    }
    if (BRepGraph_Tool::Edge::Degenerated(theGraph, i)
        || !BRepGraph_Tool::Edge::HasCurve(theGraph, i))
    {
      continue;
    }

    const GeomAdaptor_TransformedCurve aCurveAdaptor =
      BRepGraph_Tool::Edge::CurveAdaptor(theGraph, i);
    if (isLinear(aCurveAdaptor))
    {
      // Skip linear edge - vertices already added.
      continue;
    }

    if (!theIsTriangulationUsed)
    {
      return 0;
    }

    // For free edges, we need polygon data. Reconstruct the edge to get Polygon3D.
    // This is a rare case (free non-linear edge without face context).
    // For simplicity, sample the curve at N points.
    constexpr int THE_N_SAMPLES = 20;
    const double  aFirst        = aCurveAdaptor.FirstParameter();
    const double  aLast         = aCurveAdaptor.LastParameter();
    const double  aStep         = (aLast - aFirst) / THE_N_SAMPLES;
    for (int j = 0; j <= THE_N_SAMPLES; ++j)
    {
      if (thePts != nullptr)
      {
        const double aParam = aFirst + j * aStep;
        (*thePts)(aRetVal)  = aCurveAdaptor.Value(aParam);
      }
      if (theArrOfToler != nullptr)
      {
        (*theArrOfToler)(aRetVal) = BRepGraph_Tool::Edge::Tolerance(theGraph, i);
      }
      ++aRetVal;
    }
  }

  return aRetVal;
}

//! Try to build OBB from collected points (DiTO algorithm).
static bool checkPoints(const BRepGraph& theGraph,
                        bool             theIsTriangulationUsed,
                        bool             theIsOptimal,
                        bool             theIsShapeToleranceUsed,
                        Bnd_OBB&         theOBB)
{
  const int aNbPnts = pointsForOBB(theGraph, theIsTriangulationUsed);
  if (aNbPnts < 1)
  {
    return false;
  }

  NCollection_Array1<gp_Pnt> anArrPnts(0, theOBB.IsVoid() ? aNbPnts - 1 : aNbPnts + 7);
  NCollection_Array1<double> anArrOfTolerances;
  if (theIsShapeToleranceUsed)
  {
    anArrOfTolerances.Resize(anArrPnts.Lower(), anArrPnts.Upper(), false);
    anArrOfTolerances.Init(0.0);
  }

  NCollection_Array1<double>* aPtrArrTol = theIsShapeToleranceUsed ? &anArrOfTolerances : nullptr;

  pointsForOBB(theGraph, theIsTriangulationUsed, &anArrPnts, aPtrArrTol);

  if (!theOBB.IsVoid())
  {
    theOBB.GetVertex(&anArrPnts(aNbPnts));
  }

  theOBB.ReBuild(anArrPnts, aPtrArrTol, theIsOptimal);

  return !theOBB.IsVoid();
}

//! Returns 0 if the direction does not match any axis of WCS.
//! Otherwise, returns the index of correspond axis.
static int isWCS(const gp_Dir& theDir)
{
  constexpr double aToler = Precision::Angular() * Precision::Angular();

  const double aX = theDir.X(), aY = theDir.Y(), aZ = theDir.Z();
  const double aVx = aY * aY + aZ * aZ;
  const double aVy = aX * aX + aZ * aZ;
  const double aVz = aX * aX + aY * aY;

  if (aVz < aToler)
    return 3;
  if (aVy < aToler)
    return 2;
  if (aVx < aToler)
    return 1;
  return 0;
}

//! Compute mass properties from graph data using BRepGProp.
static void computeProperties(const BRepGraph& theGraph, GProp_GProps& theGCommon)
{
  // For solids: volume properties.
  const int aNbSolids = theGraph.Defs().NbSolids();
  for (int i = 0; i < aNbSolids; ++i)
  {
    const BRepGraph_NodeId aSolidId = BRepGraph_NodeId::Solid(i);
    const TopoDS_Shape     aShape   = theGraph.Shapes().Shape(aSolidId);
    GProp_GProps           aG;
    BRepGProp::VolumeProperties(aShape, aG, true);
    theGCommon.Add(aG);
  }

  // For faces not in solids: surface properties.
  const int aNbFaces = theGraph.Defs().NbFaces();
  for (int i = 0; i < aNbFaces; ++i)
  {
    const TopoDS_Shape aFaceShape = theGraph.Shapes().ReconstructFace(i);
    GProp_GProps       aG;
    BRepGProp::SurfaceProperties(aFaceShape, aG, true);
    theGCommon.Add(aG);
  }

  // For free vertices.
  const int aNbVerts = theGraph.Defs().NbVertices();
  for (int i = 0; i < aNbVerts; ++i)
  {
    GProp_GProps aG(BRepGraph_Tool::Vertex::Pnt(theGraph, i));
    theGCommon.Add(aG);
  }
}

//! PCA fallback for OBB computation.
static void computePCA(const BRepGraph& theGraph,
                       Bnd_OBB&         theOBB,
                       bool             theIsTriangulationUsed,
                       bool             theIsOptimal,
                       bool             theIsShapeToleranceUsed)
{
  GProp_GProps aGCommon;
  computeProperties(theGraph, aGCommon);

  gp_Trsf aTrsf;

  const int anIdx1 = isWCS(aGCommon.PrincipalProperties().FirstAxisOfInertia());
  const int anIdx2 = isWCS(aGCommon.PrincipalProperties().SecondAxisOfInertia());

  if ((anIdx1 == 0) || (anIdx2 == 0))
  {
    gp_Ax3 aLocCoordSys(aGCommon.CentreOfMass(),
                        aGCommon.PrincipalProperties().ThirdAxisOfInertia(),
                        aGCommon.PrincipalProperties().FirstAxisOfInertia());
    aTrsf.SetTransformation(aLocCoordSys);
  }

  // Compute AABB in the PCA frame.
  // Reconstruct the root shape, transform, and compute AABB.
  // Use the root node (first compound or first solid or first face).
  Bnd_Box aShapeBox;
  if (aTrsf.Form() == gp_Identity)
  {
    if (theIsOptimal)
    {
      BRepGraphAlgo_BndLib::AddOptimal(theGraph,
                                       aShapeBox,
                                       theIsTriangulationUsed,
                                       theIsShapeToleranceUsed);
    }
    else
    {
      BRepGraphAlgo_BndLib::Add(theGraph, aShapeBox);
    }
  }
  else
  {
    // Need to reconstruct shape, apply transform, and compute bbox.
    // Find the root node.
    TopoDS_Shape aRootShape;
    if (theGraph.Defs().NbCompounds() > 0)
    {
      aRootShape = theGraph.Shapes().Shape(BRepGraph_NodeId::Compound(0));
    }
    else if (theGraph.Defs().NbCompSolids() > 0)
    {
      aRootShape = theGraph.Shapes().Shape(BRepGraph_NodeId::CompSolid(0));
    }
    else if (theGraph.Defs().NbSolids() > 0)
    {
      aRootShape = theGraph.Shapes().Shape(BRepGraph_NodeId::Solid(0));
    }
    else if (theGraph.Defs().NbShells() > 0)
    {
      aRootShape = theGraph.Shapes().Shape(BRepGraph_NodeId::Shell(0));
    }
    else if (theGraph.Defs().NbFaces() > 0)
    {
      aRootShape = theGraph.Shapes().ReconstructFace(0);
    }

    if (!aRootShape.IsNull())
    {
      const TopoDS_Shape aST = aRootShape.Moved(TopLoc_Location(aTrsf));
      // Build a temporary graph for the transformed shape.
      // For simplicity, use BRepBndLib-compatible approach:
      // reconstruct shape, transform, compute bbox.
      BRepGraph aTmpGraph;
      aTmpGraph.Build(aST);
      if (aTmpGraph.IsDone())
      {
        if (theIsOptimal)
        {
          BRepGraphAlgo_BndLib::AddOptimal(aTmpGraph,
                                           aShapeBox,
                                           theIsTriangulationUsed,
                                           theIsShapeToleranceUsed);
        }
        else
        {
          BRepGraphAlgo_BndLib::Add(aTmpGraph, aShapeBox);
        }
      }
    }
  }

  if (aShapeBox.IsVoid())
  {
    return;
  }

  const gp_Pnt aPMin = aShapeBox.CornerMin();
  const gp_Pnt aPMax = aShapeBox.CornerMax();

  gp_XYZ aXDir(1, 0, 0);
  gp_XYZ aYDir(0, 1, 0);
  gp_XYZ aZDir(0, 0, 1);

  gp_XYZ aCenter    = (aPMin.XYZ() + aPMax.XYZ()) / 2.;
  gp_XYZ anOBBHSize = (aPMax.XYZ() - aPMin.XYZ()) / 2.;

  if (aTrsf.Form() != gp_Identity)
  {
    aTrsf.Invert();
    aTrsf.Transforms(aCenter);

    const double* aMat = &aTrsf.HVectorialPart().Value(1, 1);
    aXDir              = gp_XYZ(aMat[0], aMat[3], aMat[6]);
    aYDir              = gp_XYZ(aMat[1], aMat[4], aMat[7]);
    aZDir              = gp_XYZ(aMat[2], aMat[5], aMat[8]);
  }

  if (theOBB.IsVoid())
  {
    theOBB.SetCenter(aCenter);
    theOBB.SetXComponent(aXDir, anOBBHSize.X());
    theOBB.SetYComponent(aYDir, anOBBHSize.Y());
    theOBB.SetZComponent(aZDir, anOBBHSize.Z());
    theOBB.SetAABox(aTrsf.Form() == gp_Identity);
  }
  else
  {
    NCollection_Array1<gp_Pnt> aListOfPnts(0, 15);
    theOBB.GetVertex(&aListOfPnts(0));

    const double aX    = anOBBHSize.X();
    const double aY    = anOBBHSize.Y();
    const double aZ    = anOBBHSize.Z();
    const gp_XYZ aXext = aX * aXDir, aYext = aY * aYDir, aZext = aZ * aZDir;

    int aPntIdx            = 8;
    aListOfPnts(aPntIdx++) = aCenter - aXext - aYext - aZext;
    aListOfPnts(aPntIdx++) = aCenter + aXext - aYext - aZext;
    aListOfPnts(aPntIdx++) = aCenter - aXext + aYext - aZext;
    aListOfPnts(aPntIdx++) = aCenter + aXext + aYext - aZext;
    aListOfPnts(aPntIdx++) = aCenter - aXext - aYext + aZext;
    aListOfPnts(aPntIdx++) = aCenter + aXext - aYext + aZext;
    aListOfPnts(aPntIdx++) = aCenter - aXext + aYext + aZext;
    aListOfPnts(aPntIdx++) = aCenter + aXext + aYext + aZext;

    theOBB.ReBuild(aListOfPnts);
  }
}

} // namespace

//=================================================================================================

void BRepGraphAlgo_BndLib::AddOBB(const BRepGraph& theGraph,
                                  Bnd_OBB&         theOBB,
                                  bool             theIsTriangulationUsed,
                                  bool             theIsOptimal,
                                  bool             theIsShapeToleranceUsed)
{
  if (checkPoints(theGraph, theIsTriangulationUsed, theIsOptimal, theIsShapeToleranceUsed, theOBB))
  {
    return;
  }

  computePCA(theGraph, theOBB, theIsTriangulationUsed, theIsOptimal, theIsShapeToleranceUsed);
}
