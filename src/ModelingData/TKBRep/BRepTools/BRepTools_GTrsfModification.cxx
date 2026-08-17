// Created on: 1996-12-30
// Created by: Stagiaire Mary FABIEN
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <BRepTools_GTrsfModification.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

#include <algorithm>
#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(BRepTools_GTrsfModification, BRepTools_Modification)

namespace
{

//=================================================================================================

double maxAbsCoefficient(const gp_GTrsf& theGTrsf)
{
  return std::max({std::abs(theGTrsf.Value(1, 1)),
                   std::abs(theGTrsf.Value(1, 2)),
                   std::abs(theGTrsf.Value(1, 3)),
                   std::abs(theGTrsf.Value(2, 1)),
                   std::abs(theGTrsf.Value(2, 2)),
                   std::abs(theGTrsf.Value(2, 3)),
                   std::abs(theGTrsf.Value(3, 1)),
                   std::abs(theGTrsf.Value(3, 2)),
                   std::abs(theGTrsf.Value(3, 3))});
}

//=================================================================================================

gp_Mat normalizedVectorialPart(const gp_GTrsf& theGTrsf, const double theScale)
{
  gp_Mat aMat = theGTrsf.VectorialPart();
  if (theScale > 0.0)
  {
    aMat.Multiply(1.0 / theScale);
  }
  return aMat;
}

//=================================================================================================

gp_Mat normalTransformation(const gp_Mat& theNormalizedVectorialPart, const bool theIsNegative)
{
  const gp_XYZ aCol1 = theNormalizedVectorialPart.Column(1);
  const gp_XYZ aCol2 = theNormalizedVectorialPart.Column(2);
  const gp_XYZ aCol3 = theNormalizedVectorialPart.Column(3);
  gp_Mat       aNormalMat(aCol2.Crossed(aCol3), aCol3.Crossed(aCol1), aCol1.Crossed(aCol2));
  if (theIsNegative)
  {
    aNormalMat *= -1.0;
  }
  return aNormalMat;
}

//=================================================================================================

gp_GTrsf representationTransformation(const gp_GTrsf&        theGTrsf,
                                      const TopLoc_Location& theSourceLocation,
                                      const TopLoc_Location& theTargetLocation)
{
  gp_GTrsf aLocalTrsf(theTargetLocation.Transformation().Inverted());
  aLocalTrsf.Multiply(theGTrsf);
  aLocalTrsf.Multiply(theSourceLocation.Transformation());
  return aLocalTrsf;
}
} // namespace

//=================================================================================================

BRepTools_GTrsfModification::BRepTools_GTrsfModification(const gp_GTrsf& T)
    : myGTrsf(T),
      myGScale(maxAbsCoefficient(T))
{
}

//=================================================================================================

void BRepTools_GTrsfModification::SetGTrsf(const gp_GTrsf& theGTrsf)
{
  myGTrsf  = theGTrsf;
  myGScale = maxAbsCoefficient(theGTrsf);
}

bool BRepTools_GTrsfModification::NewSurface(const TopoDS_Face&         F,
                                             occ::handle<Geom_Surface>& S,
                                             TopLoc_Location&           L,
                                             double&                    Tol,
                                             bool&                      RevWires,
                                             bool&                      RevFace)
{
  S = BRep_Tool::Surface(F, L);
  if (S.IsNull())
  {
    // processing the case when there is no geometry
    return false;
  }
  Tol      = BRep_Tool::Tolerance(F) * myGScale;
  RevWires = false;
  RevFace  = normalizedVectorialPart(myGTrsf, myGScale).Determinant() < 0.0;
  S        = occ::down_cast<Geom_Surface>(S->Transformed(L.Transformation()));

  occ::handle<Standard_Type> TheTypeS = S->DynamicType();
  if (TheTypeS == STANDARD_TYPE(Geom_BSplineSurface))
  {
    occ::handle<Geom_BSplineSurface> S2 = occ::down_cast<Geom_BSplineSurface>(S);
    for (int i = 1; i <= S2->NbUPoles(); i++)
    {
      for (int j = 1; j <= S2->NbVPoles(); j++)
      {
        gp_Pnt aPole = S2->Pole(i, j);
        myGTrsf.Transforms(aPole.ChangeCoord());
        S2->SetPole(i, j, aPole);
      }
    }
  }
  else if (TheTypeS == STANDARD_TYPE(Geom_BezierSurface))
  {
    occ::handle<Geom_BezierSurface> S2 = occ::down_cast<Geom_BezierSurface>(S);
    for (int i = 1; i <= S2->NbUPoles(); i++)
    {
      for (int j = 1; j <= S2->NbVPoles(); j++)
      {
        gp_Pnt aPole = S2->Pole(i, j);
        myGTrsf.Transforms(aPole.ChangeCoord());
        S2->SetPole(i, j, aPole);
      }
    }
  }
  else
  {
    throw Standard_NoSuchObject("BRepTools_GTrsfModification : Pb no BSpline/Bezier Type Surface");
  }

  L.Identity();
  return true;
}

//=================================================================================================

bool BRepTools_GTrsfModification::NewCurve(const TopoDS_Edge&       E,
                                           occ::handle<Geom_Curve>& C,
                                           TopLoc_Location&         L,
                                           double&                  Tol)
{
  double f, l;
  Tol = BRep_Tool::Tolerance(E) * myGScale;
  C   = BRep_Tool::Curve(E, L, f, l);

  if (!C.IsNull())
  {
    C = occ::down_cast<Geom_Curve>(C->Transformed(L.Transformation()));
    occ::handle<Standard_Type> TheTypeC = C->DynamicType();
    if (TheTypeC == STANDARD_TYPE(Geom_BSplineCurve))
    {
      occ::handle<Geom_BSplineCurve> C2 = occ::down_cast<Geom_BSplineCurve>(C);
      for (int i = 1; i <= C2->NbPoles(); i++)
      {
        gp_Pnt aPole = C2->Pole(i);
        myGTrsf.Transforms(aPole.ChangeCoord());
        C2->SetPole(i, aPole);
      }
    }
    else if (TheTypeC == STANDARD_TYPE(Geom_BezierCurve))
    {
      occ::handle<Geom_BezierCurve> C2 = occ::down_cast<Geom_BezierCurve>(C);
      for (int i = 1; i <= C2->NbPoles(); i++)
      {
        gp_Pnt aPole = C2->Pole(i);
        myGTrsf.Transforms(aPole.ChangeCoord());
        C2->SetPole(i, aPole);
      }
    }
    else
    {
      throw Standard_NoSuchObject("BRepTools_GTrsfModification : Pb no BSpline/Bezier Type Curve");
    }
    C = new Geom_TrimmedCurve(C, f, l);
  }
  L.Identity();
  return !C.IsNull();
}

//=================================================================================================

bool BRepTools_GTrsfModification::NewPoint(const TopoDS_Vertex& V, gp_Pnt& P, double& Tol)
{
  gp_Pnt Pnt = BRep_Tool::Pnt(V);
  Tol        = BRep_Tool::Tolerance(V) * myGScale;
  gp_XYZ coor(Pnt.Coord());
  myGTrsf.Transforms(coor);
  P.SetXYZ(coor);

  return true;
}

//=================================================================================================

bool BRepTools_GTrsfModification::NewCurve2d(const TopoDS_Edge& E,
                                             const TopoDS_Face& F,
                                             const TopoDS_Edge&,
                                             const TopoDS_Face&,
                                             occ::handle<Geom2d_Curve>& C,
                                             double&                    Tol)
{
  Tol = BRep_Tool::Tolerance(E) * myGScale;
  double f, l;
  C = BRep_Tool::CurveOnSurface(E, F, f, l);
  if (C.IsNull())
  {
    // processing the case when there is no geometry
    return false;
  }
  C = new Geom2d_TrimmedCurve(C, f, l);
  return true;
}

//=================================================================================================

bool BRepTools_GTrsfModification::NewParameter(const TopoDS_Vertex& V,
                                               const TopoDS_Edge&   E,
                                               double&              P,
                                               double&              Tol)
{
  Tol = BRep_Tool::Tolerance(V) * myGScale;
  P   = BRep_Tool::Parameter(V, E);
  return true;
}

//=================================================================================================

GeomAbs_Shape BRepTools_GTrsfModification::Continuity(const TopoDS_Edge& E,
                                                      const TopoDS_Face& F1,
                                                      const TopoDS_Face& F2,
                                                      const TopoDS_Edge&,
                                                      const TopoDS_Face&,
                                                      const TopoDS_Face&)
{
  return BRep_Tool::Continuity(E, F1, F2);
}

//=================================================================================================

bool BRepTools_GTrsfModification::NewTriangulation(
  const TopoDS_Face&               theFace,
  occ::handle<Poly_Triangulation>& theTriangulation)
{
  TopLoc_Location aLoc;
  theTriangulation = BRep_Tool::Triangulation(theFace, aLoc);
  if (theTriangulation.IsNull())
  {
    return false;
  }

  const gp_GTrsf aGTrsf = representationTransformation(myGTrsf, aLoc, theFace.Location());
  const gp_Mat   aNormalizedVectorialPart = normalizedVectorialPart(aGTrsf, myGScale);
  const bool     aIsNegative               = aNormalizedVectorialPart.Determinant() < 0.0;

  theTriangulation = theTriangulation->Copy();
  theTriangulation->SetCachedMinMax(Bnd_Box()); // clear bounding box
  theTriangulation->Deflection(theTriangulation->Deflection() * myGScale);
  // apply transformation to 3D nodes
  for (int anInd = 1; anInd <= theTriangulation->NbNodes(); ++anInd)
  {
    gp_Pnt aP = theTriangulation->Node(anInd);
    aGTrsf.Transforms(aP.ChangeCoord());
    theTriangulation->SetNode(anInd, aP);
  }
  // modify triangles orientation in case of mirror transformation
  if (aIsNegative)
  {
    for (int anInd = 1; anInd <= theTriangulation->NbTriangles(); ++anInd)
    {
      Poly_Triangle aTria = theTriangulation->Triangle(anInd);
      int           aN1, aN2, aN3;
      aTria.Get(aN1, aN2, aN3);
      aTria.Set(aN1, aN3, aN2);
      theTriangulation->SetTriangle(anInd, aTria);
    }
  }
  // modify normals
  if (theTriangulation->HasNormals())
  {
    const gp_Mat aNormalMat =
      normalTransformation(aNormalizedVectorialPart, aIsNegative);
    for (int anInd = 1; anInd <= theTriangulation->NbNodes(); ++anInd)
    {
      gp_XYZ aNormal = theTriangulation->Normal(anInd).XYZ();
      aNormal.Multiply(aNormalMat);
      const double aScale =
        std::max({std::abs(aNormal.X()), std::abs(aNormal.Y()), std::abs(aNormal.Z())});
      if (aScale == 0.0)
      {
        // A collapsed tangent plane has no transformed normal; preserve the source cache value.
        continue;
      }
      aNormal.Divide(aScale);
      theTriangulation->SetNormal(anInd, gp_Dir(aNormal));
    }
  }

  return true;
}

//=================================================================================================

bool BRepTools_GTrsfModification::NewPolygon(const TopoDS_Edge&           theEdge,
                                             occ::handle<Poly_Polygon3D>& thePoly)
{
  TopLoc_Location aLoc;
  thePoly = BRep_Tool::Polygon3D(theEdge, aLoc);
  if (thePoly.IsNull())
  {
    return false;
  }

  const gp_GTrsf aGTrsf = representationTransformation(myGTrsf, aLoc, theEdge.Location());

  thePoly = thePoly->Copy();
  thePoly->Deflection(thePoly->Deflection() * myGScale);
  // transform nodes
  NCollection_Array1<gp_Pnt>& aNodesArray = thePoly->ChangeNodes();
  for (int anId = aNodesArray.Lower(); anId <= aNodesArray.Upper(); ++anId)
  {
    gp_Pnt& aP = aNodesArray.ChangeValue(anId);
    aGTrsf.Transforms(aP.ChangeCoord());
  }
  return true;
}

//=================================================================================================

bool BRepTools_GTrsfModification::NewPolygonOnTriangulation(
  const TopoDS_Edge&                        theEdge,
  const TopoDS_Face&                        theFace,
  occ::handle<Poly_PolygonOnTriangulation>& thePoly)
{
  TopLoc_Location                 aLoc;
  occ::handle<Poly_Triangulation> aT = BRep_Tool::Triangulation(theFace, aLoc);
  if (aT.IsNull())
  {
    return false;
  }

  thePoly = BRep_Tool::PolygonOnTriangulation(theEdge, aT, aLoc);
  if (!thePoly.IsNull())
  {
    thePoly = thePoly->Copy();
  }
  return !thePoly.IsNull();
}
