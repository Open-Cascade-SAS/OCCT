// Copyright (c) 1999-2017 OPEN CASCADE SAS
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

#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <BRepBndLib.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Bnd_OBB.hxx>
#include <BRepGProp.hxx>
#include <TopExp_Explorer.hxx>
#include <GProp_PrincipalProps.hxx>
#include <gp_Ax3.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_List.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HSurface.hxx>

#include <Geom_OffsetCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>

//=======================================================================
// Function : IsLinear
// purpose : Returns TRUE if theC is line-like.
//=======================================================================
static Standard_Boolean IsLinear(const Adaptor3d_Curve& theC)
{
  const GeomAbs_CurveType aCT = theC.GetType();
  if(aCT == GeomAbs_OffsetCurve)
  {
    return IsLinear(GeomAdaptor_Curve(theC.OffsetCurve()->BasisCurve()));
  }

  if((aCT == GeomAbs_BSplineCurve) || (aCT == GeomAbs_BezierCurve))
  {
    // Indeed, curves with C0-continuity and degree==1, may be 
    // represented with set of points. It will be possible made
    // in the future.

    return ((theC.Degree() == 1) &&
            (theC.Continuity() != GeomAbs_C0));
  }

  if(aCT == GeomAbs_Line)
  {
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
// Function : IsPlanar
// purpose : Returns TRUE if theS is plane-like.
//=======================================================================
static Standard_Boolean IsPlanar(const Adaptor3d_Surface& theS)
{
  const GeomAbs_SurfaceType aST = theS.GetType();
  if(aST == GeomAbs_OffsetSurface)
  {
    return IsPlanar(theS.BasisSurface()->Surface());
  }

  if(aST == GeomAbs_SurfaceOfExtrusion)
  {
    return IsLinear(theS.BasisCurve()->Curve());
  }

  if((aST == GeomAbs_BSplineSurface) || (aST == GeomAbs_BezierSurface))
  {
    if((theS.UDegree() != 1) || (theS.VDegree() != 1))
      return Standard_False;

    // Indeed, surfaces with C0-continuity and degree==1, may be 
    // represented with set of points. It will be possible made
    // in the future.

    return ((theS.UContinuity() != GeomAbs_C0) && (theS.VContinuity() != GeomAbs_C0));
  }

  if(aST == GeomAbs_Plane)
  {
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
// Function : PointsForOBB
// purpose : Returns number of points for array.
//
// Attention!!! 
//  1. Start index for thePts must be 0 strictly.
//  2. Currently, infinite edges/faces (e.g. half-space) are not
//      processed correctly because computation of UV-bounds is a costly operation.
//=======================================================================
static Standard_Integer PointsForOBB(const TopoDS_Shape& theS,
                                     const Standard_Boolean theIsTriangulationUsed,
                                     TColgp_Array1OfPnt* thePts = 0,
                                     TColStd_Array1OfReal* theArrOfToler = 0)
{
  Standard_Integer aRetVal = 0;
  TopExp_Explorer anExpF, anExpE;

  // get all vertices from the shape
  for(anExpF.Init(theS, TopAbs_VERTEX); anExpF.More(); anExpF.Next())
  {
    const TopoDS_Vertex &aVert = TopoDS::Vertex(anExpF.Current());
    if(thePts)
    {
      const gp_Pnt aP = BRep_Tool::Pnt(aVert);
      (*thePts)(aRetVal) = aP;
    }

    if(theArrOfToler)
    {
      (*theArrOfToler) (aRetVal) = BRep_Tool::Tolerance(aVert);
    }

    ++aRetVal;
  }

  if(aRetVal == 0)
    return 0;

  // analyze the faces of the shape on planarity and existence of triangulation
  TopLoc_Location aLoc;
  for(anExpF.Init(theS, TopAbs_FACE); anExpF.More(); anExpF.Next())
  {
    const TopoDS_Face &aF = TopoDS::Face(anExpF.Current());
    const BRepAdaptor_Surface anAS(aF, Standard_False);

    if (!IsPlanar(anAS.Surface()))
    {
      if (!theIsTriangulationUsed)
        // not planar and triangulation usage disabled
        return 0;
    }
    else
    {
      // planar face
      for(anExpE.Init(aF, TopAbs_EDGE); anExpE.More(); anExpE.Next())
      {
        const TopoDS_Edge &anE = TopoDS::Edge(anExpE.Current());
        const BRepAdaptor_Curve anAC(anE);
        if (!IsLinear(anAC))
        {
          if (!theIsTriangulationUsed)
            // not linear and triangulation usage disabled
            return 0;

          break;
        }
      }

      if (!anExpE.More())
        // skip planar face with linear edges as its vertices have already been added
        continue;
    }

    // Use triangulation of the face
    const Handle(Poly_Triangulation) &aTrng = BRep_Tool::Triangulation(aF, aLoc);
    if (aTrng.IsNull())
      // no triangulation on the face
      return 0;

    const Standard_Integer aCNode = aTrng->NbNodes();
    const TColgp_Array1OfPnt& aNodesArr = aTrng->Nodes();
    for (Standard_Integer i = 1; i <= aCNode; i++)
    {
      if (thePts)
      {
        const gp_Pnt aP = aLoc.IsIdentity() ? aNodesArr(i) :
          aNodesArr(i).Transformed(aLoc);
        (*thePts)(aRetVal) = aP;
      }

      if (theArrOfToler)
      {
        (*theArrOfToler) (aRetVal) = aTrng->Deflection();
      }

      ++aRetVal;
    }
  }

  // Consider edges without faces

  for(anExpE.Init(theS, TopAbs_EDGE, TopAbs_FACE); anExpE.More(); anExpE.Next())
  {
    const TopoDS_Edge &anE = TopoDS::Edge(anExpE.Current());
    const BRepAdaptor_Curve anAC(anE);

    if (IsLinear(anAC))
      // skip linear edge as its vertices have already been added
      continue;

    if (!theIsTriangulationUsed)
      // not linear and triangulation usage disabled
      return 0;

    const Handle(Poly_Polygon3D) &aPolygon = BRep_Tool::Polygon3D(anE, aLoc);
    if (aPolygon.IsNull())
      return 0;

    const Standard_Integer aCNode = aPolygon->NbNodes();
    const TColgp_Array1OfPnt& aNodesArr = aPolygon->Nodes();
    for (Standard_Integer i = 1; i <= aCNode; i++)
    {
      if (thePts)
      {
        const gp_Pnt aP = aLoc.IsIdentity() ? aNodesArr(i) :
          aNodesArr(i).Transformed(aLoc);
        (*thePts)(aRetVal) = aP;
      }

      if (theArrOfToler)
      {
        (*theArrOfToler) (aRetVal) = aPolygon->Deflection();
      }

      ++aRetVal;
    }
  }

  return aRetVal;
}

//=======================================================================
// Function : IsWCS
// purpose : Returns 0 if the theDir does not match any axis of WCS.
//            Otherwise, returns the index of correspond axis.
//=======================================================================
static Standard_Integer IsWCS(const gp_Dir& theDir)
{
  const Standard_Real aToler = Precision::Angular()*Precision::Angular();

  const Standard_Real aX = theDir.X(),
                      aY = theDir.Y(),
                      aZ = theDir.Z();

  const Standard_Real aVx = aY*aY + aZ*aZ,
                      aVy = aX*aX + aZ*aZ,
                      aVz = aX*aX + aY*aY;

  if(aVz < aToler)
    return 3; // Z-axis

  if(aVy < aToler)
    return 2; // Y-axis

  if(aVx < aToler)
    return 1; // X-axis

  return 0;
}

//=======================================================================
// Function : CheckPoints
// purpose : Collects points for DiTO algorithm for OBB construction on
//            linear/planar shapes and shapes having triangulation
//            (http://www.idt.mdh.se/~tla/publ/FastOBBs.pdf).
//=======================================================================
static Standard_Boolean CheckPoints(const TopoDS_Shape& theS,
                                    const Standard_Boolean theIsTriangulationUsed,
                                    const Standard_Boolean theIsShapeToleranceUsed,
                                    Bnd_OBB& theOBB)
{
  const Standard_Integer aNbPnts = PointsForOBB(theS, theIsTriangulationUsed);

  if(aNbPnts < 1)
    return Standard_False;

  TColgp_Array1OfPnt anArrPnts(0, theOBB.IsVoid() ? aNbPnts - 1 : aNbPnts + 7);
  TColStd_Array1OfReal anArrOfTolerances;
  if(theIsShapeToleranceUsed)
  {
    anArrOfTolerances.Resize(anArrPnts.Lower(), anArrPnts.Upper(), Standard_False);
    anArrOfTolerances.Init(0.0);
  }

  TColStd_Array1OfReal *aPtrArrTol = theIsShapeToleranceUsed ? &anArrOfTolerances : 0;

  PointsForOBB(theS, theIsTriangulationUsed, &anArrPnts, aPtrArrTol);

  if(!theOBB.IsVoid())
  {
    // All points of old OBB have zero-tolerance
    theOBB.GetVertex(&anArrPnts(aNbPnts));
  }

#if 0
  for(Standard_Integer i = anArrPnts.Lower(); i <= anArrPnts.Upper(); i++)
  {
    const gp_Pnt &aP = anArrPnts(i);
    std::cout << "point p" << i << " " << aP.X() << ", " << 
                                          aP.Y() << ", " << 
                                          aP.Z() << ", "<< std::endl;
  }
#endif

  theOBB.ReBuild(anArrPnts, aPtrArrTol);

  return (!theOBB.IsVoid());
}

//=======================================================================
// Function : ComputeProperties
// purpose : Computes properties of theS.
//=======================================================================
static void ComputeProperties(const TopoDS_Shape& theS,
                              GProp_GProps& theGCommon)
{
  TopExp_Explorer anExp;
  for(anExp.Init(theS, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    GProp_GProps aG;
    BRepGProp::VolumeProperties(anExp.Current(), aG, Standard_True);
    theGCommon.Add(aG);
  }

  for(anExp.Init(theS, TopAbs_FACE, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    GProp_GProps aG;
    BRepGProp::SurfaceProperties(anExp.Current(), aG, Standard_True);
    theGCommon.Add(aG);
  }

  for(anExp.Init(theS, TopAbs_EDGE, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    GProp_GProps aG;
    BRepGProp::LinearProperties(anExp.Current(), aG, Standard_True);
    theGCommon.Add(aG);
  }

  for(anExp.Init(theS, TopAbs_VERTEX, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    GProp_GProps aG(BRep_Tool::Pnt(TopoDS::Vertex(anExp.Current())));
    theGCommon.Add(aG);
  }
}

//=======================================================================
// Function : ComputePCA
// purpose : Creates OBB with axes of inertia.
//=======================================================================
static void ComputePCA(const TopoDS_Shape& theS,
                       Bnd_OBB& theOBB,
                       const Standard_Boolean theIsTriangulationUsed,
                       const Standard_Boolean theIsOptimal,
                       const Standard_Boolean theIsShapeToleranceUsed)
{
  // Compute the transformation matrix to obtain more tight bounding box
  GProp_GProps aGCommon;
  ComputeProperties(theS, aGCommon);

  // Transform the shape to the local coordinate system
  gp_Trsf aTrsf;

  const Standard_Integer anIdx1 =
                  IsWCS(aGCommon.PrincipalProperties().FirstAxisOfInertia());
  const Standard_Integer anIdx2 =
                  IsWCS(aGCommon.PrincipalProperties().SecondAxisOfInertia());

  if((anIdx1 == 0) || (anIdx2 == 0))
  {
    // Coordinate system in which the shape will have the optimal bounding box
    gp_Ax3 aLocCoordSys(aGCommon.CentreOfMass(),
                        aGCommon.PrincipalProperties().ThirdAxisOfInertia(),
                        aGCommon.PrincipalProperties().FirstAxisOfInertia());
    aTrsf.SetTransformation(aLocCoordSys);
  }

  const TopoDS_Shape aST = (aTrsf.Form() == gp_Identity) ? theS :
                                              theS.Moved(TopLoc_Location(aTrsf));

  // Initial axis-aligned BndBox
  Bnd_Box aShapeBox;
  if(theIsOptimal)
  {
    BRepBndLib::AddOptimal(aST, aShapeBox, theIsTriangulationUsed, theIsShapeToleranceUsed);
  }
  else
  {
    BRepBndLib::Add(aST, aShapeBox);
  }

  gp_Pnt aPMin = aShapeBox.CornerMin();
  gp_Pnt aPMax = aShapeBox.CornerMax();

  gp_XYZ aXDir(1, 0, 0);
  gp_XYZ aYDir(0, 1, 0);
  gp_XYZ aZDir(0, 0, 1);

  // Compute the center of the box
  gp_XYZ aCenter = (aPMin.XYZ() + aPMax.XYZ()) / 2.;

  // Compute the half diagonal size of the box.
  // It takes into account the gap.
  gp_XYZ anOBBHSize = (aPMax.XYZ() - aPMin.XYZ()) / 2.;

  // Apply transformation if necessary
  if(aTrsf.Form() != gp_Identity)
  {
    aTrsf.Invert();
    aTrsf.Transforms(aCenter);

    // Make transformation
    const Standard_Real * aMat = &aTrsf.HVectorialPart().Value(1, 1);
    // Compute axes directions of the box
    aXDir = gp_XYZ(aMat[0], aMat[3], aMat[6]);
    aYDir = gp_XYZ(aMat[1], aMat[4], aMat[7]);
    aZDir = gp_XYZ(aMat[2], aMat[5], aMat[8]);
  }

  if(theOBB.IsVoid())
  {
    // Create the OBB box

    // Set parameters to the OBB
    theOBB.SetCenter(aCenter);

    theOBB.SetXComponent(aXDir, anOBBHSize.X());
    theOBB.SetYComponent(aYDir, anOBBHSize.Y());
    theOBB.SetZComponent(aZDir, anOBBHSize.Z());
    theOBB.SetAABox(aTrsf.Form() == gp_Identity);
  }
  else
  {
    // Recreate the OBB box

    TColgp_Array1OfPnt aListOfPnts(0, 15);
    theOBB.GetVertex(&aListOfPnts(0));

    const Standard_Real aX = anOBBHSize.X();
    const Standard_Real aY = anOBBHSize.Y();
    const Standard_Real aZ = anOBBHSize.Z();

    const gp_XYZ aXext = aX*aXDir,
                 aYext = aY*aYDir,
                 aZext = aZ*aZDir;

    Standard_Integer aPntIdx = 8;
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

//=======================================================================
// Function : AddOBB
// purpose : 
//=======================================================================
void BRepBndLib::AddOBB(const TopoDS_Shape& theS,
                        Bnd_OBB& theOBB,
                        const Standard_Boolean theIsTriangulationUsed,
                        const Standard_Boolean theIsOptimal,
                        const Standard_Boolean theIsShapeToleranceUsed)
{
  if(CheckPoints(theS, theIsTriangulationUsed, theIsShapeToleranceUsed, theOBB))
    return;

  ComputePCA(theS, theOBB, theIsTriangulationUsed, theIsOptimal, theIsShapeToleranceUsed);
}
