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

//=======================================================================
//function : BRepTools_CopyModification
//purpose  : 
//=======================================================================
BRepTools_CopyModification::BRepTools_CopyModification(const Standard_Boolean copyGeom,
                                                       const Standard_Boolean copyMesh)
  : myCopyGeom(copyGeom),
    myCopyMesh(copyMesh)
{
}

//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_CopyModification::NewSurface(const TopoDS_Face&    theFace,
                                                        Handle(Geom_Surface)& theSurf,
                                                        TopLoc_Location&      theLoc,
                                                        Standard_Real&        theTol,
                                                        Standard_Boolean&     theRevWires,
                                                        Standard_Boolean&     theRevFace)
{
  theSurf = BRep_Tool::Surface(theFace, theLoc);
  theTol = BRep_Tool::Tolerance(theFace);
  theRevWires = theRevFace = Standard_False;

  if (!theSurf.IsNull() && myCopyGeom)
    theSurf = Handle(Geom_Surface)::DownCast(theSurf->Copy());

  return Standard_True;
}

//=======================================================================
//function : NewTriangulation
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_CopyModification::NewTriangulation(const TopoDS_Face&          theFace,
                                                              Handle(Poly_Triangulation)& theTri)
{
  if (!myCopyMesh && BRep_Tool::IsGeometric(theFace))
  {
    return Standard_False;
  }

  TopLoc_Location aLoc;
  theTri = BRep_Tool::Triangulation(theFace, aLoc);

  if (theTri.IsNull())
    return Standard_False;

  // mesh is copied if and only if the geometry need to be copied too
  if (myCopyGeom)
    theTri = theTri->Copy();
  return Standard_True;
}

//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_CopyModification::NewCurve(const TopoDS_Edge&  theEdge,
                                                      Handle(Geom_Curve)& theCurve,
                                                      TopLoc_Location&    theLoc,
                                                      Standard_Real&      theTol)
{
  Standard_Real aFirst, aLast;
  theCurve = BRep_Tool::Curve(theEdge, theLoc, aFirst, aLast);
  theTol = BRep_Tool::Tolerance(theEdge);

  if (!theCurve.IsNull() && myCopyGeom)
    theCurve = Handle(Geom_Curve)::DownCast(theCurve->Copy());

  return Standard_True;
}

//=======================================================================
//function : NewPolygon
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_CopyModification::NewPolygon(const TopoDS_Edge&      theEdge,
                                                        Handle(Poly_Polygon3D)& thePoly)
{
  if (!myCopyMesh && BRep_Tool::IsGeometric(theEdge))
  {
    return Standard_False;
  }

  TopLoc_Location aLoc;
  thePoly = BRep_Tool::Polygon3D(theEdge, aLoc);

  if (thePoly.IsNull())
    return Standard_False;

  // polygon is copied if and only if the geometry need to be copied too
  if (myCopyGeom)
    thePoly = thePoly->Copy();
  return Standard_True;
}

//=======================================================================
//function : NewPolygonOnTriangulation
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_CopyModification::NewPolygonOnTriangulation(
  const TopoDS_Edge&                   theEdge,
  const TopoDS_Face&                   theFace,
  Handle(Poly_PolygonOnTriangulation)& thePoly)
{
  if (!myCopyMesh && BRep_Tool::IsGeometric(theEdge))
  {
    return Standard_False;
  }

  TopLoc_Location aLoc;
  Handle(Poly_Triangulation) aTria = BRep_Tool::Triangulation(theFace, aLoc);
  thePoly = BRep_Tool::PolygonOnTriangulation(theEdge, aTria, aLoc);

  if (thePoly.IsNull())
    return Standard_False;

  // polygon is copied if and only if the geometry need to be copied too
  if (myCopyGeom)
    thePoly = thePoly->Copy();
  return Standard_True;
}

//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_CopyModification::NewPoint(const TopoDS_Vertex& theVertex, 
                                                      gp_Pnt&              thePnt,
                                                      Standard_Real&       theTol)
{
  thePnt = BRep_Tool::Pnt(theVertex);
  theTol = BRep_Tool::Tolerance(theVertex);
  return Standard_True;
}

//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_CopyModification::NewCurve2d(const TopoDS_Edge&    theEdge,
                                                        const TopoDS_Face&    theFace,
                                                        const TopoDS_Edge&,
                                                        const TopoDS_Face&,
                                                        Handle(Geom2d_Curve)& theCurve,
                                                        Standard_Real&        theTol)
{
  theTol = BRep_Tool::Tolerance(theEdge);
  Standard_Real aFirst, aLast;
  theCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirst, aLast);

  if (!theCurve.IsNull() && myCopyGeom)
    theCurve = Handle(Geom2d_Curve)::DownCast(theCurve->Copy());

  return Standard_True;
}

//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_CopyModification::NewParameter(const TopoDS_Vertex& theVertex,
                                                          const TopoDS_Edge&   theEdge,
                                                          Standard_Real&       thePnt,
                                                          Standard_Real&       theTol)
{
  if (theVertex.IsNull())
    return Standard_False; // infinite edge may have Null vertex

  theTol = BRep_Tool::Tolerance(theVertex);
  thePnt = BRep_Tool::Parameter(theVertex, theEdge);

  return Standard_True;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
GeomAbs_Shape BRepTools_CopyModification::Continuity(const TopoDS_Edge& theEdge,
                                                     const TopoDS_Face& theFace1,
                                                     const TopoDS_Face& theFace2,
                                                     const TopoDS_Edge&,
                                                     const TopoDS_Face&,
                                                     const TopoDS_Face&)
{
  return BRep_Tool::Continuity(theEdge, theFace1, theFace2);
}


