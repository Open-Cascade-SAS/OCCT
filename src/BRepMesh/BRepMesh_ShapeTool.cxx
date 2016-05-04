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

#include <BRepMesh_ShapeTool.hxx>

#include <Bnd_Box.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBndLib.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Triangulation.hxx>
#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <BRepAdaptor_Curve.hxx>

namespace {
  //! Auxilary struct to take a tolerance of edge.
  struct EdgeTolerance
  {
    static Standard_Real Get(const TopoDS_Shape& theEdge)
    {
      return BRep_Tool::Tolerance(TopoDS::Edge(theEdge));
    }
  };

  //! Auxilary struct to take a tolerance of vertex.
  struct VertexTolerance
  {
    static Standard_Real Get(const TopoDS_Shape& theVertex)
    {
      return BRep_Tool::Tolerance(TopoDS::Vertex(theVertex));
    }
  };

  //! Returns maximum tolerance of face element of the specified type.
  template<TopAbs_ShapeEnum ShapeType, class ToleranceExtractor>
  Standard_Real MaxTolerance(const TopoDS_Face& theFace)
  {
    Standard_Real aMaxTolerance = RealFirst();
    TopExp_Explorer aExplorer(theFace, ShapeType);
    for (; aExplorer.More(); aExplorer.Next())
    {
      Standard_Real aTolerance = ToleranceExtractor::Get(aExplorer.Current());
      if (aTolerance > aMaxTolerance)
        aMaxTolerance = aTolerance;
    }

    return aMaxTolerance;
  }
}

//=======================================================================
//function : BoxMaxDimension
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_ShapeTool::MaxFaceTolerance(const TopoDS_Face& theFace)
{
  Standard_Real aMaxTolerance = BRep_Tool::Tolerance(theFace);

  Standard_Real aTolerance = Max(
    MaxTolerance<TopAbs_EDGE,   EdgeTolerance  >(theFace),
    MaxTolerance<TopAbs_VERTEX, VertexTolerance>(theFace));

  return Max(aMaxTolerance, aTolerance);
}

//=======================================================================
//function : BoxMaxDimension
//purpose  : 
//=======================================================================
void BRepMesh_ShapeTool::BoxMaxDimension(const Bnd_Box& theBox,
                                         Standard_Real& theMaxDimension)
{
  if(theBox.IsVoid())
    return;

  Standard_Real aMinX, aMinY, aMinZ, aMaxX, aMaxY, aMaxZ;
  theBox.Get(aMinX, aMinY, aMinZ, aMaxX, aMaxY, aMaxZ);

  theMaxDimension = Max(aMaxX - aMinX, Max(aMaxY - aMinY, aMaxZ - aMinZ));
}

//=======================================================================
//function : RelativeEdgeDeflection
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_ShapeTool::RelativeEdgeDeflection(
  const TopoDS_Edge&  theEdge,
  const Standard_Real theDeflection,
  const Standard_Real theMaxShapeSize,
  Standard_Real&      theAdjustmentCoefficient)
{
  theAdjustmentCoefficient = 1.;
  Standard_Real aDefEdge = theDeflection;
  if(theEdge.IsNull())
    return aDefEdge;

  Bnd_Box aBox;
  BRepBndLib::Add(theEdge, aBox, Standard_False);
  BoxMaxDimension(aBox, aDefEdge);
            
  // Adjust resulting value in relation to the total size
  theAdjustmentCoefficient = theMaxShapeSize / (2 * aDefEdge);
  if (theAdjustmentCoefficient < 0.5)
    theAdjustmentCoefficient = 0.5;
  else if (theAdjustmentCoefficient > 2.)
    theAdjustmentCoefficient = 2.;

  return (theAdjustmentCoefficient * aDefEdge * theDeflection);
}

//=======================================================================
//function : FindUV
//purpose  : 
//=======================================================================
gp_XY BRepMesh_ShapeTool::FindUV(
  const Standard_Integer                theIndexOfPnt3d,
  const gp_Pnt2d&                       thePnt2d,
  const Standard_Real                   theMinDistance,
  const Handle(BRepMesh_FaceAttribute)& theFaceAttribute)
{
  const gp_XY& aPnt2d = thePnt2d.Coord();
  BRepMesh::HDMapOfIntegerListOfXY& aLocation2D =
    theFaceAttribute->ChangeLocation2D();

  if (!aLocation2D->IsBound(theIndexOfPnt3d))
  {
    BRepMesh::ListOfXY aPoints2d;
    aPoints2d.Append(aPnt2d);
    aLocation2D->Bind(theIndexOfPnt3d, aPoints2d);
    return aPnt2d;
  }

  BRepMesh::ListOfXY& aPoints2d = aLocation2D->ChangeFind(theIndexOfPnt3d);

  // Find the most closest 2d point to the given one.
  gp_XY aUV;
  Standard_Real aMinDist = RealLast();
  BRepMesh::ListOfXY::Iterator aPoint2dIt(aPoints2d);
  for (; aPoint2dIt.More(); aPoint2dIt.Next())
  {
    const gp_XY& aCurPnt2d = aPoint2dIt.Value();

    Standard_Real aDist = (aPnt2d - aCurPnt2d).Modulus();
    if (aDist < aMinDist)
    {
      aUV      = aCurPnt2d;
      aMinDist = aDist;
    }
  }

  const Standard_Real aTolerance = theMinDistance;

  // Get face limits
  Standard_Real aDiffU = theFaceAttribute->GetUMax() - theFaceAttribute->GetUMin();
  Standard_Real aDiffV = theFaceAttribute->GetVMax() - theFaceAttribute->GetVMin();

  const Standard_Real Utol2d = .5 * aDiffU;
  const Standard_Real Vtol2d = .5 * aDiffV;

  const Handle(BRepAdaptor_HSurface)& aSurface = theFaceAttribute->Surface();
  const gp_Pnt aPnt1 = aSurface->Value(aUV.X(), aUV.Y());
  const gp_Pnt aPnt2 = aSurface->Value(aPnt2d.X(), aPnt2d.Y());

  //! If selected point is too far from the given one in parametric space
  //! or their positions in 3d are different, add the given point as unique.
  if (Abs(aUV.X() - aPnt2d.X()) > Utol2d ||
      Abs(aUV.Y() - aPnt2d.Y()) > Vtol2d ||
      !aPnt1.IsEqual(aPnt2, aTolerance))
  {
    aUV = aPnt2d;
    aPoints2d.Append(aUV);
  }

  return aUV;
}

//=======================================================================
//function : AddInFace
//purpose  : 
//=======================================================================
void BRepMesh_ShapeTool::AddInFace(
  const TopoDS_Face&          theFace,
  Handle(Poly_Triangulation)& theTriangulation)
{
  const TopLoc_Location& aLoc = theFace.Location();
  if (!aLoc.IsIdentity())
  {
    gp_Trsf aTrsf = aLoc.Transformation();
    aTrsf.Invert();

    TColgp_Array1OfPnt& aNodes = theTriangulation->ChangeNodes();
    for (Standard_Integer i = aNodes.Lower(); i <= aNodes.Upper(); ++i) 
      aNodes(i).Transform(aTrsf);
  }

  BRep_Builder aBuilder;
  aBuilder.UpdateFace(theFace, theTriangulation);
}

//=======================================================================
//function : NullifyFace
//purpose  : 
//=======================================================================
void BRepMesh_ShapeTool::NullifyFace(const TopoDS_Face& theFace)
{
  BRep_Builder aBuilder;
  aBuilder.UpdateFace(theFace, Handle(Poly_Triangulation)());
}

//=======================================================================
//function : NullifyEdge
//purpose  : 
//=======================================================================
void BRepMesh_ShapeTool::NullifyEdge(
  const TopoDS_Edge&                theEdge,
  const Handle(Poly_Triangulation)& theTriangulation,
  const TopLoc_Location&            theLocation)
{
  UpdateEdge(theEdge, Handle(Poly_PolygonOnTriangulation)(),
    theTriangulation, theLocation);
}

//=======================================================================
//function : UpdateEdge
//purpose  : 
//=======================================================================
void BRepMesh_ShapeTool::UpdateEdge(
  const TopoDS_Edge&                         theEdge,
  const Handle(Poly_PolygonOnTriangulation)& thePolygon,
  const Handle(Poly_Triangulation)&          theTriangulation,
  const TopLoc_Location&                     theLocation)
{
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(theEdge, thePolygon, theTriangulation, theLocation);
}

//=======================================================================
//function : UpdateEdge
//purpose  : 
//=======================================================================
void BRepMesh_ShapeTool::UpdateEdge(
  const TopoDS_Edge&                         theEdge,
  const Handle(Poly_PolygonOnTriangulation)& thePolygon1,
  const Handle(Poly_PolygonOnTriangulation)& thePolygon2,
  const Handle(Poly_Triangulation)&          theTriangulation,
  const TopLoc_Location&                     theLocation)
{
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(theEdge, thePolygon1, thePolygon2, 
    theTriangulation, theLocation);
}

//=======================================================================
//function : UseLocation
//purpose  : 
//=======================================================================
gp_Pnt BRepMesh_ShapeTool::UseLocation(const gp_Pnt&          thePnt,
                                       const TopLoc_Location& theLoc)
{
  if (theLoc.IsIdentity())
    return thePnt;

  return thePnt.Transformed(theLoc.Transformation());
}

//=======================================================================
//function : IsDegenerated
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_ShapeTool::IsDegenerated(
  const TopoDS_Edge& theEdge,
  const TopoDS_Face& theFace)
{
  // Get vertices
  TopoDS_Vertex pBegin, pEnd;
  TopExp::Vertices(theEdge, pBegin, pEnd);
  if (pBegin.IsNull() || pEnd.IsNull())
    return Standard_True;

  if (BRep_Tool::Degenerated(theEdge))
    return Standard_True;

  if (!pBegin.IsSame(pEnd))
    return Standard_False;

  Standard_Real wFirst, wLast;
  BRep_Tool::Range(theEdge, theFace, wFirst, wLast);

  // calculation of the length of the edge in 3D
  Standard_Real longueur = 0.0;
  Standard_Real du = (wLast - wFirst) * 0.05;
  gp_Pnt P1, P2;
  BRepAdaptor_Curve BC(theEdge);
  BC.D0(wFirst, P1);
  Standard_Real tolV = BRep_Tool::Tolerance(pBegin);
  Standard_Real tolV2 = 1.2 * tolV;

  for (Standard_Integer l = 1; l <= 20; ++l)
  {
    BC.D0(wFirst + l * du, P2);
    longueur += P1.Distance(P2);

    if (longueur > tolV2)
      break;

    P1 = P2;
  }

  if (longueur < tolV2)
    return Standard_True;

  return Standard_False;
}
