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

// =======================================================================
// function : BRepLib_PointCloudShape
// purpose  :
// =======================================================================
BRepLib_PointCloudShape::BRepLib_PointCloudShape (const TopoDS_Shape& theShape,
                                                  const Standard_Real theTol)
: myShape (theShape),
  myDist (0.0),
  myTol (theTol),
  myNbPoints (0)
{
  //
}

// =======================================================================
// function : ~BRepLib_PointCloudShape
// purpose  :
// =======================================================================
BRepLib_PointCloudShape::~BRepLib_PointCloudShape()
{
  //
}

// =======================================================================
// function : NbPointsByDensity
// purpose  :
// =======================================================================
Standard_Integer BRepLib_PointCloudShape::NbPointsByDensity (const Standard_Real theDensity)
{
  clear();
  Standard_Real aDensity = (theDensity < Precision::Confusion() ? computeDensity() : theDensity);
  if (aDensity < Precision::Confusion())
  {
    return 0;
  }

  Standard_Integer aNbPoints = 0;
  for (TopExp_Explorer aExpF(myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    Standard_Real anArea = faceArea(aExpF.Current());

    Standard_Integer aNbPnts = Max ((Standard_Integer)std::ceil(anArea / theDensity), 1);
    myFacePoints.Bind(aExpF.Current(), aNbPnts);
    aNbPoints+= aNbPnts;
  }
  return aNbPoints;
}

// =======================================================================
// function : GeneratePointsByDensity
// purpose  :
// =======================================================================
Standard_Boolean BRepLib_PointCloudShape::GeneratePointsByDensity (const Standard_Real theDensity)
{
  if (myFacePoints.IsEmpty())
  {
    if (NbPointsByDensity (theDensity) == 0)
    {
      return Standard_False;
    }
  }

  Standard_Integer aNbAdded = 0;
  for (TopExp_Explorer aExpF (myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    if (addDensityPoints (aExpF.Current()))
    {
      aNbAdded++;
    }
  }
  return (aNbAdded > 0);
}

// =======================================================================
// function : GeneratePointsByTriangulation
// purpose  :
// =======================================================================
Standard_Boolean BRepLib_PointCloudShape::GeneratePointsByTriangulation()
{
  clear();

  Standard_Integer aNbAdded = 0;
  for (TopExp_Explorer aExpF (myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    if (addTriangulationPoints (aExpF.Current()))
    {
      aNbAdded++;
    }
  }
  return (aNbAdded > 0);
}

// =======================================================================
// function : faceArea
// purpose  :
// =======================================================================
Standard_Real BRepLib_PointCloudShape::faceArea (const TopoDS_Shape& theShape)
{
  Standard_Real anArea = 0.0;
  if (myFaceArea.Find (theShape, anArea))
  {
    return anArea;
  }

  GProp_GProps aFaceProps;
  BRepGProp::SurfaceProperties (theShape, aFaceProps);
  anArea = aFaceProps.Mass();
  myFaceArea.Bind (theShape, anArea);
  return anArea;
}

// =======================================================================
// function : computeDensity
// purpose  :
// =======================================================================
Standard_Real BRepLib_PointCloudShape::computeDensity()
{
  // at first step find the face with smallest area
  Standard_Real anAreaMin = Precision::Infinite();
  for (TopExp_Explorer aExpF (myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    Standard_Real anArea = faceArea (aExpF.Current());
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

// =======================================================================
// function : NbPointsByTriangulation
// purpose  :
// =======================================================================
Standard_Integer BRepLib_PointCloudShape::NbPointsByTriangulation() const
{
  // at first step find the face with smallest area
  Standard_Integer aNbPoints = 0;
  for (TopExp_Explorer aExpF (myShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (TopoDS::Face (aExpF.Current()), aLoc);
    if (aTriangulation.IsNull())
    {
      continue;
    }

    aNbPoints += aTriangulation->NbNodes();
  }
  return aNbPoints;
}

// =======================================================================
// function : addDensityPoints
// purpose  :
// =======================================================================
Standard_Boolean BRepLib_PointCloudShape::addDensityPoints (const TopoDS_Shape& theFace)
{
  //addition of the points with specified density on the face by random way
  Standard_Integer aNbPnts = (myFacePoints.IsBound (theFace) ? myFacePoints.Find (theFace) : 0);
  if (aNbPnts == 0)
  {
    return Standard_False;
  }

  TopoDS_Face aFace = TopoDS::Face (theFace);
  Standard_Real anUMin = 0.0, anUMax = 0.0, aVMin = 0.0, aVMax = 0.0;
  BRepTools::UVBounds (aFace, anUMin, anUMax, aVMin, aVMax);
  BRepTopAdaptor_FClass2d aClassifier (aFace, Precision::Confusion());

  const TopLoc_Location& aLoc = theFace.Location();
  const gp_Trsf& aTrsf = aLoc.Transformation();
  TopLoc_Location aLoc1;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface (aFace, aLoc1);
  if (aSurf.IsNull())
  {
    return Standard_False;
  }

  std::mt19937 aRandomGenerator(0);
  std::uniform_real_distribution<> anUDistrib(anUMin, anUMax);
  std::uniform_real_distribution<> aVDistrib (aVMin,  aVMax);
  for (Standard_Integer nbCurPnts = 1; nbCurPnts <= aNbPnts;)
  {
    const Standard_Real aU = anUDistrib(aRandomGenerator);
    const Standard_Real aV = aVDistrib (aRandomGenerator);
    gp_Pnt2d aUVNode (aU, aV);
    const TopAbs_State  aState = aClassifier.Perform (aUVNode);
    if (aState == TopAbs_OUT)
    {
      continue;
    }

    nbCurPnts++;

    gp_Pnt aP1;
    gp_Vec dU, dV;
    aSurf->D1 (aU, aV, aP1, dU, dV);

    gp_Vec aNorm = dU ^ dV;
    if (aFace.Orientation() == TopAbs_REVERSED)
    {
      aNorm.Reverse();
    }
    const Standard_Real aNormMod = aNorm.Magnitude();
    if (aNormMod > gp::Resolution())
    {
      aNorm /= aNormMod;
    }
    if (myDist > Precision::Confusion())
    {
      std::uniform_real_distribution<> aDistanceDistrib (0.0, myDist);
      gp_XYZ aDeflPoint =  aP1.XYZ() + aNorm.XYZ() * aDistanceDistrib (aRandomGenerator);
      aP1.SetXYZ (aDeflPoint);
    }
    aP1.Transform (aTrsf);
    if (aNormMod > gp::Resolution())
    {
      aNorm = gp_Dir (aNorm).Transformed (aTrsf);
    }
    addPoint (aP1, aNorm, aUVNode, aFace);
  }
  return Standard_True;
}

// =======================================================================
// function : addTriangulationPoints
// purpose  :
// =======================================================================
Standard_Boolean  BRepLib_PointCloudShape::addTriangulationPoints (const TopoDS_Shape& theFace)
{
  TopLoc_Location aLoc;
  TopoDS_Face aFace = TopoDS::Face (theFace);
  Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (aFace, aLoc);
  if (aTriangulation.IsNull())
  {
    return Standard_False;
  }

  TopLoc_Location aLoc1;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface (aFace, aLoc1);
  const gp_Trsf& aTrsf = aLoc.Transformation();

  BRepLib_ToolTriangulatedShape::ComputeNormals (aFace, aTriangulation);
  Standard_Boolean aHasUVNode = aTriangulation->HasUVNodes();
  for (Standard_Integer aNodeIter = 1; aNodeIter <= aTriangulation->NbNodes(); ++aNodeIter)
  {
    gp_Pnt aP1     = aTriangulation->Node  (aNodeIter);
    gp_Dir aNormal = aTriangulation->Normal(aNodeIter);
    if (!aLoc.IsIdentity())
    {
      aP1    .Transform (aTrsf);
      aNormal.Transform (aTrsf);
    }

    const gp_Pnt2d anUVNode = aHasUVNode ? aTriangulation->UVNode (aNodeIter) : gp_Pnt2d();
    addPoint (aP1, aNormal, anUVNode, aFace);
  }
  return Standard_True;
}

// =======================================================================
// function : clear
// purpose  :
// =======================================================================
void BRepLib_PointCloudShape::clear()
{
  myFaceArea.Clear();
  myFacePoints.Clear();
}
