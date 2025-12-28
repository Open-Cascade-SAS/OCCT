// Created on: 2016-10-14
// Created by: Alexander MALYSHEV
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2016 OPEN CASCADE SAS
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

// Include self.
#include <BRepOffset_SimpleOffset.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepLib.hxx>
#include <BRepLib_ValidateEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepOffset.hxx>
#include <Geom_OffsetSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <NCollection_Vector.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

BRepOffset_SimpleOffset::BRepOffset_SimpleOffset(const TopoDS_Shape& theInputShape,
                                                 const double theOffsetValue,
                                                 const double theTolerance)
    : myOffsetValue(theOffsetValue),
      myTolerance(theTolerance)
{
  FillOffsetData(theInputShape);
}

//=================================================================================================

bool BRepOffset_SimpleOffset::NewSurface(const TopoDS_Face&    F,
                                                     occ::handle<Geom_Surface>& S,
                                                     TopLoc_Location&      L,
                                                     double&        Tol,
                                                     bool&     RevWires,
                                                     bool&     RevFace)
{
  if (!myFaceInfo.IsBound(F))
    return false;

  const NewFaceData& aNFD = myFaceInfo.Find(F);

  S        = aNFD.myOffsetS;
  L        = aNFD.myL;
  Tol      = aNFD.myTol;
  RevWires = aNFD.myRevWires;
  RevFace  = aNFD.myRevFace;

  return true;
}

//=================================================================================================

bool BRepOffset_SimpleOffset::NewCurve(const TopoDS_Edge&  E,
                                                   occ::handle<Geom_Curve>& C,
                                                   TopLoc_Location&    L,
                                                   double&      Tol)
{
  if (!myEdgeInfo.IsBound(E))
    return false;

  const NewEdgeData& aNED = myEdgeInfo.Find(E);

  C   = aNED.myOffsetC;
  L   = aNED.myL;
  Tol = aNED.myTol;

  return true;
}

//=================================================================================================

bool BRepOffset_SimpleOffset::NewPoint(const TopoDS_Vertex& V,
                                                   gp_Pnt&              P,
                                                   double&       Tol)
{
  if (!myVertexInfo.IsBound(V))
    return false;

  const NewVertexData& aNVD = myVertexInfo.Find(V);

  P   = aNVD.myP;
  Tol = aNVD.myTol;

  return true;
}

//=================================================================================================

bool BRepOffset_SimpleOffset::NewCurve2d(const TopoDS_Edge& E,
                                                     const TopoDS_Face& F,
                                                     const TopoDS_Edge& /*NewE*/,
                                                     const TopoDS_Face& /*NewF*/,
                                                     occ::handle<Geom2d_Curve>& C,
                                                     double&        Tol)
{
  // Use original pcurve.
  double aF, aL;
  C   = BRep_Tool::CurveOnSurface(E, F, aF, aL);
  Tol = BRep_Tool::Tolerance(E);

  if (myEdgeInfo.IsBound(E))
    Tol = myEdgeInfo.Find(E).myTol;

  return true;
}

//=================================================================================================

bool BRepOffset_SimpleOffset::NewParameter(const TopoDS_Vertex& V,
                                                       const TopoDS_Edge&   E,
                                                       double&       P,
                                                       double&       Tol)
{
  // Use original parameter.
  P   = BRep_Tool::Parameter(V, E);
  Tol = BRep_Tool::Tolerance(V);

  if (myVertexInfo.IsBound(V))
    Tol = myVertexInfo.Find(V).myTol;

  return true;
}

//=================================================================================================

GeomAbs_Shape BRepOffset_SimpleOffset::Continuity(const TopoDS_Edge& E,
                                                  const TopoDS_Face& F1,
                                                  const TopoDS_Face& F2,
                                                  const TopoDS_Edge& /*NewE*/,
                                                  const TopoDS_Face& /*NewF1*/,
                                                  const TopoDS_Face& /*NewF2*/)
{
  // Compute result using original continuity.
  return BRep_Tool::Continuity(E, F1, F2);
}

//=================================================================================================

void BRepOffset_SimpleOffset::FillOffsetData(const TopoDS_Shape& theShape)
{
  // Clears old data.
  myFaceInfo.Clear();
  myEdgeInfo.Clear();
  myVertexInfo.Clear();

  // Faces loop. Compute offset surface for each face.
  TopExp_Explorer anExpSF(theShape, TopAbs_FACE);
  for (; anExpSF.More(); anExpSF.Next())
  {
    const TopoDS_Face& aCurrFace = TopoDS::Face(anExpSF.Current());
    FillFaceData(aCurrFace);
  }

  // Iterate over edges to compute 3d curve.
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aEdgeFaceMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, aEdgeFaceMap);
  for (int anIdx = 1; anIdx <= aEdgeFaceMap.Size(); ++anIdx)
  {
    const TopoDS_Edge& aCurrEdge = TopoDS::Edge(aEdgeFaceMap.FindKey(anIdx));
    FillEdgeData(aCurrEdge, aEdgeFaceMap, anIdx);
  }

  // Iterate over vertices to compute new vertex.
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aVertexEdgeMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_VERTEX, TopAbs_EDGE, aVertexEdgeMap);
  for (int anIdx = 1; anIdx <= aVertexEdgeMap.Size(); ++anIdx)
  {
    const TopoDS_Vertex& aCurrVertex = TopoDS::Vertex(aVertexEdgeMap.FindKey(anIdx));
    FillVertexData(aCurrVertex, aVertexEdgeMap, anIdx);
  }
}

//=================================================================================================

void BRepOffset_SimpleOffset::FillFaceData(const TopoDS_Face& theFace)
{
  NewFaceData aNFD;
  aNFD.myRevWires = false;
  aNFD.myRevFace  = false;
  aNFD.myTol      = BRep_Tool::Tolerance(theFace);

  // Create offset surface.

  // Any existing transformation is applied to the surface.
  // New face will have null transformation.
  occ::handle<Geom_Surface> aS = BRep_Tool::Surface(theFace);
  aS                      = BRepOffset::CollapseSingularities(aS, theFace, myTolerance);

  // Take into account face orientation.
  double aMult = 1.0;
  if (theFace.Orientation() == TopAbs_REVERSED)
    aMult = -1.0;

  BRepOffset_Status aStatus; // set by BRepOffset::Surface(), could be used to check result...
  aNFD.myOffsetS = BRepOffset::Surface(aS, aMult * myOffsetValue, aStatus, true);
  aNFD.myL       = TopLoc_Location(); // Null transformation.

  // Save offset surface in map.
  myFaceInfo.Bind(theFace, aNFD);
}

//=================================================================================================

void BRepOffset_SimpleOffset::FillEdgeData(
  const TopoDS_Edge&                               theEdge,
  const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theEdgeFaceMap,
  const int                           theIdx)
{
  const NCollection_List<TopoDS_Shape>& aFacesList = theEdgeFaceMap(theIdx);

  if (aFacesList.Size() == 0)
    return; // Free edges are skipped.

  // Get offset surface.
  const TopoDS_Face& aCurrFace = TopoDS::Face(aFacesList.First());

  if (!myFaceInfo.IsBound(aCurrFace))
    return;

  // No need to deal with transformation - it is applied in fill faces data method.
  const NewFaceData&   aNFD         = myFaceInfo.Find(aCurrFace);
  occ::handle<Geom_Surface> anOffsetSurf = aNFD.myOffsetS;

  // Compute offset 3d curve.
  double        aF, aL;
  occ::handle<Geom2d_Curve> aC2d = BRep_Tool::CurveOnSurface(theEdge, aCurrFace, aF, aL);

  BRepBuilderAPI_MakeEdge anEdgeMaker(aC2d, anOffsetSurf, aF, aL);
  TopoDS_Edge             aNewEdge = anEdgeMaker.Edge();

  // Compute max tolerance. Vertex tolerance usage is taken from existing offset computation
  // algorithm. This piece of code significantly influences resulting performance.
  double aTol = BRep_Tool::MaxTolerance(theEdge, TopAbs_VERTEX);
  BRepLib::BuildCurves3d(aNewEdge, aTol);

  NewEdgeData aNED;
  aNED.myOffsetC = BRep_Tool::Curve(aNewEdge, aNED.myL, aF, aL);

  // Iterate over adjacent faces for the current edge and compute max deviation.
  double                  anEdgeTol = 0.0;
  NCollection_List<TopoDS_Shape>::Iterator anIter(aFacesList);
  for (; !aNED.myOffsetC.IsNull() && anIter.More(); anIter.Next())
  {
    const TopoDS_Face& aCurFace = TopoDS::Face(anIter.Value());

    if (!myFaceInfo.IsBound(aCurFace))
      continue;

    // Create offset curve on surface.
    const occ::handle<Geom2d_Curve>      aC2dNew = BRep_Tool::CurveOnSurface(theEdge, aCurFace, aF, aL);
    const occ::handle<Adaptor2d_Curve2d> aHCurve2d = new Geom2dAdaptor_Curve(aC2dNew, aF, aL);
    const occ::handle<Adaptor3d_Surface> aHSurface =
      new GeomAdaptor_Surface(myFaceInfo.Find(aCurFace).myOffsetS);
    const occ::handle<Adaptor3d_CurveOnSurface> aCurveOnSurf =
      new Adaptor3d_CurveOnSurface(aHCurve2d, aHSurface);

    // Extract 3d-curve (it is not null).
    const occ::handle<Adaptor3d_Curve> aCurve3d = new GeomAdaptor_Curve(aNED.myOffsetC, aF, aL);

    // It is necessary to compute maximal deviation (tolerance).
    BRepLib_ValidateEdge aValidateEdge(aCurve3d, aCurveOnSurf, true);
    aValidateEdge.Process();
    if (aValidateEdge.IsDone())
    {
      double aMaxTol1 = aValidateEdge.GetMaxDistance();
      anEdgeTol              = std::max(anEdgeTol, aMaxTol1);
    }
  }
  aNED.myTol = std::max(BRep_Tool::Tolerance(aNewEdge), anEdgeTol);

  // Save computed 3d curve in map.
  myEdgeInfo.Bind(theEdge, aNED);
}

//=================================================================================================

void BRepOffset_SimpleOffset::FillVertexData(
  const TopoDS_Vertex&                             theVertex,
  const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theVertexEdgeMap,
  const int                           theIdx)
{
  // Algorithm:
  // Find adjacent edges for the given vertex.
  // Find corresponding end on the each adjacent edge.
  // Get offset points for founded end.
  // Set result vertex position as barycenter of founded points.

  gp_Pnt aCurrPnt = BRep_Tool::Pnt(theVertex);

  const NCollection_List<TopoDS_Shape>& aEdgesList = theVertexEdgeMap(theIdx);

  if (aEdgesList.Size() == 0)
    return; // Free verices are skipped.

  // Array to store offset points.
  NCollection_Vector<gp_Pnt> anOffsetPointVec;

  double aMaxEdgeTol = 0.0;

  // Iterate over adjacent edges.
  NCollection_List<TopoDS_Shape>::Iterator anIterEdges(aEdgesList);
  for (; anIterEdges.More(); anIterEdges.Next())
  {
    const TopoDS_Edge& aCurrEdge = TopoDS::Edge(anIterEdges.Value());

    if (!myEdgeInfo.IsBound(aCurrEdge))
      continue; // Skip shared edges with wrong orientation.

    // Find the closest bound.
    double      aF, aL;
    occ::handle<Geom_Curve> aC3d = BRep_Tool::Curve(aCurrEdge, aF, aL);

    // Protection from degenerated edges.
    if (aC3d.IsNull())
      continue;

    const gp_Pnt aPntF = aC3d->Value(aF);
    const gp_Pnt aPntL = aC3d->Value(aL);

    const double aSqDistF = aPntF.SquareDistance(aCurrPnt);
    const double aSqDistL = aPntL.SquareDistance(aCurrPnt);

    double aMinParam = aF, aMaxParam = aL;
    if (aSqDistL < aSqDistF)
    {
      // Square distance to last point is closer.
      aMinParam = aL;
      aMaxParam = aF;
    }

    // Compute point on offset edge.
    const NewEdgeData&        aNED          = myEdgeInfo.Find(aCurrEdge);
    const occ::handle<Geom_Curve>& anOffsetCurve = aNED.myOffsetC;
    const gp_Pnt              anOffsetPoint = anOffsetCurve->Value(aMinParam);
    anOffsetPointVec.Append(anOffsetPoint);

    // Handle situation when edge is closed.
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(aCurrEdge, aV1, aV2);
    if (aV1.IsSame(aV2))
    {
      const gp_Pnt anOffsetPointLast = anOffsetCurve->Value(aMaxParam);
      anOffsetPointVec.Append(anOffsetPointLast);
    }

    aMaxEdgeTol = std::max(aMaxEdgeTol, aNED.myTol);
  }

  // NCollection_Vector starts from 0 by default.
  // It's better to use lower() and upper() in this case instead of direct indexes range.
  gp_Pnt aCenter(0.0, 0.0, 0.0);
  for (int i = anOffsetPointVec.Lower(); i <= anOffsetPointVec.Upper(); ++i)
  {
    aCenter.SetXYZ(aCenter.XYZ() + anOffsetPointVec.Value(i).XYZ());
  }
  aCenter.SetXYZ(aCenter.XYZ() / anOffsetPointVec.Size());

  // Compute max distance.
  double aSqMaxDist = 0.0;
  for (int i = anOffsetPointVec.Lower(); i <= anOffsetPointVec.Upper(); ++i)
  {
    const double aSqDist = aCenter.SquareDistance(anOffsetPointVec.Value(i));
    if (aSqDist > aSqMaxDist)
      aSqMaxDist = aSqDist;
  }

  const double aResTol = std::max(aMaxEdgeTol, std::sqrt(aSqMaxDist));

  const double aMultCoeff = 1.001; // Avoid tolernace problems.
  NewVertexData       aNVD;
  aNVD.myP   = aCenter;
  aNVD.myTol = aResTol * aMultCoeff;

  // Save computed vertex info.
  myVertexInfo.Bind(theVertex, aNVD);
}
