// Copyright (c) 1999-2022 OPEN CASCADE SAS
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

#include <BRepTools_CopyModification.hxx>

#include <BRep_Tool.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTools_CopyModification, BRepTools_Modification)

//=================================================================================================

BRepTools_CopyModification::BRepTools_CopyModification(const bool copyGeom,
                                                       const bool copyMesh)
    : myCopyGeom(copyGeom),
      myCopyMesh(copyMesh)
{
}

//=================================================================================================

bool BRepTools_CopyModification::NewSurface(const TopoDS_Face&    theFace,
                                                        occ::handle<Geom_Surface>& theSurf,
                                                        TopLoc_Location&      theLoc,
                                                        double&        theTol,
                                                        bool&     theRevWires,
                                                        bool&     theRevFace)
{
  theSurf     = BRep_Tool::Surface(theFace, theLoc);
  theTol      = BRep_Tool::Tolerance(theFace);
  theRevWires = theRevFace = false;

  if (!theSurf.IsNull() && myCopyGeom)
    theSurf = occ::down_cast<Geom_Surface>(theSurf->Copy());

  return true;
}

//=================================================================================================

bool BRepTools_CopyModification::NewTriangulation(const TopoDS_Face&          theFace,
                                                              occ::handle<Poly_Triangulation>& theTri)
{
  if (!myCopyMesh && BRep_Tool::IsGeometric(theFace))
  {
    return false;
  }

  TopLoc_Location aLoc;
  theTri = BRep_Tool::Triangulation(theFace, aLoc);

  if (theTri.IsNull())
    return false;

  // mesh is copied if and only if the geometry need to be copied too
  if (myCopyGeom)
    theTri = theTri->Copy();
  return true;
}

//=================================================================================================

bool BRepTools_CopyModification::NewCurve(const TopoDS_Edge&  theEdge,
                                                      occ::handle<Geom_Curve>& theCurve,
                                                      TopLoc_Location&    theLoc,
                                                      double&      theTol)
{
  double aFirst, aLast;
  theCurve = BRep_Tool::Curve(theEdge, theLoc, aFirst, aLast);
  theTol   = BRep_Tool::Tolerance(theEdge);

  if (!theCurve.IsNull() && myCopyGeom)
    theCurve = occ::down_cast<Geom_Curve>(theCurve->Copy());

  return true;
}

//=================================================================================================

bool BRepTools_CopyModification::NewPolygon(const TopoDS_Edge&      theEdge,
                                                        occ::handle<Poly_Polygon3D>& thePoly)
{
  if (!myCopyMesh && BRep_Tool::IsGeometric(theEdge))
  {
    return false;
  }

  TopLoc_Location aLoc;
  thePoly = BRep_Tool::Polygon3D(theEdge, aLoc);

  if (thePoly.IsNull())
    return false;

  // polygon is copied if and only if the geometry need to be copied too
  if (myCopyGeom)
    thePoly = thePoly->Copy();
  return true;
}

//=================================================================================================

bool BRepTools_CopyModification::NewPolygonOnTriangulation(
  const TopoDS_Edge&                   theEdge,
  const TopoDS_Face&                   theFace,
  occ::handle<Poly_PolygonOnTriangulation>& thePoly)
{
  if (!myCopyMesh && BRep_Tool::IsGeometric(theEdge))
  {
    return false;
  }

  TopLoc_Location            aLoc;
  occ::handle<Poly_Triangulation> aTria = BRep_Tool::Triangulation(theFace, aLoc);
  thePoly                          = BRep_Tool::PolygonOnTriangulation(theEdge, aTria, aLoc);

  if (thePoly.IsNull())
    return false;

  // polygon is copied if and only if the geometry need to be copied too
  if (myCopyGeom)
    thePoly = thePoly->Copy();
  return true;
}

//=================================================================================================

bool BRepTools_CopyModification::NewPoint(const TopoDS_Vertex& theVertex,
                                                      gp_Pnt&              thePnt,
                                                      double&       theTol)
{
  thePnt = BRep_Tool::Pnt(theVertex);
  theTol = BRep_Tool::Tolerance(theVertex);
  return true;
}

//=================================================================================================

bool BRepTools_CopyModification::NewCurve2d(const TopoDS_Edge& theEdge,
                                                        const TopoDS_Face& theFace,
                                                        const TopoDS_Edge&,
                                                        const TopoDS_Face&,
                                                        occ::handle<Geom2d_Curve>& theCurve,
                                                        double&        theTol)
{
  theTol = BRep_Tool::Tolerance(theEdge);
  double aFirst, aLast;
  theCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirst, aLast);

  if (!theCurve.IsNull() && myCopyGeom)
    theCurve = occ::down_cast<Geom2d_Curve>(theCurve->Copy());

  return true;
}

//=================================================================================================

bool BRepTools_CopyModification::NewParameter(const TopoDS_Vertex& theVertex,
                                                          const TopoDS_Edge&   theEdge,
                                                          double&       thePnt,
                                                          double&       theTol)
{
  if (theVertex.IsNull())
    return false; // infinite edge may have Null vertex

  theTol = BRep_Tool::Tolerance(theVertex);
  thePnt = BRep_Tool::Parameter(theVertex, theEdge);

  return true;
}

//=================================================================================================

GeomAbs_Shape BRepTools_CopyModification::Continuity(const TopoDS_Edge& theEdge,
                                                     const TopoDS_Face& theFace1,
                                                     const TopoDS_Face& theFace2,
                                                     const TopoDS_Edge&,
                                                     const TopoDS_Face&,
                                                     const TopoDS_Face&)
{
  return BRep_Tool::Continuity(theEdge, theFace1, theFace2);
}
