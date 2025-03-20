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
#include <TColgp_HArray1OfXYZ.hxx>
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
static Standard_Integer GetNbPars(const GeomAbs_CurveType theTarget);
static Standard_Integer GetNbPars(const GeomAbs_SurfaceType theTarget);
static Standard_Boolean SetConicParameters(const GeomAbs_CurveType   theTarget,
                                           const Handle(Geom_Curve)& theConic,
                                           gp_Ax2&                   thePos,
                                           TColStd_Array1OfReal&     theParams);
static Standard_Boolean CompareConicParams(const GeomAbs_CurveType     theTarget,
                                           const Standard_Real         theTol,
                                           const gp_Ax2&               theRefPos,
                                           const TColStd_Array1OfReal& theRefParams,
                                           const gp_Ax2&               thePos,
                                           const TColStd_Array1OfReal& theParams);
static Standard_Boolean SetSurfParams(const GeomAbs_SurfaceType   theTarget,
                                      const Handle(Geom_Surface)& theElemSurf,
                                      gp_Ax3&                     thePos,
                                      TColStd_Array1OfReal&       theParams);
static Standard_Boolean CompareSurfParams(const GeomAbs_SurfaceType   theTarget,
                                          const Standard_Real         theTol,
                                          const gp_Ax3&               theRefPos,
                                          const TColStd_Array1OfReal& theRefParams,
                                          const gp_Ax3&               thePos,
                                          const TColStd_Array1OfReal& theParams);
static Standard_Real    DeviationSurfParams(const GeomAbs_SurfaceType   theTarget,
                                            const gp_Ax3&               theRefPos,
                                            const TColStd_Array1OfReal& theRefParams,
                                            const gp_Ax3&               thePos,
                                            const TColStd_Array1OfReal& theParams);
static Standard_Boolean GetSamplePoints(const TopoDS_Wire&           theWire,
                                        const Standard_Real          theTol,
                                        const Standard_Integer       theMaxNbInt,
                                        Handle(TColgp_HArray1OfXYZ)& thePoints);
static Standard_Real    GetLSGap(const Handle(TColgp_HArray1OfXYZ)& thePoints,
                                 const GeomAbs_SurfaceType          theTarget,
                                 const gp_Ax3&                      thePos,
                                 const TColStd_Array1OfReal&        theParams);
static void             FillSolverData(const GeomAbs_SurfaceType   theTarget,
                                       const gp_Ax3&               thePos,
                                       const TColStd_Array1OfReal& theParams,
                                       math_Vector&                theStartPoint,
                                       math_Vector&                theFBnd,
                                       math_Vector&                theLBnd,
                                       const Standard_Real         theRelDev = 0.2);
static void             SetCanonicParameters(const GeomAbs_SurfaceType theTarget,
                                             const math_Vector&        theSol,
                                             gp_Ax3&                   thePos,
                                             TColStd_Array1OfReal&     theParams);

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

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsElementarySurf(
  const GeomAbs_SurfaceType theTarget,
  const Standard_Real       theTol,
  gp_Ax3&                   thePos,
  TColStd_Array1OfReal&     theParams)
{
  if (myStatus != 0)
    return Standard_False;
  //
  if (mySType == TopAbs_FACE)
  {
    Handle(Geom_Surface) anElemSurf =
      GetSurface(TopoDS::Face(myShape), theTol, GeomConvert_Target, theTarget, myGap, myStatus);
    if (anElemSurf.IsNull())
      return Standard_False;
    //
    Standard_Boolean isOK = SetSurfParams(theTarget, anElemSurf, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return Standard_False;
    }
    return Standard_True;
  }
  else if (mySType == TopAbs_SHELL)
  {
    Handle(Geom_Surface) anElemSurf =
      GetSurface(TopoDS::Shell(myShape), theTol, GeomConvert_Target, theTarget, myGap, myStatus);
    if (anElemSurf.IsNull())
    {
      return Standard_False;
    }
    Standard_Boolean isOK = SetSurfParams(theTarget, anElemSurf, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return Standard_False;
    }
    return Standard_True;
  }
  else if (mySType == TopAbs_EDGE)
  {
    Handle(Geom_Surface) anElemSurf = GetSurface(TopoDS::Edge(myShape),
                                                 theTol,
                                                 GeomConvert_Target,
                                                 theTarget,
                                                 thePos,
                                                 theParams,
                                                 myGap,
                                                 myStatus);

    Standard_Boolean isOK = SetSurfParams(theTarget, anElemSurf, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return Standard_False;
    }
    return Standard_True;
  }
  else if (mySType == TopAbs_WIRE)
  {
    Handle(Geom_Surface) anElemSurf = GetSurface(TopoDS::Wire(myShape),
                                                 theTol,
                                                 GeomConvert_Target,
                                                 theTarget,
                                                 thePos,
                                                 theParams,
                                                 myGap,
                                                 myStatus);

    Standard_Boolean isOK = SetSurfParams(theTarget, anElemSurf, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return Standard_False;
    }
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsPlane(const Standard_Real theTol,
                                                             gp_Pln&             thePln)
{
  gp_Ax3               aPos = thePln.Position();
  TColStd_Array1OfReal aParams(1, 1);
  //
  GeomAbs_SurfaceType aTarget = GeomAbs_Plane;
  if (IsElementarySurf(aTarget, theTol, aPos, aParams))
  {
    thePln.SetPosition(aPos);
    return Standard_True;
  }
  // Try to build plane for wire or edge
  if (mySType == TopAbs_EDGE || mySType == TopAbs_WIRE)
  {
    BRepLib_FindSurface aFndSurf(myShape, theTol, Standard_True, Standard_False);
    if (aFndSurf.Found())
    {
      Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aFndSurf.Surface());
      thePln                    = aPlane->Pln();
      myGap                     = aFndSurf.ToleranceReached();
      myStatus                  = 0;
      return Standard_True;
    }
    else
      myStatus = 1;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsCylinder(const Standard_Real theTol,
                                                                gp_Cylinder&        theCyl)
{
  gp_Ax3               aPos = theCyl.Position();
  TColStd_Array1OfReal aParams(1, 1);
  aParams(1) = theCyl.Radius();
  //
  GeomAbs_SurfaceType aTarget = GeomAbs_Cylinder;
  if (IsElementarySurf(aTarget, theTol, aPos, aParams))
  {
    theCyl.SetPosition(aPos);
    theCyl.SetRadius(aParams(1));
    return Standard_True;
  }

  if (aParams(1) > Precision::Infinite())
  {
    // Sample cylinder does not seem to be set, least square method is not applicable.
    return Standard_False;
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
    Standard_Boolean isDone =
      GetSurfaceByLS(aWire, theTol, aTarget, aPos, aParams, myGap, myStatus);
    if (isDone)
    {
      theCyl.SetPosition(aPos);
      theCyl.SetRadius(aParams(1));
      return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsCone(const Standard_Real theTol,
                                                            gp_Cone&            theCone)
{
  gp_Ax3               aPos = theCone.Position();
  TColStd_Array1OfReal aParams(1, 2);
  aParams(1) = theCone.SemiAngle();
  aParams(2) = theCone.RefRadius();
  //
  GeomAbs_SurfaceType aTarget = GeomAbs_Cone;
  if (IsElementarySurf(aTarget, theTol, aPos, aParams))
  {
    theCone.SetPosition(aPos);
    theCone.SetSemiAngle(aParams(1));
    theCone.SetRadius(aParams(2));
    return Standard_True;
  }

  if (aParams(2) > Precision::Infinite())
  {
    // Sample cone does not seem to be set, least square method is not applicable.
    return Standard_False;
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
    Standard_Boolean isDone =
      GetSurfaceByLS(aWire, theTol, aTarget, aPos, aParams, myGap, myStatus);
    if (isDone)
    {
      theCone.SetPosition(aPos);
      theCone.SetSemiAngle(aParams(1));
      theCone.SetRadius(aParams(2));
      return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsSphere(const Standard_Real theTol,
                                                              gp_Sphere&          theSphere)
{
  gp_Ax3               aPos = theSphere.Position();
  TColStd_Array1OfReal aParams(1, 1);
  aParams(1) = theSphere.Radius();
  //
  GeomAbs_SurfaceType aTarget = GeomAbs_Sphere;
  if (IsElementarySurf(aTarget, theTol, aPos, aParams))
  {
    theSphere.SetPosition(aPos);
    theSphere.SetRadius(aParams(1));
    return Standard_True;
  }
  //
  if (aParams(1) > Precision::Infinite())
  {
    // Sample sphere does not seem to be set, least square method is not applicable.
    return Standard_False;
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
    Standard_Boolean isDone =
      GetSurfaceByLS(aWire, theTol, aTarget, aPos, aParams, myGap, myStatus);
    if (isDone)
    {
      theSphere.SetPosition(aPos);
      theSphere.SetRadius(aParams(1));
      return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsConic(const GeomAbs_CurveType theTarget,
                                                             const Standard_Real     theTol,
                                                             gp_Ax2&                 thePos,
                                                             TColStd_Array1OfReal&   theParams)
{
  if (myStatus != 0)
    return Standard_False;

  if (mySType == TopAbs_EDGE)
  {
    Handle(Geom_Curve) aConic =
      GetCurve(TopoDS::Edge(myShape), theTol, GeomConvert_Target, theTarget, myGap, myStatus);

    if (aConic.IsNull())
      return Standard_False;

    Standard_Boolean isOK = SetConicParameters(theTarget, aConic, thePos, theParams);

    if (!isOK)
    {
      myStatus = 1;
      return Standard_False;
    }
    return Standard_True;
  }
  else if (mySType == TopAbs_WIRE)
  {
    TopoDS_Iterator anIter(myShape);
    if (!anIter.More())
    {
      myStatus = 1;
      return Standard_False;
    }
    gp_Ax2               aPos;
    TColStd_Array1OfReal aParams(1, theParams.Length());
    const TopoDS_Shape&  anEdge = anIter.Value();

    Handle(Geom_Curve) aConic =
      GetCurve(TopoDS::Edge(anEdge), theTol, GeomConvert_Target, theTarget, myGap, myStatus);
    if (aConic.IsNull())
    {
      return Standard_False;
    }
    Standard_Boolean isOK = SetConicParameters(theTarget, aConic, thePos, theParams);
    if (!isOK)
    {
      myStatus = 1;
      return Standard_False;
    }
    if (!anIter.More())
    {
      return Standard_True;
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
        return Standard_False;
      }
      isOK = SetConicParameters(theTarget, aConic, aPos, aParams);
      isOK = CompareConicParams(theTarget, theTol, thePos, theParams, aPos, aParams);

      if (!isOK)
      {
        return Standard_False;
      }
    }
    return Standard_True;
  }
  myStatus = 1;
  return Standard_False;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsLine(const Standard_Real theTol,
                                                            gp_Lin&             theLin)
{
  gp_Ax2               aPos;
  TColStd_Array1OfReal aParams(1, 1);

  GeomAbs_CurveType aTarget = GeomAbs_Line;
  Standard_Boolean  isOK    = IsConic(aTarget, theTol, aPos, aParams);
  if (isOK)
  {
    theLin.SetPosition(aPos.Axis());
  }
  return isOK;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsCircle(const Standard_Real theTol,
                                                              gp_Circ&            theCirc)
{
  gp_Ax2               aPos;
  TColStd_Array1OfReal aParams(1, 1);

  GeomAbs_CurveType aTarget = GeomAbs_Circle;
  Standard_Boolean  isOK    = IsConic(aTarget, theTol, aPos, aParams);
  if (isOK)
  {
    theCirc.SetPosition(aPos);
    theCirc.SetRadius(aParams(1));
  }
  return isOK;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::IsEllipse(const Standard_Real theTol,
                                                               gp_Elips&           theElips)
{
  gp_Ax2               aPos;
  TColStd_Array1OfReal aParams(1, 2);

  GeomAbs_CurveType aTarget = GeomAbs_Ellipse;
  Standard_Boolean  isOK    = IsConic(aTarget, theTol, aPos, aParams);
  if (isOK)
  {
    theElips.SetPosition(aPos);
    theElips.SetMajorRadius(aParams(1));
    theElips.SetMinorRadius(aParams(2));
  }
  return isOK;
}

//=================================================================================================

Handle(Geom_Surface) ShapeAnalysis_CanonicalRecognition::GetSurface(
  const TopoDS_Face&         theFace,
  const Standard_Real        theTol,
  const GeomConvert_ConvType theType,
  const GeomAbs_SurfaceType  theTarget,
  Standard_Real&             theGap,
  Standard_Integer&          theStatus)
{
  theStatus = 0;
  TopLoc_Location             aLoc;
  const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(theFace, aLoc);
  if (aSurf.IsNull())
  {
    theStatus = 1;
    return aSurf;
  }
  GeomConvert_SurfToAnaSurf aConv(aSurf);
  aConv.SetConvType(theType);
  aConv.SetTarget(theTarget);
  Handle(Geom_Surface) anAnaSurf = aConv.ConvertToAnalytical(theTol);
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

Handle(Geom_Surface) ShapeAnalysis_CanonicalRecognition::GetSurface(
  const TopoDS_Shell&        theShell,
  const Standard_Real        theTol,
  const GeomConvert_ConvType theType,
  const GeomAbs_SurfaceType  theTarget,
  Standard_Real&             theGap,
  Standard_Integer&          theStatus)
{
  Handle(Geom_Surface) anElemSurf;
  TopoDS_Iterator      anIter(theShell);
  if (!anIter.More())
  {
    theStatus = 1;
    return anElemSurf;
  }
  gp_Ax3               aPos1;
  TColStd_Array1OfReal aParams1(1, Max(1, GetNbPars(theTarget)));
  const TopoDS_Shape&  aFace = anIter.Value();

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
  gp_Ax3               aPos;
  TColStd_Array1OfReal aParams(1, Max(1, GetNbPars(theTarget)));
  Standard_Boolean     isOK;
  for (; anIter.More(); anIter.Next())
  {
    Handle(Geom_Surface) anElemSurf1 =
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

Handle(Geom_Surface) ShapeAnalysis_CanonicalRecognition::GetSurface(
  const TopoDS_Edge&         theEdge,
  const Standard_Real        theTol,
  const GeomConvert_ConvType theType,
  const GeomAbs_SurfaceType  theTarget,
  gp_Ax3&                    thePos,
  TColStd_Array1OfReal&      theParams,
  Standard_Real&             theGap,
  Standard_Integer&          theStatus)
{
  // Get surface list
  NCollection_Vector<Handle(Geom_Surface)> aSurfs;
  NCollection_Vector<Standard_Real>        aGaps;
  Standard_Integer                         j = 0;
  for (;;)
  {
    j++;
    Handle(Geom_Surface) aSurf;
    TopLoc_Location      aLoc;
    Handle(Geom2d_Curve) aPCurve;
    Standard_Real        ff, ll;
    BRep_Tool::CurveOnSurface(theEdge, aPCurve, aSurf, aLoc, ff, ll, j);
    if (aSurf.IsNull())
    {
      break;
    }
    GeomConvert_SurfToAnaSurf aConv(aSurf);
    aConv.SetConvType(theType);
    aConv.SetTarget(theTarget);
    Handle(Geom_Surface) anAnaSurf = aConv.ConvertToAnalytical(theTol);
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
    return NULL;
  }

  gp_Ax3               aPos;
  Standard_Integer     aNbPars = Max(1, GetNbPars(theTarget));
  TColStd_Array1OfReal aParams(1, aNbPars);

  Standard_Integer ifit    = -1, i;
  Standard_Real    aMinDev = RealLast();
  if (aSurfs.Size() == 1)
  {
    ifit = 0;
  }
  else
  {
    for (i = 0; i < aSurfs.Size(); ++i)
    {
      SetSurfParams(theTarget, aSurfs(i), aPos, aParams);
      Standard_Real aDev = DeviationSurfParams(theTarget, thePos, theParams, aPos, aParams);
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
    return NULL;
  }
}

//=================================================================================================

Handle(Geom_Surface) ShapeAnalysis_CanonicalRecognition::GetSurface(
  const TopoDS_Wire&         theWire,
  const Standard_Real        theTol,
  const GeomConvert_ConvType theType,
  const GeomAbs_SurfaceType  theTarget,
  gp_Ax3&                    thePos,
  TColStd_Array1OfReal&      theParams,
  Standard_Real&             theGap,
  Standard_Integer&          theStatus)
{
  // Get surface list
  NCollection_Vector<Handle(Geom_Surface)> aSurfs;
  NCollection_Vector<Standard_Real>        aGaps;

  TopoDS_Iterator anIter(theWire);
  if (!anIter.More())
  {
    // Empty wire
    theStatus = 1;
    return NULL;
  }
  // First edge
  const TopoDS_Edge&   anEdge1 = TopoDS::Edge(anIter.Value());
  gp_Ax3               aPos1   = thePos;
  Standard_Integer     aNbPars = GetNbPars(theTarget);
  TColStd_Array1OfReal aParams1(1, Max(1, aNbPars));
  Standard_Real        aGap1;
  Standard_Integer     i;
  for (i = 1; i <= aNbPars; ++i)
  {
    aParams1(i) = theParams(i);
  }
  Handle(Geom_Surface) anElemSurf1 =
    GetSurface(anEdge1, theTol, theType, theTarget, aPos1, aParams1, aGap1, theStatus);
  if (theStatus != 0 || anElemSurf1.IsNull())
  {
    return NULL;
  }
  anIter.Next();
  for (; anIter.More(); anIter.Next())
  {
    gp_Ax3 aPos = aPos1;
    aNbPars     = GetNbPars(theTarget);
    TColStd_Array1OfReal aParams(1, Max(1, aNbPars));
    for (i = 1; i <= aNbPars; ++i)
    {
      aParams(i) = aParams1(i);
    }
    Standard_Real        aGap;
    const TopoDS_Edge&   anEdge = TopoDS::Edge(anIter.Value());
    Handle(Geom_Surface) anElemSurf =
      GetSurface(anEdge, theTol, theType, theTarget, aPos, aParams, aGap, theStatus);
    if (theStatus != 0 || anElemSurf.IsNull())
    {
      return NULL;
    }
    Standard_Boolean isOK = CompareSurfParams(theTarget, theTol, aPos1, aParams1, aPos, aParams);
    if (!isOK)
    {
      return NULL;
    }
  }

  SetSurfParams(theTarget, anElemSurf1, thePos, theParams);
  theGap = aGap1;
  return anElemSurf1;
}

//=================================================================================================

Standard_Boolean ShapeAnalysis_CanonicalRecognition::GetSurfaceByLS(
  const TopoDS_Wire&        theWire,
  const Standard_Real       theTol,
  const GeomAbs_SurfaceType theTarget,
  gp_Ax3&                   thePos,
  TColStd_Array1OfReal&     theParams,
  Standard_Real&            theGap,
  Standard_Integer&         theStatus)
{
  Handle(TColgp_HArray1OfXYZ) aPoints;
  Standard_Integer            aNbMaxInt = 100;
  if (!GetSamplePoints(theWire, theTol, aNbMaxInt, aPoints))
    return Standard_False;

  theGap = GetLSGap(aPoints, theTarget, thePos, theParams);
  if (theGap <= theTol)
  {
    theStatus = 0;
    return Standard_True;
  }

  Standard_Integer aNbVar = 0;
  if (theTarget == GeomAbs_Sphere)
    aNbVar = 4;
  else if (theTarget == GeomAbs_Cylinder)
    aNbVar = 4;
  else if (theTarget == GeomAbs_Cone)
    aNbVar = 5;
  else
    return Standard_False;

  math_Vector aFBnd(1, aNbVar), aLBnd(1, aNbVar), aStartPoint(1, aNbVar);

  Standard_Real aRelDev = 0.2; // Customer can set parameters of sample surface
                               //  with relative precision about aRelDev.
                               //  For example, if radius of sample surface is R,
                               //  it means, that "exact" value is in interav
                               //[R - aRelDev*R, R + aRelDev*R]. This interval is set
                               //  for R as boundary values for optimization algo.
  FillSolverData(theTarget, thePos, theParams, aStartPoint, aFBnd, aLBnd, aRelDev);

  //
  constexpr Standard_Real        aTol = Precision::Confusion();
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
    aPFunc = NULL;
  //
  math_Vector      aSteps(1, aNbVar);
  Standard_Integer aNbInt = 10;
  Standard_Integer i;
  for (i = 1; i <= aNbVar; ++i)
  {
    aSteps(i) = (aLBnd(i) - aFBnd(i)) / aNbInt;
  }
  math_PSO      aGlobSolver(aPFunc, aFBnd, aLBnd, aSteps);
  Standard_Real aLSDist;
  aGlobSolver.Perform(aSteps, aLSDist, aStartPoint);
  SetCanonicParameters(theTarget, aStartPoint, thePos, theParams);

  theGap = GetLSGap(aPoints, theTarget, thePos, theParams);
  if (theGap <= theTol)
  {
    theStatus = 0;
    return Standard_True;
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
      return Standard_True;
  }
  else
    theStatus = 1;

  return Standard_False;
}

//=================================================================================================

Handle(Geom_Curve) ShapeAnalysis_CanonicalRecognition::GetCurve(const TopoDS_Edge&         theEdge,
                                                                const Standard_Real        theTol,
                                                                const GeomConvert_ConvType theType,
                                                                const GeomAbs_CurveType theTarget,
                                                                Standard_Real&          theGap,
                                                                Standard_Integer&       theStatus)
{
  theStatus = 0;
  TopLoc_Location           aLoc;
  Standard_Real             f, l, nf, nl;
  const Handle(Geom_Curve)& aCurv = BRep_Tool::Curve(theEdge, aLoc, f, l);
  if (aCurv.IsNull())
  {
    theStatus = 1;
    return aCurv;
  }
  GeomConvert_CurveToAnaCurve aConv(aCurv);
  aConv.SetConvType(theType);
  aConv.SetTarget(theTarget);
  Handle(Geom_Curve) anAnaCurv;
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

Standard_Integer GetNbPars(const GeomAbs_CurveType theTarget)
{
  Standard_Integer aNbPars = 0;
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

Standard_Integer GetNbPars(const GeomAbs_SurfaceType theTarget)
{
  Standard_Integer aNbPars = 0;
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

Standard_Boolean SetConicParameters(const GeomAbs_CurveType   theTarget,
                                    const Handle(Geom_Curve)& theConic,
                                    gp_Ax2&                   thePos,
                                    TColStd_Array1OfReal&     theParams)
{
  if (theConic.IsNull())
    return Standard_False;
  GeomAdaptor_Curve aGAC(theConic);
  if (aGAC.GetType() != theTarget)
    return Standard_False;

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
    return Standard_False;
  return Standard_True;
}

//=================================================================================================

Standard_Boolean CompareConicParams(const GeomAbs_CurveType     theTarget,
                                    const Standard_Real         theTol,
                                    const gp_Ax2&               theRefPos,
                                    const TColStd_Array1OfReal& theRefParams,
                                    const gp_Ax2&               thePos,
                                    const TColStd_Array1OfReal& theParams)
{
  Standard_Integer i, aNbPars = GetNbPars(theTarget);

  for (i = 1; i <= aNbPars; ++i)
  {
    if (Abs(theRefParams(i) - theParams(i)) > theTol)
      return Standard_False;
  }

  Standard_Real anAngTol = theTol / (2. * M_PI);
  Standard_Real aTol     = theTol;
  if (theTarget == GeomAbs_Line)
    aTol = Precision::Infinite();

  const gp_Ax1& aRef     = theRefPos.Axis();
  const gp_Ax1& anAx1    = thePos.Axis();
  gp_Ax1        anAx1Rev = anAx1.Reversed();

  if (aRef.IsCoaxial(anAx1, anAngTol, aTol) || aRef.IsCoaxial(anAx1Rev, anAngTol, aTol))
  {
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean SetSurfParams(const GeomAbs_SurfaceType   theTarget,
                               const Handle(Geom_Surface)& theElemSurf,
                               gp_Ax3&                     thePos,
                               TColStd_Array1OfReal&       theParams)
{
  //
  if (theElemSurf.IsNull())
    return Standard_False;
  GeomAdaptor_Surface aGAS(theElemSurf);
  if (aGAS.GetType() != theTarget)
    return Standard_False;

  Standard_Real aNbPars = GetNbPars(theTarget);
  if (theParams.Length() < aNbPars)
    return Standard_False;

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
    return Standard_False;
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean CompareSurfParams(const GeomAbs_SurfaceType   theTarget,
                                   const Standard_Real         theTol,
                                   const gp_Ax3&               theRefPos,
                                   const TColStd_Array1OfReal& theRefParams,
                                   const gp_Ax3&               thePos,
                                   const TColStd_Array1OfReal& theParams)
{
  if (theTarget != GeomAbs_Plane)
  {
    if (Abs(theRefParams(1) - theParams(1)) > theTol)
    {
      return Standard_False;
    }
  }
  //
  if (theTarget == GeomAbs_Sphere)
  {
    gp_Pnt aRefLoc = theRefPos.Location(), aLoc = thePos.Location();
    if (aRefLoc.SquareDistance(aLoc) <= theTol * theTol)
    {
      return Standard_True;
    }
    return Standard_False;
  }
  //
  Standard_Real anAngTol = theTol / (2. * M_PI);
  Standard_Real aTol     = theTol;
  if (theTarget == GeomAbs_Cylinder || theTarget == GeomAbs_Cone)
  {
    aTol = Precision::Infinite();
  }

  const gp_Ax1& aRef     = theRefPos.Axis();
  const gp_Ax1& anAx1    = thePos.Axis();
  gp_Ax1        anAx1Rev = anAx1.Reversed();
  if (!(aRef.IsCoaxial(anAx1, anAngTol, aTol) || aRef.IsCoaxial(anAx1Rev, anAngTol, aTol)))
  {
    return Standard_False;
  }

  if (theTarget == GeomAbs_Cone)
  {
    gp_Cone aRefCone(theRefPos, theRefParams(1), theRefParams(2));
    gp_Cone aCone(thePos, theParams(1), theParams(2));
    gp_Pnt  aRefApex = aRefCone.Apex();
    gp_Pnt  anApex   = aCone.Apex();
    if (aRefApex.SquareDistance(anApex) <= theTol * theTol)
    {
      return Standard_True;
    }
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Real DeviationSurfParams(const GeomAbs_SurfaceType   theTarget,
                                  const gp_Ax3&               theRefPos,
                                  const TColStd_Array1OfReal& theRefParams,
                                  const gp_Ax3&               thePos,
                                  const TColStd_Array1OfReal& theParams)
{
  Standard_Real aDevPars = 0.;
  if (theTarget != GeomAbs_Plane)
  {
    aDevPars = Abs(theRefParams(1) - theParams(1));
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
    Standard_Real anAngDev = (1. - Abs(aRefDir * aDir));
    aDevPars += anAngDev;
  }

  return aDevPars;
}

//=================================================================================================

Standard_Boolean GetSamplePoints(const TopoDS_Wire&           theWire,
                                 const Standard_Real          theTol,
                                 const Standard_Integer       theMaxNbInt,
                                 Handle(TColgp_HArray1OfXYZ)& thePoints)
{
  NCollection_Vector<Standard_Real>     aLengths;
  NCollection_Vector<BRepAdaptor_Curve> aCurves;
  NCollection_Vector<gp_XYZ>            aPoints;
  Standard_Real                         aTol         = Max(1.e-3, theTol / 10.);
  Standard_Real                         aTotalLength = 0.;
  TopoDS_Iterator                       anEIter(theWire);
  for (; anEIter.More(); anEIter.Next())
  {
    const TopoDS_Edge& anE = TopoDS::Edge(anEIter.Value());
    if (BRep_Tool::Degenerated(anE))
      continue;
    BRepAdaptor_Curve aBAC(anE);
    Standard_Real     aClength = GCPnts_AbscissaPoint::Length(aBAC, aTol);
    aTotalLength += aClength;
    aCurves.Append(aBAC);
    aLengths.Append(aClength);
  }

  if (aTotalLength < theTol)
    return Standard_False;

  Standard_Integer i, aNb = aLengths.Length();
  for (i = 0; i < aNb; ++i)
  {
    const BRepAdaptor_Curve& aC        = aCurves(i);
    Standard_Real            aClength  = GCPnts_AbscissaPoint::Length(aC, aTol);
    Standard_Integer         aNbPoints = RealToInt(aClength / aTotalLength * theMaxNbInt + 1);
    aNbPoints                          = Max(2, aNbPoints);
    GCPnts_QuasiUniformAbscissa aPointGen(aC, aNbPoints);
    if (!aPointGen.IsDone())
      continue;
    aNbPoints = aPointGen.NbPoints();
    Standard_Integer j;
    for (j = 1; j <= aNbPoints; ++j)
    {
      Standard_Real t  = aPointGen.Parameter(j);
      gp_Pnt        aP = aC.Value(t);
      aPoints.Append(aP.XYZ());
    }
  }

  if (aPoints.Length() < 1)
    return Standard_False;

  thePoints = new TColgp_HArray1OfXYZ(1, aPoints.Length());
  for (i = 0; i < aPoints.Length(); ++i)
  {
    thePoints->SetValue(i + 1, aPoints(i));
  }

  return Standard_True;
}

//=================================================================================================

static Standard_Real GetLSGap(const Handle(TColgp_HArray1OfXYZ)& thePoints,
                              const GeomAbs_SurfaceType          theTarget,
                              const gp_Ax3&                      thePos,
                              const TColStd_Array1OfReal&        theParams)
{

  Standard_Real aGap = 0.;
  gp_XYZ        aLoc = thePos.Location().XYZ();
  gp_Vec        aDir(thePos.Direction());

  Standard_Integer i;
  if (theTarget == GeomAbs_Sphere)
  {
    Standard_Real anR = theParams(1);
    for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i)
    {
      gp_XYZ aD = thePoints->Value(i) - aLoc;
      aGap      = Max(aGap, Abs((aD.Modulus() - anR)));
    }
  }
  else if (theTarget == GeomAbs_Cylinder)
  {
    Standard_Real anR = theParams(1);
    for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i)
    {
      gp_Vec aD(thePoints->Value(i) - aLoc);
      aD.Cross(aDir);
      aGap = Max(aGap, Abs((aD.Magnitude() - anR)));
    }
  }
  else if (theTarget == GeomAbs_Cone)
  {
    Standard_Real anAng = theParams(1);
    Standard_Real anR   = theParams(2);
    for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i)
    {
      Standard_Real u, v;
      gp_Pnt        aPi(thePoints->Value(i));
      ElSLib::ConeParameters(thePos, anR, anAng, aPi, u, v);
      gp_Pnt aPp;
      ElSLib::ConeD0(u, v, thePos, anR, anAng, aPp);
      aGap = Max(aGap, aPi.SquareDistance(aPp));
    }
    aGap = Sqrt(aGap);
  }

  return aGap;
}

//=================================================================================================

void FillSolverData(const GeomAbs_SurfaceType   theTarget,
                    const gp_Ax3&               thePos,
                    const TColStd_Array1OfReal& theParams,
                    math_Vector&                theStartPoint,
                    math_Vector&                theFBnd,
                    math_Vector&                theLBnd,
                    const Standard_Real         theRelDev)
{
  if (theTarget == GeomAbs_Sphere || theTarget == GeomAbs_Cylinder)
  {
    theStartPoint(1)       = thePos.Location().X();
    theStartPoint(2)       = thePos.Location().Y();
    theStartPoint(3)       = thePos.Location().Z();
    theStartPoint(4)       = theParams(1);
    Standard_Real    aDR   = theRelDev * theParams(1);
    Standard_Real    aDXYZ = aDR;
    Standard_Integer i;
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
    theStartPoint(1)  = thePos.Location().X();
    theStartPoint(2)  = thePos.Location().Y();
    theStartPoint(3)  = thePos.Location().Z();
    theStartPoint(4)  = theParams(1); // SemiAngle
    theStartPoint(5)  = theParams(2); // Radius
    Standard_Real aDR = theRelDev * theParams(2);
    if (aDR < Precision::Confusion())
    {
      aDR = 0.1;
    }
    Standard_Real    aDXYZ = aDR;
    Standard_Real    aDAng = theRelDev * Abs(theParams(1));
    Standard_Integer i;
    for (i = 1; i <= 3; ++i)
    {
      theFBnd(i) = theStartPoint(i) - aDXYZ;
      theLBnd(i) = theStartPoint(i) + aDXYZ;
    }
    if (theParams(1) >= 0.)
    {
      theFBnd(4) = theStartPoint(4) - aDAng;
      theLBnd(4) = Min(M_PI_2, theStartPoint(4) + aDR);
    }
    else
    {
      theFBnd(4) = Max(-M_PI_2, theStartPoint(4) - aDAng);
      theLBnd(4) = theStartPoint(4) + aDAng;
    }
    theFBnd(5) = theStartPoint(5) - aDR;
    theLBnd(5) = theStartPoint(5) + aDR;
  }
}

//=================================================================================================

void SetCanonicParameters(const GeomAbs_SurfaceType theTarget,
                          const math_Vector&        theSol,
                          gp_Ax3&                   thePos,
                          TColStd_Array1OfReal&     theParams)
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