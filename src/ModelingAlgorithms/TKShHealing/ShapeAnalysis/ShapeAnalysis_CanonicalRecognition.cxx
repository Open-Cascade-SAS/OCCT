// Created on: 2000-01-20
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software{ you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <ShapeAnalysis_CanonicalRecognition.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <GeomConvert_SurfToAnaSurf.hxx>
#include <GeomConvert_CurveToAnaCurve.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <GeomConvert_ConvType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <NCollection_Vector.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <BRepLib_FindSurface.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <math_Vector.hxx>
#include <GeomConvert_FuncSphereLSDist.hxx>
#include <GeomConvert_FuncCylinderLSDist.hxx>
#include <GeomConvert_FuncConeLSDist.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <math_PSO.hxx>
#include <math_Powell.hxx>
#include <ElSLib.hxx>

// Declaration of static functions
static int    GetNbPars(const GeomAbs_CurveType theTarget);
static int    GetNbPars(const GeomAbs_SurfaceType theTarget);
static bool   SetConicParameters(const GeomAbs_CurveType        theTarget,
                                 const occ::handle<Geom_Curve>& theConic,
                                 gp_Ax2&                        thePos,
                                 NCollection_Array1<double>&    theParams);
static bool   CompareConicParams(const GeomAbs_CurveType           theTarget,
                                 const double                      theTol,
                                 const gp_Ax2&                     theRefPos,
                                 const NCollection_Array1<double>& theRefParams,
                                 const gp_Ax2&                     thePos,
                                 const NCollection_Array1<double>& theParams);
static bool   SetSurfParams(const GeomAbs_SurfaceType        theTarget,
                            const occ::handle<Geom_Surface>& theElemSurf,
                            gp_Ax3&                          thePos,
                            NCollection_Array1<double>&      theParams);
static bool   CompareSurfParams(const GeomAbs_SurfaceType         theTarget,
                                const double                      theTol,
                                const gp_Ax3&                     theRefPos,
                                const NCollection_Array1<double>& theRefParams,
                                const gp_Ax3&                     thePos,
                                const NCollection_Array1<double>& theParams);
static double DeviationSurfParams(const GeomAbs_SurfaceType         theTarget,
                                  const gp_Ax3&                     theRefPos,
                                  const NCollection_Array1<double>& theRefParams,
                                  const gp_Ax3&                     thePos,
                                  const NCollection_Array1<double>& theParams);
static bool   GetSamplePoints(const TopoDS_Wire&                        theWire,
                              const double                              theTol,
                              const int                                 theMaxNbInt,
                              occ::handle<NCollection_HArray1<gp_XYZ>>& thePoints);
static double GetLSGap(const occ::handle<NCollection_HArray1<gp_XYZ>>& thePoints,
                       const GeomAbs_SurfaceType                       theTarget,
                       const gp_Ax3&                                   thePos,
                       const NCollection_Array1<double>&               theParams);
static void   FillSolverData(const GeomAbs_SurfaceType         theTarget,
                             const gp_Ax3&                     thePos,
                             const NCollection_Array1<double>& theParams,
                             math_Vector&                      theStartPoint,
                             math_Vector&                      theFBnd,
                             math_Vector&                      theLBnd,
                             const double                      theRelDev = 0.2);
static void   SetCanonicParameters(const GeomAbs_SurfaceType   theTarget,
                                   const math_Vector&          theSol,
                                   gp_Ax3&                     thePos,
                                   NCollection_Array1<double>& theParams);

//=================================================================================================

ShapeAnalysis_CanonicalRecognition::ShapeAnalysis_CanonicalRecognition()
    : mySType(TopAbs_SHAPE),
      myGap(-1.),
      myStatus(-1)
{
}

ShapeAnalysis_CanonicalRecognition::ShapeAnalysis_CanonicalRecognition(const TopoDS_Shape& theShape)
    : mySType(TopAbs_SHAPE),
      myGap(-1.),
      myStatus(-1)
{
  Init(theShape);
}

void ShapeAnalysis_CanonicalRecognition::SetShape(const TopoDS_Shape& theShape)
{
  Init(theShape);
}

//=================================================================================================

void ShapeAnalysis_CanonicalRecognition::Init(const TopoDS_Shape& theShape)
{
  TopAbs_ShapeEnum aT = theShape.ShapeType();
  switch (aT)
  {
    case TopAbs_SHELL:
    case TopAbs_FACE:
    case TopAbs_WIRE:
    case TopAbs_EDGE:
      myShape  = theShape;
      mySType  = aT;
      myStatus = 0;
      break;
    default:
      myStatus = 1;
      break;
  }
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsElementarySurf(const GeomAbs_SurfaceType   theTarget,
                                                          const double                theTol,
                                                          gp_Ax3&                     thePos,
                                                          NCollection_Array1<double>& theParams)
{
  if (myStatus != 0)
    return false;
  //
  if (mySType == TopAbs_FACE)
  {
    occ::handle<Geom_Surface> anElemSurf =
      GetSurface(TopoDS::Face(myShape), theTol, GeomConvert_Target, theTarget, myGap, myStatus);
    if (anElemSurf.IsNull())
      return false;
    //
    bool isOK = SetSurfParams(theTarget, anElemSurf, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return false;
    }
    return true;
  }
  else if (mySType == TopAbs_SHELL)
  {
    occ::handle<Geom_Surface> anElemSurf =
      GetSurface(TopoDS::Shell(myShape), theTol, GeomConvert_Target, theTarget, myGap, myStatus);
    if (anElemSurf.IsNull())
    {
      return false;
    }
    bool isOK = SetSurfParams(theTarget, anElemSurf, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return false;
    }
    return true;
  }
  else if (mySType == TopAbs_EDGE)
  {
    occ::handle<Geom_Surface> anElemSurf = GetSurface(TopoDS::Edge(myShape),
                                                      theTol,
                                                      GeomConvert_Target,
                                                      theTarget,
                                                      thePos,
                                                      theParams,
                                                      myGap,
                                                      myStatus);

    bool isOK = SetSurfParams(theTarget, anElemSurf, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return false;
    }
    return true;
  }
  else if (mySType == TopAbs_WIRE)
  {
    occ::handle<Geom_Surface> anElemSurf = GetSurface(TopoDS::Wire(myShape),
                                                      theTol,
                                                      GeomConvert_Target,
                                                      theTarget,
                                                      thePos,
                                                      theParams,
                                                      myGap,
                                                      myStatus);

    bool isOK = SetSurfParams(theTarget, anElemSurf, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return false;
    }
    return true;
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsPlane(const double theTol, gp_Pln& thePln)
{
  gp_Ax3                     aPos = thePln.Position();
  NCollection_Array1<double> aParams(1, 1);
  //
  GeomAbs_SurfaceType aTarget = GeomAbs_Plane;
  if (IsElementarySurf(aTarget, theTol, aPos, aParams))
  {
    thePln.SetPosition(aPos);
    return true;
  }
  // Try to build plane for wire or edge
  if (mySType == TopAbs_EDGE || mySType == TopAbs_WIRE)
  {
    BRepLib_FindSurface aFndSurf(myShape, theTol, true, false);
    if (aFndSurf.Found())
    {
      occ::handle<Geom_Plane> aPlane = occ::down_cast<Geom_Plane>(aFndSurf.Surface());
      thePln                         = aPlane->Pln();
      myGap                          = aFndSurf.ToleranceReached();
      myStatus                       = 0;
      return true;
    }
    else
      myStatus = 1;
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsCylinder(const double theTol, gp_Cylinder& theCyl)
{
  gp_Ax3                     aPos = theCyl.Position();
  NCollection_Array1<double> aParams(1, 1);
  aParams(1) = theCyl.Radius();
  //
  GeomAbs_SurfaceType aTarget = GeomAbs_Cylinder;
  if (IsElementarySurf(aTarget, theTol, aPos, aParams))
  {
    theCyl.SetPosition(aPos);
    theCyl.SetRadius(aParams(1));
    return true;
  }

  if (aParams(1) > Precision::Infinite())
  {
    // Sample cylinder does not seem to be set, least square method is not applicable.
    return false;
  }
  if (myShape.ShapeType() == TopAbs_EDGE || myShape.ShapeType() == TopAbs_WIRE)
  {
    // Try to build surface by least square method;
    TopoDS_Wire aWire;
    if (myShape.ShapeType() == TopAbs_EDGE)
    {
      BRep_Builder aBB;
      aBB.MakeWire(aWire);
      aBB.Add(aWire, myShape);
    }
    else
    {
      aWire = TopoDS::Wire(myShape);
    }
    bool isDone = GetSurfaceByLS(aWire, theTol, aTarget, aPos, aParams, myGap, myStatus);
    if (isDone)
    {
      theCyl.SetPosition(aPos);
      theCyl.SetRadius(aParams(1));
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsCone(const double theTol, gp_Cone& theCone)
{
  gp_Ax3                     aPos = theCone.Position();
  NCollection_Array1<double> aParams(1, 2);
  aParams(1) = theCone.SemiAngle();
  aParams(2) = theCone.RefRadius();
  //
  GeomAbs_SurfaceType aTarget = GeomAbs_Cone;
  if (IsElementarySurf(aTarget, theTol, aPos, aParams))
  {
    theCone.SetPosition(aPos);
    theCone.SetSemiAngle(aParams(1));
    theCone.SetRadius(aParams(2));
    return true;
  }

  if (aParams(2) > Precision::Infinite())
  {
    // Sample cone does not seem to be set, least square method is not applicable.
    return false;
  }
  if (myShape.ShapeType() == TopAbs_EDGE || myShape.ShapeType() == TopAbs_WIRE)
  {
    // Try to build surface by least square method;
    TopoDS_Wire aWire;
    if (myShape.ShapeType() == TopAbs_EDGE)
    {
      BRep_Builder aBB;
      aBB.MakeWire(aWire);
      aBB.Add(aWire, myShape);
    }
    else
    {
      aWire = TopoDS::Wire(myShape);
    }
    bool isDone = GetSurfaceByLS(aWire, theTol, aTarget, aPos, aParams, myGap, myStatus);
    if (isDone)
    {
      theCone.SetPosition(aPos);
      theCone.SetSemiAngle(aParams(1));
      theCone.SetRadius(aParams(2));
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsSphere(const double theTol, gp_Sphere& theSphere)
{
  gp_Ax3                     aPos = theSphere.Position();
  NCollection_Array1<double> aParams(1, 1);
  aParams(1) = theSphere.Radius();
  //
  GeomAbs_SurfaceType aTarget = GeomAbs_Sphere;
  if (IsElementarySurf(aTarget, theTol, aPos, aParams))
  {
    theSphere.SetPosition(aPos);
    theSphere.SetRadius(aParams(1));
    return true;
  }
  //
  if (aParams(1) > Precision::Infinite())
  {
    // Sample sphere does not seem to be set, least square method is not applicable.
    return false;
  }
  if (myShape.ShapeType() == TopAbs_EDGE || myShape.ShapeType() == TopAbs_WIRE)
  {
    // Try to build surface by least square method;
    TopoDS_Wire aWire;
    if (myShape.ShapeType() == TopAbs_EDGE)
    {
      BRep_Builder aBB;
      aBB.MakeWire(aWire);
      aBB.Add(aWire, myShape);
    }
    else
    {
      aWire = TopoDS::Wire(myShape);
    }
    bool isDone = GetSurfaceByLS(aWire, theTol, aTarget, aPos, aParams, myGap, myStatus);
    if (isDone)
    {
      theSphere.SetPosition(aPos);
      theSphere.SetRadius(aParams(1));
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsConic(const GeomAbs_CurveType     theTarget,
                                                 const double                theTol,
                                                 gp_Ax2&                     thePos,
                                                 NCollection_Array1<double>& theParams)
{
  if (myStatus != 0)
    return false;

  if (mySType == TopAbs_EDGE)
  {
    occ::handle<Geom_Curve> aConic =
      GetCurve(TopoDS::Edge(myShape), theTol, GeomConvert_Target, theTarget, myGap, myStatus);

    if (aConic.IsNull())
      return false;

    bool isOK = SetConicParameters(theTarget, aConic, thePos, theParams);

    if (!isOK)
    {
      myStatus = 1;
      return false;
    }
    return true;
  }
  else if (mySType == TopAbs_WIRE)
  {
    TopoDS_Iterator anIter(myShape);
    if (!anIter.More())
    {
      myStatus = 1;
      return false;
    }
    gp_Ax2                     aPos;
    NCollection_Array1<double> aParams(1, theParams.Length());
    const TopoDS_Shape&        anEdge = anIter.Value();

    occ::handle<Geom_Curve> aConic =
      GetCurve(TopoDS::Edge(anEdge), theTol, GeomConvert_Target, theTarget, myGap, myStatus);
    if (aConic.IsNull())
    {
      return false;
    }
    bool isOK = SetConicParameters(theTarget, aConic, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return false;
    }
    if (!anIter.More())
    {
      return true;
    }
    else
    {
      anIter.Next();
    }
    for (; anIter.More(); anIter.Next())
    {
      aConic = GetCurve(TopoDS::Edge(anIter.Value()),
                        theTol,
                        GeomConvert_Target,
                        theTarget,
                        myGap,
                        myStatus);
      if (aConic.IsNull())
      {
        return false;
      }
      isOK = SetConicParameters(theTarget, aConic, aPos, aParams);
      isOK = CompareConicParams(theTarget, theTol, thePos, theParams, aPos, aParams);

      if (!isOK)
      {
        return false;
      }
    }
    return true;
  }
  myStatus = 1;
  return false;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsLine(const double theTol, gp_Lin& theLin)
{
  gp_Ax2                     aPos;
  NCollection_Array1<double> aParams(1, 1);

  GeomAbs_CurveType aTarget = GeomAbs_Line;
  bool              isOK    = IsConic(aTarget, theTol, aPos, aParams);
  if (isOK)
  {
    theLin.SetPosition(aPos.Axis());
  }
  return isOK;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsCircle(const double theTol, gp_Circ& theCirc)
{
  gp_Ax2                     aPos;
  NCollection_Array1<double> aParams(1, 1);

  GeomAbs_CurveType aTarget = GeomAbs_Circle;
  bool              isOK    = IsConic(aTarget, theTol, aPos, aParams);
  if (isOK)
  {
    theCirc.SetPosition(aPos);
    theCirc.SetRadius(aParams(1));
  }
  return isOK;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::IsEllipse(const double theTol, gp_Elips& theElips)
{
  gp_Ax2                     aPos;
  NCollection_Array1<double> aParams(1, 2);

  GeomAbs_CurveType aTarget = GeomAbs_Ellipse;
  bool              isOK    = IsConic(aTarget, theTol, aPos, aParams);
  if (isOK)
  {
    theElips.SetPosition(aPos);
    theElips.SetMajorRadius(aParams(1));
    theElips.SetMinorRadius(aParams(2));
  }
  return isOK;
}

//=================================================================================================

occ::handle<Geom_Surface> ShapeAnalysis_CanonicalRecognition::GetSurface(
  const TopoDS_Face&         theFace,
  const double               theTol,
  const GeomConvert_ConvType theType,
  const GeomAbs_SurfaceType  theTarget,
  double&                    theGap,
  int&                       theStatus)
{
  theStatus = 0;
  TopLoc_Location                  aLoc;
  const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(theFace, aLoc);
  if (aSurf.IsNull())
  {
    theStatus = 1;
    return aSurf;
  }
  GeomConvert_SurfToAnaSurf aConv(aSurf);
  aConv.SetConvType(theType);
  aConv.SetTarget(theTarget);
  occ::handle<Geom_Surface> anAnaSurf = aConv.ConvertToAnalytical(theTol);
  if (anAnaSurf.IsNull())
    return anAnaSurf;
  //
  if (!aLoc.IsIdentity())
    anAnaSurf->Transform(aLoc.Transformation());
  //
  theGap = aConv.Gap();
  return anAnaSurf;
}

//=================================================================================================

occ::handle<Geom_Surface> ShapeAnalysis_CanonicalRecognition::GetSurface(
  const TopoDS_Shell&        theShell,
  const double               theTol,
  const GeomConvert_ConvType theType,
  const GeomAbs_SurfaceType  theTarget,
  double&                    theGap,
  int&                       theStatus)
{
  occ::handle<Geom_Surface> anElemSurf;
  TopoDS_Iterator           anIter(theShell);
  if (!anIter.More())
  {
    theStatus = 1;
    return anElemSurf;
  }
  gp_Ax3                     aPos1;
  NCollection_Array1<double> aParams1(1, std::max(1, GetNbPars(theTarget)));
  const TopoDS_Shape&        aFace = anIter.Value();

  anElemSurf = GetSurface(TopoDS::Face(aFace), theTol, theType, theTarget, theGap, theStatus);
  if (anElemSurf.IsNull())
  {
    return anElemSurf;
  }
  SetSurfParams(theTarget, anElemSurf, aPos1, aParams1);
  if (!anIter.More())
  {
    return anElemSurf;
  }
  else
  {
    anIter.Next();
  }
  gp_Ax3                     aPos;
  NCollection_Array1<double> aParams(1, std::max(1, GetNbPars(theTarget)));
  bool                       isOK;
  for (; anIter.More(); anIter.Next())
  {
    occ::handle<Geom_Surface> anElemSurf1 =
      GetSurface(TopoDS::Face(anIter.Value()), theTol, theType, theTarget, theGap, theStatus);
    if (anElemSurf1.IsNull())
    {
      return anElemSurf1;
    }
    SetSurfParams(theTarget, anElemSurf, aPos, aParams);
    isOK = CompareSurfParams(theTarget, theTol, aPos1, aParams1, aPos, aParams);

    if (!isOK)
    {
      anElemSurf.Nullify();
      return anElemSurf;
    }
  }
  return anElemSurf;
}

//=================================================================================================

occ::handle<Geom_Surface> ShapeAnalysis_CanonicalRecognition::GetSurface(
  const TopoDS_Edge&          theEdge,
  const double                theTol,
  const GeomConvert_ConvType  theType,
  const GeomAbs_SurfaceType   theTarget,
  gp_Ax3&                     thePos,
  NCollection_Array1<double>& theParams,
  double&                     theGap,
  int&                        theStatus)
{
  // Get surface list
  NCollection_Vector<occ::handle<Geom_Surface>> aSurfs;
  NCollection_Vector<double>                    aGaps;
  int                                           j = 0;
  for (;;)
  {
    j++;
    occ::handle<Geom_Surface> aSurf;
    TopLoc_Location           aLoc;
    occ::handle<Geom2d_Curve> aPCurve;
    double                    ff, ll;
    BRep_Tool::CurveOnSurface(theEdge, aPCurve, aSurf, aLoc, ff, ll, j);
    if (aSurf.IsNull())
    {
      break;
    }
    GeomConvert_SurfToAnaSurf aConv(aSurf);
    aConv.SetConvType(theType);
    aConv.SetTarget(theTarget);
    occ::handle<Geom_Surface> anAnaSurf = aConv.ConvertToAnalytical(theTol);
    if (anAnaSurf.IsNull())
      continue;
    //
    if (!aLoc.IsIdentity())
      anAnaSurf->Transform(aLoc.Transformation());
    //
    aGaps.Append(aConv.Gap());
    aSurfs.Append(anAnaSurf);
  }

  if (aSurfs.Size() == 0)
  {
    theStatus = 1;
    return nullptr;
  }

  gp_Ax3                     aPos;
  int                        aNbPars = std::max(1, GetNbPars(theTarget));
  NCollection_Array1<double> aParams(1, aNbPars);

  int    ifit    = -1, i;
  double aMinDev = RealLast();
  if (aSurfs.Size() == 1)
  {
    ifit = 0;
  }
  else
  {
    for (i = 0; i < aSurfs.Size(); ++i)
    {
      SetSurfParams(theTarget, aSurfs(i), aPos, aParams);
      double aDev = DeviationSurfParams(theTarget, thePos, theParams, aPos, aParams);
      if (aDev < aMinDev)
      {
        aMinDev = aDev;
        ifit    = i;
      }
    }
  }
  if (ifit >= 0)
  {
    SetSurfParams(theTarget, aSurfs(ifit), thePos, theParams);
    theGap = aGaps(ifit);
    return aSurfs(ifit);
  }
  else
  {
    theStatus = 1;
    return nullptr;
  }
}

//=================================================================================================

occ::handle<Geom_Surface> ShapeAnalysis_CanonicalRecognition::GetSurface(
  const TopoDS_Wire&          theWire,
  const double                theTol,
  const GeomConvert_ConvType  theType,
  const GeomAbs_SurfaceType   theTarget,
  gp_Ax3&                     thePos,
  NCollection_Array1<double>& theParams,
  double&                     theGap,
  int&                        theStatus)
{
  // Get surface list
  NCollection_Vector<occ::handle<Geom_Surface>> aSurfs;
  NCollection_Vector<double>                    aGaps;

  TopoDS_Iterator anIter(theWire);
  if (!anIter.More())
  {
    // Empty wire
    theStatus = 1;
    return nullptr;
  }
  // First edge
  const TopoDS_Edge&         anEdge1 = TopoDS::Edge(anIter.Value());
  gp_Ax3                     aPos1   = thePos;
  int                        aNbPars = GetNbPars(theTarget);
  NCollection_Array1<double> aParams1(1, std::max(1, aNbPars));
  double                     aGap1;
  int                        i;
  for (i = 1; i <= aNbPars; ++i)
  {
    aParams1(i) = theParams(i);
  }
  occ::handle<Geom_Surface> anElemSurf1 =
    GetSurface(anEdge1, theTol, theType, theTarget, aPos1, aParams1, aGap1, theStatus);
  if (theStatus != 0 || anElemSurf1.IsNull())
  {
    return nullptr;
  }
  anIter.Next();
  for (; anIter.More(); anIter.Next())
  {
    gp_Ax3 aPos = aPos1;
    aNbPars     = GetNbPars(theTarget);
    NCollection_Array1<double> aParams(1, std::max(1, aNbPars));
    for (i = 1; i <= aNbPars; ++i)
    {
      aParams(i) = aParams1(i);
    }
    double                    aGap;
    const TopoDS_Edge&        anEdge = TopoDS::Edge(anIter.Value());
    occ::handle<Geom_Surface> anElemSurf =
      GetSurface(anEdge, theTol, theType, theTarget, aPos, aParams, aGap, theStatus);
    if (theStatus != 0 || anElemSurf.IsNull())
    {
      return nullptr;
    }
    bool isOK = CompareSurfParams(theTarget, theTol, aPos1, aParams1, aPos, aParams);
    if (!isOK)
    {
      return nullptr;
    }
  }

  SetSurfParams(theTarget, anElemSurf1, thePos, theParams);
  theGap = aGap1;
  return anElemSurf1;
}

//=================================================================================================

bool ShapeAnalysis_CanonicalRecognition::GetSurfaceByLS(const TopoDS_Wire&          theWire,
                                                        const double                theTol,
                                                        const GeomAbs_SurfaceType   theTarget,
                                                        gp_Ax3&                     thePos,
                                                        NCollection_Array1<double>& theParams,
                                                        double&                     theGap,
                                                        int&                        theStatus)
{
  occ::handle<NCollection_HArray1<gp_XYZ>> aPoints;
  int                                      aNbMaxInt = 100;
  if (!GetSamplePoints(theWire, theTol, aNbMaxInt, aPoints))
    return false;

  theGap = GetLSGap(aPoints, theTarget, thePos, theParams);
  if (theGap <= theTol)
  {
    theStatus = 0;
    return true;
  }

  int aNbVar = 0;
  if (theTarget == GeomAbs_Sphere)
    aNbVar = 4;
  else if (theTarget == GeomAbs_Cylinder)
    aNbVar = 4;
  else if (theTarget == GeomAbs_Cone)
    aNbVar = 5;
  else
    return false;

  math_Vector aFBnd(1, aNbVar), aLBnd(1, aNbVar), aStartPoint(1, aNbVar);

  double aRelDev = 0.2; // Customer can set parameters of sample surface
                        //  with relative precision about aRelDev.
                        //  For example, if radius of sample surface is R,
                        //  it means, that "exact" value is in interav
                        //[R - aRelDev*R, R + aRelDev*R]. This interval is set
                        //  for R as boundary values for optimization algo.
  FillSolverData(theTarget, thePos, theParams, aStartPoint, aFBnd, aLBnd, aRelDev);

  //
  constexpr double               aTol = Precision::Confusion();
  math_MultipleVarFunction*      aPFunc;
  GeomConvert_FuncSphereLSDist   aFuncSph(aPoints);
  GeomConvert_FuncCylinderLSDist aFuncCyl(aPoints, thePos.Direction());
  GeomConvert_FuncConeLSDist     aFuncCon(aPoints, thePos.Direction());
  if (theTarget == GeomAbs_Sphere)
  {
    aPFunc = (math_MultipleVarFunction*)&aFuncSph;
  }
  else if (theTarget == GeomAbs_Cylinder)
  {
    aPFunc = (math_MultipleVarFunction*)&aFuncCyl;
  }
  else if (theTarget == GeomAbs_Cone)
  {
    aPFunc = (math_MultipleVarFunction*)&aFuncCon;
  }
  else
    aPFunc = nullptr;
  //
  math_Vector aSteps(1, aNbVar);
  int         aNbInt = 10;
  int         i;
  for (i = 1; i <= aNbVar; ++i)
  {
    aSteps(i) = (aLBnd(i) - aFBnd(i)) / aNbInt;
  }
  math_PSO aGlobSolver(aPFunc, aFBnd, aLBnd, aSteps);
  double   aLSDist;
  aGlobSolver.Perform(aSteps, aLSDist, aStartPoint);
  SetCanonicParameters(theTarget, aStartPoint, thePos, theParams);

  theGap = GetLSGap(aPoints, theTarget, thePos, theParams);
  if (theGap <= theTol)
  {
    theStatus = 0;
    return true;
  }
  //
  math_Matrix aDirMatrix(1, aNbVar, 1, aNbVar, 0.0);
  for (i = 1; i <= aNbVar; i++)
    aDirMatrix(i, i) = 1.0;

  if (theTarget == GeomAbs_Cylinder || theTarget == GeomAbs_Cone)
  {
    // Set search direction for location to be perpendicular to axis to avoid
    // searching along axis
    const gp_Dir aDir = thePos.Direction();
    gp_Pln       aPln(thePos.Location(), aDir);
    gp_Dir       aUDir = aPln.Position().XDirection();
    gp_Dir       aVDir = aPln.Position().YDirection();
    for (i = 1; i <= 3; ++i)
    {
      aDirMatrix(i, 1) = aUDir.Coord(i);
      aDirMatrix(i, 2) = aVDir.Coord(i);
      gp_Dir aUVDir(aUDir.XYZ() + aVDir.XYZ());
      aDirMatrix(i, 3) = aUVDir.Coord(i);
    }
  }

  math_Powell aSolver(*aPFunc, aTol);
  aSolver.Perform(*aPFunc, aStartPoint, aDirMatrix);

  if (aSolver.IsDone())
  {
    aSolver.Location(aStartPoint);
    theStatus = 0;
    SetCanonicParameters(theTarget, aStartPoint, thePos, theParams);
    theGap    = GetLSGap(aPoints, theTarget, thePos, theParams);
    theStatus = 0;
    if (theGap <= theTol)
      return true;
  }
  else
    theStatus = 1;

  return false;
}

//=================================================================================================

occ::handle<Geom_Curve> ShapeAnalysis_CanonicalRecognition::GetCurve(
  const TopoDS_Edge&         theEdge,
  const double               theTol,
  const GeomConvert_ConvType theType,
  const GeomAbs_CurveType    theTarget,
  double&                    theGap,
  int&                       theStatus)
{
  theStatus = 0;
  TopLoc_Location                aLoc;
  double                         f, l, nf, nl;
  const occ::handle<Geom_Curve>& aCurv = BRep_Tool::Curve(theEdge, aLoc, f, l);
  if (aCurv.IsNull())
  {
    theStatus = 1;
    return aCurv;
  }
  GeomConvert_CurveToAnaCurve aConv(aCurv);
  aConv.SetConvType(theType);
  aConv.SetTarget(theTarget);
  occ::handle<Geom_Curve> anAnaCurv;
  aConv.ConvertToAnalytical(theTol, anAnaCurv, f, l, nf, nl);
  if (anAnaCurv.IsNull())
    return anAnaCurv;
  //
  if (!aLoc.IsIdentity())
    anAnaCurv->Transform(aLoc.Transformation());
  //
  theGap = aConv.Gap();
  return anAnaCurv;
}

// Static methods
//=================================================================================================

int GetNbPars(const GeomAbs_CurveType theTarget)
{
  int aNbPars = 0;
  switch (theTarget)
  {
    case GeomAbs_Line:
      aNbPars = 0;
      break;
    case GeomAbs_Circle:
      aNbPars = 1;
      break;
    case GeomAbs_Ellipse:
      aNbPars = 2;
      break;
    default:
      aNbPars = 0;
      break;
  }

  return aNbPars;
}

//=================================================================================================

int GetNbPars(const GeomAbs_SurfaceType theTarget)
{
  int aNbPars = 0;
  switch (theTarget)
  {
    case GeomAbs_Plane:
      aNbPars = 0;
      break;
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
      aNbPars = 1;
      break;
    case GeomAbs_Cone:
      aNbPars = 2;
      break;
    default:
      aNbPars = 0;
      break;
  }

  return aNbPars;
}

//=================================================================================================

bool SetConicParameters(const GeomAbs_CurveType        theTarget,
                        const occ::handle<Geom_Curve>& theConic,
                        gp_Ax2&                        thePos,
                        NCollection_Array1<double>&    theParams)
{
  if (theConic.IsNull())
    return false;
  GeomAdaptor_Curve aGAC(theConic);
  if (aGAC.GetType() != theTarget)
    return false;

  if (theTarget == GeomAbs_Line)
  {
    gp_Lin aLin = aGAC.Line();
    thePos.SetAxis(aLin.Position());
  }
  else if (theTarget == GeomAbs_Circle)
  {
    gp_Circ aCirc = aGAC.Circle();
    thePos        = aCirc.Position();
    theParams(1)  = aCirc.Radius();
  }
  else if (theTarget == GeomAbs_Ellipse)
  {
    gp_Elips anElips = aGAC.Ellipse();
    thePos           = anElips.Position();
    theParams(1)     = anElips.MajorRadius();
    theParams(2)     = anElips.MinorRadius();
  }
  else
    return false;
  return true;
}

//=================================================================================================

bool CompareConicParams(const GeomAbs_CurveType           theTarget,
                        const double                      theTol,
                        const gp_Ax2&                     theRefPos,
                        const NCollection_Array1<double>& theRefParams,
                        const gp_Ax2&                     thePos,
                        const NCollection_Array1<double>& theParams)
{
  int i, aNbPars = GetNbPars(theTarget);

  for (i = 1; i <= aNbPars; ++i)
  {
    if (std::abs(theRefParams(i) - theParams(i)) > theTol)
      return false;
  }

  double anAngTol = theTol / (2. * M_PI);
  double aTol     = theTol;
  if (theTarget == GeomAbs_Line)
    aTol = Precision::Infinite();

  const gp_Ax1& aRef     = theRefPos.Axis();
  const gp_Ax1& anAx1    = thePos.Axis();
  gp_Ax1        anAx1Rev = anAx1.Reversed();

  return aRef.IsCoaxial(anAx1, anAngTol, aTol) || aRef.IsCoaxial(anAx1Rev, anAngTol, aTol);
}

//=================================================================================================

bool SetSurfParams(const GeomAbs_SurfaceType        theTarget,
                   const occ::handle<Geom_Surface>& theElemSurf,
                   gp_Ax3&                          thePos,
                   NCollection_Array1<double>&      theParams)
{
  //
  if (theElemSurf.IsNull())
    return false;
  GeomAdaptor_Surface aGAS(theElemSurf);
  if (aGAS.GetType() != theTarget)
    return false;

  double aNbPars = GetNbPars(theTarget);
  if (theParams.Length() < aNbPars)
    return false;

  if (theTarget == GeomAbs_Plane)
  {
    gp_Pln aPln = aGAS.Plane();
    thePos      = aPln.Position();
  }
  else if (theTarget == GeomAbs_Cylinder)
  {
    gp_Cylinder aCyl = aGAS.Cylinder();
    thePos           = aCyl.Position();
    theParams(1)     = aCyl.Radius();
  }
  else if (theTarget == GeomAbs_Cone)
  {
    gp_Cone aCon = aGAS.Cone();
    thePos       = aCon.Position();
    theParams(1) = aCon.SemiAngle();
    theParams(2) = aCon.RefRadius();
  }
  else if (theTarget == GeomAbs_Sphere)
  {
    gp_Sphere aSph = aGAS.Sphere();
    thePos         = aSph.Position();
    theParams(1)   = aSph.Radius();
  }
  else
  {
    return false;
  }
  return true;
}

//=================================================================================================

bool CompareSurfParams(const GeomAbs_SurfaceType         theTarget,
                       const double                      theTol,
                       const gp_Ax3&                     theRefPos,
                       const NCollection_Array1<double>& theRefParams,
                       const gp_Ax3&                     thePos,
                       const NCollection_Array1<double>& theParams)
{
  if (theTarget != GeomAbs_Plane)
  {
    if (std::abs(theRefParams(1) - theParams(1)) > theTol)
    {
      return false;
    }
  }
  //
  if (theTarget == GeomAbs_Sphere)
  {
    gp_Pnt aRefLoc = theRefPos.Location(), aLoc = thePos.Location();
    return aRefLoc.SquareDistance(aLoc) <= theTol * theTol;
  }
  //
  double anAngTol = theTol / (2. * M_PI);
  double aTol     = theTol;
  if (theTarget == GeomAbs_Cylinder || theTarget == GeomAbs_Cone)
  {
    aTol = Precision::Infinite();
  }

  const gp_Ax1& aRef     = theRefPos.Axis();
  const gp_Ax1& anAx1    = thePos.Axis();
  gp_Ax1        anAx1Rev = anAx1.Reversed();
  if (!(aRef.IsCoaxial(anAx1, anAngTol, aTol) || aRef.IsCoaxial(anAx1Rev, anAngTol, aTol)))
  {
    return false;
  }

  if (theTarget == GeomAbs_Cone)
  {
    gp_Cone aRefCone(theRefPos, theRefParams(1), theRefParams(2));
    gp_Cone aCone(thePos, theParams(1), theParams(2));
    gp_Pnt  aRefApex = aRefCone.Apex();
    gp_Pnt  anApex   = aCone.Apex();
    return aRefApex.SquareDistance(anApex) <= theTol * theTol;
  }

  return true;
}

//=================================================================================================

double DeviationSurfParams(const GeomAbs_SurfaceType         theTarget,
                           const gp_Ax3&                     theRefPos,
                           const NCollection_Array1<double>& theRefParams,
                           const gp_Ax3&                     thePos,
                           const NCollection_Array1<double>& theParams)
{
  double aDevPars = 0.;
  if (theTarget != GeomAbs_Plane)
  {
    aDevPars = std::abs(theRefParams(1) - theParams(1));
  }
  //
  if (theTarget == GeomAbs_Sphere)
  {
    gp_Pnt aRefLoc = theRefPos.Location(), aLoc = thePos.Location();
    aDevPars += aRefLoc.Distance(aLoc);
  }
  else
  {
    const gp_Dir& aRefDir  = theRefPos.Direction();
    const gp_Dir& aDir     = thePos.Direction();
    double        anAngDev = (1. - std::abs(aRefDir * aDir));
    aDevPars += anAngDev;
  }

  return aDevPars;
}

//=================================================================================================

bool GetSamplePoints(const TopoDS_Wire&                        theWire,
                     const double                              theTol,
                     const int                                 theMaxNbInt,
                     occ::handle<NCollection_HArray1<gp_XYZ>>& thePoints)
{
  NCollection_Vector<double>            aLengths;
  NCollection_Vector<BRepAdaptor_Curve> aCurves;
  NCollection_Vector<gp_XYZ>            aPoints;
  double                                aTol         = std::max(1.e-3, theTol / 10.);
  double                                aTotalLength = 0.;
  TopoDS_Iterator                       anEIter(theWire);
  for (; anEIter.More(); anEIter.Next())
  {
    const TopoDS_Edge& anE = TopoDS::Edge(anEIter.Value());
    if (BRep_Tool::Degenerated(anE))
      continue;
    BRepAdaptor_Curve aBAC(anE);
    double            aClength = GCPnts_AbscissaPoint::Length(aBAC, aTol);
    aTotalLength += aClength;
    aCurves.Append(aBAC);
    aLengths.Append(aClength);
  }

  if (aTotalLength < theTol)
    return false;

  int i, aNb = aLengths.Length();
  for (i = 0; i < aNb; ++i)
  {
    const BRepAdaptor_Curve& aC        = aCurves(i);
    double                   aClength  = GCPnts_AbscissaPoint::Length(aC, aTol);
    int                      aNbPoints = RealToInt(aClength / aTotalLength * theMaxNbInt + 1);
    aNbPoints                          = std::max(2, aNbPoints);
    GCPnts_QuasiUniformAbscissa aPointGen(aC, aNbPoints);
    if (!aPointGen.IsDone())
      continue;
    aNbPoints = aPointGen.NbPoints();
    int j;
    for (j = 1; j <= aNbPoints; ++j)
    {
      double t  = aPointGen.Parameter(j);
      gp_Pnt aP = aC.Value(t);
      aPoints.Append(aP.XYZ());
    }
  }

  if (aPoints.Length() < 1)
    return false;

  thePoints = new NCollection_HArray1<gp_XYZ>(1, aPoints.Length());
  for (i = 0; i < aPoints.Length(); ++i)
  {
    thePoints->SetValue(i + 1, aPoints(i));
  }

  return true;
}

//=================================================================================================

static double GetLSGap(const occ::handle<NCollection_HArray1<gp_XYZ>>& thePoints,
                       const GeomAbs_SurfaceType                       theTarget,
                       const gp_Ax3&                                   thePos,
                       const NCollection_Array1<double>&               theParams)
{

  double aGap = 0.;
  gp_XYZ aLoc = thePos.Location().XYZ();
  gp_Vec aDir(thePos.Direction());

  int i;
  if (theTarget == GeomAbs_Sphere)
  {
    double anR = theParams(1);
    for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i)
    {
      gp_XYZ aD = thePoints->Value(i) - aLoc;
      aGap      = std::max(aGap, std::abs((aD.Modulus() - anR)));
    }
  }
  else if (theTarget == GeomAbs_Cylinder)
  {
    double anR = theParams(1);
    for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i)
    {
      gp_Vec aD(thePoints->Value(i) - aLoc);
      aD.Cross(aDir);
      aGap = std::max(aGap, std::abs((aD.Magnitude() - anR)));
    }
  }
  else if (theTarget == GeomAbs_Cone)
  {
    double anAng = theParams(1);
    double anR   = theParams(2);
    for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i)
    {
      double u, v;
      gp_Pnt aPi(thePoints->Value(i));
      ElSLib::ConeParameters(thePos, anR, anAng, aPi, u, v);
      gp_Pnt aPp;
      ElSLib::ConeD0(u, v, thePos, anR, anAng, aPp);
      aGap = std::max(aGap, aPi.SquareDistance(aPp));
    }
    aGap = std::sqrt(aGap);
  }

  return aGap;
}

//=================================================================================================

void FillSolverData(const GeomAbs_SurfaceType         theTarget,
                    const gp_Ax3&                     thePos,
                    const NCollection_Array1<double>& theParams,
                    math_Vector&                      theStartPoint,
                    math_Vector&                      theFBnd,
                    math_Vector&                      theLBnd,
                    const double                      theRelDev)
{
  if (theTarget == GeomAbs_Sphere || theTarget == GeomAbs_Cylinder)
  {
    theStartPoint(1) = thePos.Location().X();
    theStartPoint(2) = thePos.Location().Y();
    theStartPoint(3) = thePos.Location().Z();
    theStartPoint(4) = theParams(1);
    double aDR       = theRelDev * theParams(1);
    double aDXYZ     = aDR;
    int    i;
    for (i = 1; i <= 3; ++i)
    {
      theFBnd(i) = theStartPoint(i) - aDXYZ;
      theLBnd(i) = theStartPoint(i) + aDXYZ;
    }
    theFBnd(4) = theStartPoint(4) - aDR;
    theLBnd(4) = theStartPoint(4) + aDR;
  }
  if (theTarget == GeomAbs_Cone)
  {
    theStartPoint(1) = thePos.Location().X();
    theStartPoint(2) = thePos.Location().Y();
    theStartPoint(3) = thePos.Location().Z();
    theStartPoint(4) = theParams(1); // SemiAngle
    theStartPoint(5) = theParams(2); // Radius
    double aDR       = theRelDev * theParams(2);
    if (aDR < Precision::Confusion())
    {
      aDR = 0.1;
    }
    double aDXYZ = aDR;
    double aDAng = theRelDev * std::abs(theParams(1));
    int    i;
    for (i = 1; i <= 3; ++i)
    {
      theFBnd(i) = theStartPoint(i) - aDXYZ;
      theLBnd(i) = theStartPoint(i) + aDXYZ;
    }
    if (theParams(1) >= 0.)
    {
      theFBnd(4) = theStartPoint(4) - aDAng;
      theLBnd(4) = std::min(M_PI_2, theStartPoint(4) + aDR);
    }
    else
    {
      theFBnd(4) = std::max(-M_PI_2, theStartPoint(4) - aDAng);
      theLBnd(4) = theStartPoint(4) + aDAng;
    }
    theFBnd(5) = theStartPoint(5) - aDR;
    theLBnd(5) = theStartPoint(5) + aDR;
  }
}

//=================================================================================================

void SetCanonicParameters(const GeomAbs_SurfaceType   theTarget,
                          const math_Vector&          theSol,
                          gp_Ax3&                     thePos,
                          NCollection_Array1<double>& theParams)
{
  gp_Pnt aLoc(theSol(1), theSol(2), theSol(3));
  thePos.SetLocation(aLoc);
  if (theTarget == GeomAbs_Sphere || theTarget == GeomAbs_Cylinder)
  {
    theParams(1) = theSol(4); // radius
  }
  else if (theTarget == GeomAbs_Cone)
  {
    theParams(1) = theSol(4); // semiangle
    theParams(2) = theSol(5); // radius
  }
}