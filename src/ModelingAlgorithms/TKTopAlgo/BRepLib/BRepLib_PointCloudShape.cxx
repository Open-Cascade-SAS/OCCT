// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <BRepLib_PointCloudShape.hxx>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepLib_ToolTriangulatedShape.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom_Surface.hxx>
#include <GProp_GProps.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <random>

//=================================================================================================

BRepLib_PointCloudShape::BRepLib_PointCloudShape(const TopoDS_Shape& theShape, const double theTol)
    : myShape(theShape),
      myDist(0.0),
      myTol(theTol),
      myNbPoints(0)
{}

//=================================================================================================

BRepLib_PointCloudShape::~BRepLib_PointCloudShape()
= default;

//=================================================================================================

int BRepLib_PointCloudShape::NbPointsByDensity(const double theDensity)
{
  clear();
  double aDensity = (theDensity < Precision::Confusion() ? computeDensity() : theDensity);
  if (aDensity < Precision::Confusion())
  {
    return 0;
  }

  int aNbPoints = 0;
  for (TopExp_Explorer aExpF(myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    double anArea = faceArea(aExpF.Current());

    int aNbPnts = std::max((int)std::ceil(anArea / theDensity), 1);
    myFacePoints.Bind(aExpF.Current(), aNbPnts);
    aNbPoints += aNbPnts;
  }
  return aNbPoints;
}

//=================================================================================================

bool BRepLib_PointCloudShape::GeneratePointsByDensity(const double theDensity)
{
  if (myFacePoints.IsEmpty())
  {
    if (NbPointsByDensity(theDensity) == 0)
    {
      return false;
    }
  }

  int aNbAdded = 0;
  for (TopExp_Explorer aExpF(myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    if (addDensityPoints(aExpF.Current()))
    {
      aNbAdded++;
    }
  }
  return (aNbAdded > 0);
}

//=================================================================================================

bool BRepLib_PointCloudShape::GeneratePointsByTriangulation()
{
  clear();

  int aNbAdded = 0;
  for (TopExp_Explorer aExpF(myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    if (addTriangulationPoints(aExpF.Current()))
    {
      aNbAdded++;
    }
  }
  return (aNbAdded > 0);
}

//=================================================================================================

double BRepLib_PointCloudShape::faceArea(const TopoDS_Shape& theShape)
{
  double anArea = 0.0;
  if (myFaceArea.Find(theShape, anArea))
  {
    return anArea;
  }

  GProp_GProps aFaceProps;
  BRepGProp::SurfaceProperties(theShape, aFaceProps);
  anArea = aFaceProps.Mass();
  myFaceArea.Bind(theShape, anArea);
  return anArea;
}

//=================================================================================================

double BRepLib_PointCloudShape::computeDensity()
{
  // at first step find the face with smallest area
  double anAreaMin = Precision::Infinite();
  for (TopExp_Explorer aExpF(myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    double anArea = faceArea(aExpF.Current());
    if (anArea < myTol * myTol)
    {
      continue;
    }

    if (anArea < anAreaMin)
    {
      anAreaMin = anArea;
    }
  }
  return anAreaMin * 0.1;
}

//=================================================================================================

int BRepLib_PointCloudShape::NbPointsByTriangulation() const
{
  // at first step find the face with smallest area
  int aNbPoints = 0;
  for (TopExp_Explorer aExpF(myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    TopLoc_Location                 aLoc;
    occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(aExpF.Current()), aLoc);
    if (aTriangulation.IsNull())
    {
      continue;
    }

    aNbPoints += aTriangulation->NbNodes();
  }
  return aNbPoints;
}

//=================================================================================================

bool BRepLib_PointCloudShape::addDensityPoints(const TopoDS_Shape& theFace)
{
  // addition of the points with specified density on the face by random way
  int aNbPnts = (myFacePoints.IsBound(theFace) ? myFacePoints.Find(theFace) : 0);
  if (aNbPnts == 0)
  {
    return false;
  }

  TopoDS_Face aFace  = TopoDS::Face(theFace);
  double      anUMin = 0.0, anUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  BRepTools::UVBounds(aFace, anUMin, anUMax, aVMin, aVMax);
  BRepTopAdaptor_FClass2d aClassifier(aFace, Precision::Confusion());

  const TopLoc_Location&    aLoc  = theFace.Location();
  const gp_Trsf&            aTrsf = aLoc.Transformation();
  TopLoc_Location           aLoc1;
  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace, aLoc1);
  if (aSurf.IsNull())
  {
    return false;
  }

  std::mt19937                     aRandomGenerator(0);
  std::uniform_real_distribution<> anUDistrib(anUMin, anUMax);
  std::uniform_real_distribution<> aVDistrib(aVMin, aVMax);
  for (int nbCurPnts = 1; nbCurPnts <= aNbPnts;)
  {
    const double       aU = anUDistrib(aRandomGenerator);
    const double       aV = aVDistrib(aRandomGenerator);
    gp_Pnt2d           aUVNode(aU, aV);
    const TopAbs_State aState = aClassifier.Perform(aUVNode);
    if (aState == TopAbs_OUT)
    {
      continue;
    }

    nbCurPnts++;

    gp_Pnt aP1;
    gp_Vec dU, dV;
    aSurf->D1(aU, aV, aP1, dU, dV);

    gp_Vec aNorm = dU ^ dV;
    if (aFace.Orientation() == TopAbs_REVERSED)
    {
      aNorm.Reverse();
    }
    const double aNormMod = aNorm.Magnitude();
    if (aNormMod > gp::Resolution())
    {
      aNorm /= aNormMod;
    }
    if (myDist > Precision::Confusion())
    {
      std::uniform_real_distribution<> aDistanceDistrib(0.0, myDist);
      gp_XYZ aDeflPoint = aP1.XYZ() + aNorm.XYZ() * aDistanceDistrib(aRandomGenerator);
      aP1.SetXYZ(aDeflPoint);
    }
    aP1.Transform(aTrsf);
    if (aNormMod > gp::Resolution())
    {
      aNorm = gp_Dir(aNorm).Transformed(aTrsf);
    }
    addPoint(aP1, aNorm, aUVNode, aFace);
  }
  return true;
}

//=================================================================================================

bool BRepLib_PointCloudShape::addTriangulationPoints(const TopoDS_Shape& theFace)
{
  TopLoc_Location                 aLoc;
  TopoDS_Face                     aFace          = TopoDS::Face(theFace);
  occ::handle<Poly_Triangulation> aTriangulation = BRep_Tool::Triangulation(aFace, aLoc);
  if (aTriangulation.IsNull())
  {
    return false;
  }

  TopLoc_Location           aLoc1;
  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace, aLoc1);
  const gp_Trsf&            aTrsf = aLoc.Transformation();

  BRepLib_ToolTriangulatedShape::ComputeNormals(aFace, aTriangulation);
  bool aHasUVNode = aTriangulation->HasUVNodes();
  for (int aNodeIter = 1; aNodeIter <= aTriangulation->NbNodes(); ++aNodeIter)
  {
    gp_Pnt aP1     = aTriangulation->Node(aNodeIter);
    gp_Dir aNormal = aTriangulation->Normal(aNodeIter);
    if (!aLoc.IsIdentity())
    {
      aP1.Transform(aTrsf);
      aNormal.Transform(aTrsf);
    }

    const gp_Pnt2d anUVNode = aHasUVNode ? aTriangulation->UVNode(aNodeIter) : gp_Pnt2d();
    addPoint(aP1, aNormal, anUVNode, aFace);
  }
  return true;
}

//=================================================================================================

void BRepLib_PointCloudShape::clear()
{
  myFaceArea.Clear();
  myFacePoints.Clear();
}
