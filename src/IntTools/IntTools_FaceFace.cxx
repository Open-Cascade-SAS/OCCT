// Created on: 2000-11-23
// Created by: Michael KLOKOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <IntTools_FaceFace.hxx>

#include <Precision.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_Array1OfListOfInteger.hxx>

#include <gp_Lin2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Torus.hxx>
#include <gp_Cylinder.hxx>

#include <Bnd_Box.hxx>

#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <IntAna_QuadQuadGeo.hxx>

#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>
#include <IntRes2d_Domain.hxx>
#include <ProjLib_Plane.hxx>

#include <IntPatch_GLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_ALineToWLine.hxx>

#include <ElSLib.hxx>
#include <ElCLib.hxx>

#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <BndLib_AddSurface.hxx>

#include <Adaptor3d_SurfacePtr.hxx>
#include <Adaptor2d_HLine2d.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Conic.hxx>

#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Circle.hxx>

#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dAdaptor.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>

#include <GeomInt_WLApprox.hxx>
#include <GeomProjLib.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <IntTools_Curve.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_TopolTool.hxx>
#include <IntTools_PntOnFace.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <IntTools_Context.hxx>
#include <IntSurf_ListIteratorOfListOfPntOn2S.hxx>
#include <GeomInt.hxx>

#include <Approx_CurveOnSurface.hxx>
#include <GeomAdaptor.hxx>
#include <GeomInt_IntSS.hxx>

static
  void RefineVector(gp_Vec2d& aV2D);
#ifdef OCCT_DEBUG_DUMPWLINE
static
  void DumpWLine(const Handle(IntPatch_WLine)& aWLine);
#endif
//
static
  void TolR3d(const TopoDS_Face& ,
              const TopoDS_Face& ,
              Standard_Real& );
static 
  Handle(Geom_Curve) MakeBSpline (const Handle(IntPatch_WLine)&,
                                  const Standard_Integer,
                                  const Standard_Integer);

static 
  void Parameters(const Handle(GeomAdaptor_HSurface)&,
                  const Handle(GeomAdaptor_HSurface)&,
                  const gp_Pnt&,
                  Standard_Real&,
                  Standard_Real&,
                  Standard_Real&,
                  Standard_Real&);

static 
  void CorrectSurfaceBoundaries(const TopoDS_Face&  theFace,
                                const Standard_Real theTolerance,
                                Standard_Real&      theumin,
                                Standard_Real&      theumax, 
                                Standard_Real&      thevmin, 
                                Standard_Real&      thevmax);

static
  Standard_Boolean NotUseSurfacesForApprox
          (const TopoDS_Face& aF1,
           const TopoDS_Face& aF2,
           const Handle(IntPatch_WLine)& WL,
           const Standard_Integer ifprm,
           const Standard_Integer ilprm);

static 
  Handle(IntPatch_WLine) ComputePurgedWLine(const Handle(IntPatch_WLine)       &theWLine,
                                            const Handle(GeomAdaptor_HSurface) &theS1,
                                            const Handle(GeomAdaptor_HSurface) &theS2);

static 
  Handle(Geom2d_BSplineCurve) MakeBSpline2d(const Handle(IntPatch_WLine)& theWLine,
                                            const Standard_Integer                         ideb,
                                            const Standard_Integer                         ifin,
                                            const Standard_Boolean                         onFirst);

static 
  Standard_Boolean DecompositionOfWLine(const Handle(IntPatch_WLine)& theWLine,
                                        const Handle(GeomAdaptor_HSurface)&            theSurface1, 
                                        const Handle(GeomAdaptor_HSurface)&            theSurface2,
                                        const TopoDS_Face&                             theFace1,
                                        const TopoDS_Face&                             theFace2,
                                        const GeomInt_LineConstructor&                 theLConstructor,
                                        const Standard_Boolean                         theAvoidLConstructor,
                                        IntPatch_SequenceOfLine&                       theNewLines,
                                        Standard_Real&                                 theReachedTol3d,
                                        const Handle(IntTools_Context)& );

static 
  Standard_Boolean ParameterOutOfBoundary(const Standard_Real       theParameter, 
                                          const Handle(Geom_Curve)& theCurve, 
                                          const TopoDS_Face&        theFace1, 
                                          const TopoDS_Face&        theFace2,
                                          const Standard_Real       theOtherParameter,
                                          const Standard_Boolean    bIncreasePar,
                                          Standard_Real&            theNewParameter,
                                          const Handle(IntTools_Context)& );

static 
  Standard_Boolean IsCurveValid(Handle(Geom2d_Curve)& thePCurve);

static 
  Standard_Boolean IsPointOnBoundary(const Standard_Real theParameter,
                                     const Standard_Real theFirstBoundary,
                                     const Standard_Real theSecondBoundary,
                                     const Standard_Real theResolution,
                                     Standard_Boolean&   IsOnFirstBoundary);
static
  Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
                             const gp_Pnt2d&     theLastPoint,
                             const Standard_Real theUmin, 
                             const Standard_Real theUmax,
                             const Standard_Real theVmin,
                             const Standard_Real theVmax,
                             gp_Pnt2d&           theNewPoint);


static 
  Standard_Integer ComputeTangentZones( const Handle(GeomAdaptor_HSurface)& theSurface1,
                                       const Handle(GeomAdaptor_HSurface)&  theSurface2,
                                       const TopoDS_Face&                   theFace1,
                                       const TopoDS_Face&                   theFace2,
                                       Handle(TColgp_HArray1OfPnt2d)&       theResultOnS1,
                                       Handle(TColgp_HArray1OfPnt2d)&       theResultOnS2,
                                       Handle(TColStd_HArray1OfReal)&       theResultRadius,
                                       const Handle(IntTools_Context)& );

static
  Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
                             const gp_Pnt2d&     theLastPoint,
                             const Standard_Real theUmin, 
                             const Standard_Real theUmax,
                             const Standard_Real theVmin,
                             const Standard_Real theVmax,
                             const gp_Pnt2d&     theTanZoneCenter,
                             const Standard_Real theZoneRadius,
                             Handle(GeomAdaptor_HSurface) theGASurface,
                             gp_Pnt2d&           theNewPoint);

static
  Standard_Boolean IsInsideTanZone(const gp_Pnt2d&     thePoint,
                                   const gp_Pnt2d&     theTanZoneCenter,
                                   const Standard_Real theZoneRadius,
                                   Handle(GeomAdaptor_HSurface) theGASurface);

static
  gp_Pnt2d AdjustByNeighbour(const gp_Pnt2d&     theaNeighbourPoint,
                             const gp_Pnt2d&     theOriginalPoint,
                             Handle(GeomAdaptor_HSurface) theGASurface);
static
  Standard_Boolean  ApproxWithPCurves(const gp_Cylinder& theCyl, 
                                      const gp_Sphere& theSph);

static void  PerformPlanes(const Handle(GeomAdaptor_HSurface)& theS1, 
                           const Handle(GeomAdaptor_HSurface)& theS2, 
                           const Standard_Real TolAng, 
                           const Standard_Real TolTang, 
                           const Standard_Boolean theApprox1,
                           const Standard_Boolean theApprox2,
                           IntTools_SequenceOfCurves& theSeqOfCurve, 
                           Standard_Boolean& theTangentFaces);

static Standard_Boolean ClassifyLin2d(const Handle(GeomAdaptor_HSurface)& theS, 
                                      const gp_Lin2d& theLin2d, 
                                      const Standard_Real theTol,
                                      Standard_Real& theP1, 
                                      Standard_Real& theP2);
//
static
  void ApproxParameters(const Handle(GeomAdaptor_HSurface)& aHS1,
                        const Handle(GeomAdaptor_HSurface)& aHS2,
                        Standard_Integer& iDegMin,
                        Standard_Integer& iNbIter,
                        Standard_Integer& iDegMax);

static
  void Tolerances(const Handle(GeomAdaptor_HSurface)& aHS1,
                  const Handle(GeomAdaptor_HSurface)& aHS2,
                  Standard_Real& aTolTang);

static
  Standard_Boolean SortTypes(const GeomAbs_SurfaceType aType1,
                             const GeomAbs_SurfaceType aType2);
static
  Standard_Integer IndexType(const GeomAbs_SurfaceType aType);

//
static
  Standard_Boolean CheckPCurve(const Handle(Geom2d_Curve)& aPC, 
                               const TopoDS_Face& aFace);

static
  Standard_Real MaxDistance(const Handle(Geom_Curve)& theC,
                            const Standard_Real aT,
                            GeomAPI_ProjectPointOnSurf& theProjPS);

static
  Standard_Real FindMaxDistance(const Handle(Geom_Curve)& theC,
                                const Standard_Real theFirst,
                                const Standard_Real theLast,
                                GeomAPI_ProjectPointOnSurf& theProjPS,
                                const Standard_Real theEps);

static
  Standard_Real FindMaxDistance(const Handle(Geom_Curve)& theCurve,
                                const Standard_Real theFirst,
                                const Standard_Real theLast,
                                const TopoDS_Face& theFace,
                                const Handle(IntTools_Context)& theContext);

static
  void CorrectPlaneBoundaries(Standard_Real& aUmin,
                              Standard_Real& aUmax, 
                              Standard_Real& aVmin, 
                              Standard_Real& aVmax);

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
IntTools_FaceFace::IntTools_FaceFace()
{
  myIsDone=Standard_False;
  myTangentFaces=Standard_False;
  //
  myHS1 = new GeomAdaptor_HSurface ();
  myHS2 = new GeomAdaptor_HSurface ();
  myTolReached2d=0.; 
  myTolReached3d=0.;
  SetParameters(Standard_True, Standard_True, Standard_True, 1.e-07);
  
}
//=======================================================================
//function : SetContext
//purpose  : 
//======================================================================= 
void IntTools_FaceFace::SetContext(const Handle(IntTools_Context)& aContext)
{
  myContext=aContext;
}
//=======================================================================
//function : Context
//purpose  : 
//======================================================================= 
const Handle(IntTools_Context)& IntTools_FaceFace::Context()const
{
  return myContext;
}
//=======================================================================
//function : Face1
//purpose  : 
//======================================================================= 
const TopoDS_Face& IntTools_FaceFace::Face1() const
{
  return myFace1;
}
//=======================================================================
//function : Face2
//purpose  : 
//======================================================================= 
const TopoDS_Face& IntTools_FaceFace::Face2() const
{
  return myFace2;
}
//=======================================================================
//function : TangentFaces
//purpose  : 
//======================================================================= 
Standard_Boolean IntTools_FaceFace::TangentFaces() const
{
  return myTangentFaces;
}
//=======================================================================
//function : Points
//purpose  : 
//======================================================================= 
const IntTools_SequenceOfPntOn2Faces& IntTools_FaceFace::Points() const
{
  return myPnts;
}
//=======================================================================
//function : IsDone
//purpose  : 
//======================================================================= 
Standard_Boolean IntTools_FaceFace::IsDone() const
{
  return myIsDone;
}
//=======================================================================
//function : TolReached3d
//purpose  : 
//=======================================================================
Standard_Real IntTools_FaceFace::TolReached3d() const
{
  return myTolReached3d;
}
//=======================================================================
//function : Lines
//purpose  : return lines of intersection
//=======================================================================
const IntTools_SequenceOfCurves& IntTools_FaceFace::Lines() const
{
  StdFail_NotDone_Raise_if
    (!myIsDone,
     "IntTools_FaceFace::Lines() => myIntersector NOT DONE");
  return mySeqOfCurve;
}
//=======================================================================
//function : TolReached2d
//purpose  : 
//=======================================================================
Standard_Real IntTools_FaceFace::TolReached2d() const
{
  return myTolReached2d;
}
// =======================================================================
// function: SetParameters
//
// =======================================================================
void IntTools_FaceFace::SetParameters(const Standard_Boolean ToApproxC3d,
                                      const Standard_Boolean ToApproxC2dOnS1,
                                      const Standard_Boolean ToApproxC2dOnS2,
                                      const Standard_Real ApproximationTolerance) 
{
  myApprox = ToApproxC3d;
  myApprox1 = ToApproxC2dOnS1;
  myApprox2 = ToApproxC2dOnS2;
  myTolApprox = ApproximationTolerance;
}
//=======================================================================
//function : SetList
//purpose  : 
//=======================================================================
void IntTools_FaceFace::SetList(IntSurf_ListOfPntOn2S& aListOfPnts)
{
  myListOfPnts = aListOfPnts;  
}


static Standard_Boolean isTreatAnalityc(const TopoDS_Face& theF1,
                                        const TopoDS_Face& theF2)
{
  const Standard_Real Tolang = 1.e-8;
  const Standard_Real aTolF1=BRep_Tool::Tolerance(theF1);
  const Standard_Real aTolF2=BRep_Tool::Tolerance(theF2);
  const Standard_Real aTolSum = aTolF1 + aTolF2;
  Standard_Real aHigh = 0.0;

  const BRepAdaptor_Surface aBAS1(theF1), aBAS2(theF2);
  const GeomAbs_SurfaceType aType1=aBAS1.GetType();
  const GeomAbs_SurfaceType aType2=aBAS2.GetType();
  
  gp_Pln aS1;
  gp_Cylinder aS2;
  if(aType1 == GeomAbs_Plane)
  {
    aS1=aBAS1.Plane();
  }
  else if(aType2 == GeomAbs_Plane)
  {
    aS1=aBAS2.Plane();
  }
  else
  {
    return Standard_True;
  }

  if(aType1 == GeomAbs_Cylinder)
  {
    aS2=aBAS1.Cylinder();
    const Standard_Real VMin = aBAS1.FirstVParameter();
    const Standard_Real VMax = aBAS1.LastVParameter();

    if( Precision::IsNegativeInfinite(VMin) ||
        Precision::IsPositiveInfinite(VMax))
          return Standard_True;
    else
      aHigh = VMax - VMin;
  }
  else if(aType2 == GeomAbs_Cylinder)
  {
    aS2=aBAS2.Cylinder();

    const Standard_Real VMin = aBAS2.FirstVParameter();
    const Standard_Real VMax = aBAS2.LastVParameter();

    if( Precision::IsNegativeInfinite(VMin) ||
        Precision::IsPositiveInfinite(VMax))
          return Standard_True;
    else
      aHigh = VMax - VMin;
  }
  else
  {
    return Standard_True;
  }

  IntAna_QuadQuadGeo inter;
  inter.Perform(aS1,aS2,Tolang,aTolSum, aHigh);
  if(inter.TypeInter() == IntAna_Ellipse)
  {
    const gp_Elips anEl = inter.Ellipse(1);
    const Standard_Real aMajorR = anEl.MajorRadius();
    const Standard_Real aMinorR = anEl.MinorRadius();
    
    return (aMajorR < 100000.0 * aMinorR);
  }
  else
  {
    return inter.IsDone();
  }
}
//=======================================================================
//function : Perform
//purpose  : intersect surfaces of the faces
//=======================================================================
void IntTools_FaceFace::Perform(const TopoDS_Face& aF1,
                                const TopoDS_Face& aF2)
{
  Standard_Boolean RestrictLine = Standard_False, hasCone = Standard_False;
  
  if (myContext.IsNull()) {
    myContext=new IntTools_Context;
  }

  mySeqOfCurve.Clear();
  myTolReached2d=0.;
  myTolReached3d=0.;
  myIsDone = Standard_False;
  myNbrestr=0;//?

  myFace1=aF1;
  myFace2=aF2;

  const BRepAdaptor_Surface aBAS1(myFace1, Standard_False);
  const BRepAdaptor_Surface aBAS2(myFace2, Standard_False);
  GeomAbs_SurfaceType aType1=aBAS1.GetType();
  GeomAbs_SurfaceType aType2=aBAS2.GetType();

  const Standard_Boolean bReverse=SortTypes(aType1, aType2);
  if (bReverse)
  {
    myFace1=aF2;
    myFace2=aF1;
    aType1=aBAS2.GetType();
    aType2=aBAS1.GetType();

    if (myListOfPnts.Extent())
    {
      Standard_Real aU1,aV1,aU2,aV2;
      IntSurf_ListIteratorOfListOfPntOn2S aItP2S;
      //
      aItP2S.Initialize(myListOfPnts);
      for (; aItP2S.More(); aItP2S.Next())
      {
        IntSurf_PntOn2S& aP2S=aItP2S.Value();
        aP2S.Parameters(aU1,aV1,aU2,aV2);
        aP2S.SetValue(aU2,aV2,aU1,aV1);
      }
    }
    //
    Standard_Boolean anAproxTmp = myApprox1;
    myApprox1 = myApprox2;
    myApprox2 = anAproxTmp;
  }


  const Handle(Geom_Surface) S1=BRep_Tool::Surface(myFace1);
  const Handle(Geom_Surface) S2=BRep_Tool::Surface(myFace2);

  const Standard_Real aTolF1=BRep_Tool::Tolerance(myFace1);
  const Standard_Real aTolF2=BRep_Tool::Tolerance(myFace2);

  Standard_Real TolArc = aTolF1 + aTolF2;
  Standard_Real TolTang = TolArc;

  const Standard_Boolean isFace1Quad = (aType1 == GeomAbs_Cylinder ||
                                        aType1 == GeomAbs_Cone ||
                                        aType1 == GeomAbs_Torus);

  const Standard_Boolean isFace2Quad = (aType2 == GeomAbs_Cylinder ||
                                        aType2 == GeomAbs_Cone ||
                                        aType2 == GeomAbs_Torus);

  if(aType1==GeomAbs_Plane && aType2==GeomAbs_Plane)  {
    Standard_Real umin, umax, vmin, vmax;
    //
    BRepTools::UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    //
    BRepTools::UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
    //
    Standard_Real TolAng = 1.e-8;
    //
    PerformPlanes(myHS1, myHS2, TolAng, TolTang, myApprox1, myApprox2, 
                  mySeqOfCurve, myTangentFaces);
    //
    myIsDone = Standard_True;
    
    if(!myTangentFaces) {
      const Standard_Integer NbLinPP = mySeqOfCurve.Length();
      if(NbLinPP) {
        Standard_Real aTolFMax;
        myTolReached3d = 1.e-7;
        aTolFMax=Max(aTolF1, aTolF2);
        if (aTolFMax>myTolReached3d) {
          myTolReached3d=aTolFMax;
        }
        //
        myTolReached2d = myTolReached3d;

        if (bReverse) {
          Handle(Geom2d_Curve) aC2D1, aC2D2;
          const Standard_Integer aNbLin = mySeqOfCurve.Length();
          for (Standard_Integer i = 1; i <= aNbLin; ++i) {
            IntTools_Curve& aIC=mySeqOfCurve(i);
            aC2D1=aIC.FirstCurve2d();
            aC2D2=aIC.SecondCurve2d();
            aIC.SetFirstCurve2d(aC2D2);
            aIC.SetSecondCurve2d(aC2D1);
          }
        }
      }
    }
    return;
  }//if(aType1==GeomAbs_Plane && aType2==GeomAbs_Plane){

  if ((aType1==GeomAbs_Plane) && isFace2Quad)
  {
    Standard_Real umin, umax, vmin, vmax;
    // F1
    BRepTools::UVBounds(myFace1, umin, umax, vmin, vmax); 
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    // F2
    BRepTools::UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace2, (aTolF1 + aTolF2) * 2., umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
    //
    if( aType2==GeomAbs_Cone ) { 
      TolArc = 0.0001; 
      hasCone = Standard_True; 
    }
  }
  else if ((aType2==GeomAbs_Plane) && isFace1Quad)
  {
    Standard_Real umin, umax, vmin, vmax;
    //F1
    BRepTools::UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace1, (aTolF1 + aTolF2) * 2., umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    // F2
    BRepTools::UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
    //
    if( aType1==GeomAbs_Cone ) {
      TolArc = 0.0001; 
      hasCone = Standard_True; 
    }
  }
  else
  {
    Standard_Real umin, umax, vmin, vmax;
    BRepTools::UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace1, (aTolF1 + aTolF2) * 2., umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    BRepTools::UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace2, (aTolF1 + aTolF2) * 2., umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
  }

  const Handle(IntTools_TopolTool) dom1 = new IntTools_TopolTool(myHS1);
  const Handle(IntTools_TopolTool) dom2 = new IntTools_TopolTool(myHS2);

  myLConstruct.Load(dom1, dom2, myHS1, myHS2);
  

  Tolerances(myHS1, myHS2, TolTang);

  {
    const Standard_Real UVMaxStep = 0.001;
    const Standard_Real Deflection = (hasCone) ? 0.085 : 0.1;
    myIntersector.SetTolerances(TolArc, TolTang, UVMaxStep, Deflection); 
  }
  
  if((myHS1->IsUClosed() && !myHS1->IsUPeriodic()) || 
     (myHS1->IsVClosed() && !myHS1->IsVPeriodic()) ||
     (myHS2->IsUClosed() && !myHS2->IsUPeriodic()) || 
     (myHS2->IsVClosed() && !myHS2->IsVPeriodic()))
  {
    RestrictLine = Standard_True;
  }
  //
  if((aType1 != GeomAbs_BSplineSurface) &&
      (aType1 != GeomAbs_BezierSurface)  &&
     (aType1 != GeomAbs_OtherSurface)  &&
     (aType2 != GeomAbs_BSplineSurface) &&
      (aType2 != GeomAbs_BezierSurface)  &&
     (aType2 != GeomAbs_OtherSurface))
  {
    RestrictLine = Standard_True;

    if ((aType1 == GeomAbs_Torus) ||
        (aType2 == GeomAbs_Torus))
    {
      myListOfPnts.Clear();
    }
  }

  //
  if(!RestrictLine)
  {
    TopExp_Explorer aExp;
    for(Standard_Integer i = 0; (!RestrictLine) && (i < 2); i++)
    {
      const TopoDS_Face& aF=(!i) ? myFace1 : myFace2;
      aExp.Init(aF, TopAbs_EDGE);
      for(; aExp.More(); aExp.Next())
      {
        const TopoDS_Edge& aE=TopoDS::Edge(aExp.Current());

        if(BRep_Tool::Degenerated(aE))
        {
          RestrictLine = Standard_True;
          break;
        }
      }
    }
  }

  const Standard_Boolean isGeomInt = isTreatAnalityc(aF1, aF2);
  myIntersector.Perform(myHS1, dom1, myHS2, dom2, TolArc, TolTang, 
                                  myListOfPnts, RestrictLine, isGeomInt);

  myIsDone = myIntersector.IsDone();

  if (myIsDone)
  {
    myTangentFaces=myIntersector.TangentFaces();
    if (myTangentFaces) {
      return;
    }
    //
    if(RestrictLine) {
      myListOfPnts.Clear(); // to use LineConstructor
    }
    //
    const Standard_Integer aNbLinIntersector = myIntersector.NbLines();
    for (Standard_Integer i=1; i <= aNbLinIntersector; ++i) {
      MakeCurve(i, dom1, dom2);
    }
    //
    ComputeTolReached3d();
    //
    if (bReverse) {
      Handle(Geom2d_Curve) aC2D1, aC2D2;
      //
      const Standard_Integer aNbLinSeqOfCurve =mySeqOfCurve.Length();
      for (Standard_Integer i=1; i<=aNbLinSeqOfCurve; ++i)
      {
        IntTools_Curve& aIC=mySeqOfCurve(i);
        aC2D1=aIC.FirstCurve2d();
        aC2D2=aIC.SecondCurve2d();
        aIC.SetFirstCurve2d(aC2D2);
        aIC.SetSecondCurve2d(aC2D1);
      }
    }

    // Points
    Standard_Boolean bValid2D1, bValid2D2;
    Standard_Real U1,V1,U2,V2;
    IntTools_PntOnFace aPntOnF1, aPntOnF2;
    IntTools_PntOn2Faces aPntOn2Faces;
    //
    const Standard_Integer aNbPnts = myIntersector.NbPnts();
    for (Standard_Integer i=1; i <= aNbPnts; ++i)
    {
      const IntSurf_PntOn2S& aISPnt=myIntersector.Point(i).PntOn2S();
      const gp_Pnt& aPnt=aISPnt.Value();
      aISPnt.Parameters(U1,V1,U2,V2);
      //
      // check the validity of the intersection point for the faces
      bValid2D1 = myContext->IsPointInOnFace(myFace1, gp_Pnt2d(U1, V1));
      if (!bValid2D1) {
        continue;
      }
      //
      bValid2D2 = myContext->IsPointInOnFace(myFace2, gp_Pnt2d(U2, V2));
      if (!bValid2D2) {
        continue;
      }
      //
      // add the intersection point
      aPntOnF1.Init(myFace1, aPnt, U1, V1);
      aPntOnF2.Init(myFace2, aPnt, U2, V2);
      //
      if (!bReverse)
      {
        aPntOn2Faces.SetP1(aPntOnF1);
        aPntOn2Faces.SetP2(aPntOnF2);
      }
      else
      {
        aPntOn2Faces.SetP2(aPntOnF1);
        aPntOn2Faces.SetP1(aPntOnF2);
      }

      myPnts.Append(aPntOn2Faces);
    }
  }
}

//=======================================================================
//function : ComputeTolerance
//purpose  : 
//=======================================================================
Standard_Real IntTools_FaceFace::ComputeTolerance()
{
  Standard_Integer i, j, aNbLin;
  Standard_Real aFirst, aLast, aD, aDMax, aT;
  Handle(Geom_Surface) aS1, aS2;
  //
  aDMax = 0;
  aNbLin = mySeqOfCurve.Length();
  //
  aS1 = myHS1->ChangeSurface().Surface();
  aS2 = myHS2->ChangeSurface().Surface();
  //
  for (i = 1; i <= aNbLin; ++i)
  {
    const IntTools_Curve& aIC = mySeqOfCurve(i);
    const Handle(Geom_Curve)& aC3D = aIC.Curve();
    if (aC3D.IsNull())
    {
      continue;
    }
    //
    aFirst = aC3D->FirstParameter();
    aLast  = aC3D->LastParameter();
    //
    const Handle(Geom2d_Curve)& aC2D1 = aIC.FirstCurve2d();
    const Handle(Geom2d_Curve)& aC2D2 = aIC.SecondCurve2d();
    //
    for (j = 0; j < 2; ++j)
    {
      const Handle(Geom2d_Curve)& aC2D = !j ? aC2D1 : aC2D2;
      const Handle(Geom_Surface)& aS = !j ? aS1 : aS2;
      //
      if (!aC2D.IsNull())
      {
        if (IntTools_Tools::ComputeTolerance
            (aC3D, aC2D, aS, aFirst, aLast, aD, aT))
        {
          if (aD > aDMax)
          {
            aDMax = aD;
          }
        }
      }
      else
      {
        const TopoDS_Face& aF = !j ? myFace1 : myFace2;
        aD = FindMaxDistance(aC3D, aFirst, aLast, aF, myContext);
        if (aD > aDMax)
        {
          aDMax = aD;
        }
      }
    }
  }
  //
  return aDMax;
}

//=======================================================================
//function :ComputeTolReached3d 
//purpose  : 
//=======================================================================
  void IntTools_FaceFace::ComputeTolReached3d()
{
  Standard_Integer aNbLin;
  GeomAbs_SurfaceType aType1, aType2;
  //
  aNbLin=myIntersector.NbLines();
  if (!aNbLin) {
    return;
  }
  //
  aType1=myHS1->Surface().GetType();
  aType2=myHS2->Surface().GetType();
  //
  if (aType1==GeomAbs_Cylinder && aType2==GeomAbs_Cylinder)
  {
    if (aNbLin==2)
    { 
      Handle(IntPatch_Line) aIL1, aIL2;
      IntPatch_IType aTL1, aTL2;
      //
      aIL1=myIntersector.Line(1);
      aIL2=myIntersector.Line(2);
      aTL1=aIL1->ArcType();
      aTL2=aIL2->ArcType();
      if (aTL1==IntPatch_Lin && aTL2==IntPatch_Lin) {
        Standard_Real aD, aDTresh, dTol;
        gp_Lin aL1, aL2;
        //
        dTol=1.e-8;
        aDTresh=1.5e-6;
        //
        aL1=Handle(IntPatch_GLine)::DownCast(aIL1)->Line();
        aL2=Handle(IntPatch_GLine)::DownCast(aIL2)->Line();
        aD=aL1.Distance(aL2);
        aD=0.5*aD;
        if (aD<aDTresh)
        {//In order to avoid creation too thin face
          myTolReached3d=aD+dTol;
        }
      }
    }
  }// if (aType1==GeomAbs_Cylinder && aType2==GeomAbs_Cylinder) {
  //

  Standard_Real aDMax = ComputeTolerance();
  if (aDMax > myTolReached3d)
  {
      myTolReached3d = aDMax;
    }
  }

//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================
  void IntTools_FaceFace::MakeCurve(const Standard_Integer Index,
                                    const Handle(Adaptor3d_TopolTool)& dom1,
                                    const Handle(Adaptor3d_TopolTool)& dom2) 
{
  Standard_Boolean bDone, rejectSurface, reApprox, bAvoidLineConstructor;
  Standard_Boolean ok, bPCurvesOk;
  Standard_Integer i, j, aNbParts;
  Standard_Real fprm, lprm;
  Standard_Real Tolpc;
  Handle(IntPatch_Line) L;
  IntPatch_IType typl;
  Handle(Geom_Curve) newc;
  //
  const Standard_Real TOLCHECK   =0.0000001;
  const Standard_Real TOLANGCHECK=0.1;
  //
  rejectSurface = Standard_False;
  reApprox = Standard_False;
  //
  bPCurvesOk = Standard_True;
 
 reapprox:;
  
  Tolpc = myTolApprox;
  bAvoidLineConstructor = Standard_False;
  L = myIntersector.Line(Index);
  typl = L->ArcType();
  //
  if(typl==IntPatch_Walking) {
    Handle(IntPatch_Line) anewL;
    //
    Handle(IntPatch_WLine) aWLine (Handle(IntPatch_WLine)::DownCast(L));
    anewL = ComputePurgedWLine(aWLine, myHS1, myHS2);
    if(anewL.IsNull()) {
      return;
    }
    L = anewL;

    //Handle(IntPatch_WLine) aWLineX (Handle(IntPatch_WLine)::DownCast(L));
    //DumpWLine(aWLineX);

    //
    if(!myListOfPnts.IsEmpty()) {
      bAvoidLineConstructor = Standard_True;
    }

    Standard_Integer nbp = aWLine->NbPnts();
    const IntSurf_PntOn2S& p1 = aWLine->Point(1);
    const IntSurf_PntOn2S& p2 = aWLine->Point(nbp);

    const gp_Pnt& P1 = p1.Value();
    const gp_Pnt& P2 = p2.Value();

    if(P1.SquareDistance(P2) < 1.e-14) {
      bAvoidLineConstructor = Standard_False;
    }
  }

  typl=L->ArcType();

  //
  // Line Constructor
  if(!bAvoidLineConstructor) {
    myLConstruct.Perform(L);
    //
    bDone=myLConstruct.IsDone();
    if(!bDone)
    {
      return;
    }

    if(typl != IntPatch_Restriction)
    {
      aNbParts=myLConstruct.NbParts();
      if (aNbParts <= 0)
      {
        return;
      }
    }
  }
  // Do the Curve
  
  
  switch (typl) {
  //########################################  
  // Line, Parabola, Hyperbola
  //########################################  
  case IntPatch_Lin:
  case IntPatch_Parabola: 
  case IntPatch_Hyperbola: {
    if (typl == IntPatch_Lin) {
      newc = 
        new Geom_Line (Handle(IntPatch_GLine)::DownCast(L)->Line());
    }

    else if (typl == IntPatch_Parabola) {
      newc = 
        new Geom_Parabola(Handle(IntPatch_GLine)::DownCast(L)->Parabola());
    }
    
    else if (typl == IntPatch_Hyperbola) {
      newc = 
        new Geom_Hyperbola (Handle(IntPatch_GLine)::DownCast(L)->Hyperbola());
    }
    //
    // myTolReached3d
    if (typl == IntPatch_Lin) {
      TolR3d (myFace1, myFace2, myTolReached3d);
    }
    //
    aNbParts=myLConstruct.NbParts();
    for (i=1; i<=aNbParts; i++) {
      Standard_Boolean bFNIt, bLPIt;
      //
      myLConstruct.Part(i, fprm, lprm);
        //
      bFNIt=Precision::IsNegativeInfinite(fprm);
      bLPIt=Precision::IsPositiveInfinite(lprm);
      //
      if (!bFNIt && !bLPIt) {
        //
        IntTools_Curve aCurve;
        //
        Handle(Geom_TrimmedCurve) aCT3D=new Geom_TrimmedCurve(newc, fprm, lprm);
        aCurve.SetCurve(aCT3D);
        if (typl == IntPatch_Parabola) {
          Standard_Real aTolF1, aTolF2, aTolBase;
          
          aTolF1 = BRep_Tool::Tolerance(myFace1);
          aTolF2 = BRep_Tool::Tolerance(myFace2);
          aTolBase=aTolF1+aTolF2;
          myTolReached3d=IntTools_Tools::CurveTolerance(aCT3D, aTolBase);
        }
        //
        aCurve.SetCurve(new Geom_TrimmedCurve(newc, fprm, lprm));
        if(myApprox1) { 
          Handle (Geom2d_Curve) C2d;
          GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc,
                myHS1->ChangeSurface().Surface(), newc, C2d);
          if(Tolpc>myTolReached2d || myTolReached2d==0.) { 
            myTolReached2d=Tolpc;
          }
            //            
            aCurve.SetFirstCurve2d(new Geom2d_TrimmedCurve(C2d,fprm,lprm));
          }
          else { 
            Handle(Geom2d_BSplineCurve) H1;
            //
            aCurve.SetFirstCurve2d(H1);
          }
        //
        if(myApprox2) { 
          Handle (Geom2d_Curve) C2d;
          GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc,
                    myHS2->ChangeSurface().Surface(), newc, C2d);
          if(Tolpc>myTolReached2d || myTolReached2d==0.) { 
            myTolReached2d=Tolpc;
          }
          //
          aCurve.SetSecondCurve2d(new Geom2d_TrimmedCurve(C2d,fprm,lprm));
          }
        else { 
          Handle(Geom2d_BSplineCurve) H1;
          //
          aCurve.SetSecondCurve2d(H1);
        }
        mySeqOfCurve.Append(aCurve);
      } //if (!bFNIt && !bLPIt) {
      else {
        //  on regarde si on garde
        //
        Standard_Real aTestPrm, dT=100.;
        //
        aTestPrm=0.;
        if (bFNIt && !bLPIt) {
          aTestPrm=lprm-dT;
        }
        else if (!bFNIt && bLPIt) {
          aTestPrm=fprm+dT;
        }
        else {
          // i.e, if (bFNIt && bLPIt)
          aTestPrm=IntTools_Tools::IntermediatePoint(-dT, dT);
        }
        //
        gp_Pnt ptref(newc->Value(aTestPrm));
        //
        GeomAbs_SurfaceType typS1 = myHS1->GetType();
        GeomAbs_SurfaceType typS2 = myHS2->GetType();
        if( typS1 == GeomAbs_SurfaceOfExtrusion ||
            typS1 == GeomAbs_OffsetSurface ||
            typS1 == GeomAbs_SurfaceOfRevolution ||
            typS2 == GeomAbs_SurfaceOfExtrusion ||
            typS2 == GeomAbs_OffsetSurface ||
            typS2 == GeomAbs_SurfaceOfRevolution) {
          Handle(Geom2d_BSplineCurve) H1;
          mySeqOfCurve.Append(IntTools_Curve(newc, H1, H1));
          continue;
        }

        Standard_Real u1, v1, u2, v2, Tol;
        
        Tol = Precision::Confusion();
        Parameters(myHS1, myHS2, ptref,  u1, v1, u2, v2);
        ok = (dom1->Classify(gp_Pnt2d(u1, v1), Tol) != TopAbs_OUT);
        if(ok) { 
          ok = (dom2->Classify(gp_Pnt2d(u2,v2),Tol) != TopAbs_OUT); 
        }
        if (ok) {
          Handle(Geom2d_BSplineCurve) H1;
          mySeqOfCurve.Append(IntTools_Curve(newc, H1, H1));
        }
      }
    }// for (i=1; i<=aNbParts; i++) {
  }// case IntPatch_Lin:  case IntPatch_Parabola:  case IntPatch_Hyperbola:
    break;

  //########################################  
  // Circle and Ellipse
  //########################################  
  case IntPatch_Circle: 
  case IntPatch_Ellipse: {

    if (typl == IntPatch_Circle) {
      newc = new Geom_Circle
        (Handle(IntPatch_GLine)::DownCast(L)->Circle());
    }
    else { //IntPatch_Ellipse
      newc = new Geom_Ellipse
        (Handle(IntPatch_GLine)::DownCast(L)->Ellipse());
    }
    //
    // myTolReached3d
    TolR3d (myFace1, myFace2, myTolReached3d);
    //
    aNbParts=myLConstruct.NbParts();
    //
    Standard_Real aPeriod, aNul;
    TColStd_SequenceOfReal aSeqFprm,  aSeqLprm;
    
    aNul=0.;
    aPeriod=M_PI+M_PI;

    for (i=1; i<=aNbParts; i++) {
      myLConstruct.Part(i, fprm, lprm);

      if (fprm < aNul && lprm > aNul) {
        // interval that goes through 0. is divided on two intervals;
        while (fprm<aNul || fprm>aPeriod) fprm=fprm+aPeriod;
        while (lprm<aNul || lprm>aPeriod) lprm=lprm+aPeriod;
        //
        if((aPeriod - fprm) > Tolpc) {
          aSeqFprm.Append(fprm);
          aSeqLprm.Append(aPeriod);
        }
        else {
          gp_Pnt P1 = newc->Value(fprm);
          gp_Pnt P2 = newc->Value(aPeriod);
          Standard_Real aTolDist = BRep_Tool::Tolerance(myFace1) + BRep_Tool::Tolerance(myFace2);
          aTolDist = (myTolReached3d > aTolDist) ? myTolReached3d : aTolDist;

          if(P1.Distance(P2) > aTolDist) {
            Standard_Real anewpar = fprm;

            if(ParameterOutOfBoundary(fprm, newc, myFace1, myFace2, 
                                      lprm, Standard_False, anewpar, myContext)) {
              fprm = anewpar;
            }
            aSeqFprm.Append(fprm);
            aSeqLprm.Append(aPeriod);
          }
        }

        //
        if((lprm - aNul) > Tolpc) {
          aSeqFprm.Append(aNul);
          aSeqLprm.Append(lprm);
        }
        else {
          gp_Pnt P1 = newc->Value(aNul);
          gp_Pnt P2 = newc->Value(lprm);
          Standard_Real aTolDist = BRep_Tool::Tolerance(myFace1) + BRep_Tool::Tolerance(myFace2);
          aTolDist = (myTolReached3d > aTolDist) ? myTolReached3d : aTolDist;

          if(P1.Distance(P2) > aTolDist) {
            Standard_Real anewpar = lprm;

            if(ParameterOutOfBoundary(lprm, newc, myFace1, myFace2, 
                                      fprm, Standard_True, anewpar, myContext)) {
              lprm = anewpar;
            }
            aSeqFprm.Append(aNul);
            aSeqLprm.Append(lprm);
          }
        }
      }
      else {
        // usual interval 
        aSeqFprm.Append(fprm);
        aSeqLprm.Append(lprm);
      }
    }
    
    //
    aNbParts=aSeqFprm.Length();
    for (i=1; i<=aNbParts; i++) {
      fprm=aSeqFprm(i);
      lprm=aSeqLprm(i);
      //
      Standard_Real aRealEpsilon=RealEpsilon();
      if (Abs(fprm) > aRealEpsilon || Abs(lprm-2.*M_PI) > aRealEpsilon) {
        //==============================================
        ////
        IntTools_Curve aCurve;
        Handle(Geom_TrimmedCurve) aTC3D=new Geom_TrimmedCurve(newc,fprm,lprm);
        aCurve.SetCurve(aTC3D);
        fprm=aTC3D->FirstParameter();
        lprm=aTC3D->LastParameter ();
        ////         
        if (typl == IntPatch_Circle || typl == IntPatch_Ellipse) {//// 
          if(myApprox1) { 
            Handle (Geom2d_Curve) C2d;
            GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc, 
                        myHS1->ChangeSurface().Surface(), newc, C2d);
            if(Tolpc>myTolReached2d || myTolReached2d==0) { 
              myTolReached2d=Tolpc;
            }
            //
            aCurve.SetFirstCurve2d(C2d);
          }
          else { //// 
            Handle(Geom2d_BSplineCurve) H1;
            aCurve.SetFirstCurve2d(H1);
          }


          if(myApprox2) { 
            Handle (Geom2d_Curve) C2d;
            GeomInt_IntSS::BuildPCurves(fprm,lprm,Tolpc,
                    myHS2->ChangeSurface().Surface(),newc,C2d);
            if(Tolpc>myTolReached2d || myTolReached2d==0) { 
              myTolReached2d=Tolpc;
            }
            //
            aCurve.SetSecondCurve2d(C2d);
          }
          else { 
            Handle(Geom2d_BSplineCurve) H1;
            aCurve.SetSecondCurve2d(H1);
          }
        }
        
        else { 
          Handle(Geom2d_BSplineCurve) H1;
          aCurve.SetFirstCurve2d(H1);
          aCurve.SetSecondCurve2d(H1);
        }
        mySeqOfCurve.Append(aCurve);
          //==============================================        
      } //if (Abs(fprm) > RealEpsilon() || Abs(lprm-2.*M_PI) > RealEpsilon())

      else {
        //  on regarde si on garde
        //
        if (aNbParts==1) {
//           if (Abs(fprm) < RealEpsilon() &&  Abs(lprm-2.*M_PI) < RealEpsilon()) {
          if (Abs(fprm) <= aRealEpsilon && Abs(lprm-2.*M_PI) <= aRealEpsilon) {
            IntTools_Curve aCurve;
            Handle(Geom_TrimmedCurve) aTC3D=new Geom_TrimmedCurve(newc,fprm,lprm);
            aCurve.SetCurve(aTC3D);
            fprm=aTC3D->FirstParameter();
            lprm=aTC3D->LastParameter ();
            
            if(myApprox1) { 
              Handle (Geom2d_Curve) C2d;
              GeomInt_IntSS::BuildPCurves(fprm,lprm,Tolpc,
                    myHS1->ChangeSurface().Surface(),newc,C2d);
              if(Tolpc>myTolReached2d || myTolReached2d==0) { 
                myTolReached2d=Tolpc;
              }
              //
              aCurve.SetFirstCurve2d(C2d);
            }
            else { //// 
              Handle(Geom2d_BSplineCurve) H1;
              aCurve.SetFirstCurve2d(H1);
            }

            if(myApprox2) { 
              Handle (Geom2d_Curve) C2d;
              GeomInt_IntSS::BuildPCurves(fprm,lprm,Tolpc,
                    myHS2->ChangeSurface().Surface(),newc,C2d);
              if(Tolpc>myTolReached2d || myTolReached2d==0) { 
                myTolReached2d=Tolpc;
              }
              //
              aCurve.SetSecondCurve2d(C2d);
            }
            else { 
              Handle(Geom2d_BSplineCurve) H1;
              aCurve.SetSecondCurve2d(H1);
            }
            mySeqOfCurve.Append(aCurve);
            break;
          }
        }
        //
        Standard_Real aTwoPIdiv17, u1, v1, u2, v2, Tol;

        aTwoPIdiv17=2.*M_PI/17.;

        for (j=0; j<=17; j++) {
          gp_Pnt ptref (newc->Value (j*aTwoPIdiv17));
          Tol = Precision::Confusion();

          Parameters(myHS1, myHS2, ptref, u1, v1, u2, v2);
          ok = (dom1->Classify(gp_Pnt2d(u1,v1),Tol) != TopAbs_OUT);
          if(ok) { 
            ok = (dom2->Classify(gp_Pnt2d(u2,v2),Tol) != TopAbs_OUT);
          }
          if (ok) {
            IntTools_Curve aCurve;
            aCurve.SetCurve(newc);
            //==============================================
            if (typl == IntPatch_Circle || typl == IntPatch_Ellipse) {
              
              if(myApprox1) { 
                Handle (Geom2d_Curve) C2d;
                GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc, 
                        myHS1->ChangeSurface().Surface(), newc, C2d);
                if(Tolpc>myTolReached2d || myTolReached2d==0) { 
                  myTolReached2d=Tolpc;
                }
                // 
                aCurve.SetFirstCurve2d(C2d);
              }
              else { 
                Handle(Geom2d_BSplineCurve) H1;
                aCurve.SetFirstCurve2d(H1);
              }
                
              if(myApprox2) { 
                Handle (Geom2d_Curve) C2d;
                GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc,
                        myHS2->ChangeSurface().Surface(), newc, C2d);
                if(Tolpc>myTolReached2d || myTolReached2d==0) { 
                  myTolReached2d=Tolpc;
                }
                //                 
                aCurve.SetSecondCurve2d(C2d);
              }
                
              else { 
                Handle(Geom2d_BSplineCurve) H1;
                aCurve.SetSecondCurve2d(H1);
              }
            }//  end of if (typl == IntPatch_Circle || typl == IntPatch_Ellipse)
             
            else { 
              Handle(Geom2d_BSplineCurve) H1;
              //         
              aCurve.SetFirstCurve2d(H1);
              aCurve.SetSecondCurve2d(H1);
            }
            //==============================================        
            //
            mySeqOfCurve.Append(aCurve);
            break;

            }//  end of if (ok) {
          }//  end of for (Standard_Integer j=0; j<=17; j++)
        }//  end of else { on regarde si on garde
      }// for (i=1; i<=myLConstruct.NbParts(); i++)
    }// IntPatch_Circle: IntPatch_Ellipse:
    break;
    
  case IntPatch_Analytic: {
    IntSurf_Quadric quad1,quad2;
    GeomAbs_SurfaceType typs = myHS1->Surface().GetType();
    
    switch (typs) {
      case GeomAbs_Plane:
        quad1.SetValue(myHS1->Surface().Plane());
        break;
      case GeomAbs_Cylinder:
        quad1.SetValue(myHS1->Surface().Cylinder());
        break;
      case GeomAbs_Cone:
        quad1.SetValue(myHS1->Surface().Cone());
        break;
      case GeomAbs_Sphere:
        quad1.SetValue(myHS1->Surface().Sphere());
        break;
    case GeomAbs_Torus:
      quad1.SetValue(myHS1->Surface().Torus());
      break;
      default:
        Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve 1");
      }
      
    typs = myHS2->Surface().GetType();
    
    switch (typs) {
      case GeomAbs_Plane:
        quad2.SetValue(myHS2->Surface().Plane());
        break;
      case GeomAbs_Cylinder:
        quad2.SetValue(myHS2->Surface().Cylinder());
        break;
      case GeomAbs_Cone:
        quad2.SetValue(myHS2->Surface().Cone());
        break;
      case GeomAbs_Sphere:
        quad2.SetValue(myHS2->Surface().Sphere());
        break;
    case GeomAbs_Torus:
      quad2.SetValue(myHS2->Surface().Torus());
      break;
      default:
        Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve 2");
      }
    //
    //=========
    IntPatch_ALineToWLine convert (quad1, quad2);
      
    if (!myApprox) {
      aNbParts=myLConstruct.NbParts();
      for (i=1; i<=aNbParts; i++) {
        myLConstruct.Part(i, fprm, lprm);
        Handle(IntPatch_WLine) WL = 
          convert.MakeWLine(Handle(IntPatch_ALine)::DownCast(L), fprm, lprm);
        //
        Handle(Geom2d_BSplineCurve) H1;
        Handle(Geom2d_BSplineCurve) H2;

        if(myApprox1) {
          H1 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_True);
        }
        
        if(myApprox2) {
          H2 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_False);
        }
        //          
        mySeqOfCurve.Append(IntTools_Curve(MakeBSpline(WL,1,WL->NbPnts()), H1, H2));
      }
    } // if (!myApprox)

    else { // myApprox=TRUE
      GeomInt_WLApprox theapp3d;
      // 
      Standard_Real tol2d = myTolApprox;
      //         
      theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_True);
      
      aNbParts=myLConstruct.NbParts();
      for (i=1; i<=aNbParts; i++) {
        myLConstruct.Part(i, fprm, lprm);
        Handle(IntPatch_WLine) WL = 
          convert.MakeWLine(Handle(IntPatch_ALine):: DownCast(L),fprm,lprm);

        theapp3d.Perform(myHS1,myHS2,WL,Standard_True,myApprox1,myApprox2, 1, WL->NbPnts());

        if (!theapp3d.IsDone()) {
          //
          Handle(Geom2d_BSplineCurve) H1;
          Handle(Geom2d_BSplineCurve) H2;

          if(myApprox1) {
            H1 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_True);
          }
          
          if(myApprox2) {
            H2 = MakeBSpline2d(WL, 1, WL->NbPnts(), Standard_False);
          }
          //          
          mySeqOfCurve.Append(IntTools_Curve(MakeBSpline(WL,1,WL->NbPnts()), H1, H2));
        }

        else {
          if(myApprox1 || myApprox2) { 
            if( theapp3d.TolReached2d()>myTolReached2d || myTolReached2d==0) { 
              myTolReached2d = theapp3d.TolReached2d();
            }
          }
          
          if( theapp3d.TolReached3d()>myTolReached3d || myTolReached3d==0) { 
            myTolReached3d = theapp3d.TolReached3d();
          }

          Standard_Integer aNbMultiCurves, nbpoles;
          aNbMultiCurves=theapp3d.NbMultiCurves();
          for (j=1; j<=aNbMultiCurves; j++) {
            const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
            nbpoles = mbspc.NbPoles();
            
            TColgp_Array1OfPnt tpoles(1, nbpoles);
            mbspc.Curve(1, tpoles);
            Handle(Geom_BSplineCurve) BS=new Geom_BSplineCurve(tpoles,
                                                               mbspc.Knots(),
                                                               mbspc.Multiplicities(),
                                                               mbspc.Degree());
            
            GeomLib_CheckBSplineCurve Check(BS,TOLCHECK,TOLANGCHECK);
            Check.FixTangent(Standard_True,Standard_True);
            // 
            IntTools_Curve aCurve;
            aCurve.SetCurve(BS);
            
            if(myApprox1) { 
              TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
              mbspc.Curve(2,tpoles2d);
              Handle(Geom2d_BSplineCurve) BS2=new Geom2d_BSplineCurve(tpoles2d,
                                                                      mbspc.Knots(),
                                                                      mbspc.Multiplicities(),
                                                                      mbspc.Degree());

              GeomLib_Check2dBSplineCurve newCheck(BS2,TOLCHECK,TOLANGCHECK);
              newCheck.FixTangent(Standard_True,Standard_True);
              //                 
              aCurve.SetFirstCurve2d(BS2);
            }
            else {
              Handle(Geom2d_BSplineCurve) H1;
              aCurve.SetFirstCurve2d(H1);
            }
            
            if(myApprox2) { 
              TColgp_Array1OfPnt2d tpoles2d(1, nbpoles);
              Standard_Integer TwoOrThree;
              TwoOrThree=myApprox1 ? 3 : 2;
              mbspc.Curve(TwoOrThree, tpoles2d);
              Handle(Geom2d_BSplineCurve) BS2 =new Geom2d_BSplineCurve(tpoles2d,
                                                                       mbspc.Knots(),
                                                                       mbspc.Multiplicities(),
                                                                       mbspc.Degree());
                
              GeomLib_Check2dBSplineCurve newCheck(BS2,TOLCHECK,TOLANGCHECK);
              newCheck.FixTangent(Standard_True,Standard_True);
              //         
              aCurve.SetSecondCurve2d(BS2);
            }
            else { 
              Handle(Geom2d_BSplineCurve) H2;
              aCurve.SetSecondCurve2d(H2);
            }
            // 
            mySeqOfCurve.Append(aCurve);

          }// for (j=1; j<=aNbMultiCurves; j++) {
        }// else from if (!theapp3d.IsDone())
      }// for (i=1; i<=aNbParts; i++) {
    }// else { // myApprox=TRUE
  }// case IntPatch_Analytic:
    break;

  case IntPatch_Walking:{
    Handle(IntPatch_WLine) WL = 
      Handle(IntPatch_WLine)::DownCast(L);

#ifdef OCCT_DEBUG
    //WL->Dump(0);
#endif

    //
    Standard_Integer ifprm, ilprm;
    //
    if (!myApprox) {
      aNbParts = 1;
      if(!bAvoidLineConstructor){
        aNbParts=myLConstruct.NbParts();
      }
      for (i=1; i<=aNbParts; ++i) {
        Handle(Geom2d_BSplineCurve) H1, H2;
        Handle(Geom_Curve) aBSp;
        //
        if(bAvoidLineConstructor) {
          ifprm = 1;
          ilprm = WL->NbPnts();
        }
        else {
          myLConstruct.Part(i, fprm, lprm);
          ifprm=(Standard_Integer)fprm;
          ilprm=(Standard_Integer)lprm;
        }
        //
        if(myApprox1) {
          H1 = MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
        }
        //
        if(myApprox2) {
          H2 = MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
        }
        //           
        aBSp=MakeBSpline(WL, ifprm, ilprm);
        IntTools_Curve aIC(aBSp, H1, H2);
        mySeqOfCurve.Append(aIC);
      }// for (i=1; i<=aNbParts; ++i) {
    }// if (!myApprox) {
    //
    else { // X
      Standard_Boolean bIsDecomposited;
      Standard_Integer nbiter, aNbSeqOfL;
      Standard_Real tol2d, aTolApproxImp;
      IntPatch_SequenceOfLine aSeqOfL;
      GeomInt_WLApprox theapp3d;
      Approx_ParametrizationType aParType = Approx_ChordLength;
      //
      Standard_Boolean anApprox1 = myApprox1;
      Standard_Boolean anApprox2 = myApprox2;
      //
      aTolApproxImp=1.e-5;
      tol2d = myTolApprox;

      GeomAbs_SurfaceType typs1, typs2;
      typs1 = myHS1->Surface().GetType();
      typs2 = myHS2->Surface().GetType();
      Standard_Boolean anWithPC = Standard_True;

      if(typs1 == GeomAbs_Cylinder && typs2 == GeomAbs_Sphere) {
        anWithPC = 
          ApproxWithPCurves(myHS1->Surface().Cylinder(), myHS2->Surface().Sphere());
      }
      else if (typs1 == GeomAbs_Sphere && typs2 == GeomAbs_Cylinder) {
        anWithPC = 
          ApproxWithPCurves(myHS2->Surface().Cylinder(), myHS1->Surface().Sphere());
      }
      //
      if(!anWithPC) {
        myTolApprox = aTolApproxImp;//1.e-5; 
        anApprox1 = Standard_False;
        anApprox2 = Standard_False;
        //         
        tol2d = myTolApprox;
      }
        
      if(myHS1 == myHS2) { 
        theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_False, aParType);
        rejectSurface = Standard_True;
      }
      else { 
        if(reApprox && !rejectSurface)
          theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_False, aParType);
        else {
          Standard_Integer iDegMax, iDegMin, iNbIter;
          //
          ApproxParameters(myHS1, myHS2, iDegMin, iDegMax, iNbIter);
          theapp3d.SetParameters(myTolApprox, tol2d, iDegMin, iDegMax, iNbIter, Standard_True, aParType);
        }
      }
      //
      Standard_Real aReachedTol = Precision::Confusion();
      bIsDecomposited=DecompositionOfWLine(WL,
                                           myHS1, 
                                           myHS2, 
                                           myFace1, 
                                           myFace2, 
                                           myLConstruct, 
                                           bAvoidLineConstructor, 
                                           aSeqOfL, 
                                           aReachedTol,
                                           myContext);
      if ( bIsDecomposited && ( myTolReached3d < aReachedTol ) ) {
        myTolReached3d = aReachedTol;
      }
      //
      aNbSeqOfL=aSeqOfL.Length();
      //
      if (bIsDecomposited) {
        nbiter=aNbSeqOfL;
      }
      else {
        nbiter=1;
        aNbParts=1;
        if (!bAvoidLineConstructor) {
          aNbParts=myLConstruct.NbParts();
          nbiter=aNbParts;
        }
      }
      //
      for(i = 1; i <= nbiter; ++i) {
        if(bIsDecomposited) {
          WL = Handle(IntPatch_WLine)::DownCast(aSeqOfL.Value(i));
          ifprm = 1;
          ilprm = WL->NbPnts();
        }
        else {
          if(bAvoidLineConstructor) {
            ifprm = 1;
            ilprm = WL->NbPnts();
          }
          else {
            myLConstruct.Part(i, fprm, lprm);
            ifprm = (Standard_Integer)fprm;
            ilprm = (Standard_Integer)lprm;
          }
        }
        //-- lbr : 
        //-- Si une des surfaces est un plan , on approxime en 2d
        //-- sur cette surface et on remonte les points 2d en 3d.
        if(typs1 == GeomAbs_Plane) { 
          theapp3d.Perform(myHS1, myHS2, WL, Standard_False,Standard_True, myApprox2,ifprm,ilprm);
        }          
        else if(typs2 == GeomAbs_Plane) { 
          theapp3d.Perform(myHS1,myHS2,WL,Standard_False,myApprox1,Standard_True,ifprm,ilprm);
        }
        else { 
          //
          if (myHS1 != myHS2){
            if ((typs1==GeomAbs_BezierSurface || typs1==GeomAbs_BSplineSurface) &&
                (typs2==GeomAbs_BezierSurface || typs2==GeomAbs_BSplineSurface)) {
             
              theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_True, aParType);
              
              Standard_Boolean bUseSurfaces;
              bUseSurfaces=NotUseSurfacesForApprox(myFace1, myFace2, WL, ifprm,  ilprm);
              if (bUseSurfaces) {
                // ######
                rejectSurface = Standard_True;
                // ######
                theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, Standard_False, aParType);
              }
            }
          }
          //
          theapp3d.Perform(myHS1,myHS2,WL,Standard_True,anApprox1,anApprox2,ifprm,ilprm);
        }
          //           
        if (!theapp3d.IsDone()) {
          Handle(Geom2d_BSplineCurve) H1;
          Handle(Geom2d_BSplineCurve) H2;
          //           
          Handle(Geom_Curve) aBSp=MakeBSpline(WL,ifprm, ilprm);
          //
          if(myApprox1) {
            H1 = MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
          }
          //
          if(myApprox2) {
            H2 = MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
          }
          //           
          IntTools_Curve aIC(aBSp, H1, H2);
          mySeqOfCurve.Append(aIC);
        }
        
        else {
          if(myApprox1 || myApprox2 || (typs1==GeomAbs_Plane || typs2==GeomAbs_Plane)) { 
            if( theapp3d.TolReached2d()>myTolReached2d || myTolReached2d==0.) { 
              myTolReached2d = theapp3d.TolReached2d();
            }
          }
          if(typs1==GeomAbs_Plane || typs2==GeomAbs_Plane) { 
            myTolReached3d = myTolReached2d;
            //
            if (typs1==GeomAbs_Torus || typs2==GeomAbs_Torus) {
              if (myTolReached3d<1.e-6) {
                myTolReached3d = theapp3d.TolReached3d();
                myTolReached3d=1.e-6;
              }
            }
          }
          else  if( theapp3d.TolReached3d()>myTolReached3d || myTolReached3d==0.) { 
            myTolReached3d = theapp3d.TolReached3d();
          }
          
          Standard_Integer aNbMultiCurves, nbpoles;
          aNbMultiCurves=theapp3d.NbMultiCurves(); 
          for (j=1; j<=aNbMultiCurves; j++) {
            if(typs1 == GeomAbs_Plane) {
              const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
              nbpoles = mbspc.NbPoles();
              
              TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
              TColgp_Array1OfPnt   tpoles(1,nbpoles);
              
              mbspc.Curve(1,tpoles2d);
              const gp_Pln&  Pln = myHS1->Surface().Plane();
              //
              Standard_Integer ik; 
              for(ik = 1; ik<= nbpoles; ik++) { 
                tpoles.SetValue(ik,
                                ElSLib::Value(tpoles2d.Value(ik).X(),
                                              tpoles2d.Value(ik).Y(),
                                              Pln));
              }
              //
              Handle(Geom_BSplineCurve) BS = 
                new Geom_BSplineCurve(tpoles,
                                      mbspc.Knots(),
                                      mbspc.Multiplicities(),
                                      mbspc.Degree());
              GeomLib_CheckBSplineCurve Check(BS,TOLCHECK,TOLANGCHECK);
              Check.FixTangent(Standard_True, Standard_True);
              //         
              IntTools_Curve aCurve;
              aCurve.SetCurve(BS);

              if(myApprox1) { 
                Handle(Geom2d_BSplineCurve) BS1 = 
                  new Geom2d_BSplineCurve(tpoles2d,
                                          mbspc.Knots(),
                                          mbspc.Multiplicities(),
                                          mbspc.Degree());
                GeomLib_Check2dBSplineCurve Check1(BS1,TOLCHECK,TOLANGCHECK);
                Check1.FixTangent(Standard_True,Standard_True);
                //
                // ############################################
                if(!rejectSurface && !reApprox) {
                  Standard_Boolean isValid = IsCurveValid(BS1);
                  if(!isValid) {
                    reApprox = Standard_True;
                    goto reapprox;
                  }
                }
                // ############################################
                aCurve.SetFirstCurve2d(BS1);
              }
              else {
                Handle(Geom2d_BSplineCurve) H1;
                aCurve.SetFirstCurve2d(H1);
              }

              if(myApprox2) { 
                mbspc.Curve(2, tpoles2d);
                
                Handle(Geom2d_BSplineCurve) BS2 = new Geom2d_BSplineCurve(tpoles2d,
                                                                          mbspc.Knots(),
                                                                          mbspc.Multiplicities(),
                                                                          mbspc.Degree());
                GeomLib_Check2dBSplineCurve newCheck(BS2,TOLCHECK,TOLANGCHECK);
                newCheck.FixTangent(Standard_True,Standard_True);
                
                // ###########################################
                if(!rejectSurface && !reApprox) {
                  Standard_Boolean isValid = IsCurveValid(BS2);
                  if(!isValid) {
                    reApprox = Standard_True;
                    goto reapprox;
                  }
                }
                // ###########################################
                // 
                aCurve.SetSecondCurve2d(BS2);
              }
              else { 
                Handle(Geom2d_BSplineCurve) H2;
                //                 
                  aCurve.SetSecondCurve2d(H2);
              }
              //
              mySeqOfCurve.Append(aCurve);

            }//if(typs1 == GeomAbs_Plane) {
            
            else if(typs2 == GeomAbs_Plane)
            {
              const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
              nbpoles = mbspc.NbPoles();
              
              TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
              TColgp_Array1OfPnt   tpoles(1,nbpoles);
              mbspc.Curve((myApprox1==Standard_True)? 2 : 1,tpoles2d);
              const gp_Pln&  Pln = myHS2->Surface().Plane();
              //
              Standard_Integer ik; 
              for(ik = 1; ik<= nbpoles; ik++) { 
                tpoles.SetValue(ik,
                                ElSLib::Value(tpoles2d.Value(ik).X(),
                                              tpoles2d.Value(ik).Y(),
                                              Pln));
                
              }
              //
              Handle(Geom_BSplineCurve) BS=new Geom_BSplineCurve(tpoles,
                                                                 mbspc.Knots(),
                                                                 mbspc.Multiplicities(),
                                                                 mbspc.Degree());
              GeomLib_CheckBSplineCurve Check(BS,TOLCHECK,TOLANGCHECK);
              Check.FixTangent(Standard_True,Standard_True);
              //         
              IntTools_Curve aCurve;
              aCurve.SetCurve(BS);

              if(myApprox2) {
                Handle(Geom2d_BSplineCurve) BS1=new Geom2d_BSplineCurve(tpoles2d,
                                                                        mbspc.Knots(),
                                                                        mbspc.Multiplicities(),
                                                                        mbspc.Degree());
                GeomLib_Check2dBSplineCurve Check1(BS1,TOLCHECK,TOLANGCHECK);
                Check1.FixTangent(Standard_True,Standard_True);
                //         
                // ###########################################
                if(!rejectSurface && !reApprox) {
                  Standard_Boolean isValid = IsCurveValid(BS1);
                  if(!isValid) {
                    reApprox = Standard_True;
                    goto reapprox;
                  }
                }
                // ###########################################
                bPCurvesOk = CheckPCurve(BS1, myFace2);
                aCurve.SetSecondCurve2d(BS1);
              }
              else {
                Handle(Geom2d_BSplineCurve) H2;
                aCurve.SetSecondCurve2d(H2);
              }
              
              if(myApprox1) { 
                mbspc.Curve(1,tpoles2d);
                Handle(Geom2d_BSplineCurve) BS2=new Geom2d_BSplineCurve(tpoles2d,
                                                                        mbspc.Knots(),
                                                                        mbspc.Multiplicities(),
                                                                        mbspc.Degree());
                GeomLib_Check2dBSplineCurve Check2(BS2,TOLCHECK,TOLANGCHECK);
                Check2.FixTangent(Standard_True,Standard_True);
                //
                // ###########################################
                if(!rejectSurface && !reApprox) {
                  Standard_Boolean isValid = IsCurveValid(BS2);
                  if(!isValid) {
                    reApprox = Standard_True;
                    goto reapprox;
                  }
                }
                // ###########################################
                bPCurvesOk = bPCurvesOk && CheckPCurve(BS2, myFace1);
                aCurve.SetFirstCurve2d(BS2);
              }
              else { 
                Handle(Geom2d_BSplineCurve) H1;
                //                 
                aCurve.SetFirstCurve2d(H1);
              }
              //
              //if points of the pcurves are out of the faces bounds
              //create 3d and 2d curves without approximation
              if (!bPCurvesOk) {
                Handle(Geom2d_BSplineCurve) H1, H2;
                bPCurvesOk = Standard_True;
                //           
                Handle(Geom_Curve) aBSp=MakeBSpline(WL,ifprm, ilprm);
                
                if(myApprox1) {
                  H1 = MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
                  bPCurvesOk = CheckPCurve(H1, myFace1);
                }
                
                if(myApprox2) {
                  H2 = MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
                  bPCurvesOk = bPCurvesOk && CheckPCurve(H2, myFace2);
                }
                //
                //if pcurves created without approximation are out of the 
                //faces bounds, use approximated 3d and 2d curves
                if (bPCurvesOk) {
                  IntTools_Curve aIC(aBSp, H1, H2);
                  mySeqOfCurve.Append(aIC);
                } else {
                  mySeqOfCurve.Append(aCurve);
                }
              } else {
                mySeqOfCurve.Append(aCurve);
              }

            }// else if(typs2 == GeomAbs_Plane)
            //
            else { //typs2 != GeomAbs_Plane && typs1 != GeomAbs_Plane
              Standard_Boolean bIsValid1, bIsValid2;
              Handle(Geom_BSplineCurve) BS;
              Handle(Geom2d_BSplineCurve) aH2D;        
              IntTools_Curve aCurve; 
              //
              bIsValid1=Standard_True;
              bIsValid2=Standard_True;
              //
              const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(j);
              nbpoles = mbspc.NbPoles();
              TColgp_Array1OfPnt tpoles(1,nbpoles);
              mbspc.Curve(1,tpoles);
              BS=new Geom_BSplineCurve(tpoles,
                                                                 mbspc.Knots(),
                                                                 mbspc.Multiplicities(),
                                                                 mbspc.Degree());
              GeomLib_CheckBSplineCurve Check(BS,TOLCHECK,TOLANGCHECK);
              Check.FixTangent(Standard_True,Standard_True);
              //                 
              aCurve.SetCurve(BS);
              aCurve.SetFirstCurve2d(aH2D);
              aCurve.SetSecondCurve2d(aH2D);
              //
              if(myApprox1) { 
                if(anApprox1) {
                  Handle(Geom2d_BSplineCurve) BS1;
                  TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
                  mbspc.Curve(2,tpoles2d);
                  //
                  BS1=new Geom2d_BSplineCurve(tpoles2d,
                                                                        mbspc.Knots(),
                                                                        mbspc.Multiplicities(),
                                                                        mbspc.Degree());
                  GeomLib_Check2dBSplineCurve newCheck(BS1,TOLCHECK,TOLANGCHECK);
                  newCheck.FixTangent(Standard_True,Standard_True);
                  //         
                  if (!reApprox) {
                    bIsValid1=CheckPCurve(BS1, myFace1);
                  }
                  //
                  aCurve.SetFirstCurve2d(BS1);
                }
                else {
                  Handle(Geom2d_BSplineCurve) BS1;
                  fprm = BS->FirstParameter();
                  lprm = BS->LastParameter();

                  Handle(Geom2d_Curve) C2d;
                  Standard_Real aTol = myTolApprox;
                  GeomInt_IntSS::BuildPCurves(fprm, lprm, aTol,
                            myHS1->ChangeSurface().Surface(), BS, C2d);
                  BS1 = Handle(Geom2d_BSplineCurve)::DownCast(C2d);
                  aCurve.SetFirstCurve2d(BS1);
                }
              } // if(myApprox1) { 
                //                 
              if(myApprox2) { 
                if(anApprox2) {
                  Handle(Geom2d_BSplineCurve) BS2;
                  TColgp_Array1OfPnt2d tpoles2d(1,nbpoles);
                  mbspc.Curve((myApprox1==Standard_True)? 3 : 2,tpoles2d);
                  BS2=new Geom2d_BSplineCurve(tpoles2d,
                                                                        mbspc.Knots(),
                                                                        mbspc.Multiplicities(),
                                                                        mbspc.Degree());
                  GeomLib_Check2dBSplineCurve newCheck(BS2,TOLCHECK,TOLANGCHECK);
                  newCheck.FixTangent(Standard_True,Standard_True);
                //                 
                  if (!reApprox) {
                    bIsValid2=CheckPCurve(BS2, myFace2);        
                  }
                  aCurve.SetSecondCurve2d(BS2);
                }
                else {
                  Handle(Geom2d_BSplineCurve) BS2;
                  fprm = BS->FirstParameter();
                  lprm = BS->LastParameter();

                  Handle(Geom2d_Curve) C2d;
                  Standard_Real aTol = myTolApprox;
                  GeomInt_IntSS::BuildPCurves(fprm, lprm, aTol,
                            myHS2->ChangeSurface().Surface(), BS, C2d);
                  BS2 = Handle(Geom2d_BSplineCurve)::DownCast(C2d);
                  aCurve.SetSecondCurve2d(BS2);
                }
              } //if(myApprox2) { 
              if (!bIsValid1 || !bIsValid2) {
                myTolApprox=aTolApproxImp;//1.e-5;
                tol2d = myTolApprox;
                reApprox = Standard_True;
                goto reapprox;
              }
                //                 
              mySeqOfCurve.Append(aCurve);
            }
          }
        }
      }
    }// else { // X
  }// case IntPatch_Walking:{
    break;
    
  case IntPatch_Restriction: 
    {
      Handle(IntPatch_RLine) RL = 
        Handle(IntPatch_RLine)::DownCast(L);
      Handle(Geom_Curve) aC3d;
      Handle(Geom2d_Curve) aC2d1, aC2d2;
      Standard_Real aTolReached;
      GeomInt_IntSS::TreatRLine(RL, myHS1, myHS2, aC3d,
                                  aC2d1, aC2d2, aTolReached);

      if(aC3d.IsNull())
        break;

      Bnd_Box2d aBox1, aBox2;

      const Standard_Real aU1f = myHS1->FirstUParameter(),
                          aV1f = myHS1->FirstVParameter(),
                          aU1l = myHS1->LastUParameter(),
                          aV1l = myHS1->LastVParameter();
      const Standard_Real aU2f = myHS2->FirstUParameter(),
                          aV2f = myHS2->FirstVParameter(),
                          aU2l = myHS2->LastUParameter(),
                          aV2l = myHS2->LastVParameter();

      aBox1.Add(gp_Pnt2d(aU1f, aV1f));
      aBox1.Add(gp_Pnt2d(aU1l, aV1l));
      aBox2.Add(gp_Pnt2d(aU2f, aV2f));
      aBox2.Add(gp_Pnt2d(aU2l, aV2l));

      GeomInt_VectorOfReal anArrayOfParameters;
        
      //We consider here that the intersection line is same-parameter-line
      anArrayOfParameters.Append(aC3d->FirstParameter());
      anArrayOfParameters.Append(aC3d->LastParameter());

      GeomInt_IntSS::
        TrimILineOnSurfBoundaries(aC2d1, aC2d2, aBox1, aBox2, anArrayOfParameters);

      const Standard_Integer aNbIntersSolutionsm1 = anArrayOfParameters.Length() - 1;

      //Trim RLine found.
      for(Standard_Integer anInd = 0; anInd < aNbIntersSolutionsm1; anInd++)
      {
        const Standard_Real aParF = anArrayOfParameters(anInd),
                            aParL = anArrayOfParameters(anInd+1);

        if((aParL - aParF) <= Precision::PConfusion())
          continue;

        const Standard_Real aPar = 0.5*(aParF + aParL);
        gp_Pnt2d aPt;

        Handle(Geom2d_Curve) aCurv2d1, aCurv2d2;
        if(!aC2d1.IsNull())
        {
          aC2d1->D0(aPar, aPt);

          if(aBox1.IsOut(aPt))
            continue;

          if(myApprox1)
            aCurv2d1 = new Geom2d_TrimmedCurve(aC2d1, aParF, aParL);
        }

        if(!aC2d2.IsNull())
        {
          aC2d2->D0(aPar, aPt);

          if(aBox2.IsOut(aPt))
            continue;

          if(myApprox2)
            aCurv2d2 = new Geom2d_TrimmedCurve(aC2d2, aParF, aParL);
        }

        Handle(Geom_Curve) aCurv3d = new Geom_TrimmedCurve(aC3d, aParF, aParL);

        IntTools_Curve aIC(aCurv3d, aCurv2d1, aCurv2d2);
        mySeqOfCurve.Append(aIC);
      }
    }
    break;
  default:
    break;

  }
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
 void Parameters(const Handle(GeomAdaptor_HSurface)& HS1,
                 const Handle(GeomAdaptor_HSurface)& HS2,
                 const gp_Pnt& Ptref,
                 Standard_Real& U1,
                 Standard_Real& V1,
                 Standard_Real& U2,
                 Standard_Real& V2)
{

  IntSurf_Quadric quad1,quad2;
  GeomAbs_SurfaceType typs = HS1->Surface().GetType();

  switch (typs) {
  case GeomAbs_Plane:
    quad1.SetValue(HS1->Surface().Plane());
    break;
  case GeomAbs_Cylinder:
    quad1.SetValue(HS1->Surface().Cylinder());
    break;
  case GeomAbs_Cone:
    quad1.SetValue(HS1->Surface().Cone());
    break;
  case GeomAbs_Sphere:
    quad1.SetValue(HS1->Surface().Sphere());
    break;
  case GeomAbs_Torus:
    quad1.SetValue(HS1->Surface().Torus());
    break;
  default:
    Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve");
  }
  
  typs = HS2->Surface().GetType();
  switch (typs) {
  case GeomAbs_Plane:
    quad2.SetValue(HS2->Surface().Plane());
    break;
  case GeomAbs_Cylinder:
    quad2.SetValue(HS2->Surface().Cylinder());
    break;
  case GeomAbs_Cone:
    quad2.SetValue(HS2->Surface().Cone());
    break;
  case GeomAbs_Sphere:
    quad2.SetValue(HS2->Surface().Sphere());
    break;
  case GeomAbs_Torus:
    quad2.SetValue(HS2->Surface().Torus());
    break;
  default:
    Standard_ConstructionError::Raise("GeomInt_IntSS::MakeCurve");
  }

  quad1.Parameters(Ptref,U1,V1);
  quad2.Parameters(Ptref,U2,V2);
}

//=======================================================================
//function : MakeBSpline
//purpose  : 
//=======================================================================
Handle(Geom_Curve) MakeBSpline  (const Handle(IntPatch_WLine)& WL,
                                 const Standard_Integer ideb,
                                 const Standard_Integer ifin)
{
  Standard_Integer i,nbpnt = ifin-ideb+1;
  TColgp_Array1OfPnt poles(1,nbpnt);
  TColStd_Array1OfReal knots(1,nbpnt);
  TColStd_Array1OfInteger mults(1,nbpnt);
  Standard_Integer ipidebm1;
  for(i=1,ipidebm1=i+ideb-1; i<=nbpnt;ipidebm1++, i++) {
    poles(i) = WL->Point(ipidebm1).Value();
    mults(i) = 1;
    knots(i) = i-1;
  }
  mults(1) = mults(nbpnt) = 2;
  return
    new Geom_BSplineCurve(poles,knots,mults,1);
}
//

//=======================================================================
//function : MakeBSpline2d
//purpose  : 
//=======================================================================
Handle(Geom2d_BSplineCurve) MakeBSpline2d(const Handle(IntPatch_WLine)& theWLine,
                                          const Standard_Integer ideb,
                                          const Standard_Integer ifin,
                                          const Standard_Boolean onFirst)
{
  Standard_Integer i, nbpnt = ifin-ideb+1;
  TColgp_Array1OfPnt2d poles(1,nbpnt);
  TColStd_Array1OfReal knots(1,nbpnt);
  TColStd_Array1OfInteger mults(1,nbpnt);
  Standard_Integer ipidebm1;

  for(i = 1, ipidebm1 = i+ideb-1; i <= nbpnt; ipidebm1++, i++) {
      Standard_Real U, V;
      if(onFirst)
        theWLine->Point(ipidebm1).ParametersOnS1(U, V);
      else
        theWLine->Point(ipidebm1).ParametersOnS2(U, V);
      poles(i).SetCoord(U, V);
      mults(i) = 1;
      knots(i) = i-1;
    }
    mults(1) = mults(nbpnt) = 2;

  return new Geom2d_BSplineCurve(poles,knots,mults,1);
}
//=======================================================================
//function : PrepareLines3D
//purpose  : 
//=======================================================================
  void IntTools_FaceFace::PrepareLines3D(const Standard_Boolean bToSplit)
{
  Standard_Integer i, aNbCurves;
  GeomAbs_SurfaceType aType1, aType2;
  IntTools_SequenceOfCurves aNewCvs;
  //
  // 1. Treatment closed  curves
  aNbCurves=mySeqOfCurve.Length();
  for (i=1; i<=aNbCurves; ++i) {
    const IntTools_Curve& aIC=mySeqOfCurve(i);
    //
    if (bToSplit) {
      Standard_Integer j, aNbC;
      IntTools_SequenceOfCurves aSeqCvs;
      //
      aNbC=IntTools_Tools::SplitCurve(aIC, aSeqCvs);
      if (aNbC) {
        for (j=1; j<=aNbC; ++j) {
          const IntTools_Curve& aICNew=aSeqCvs(j);
          aNewCvs.Append(aICNew);
        }
      }
      else {
        aNewCvs.Append(aIC);
      }
    }
    else {
      aNewCvs.Append(aIC);
    }
  }
  //
  // 2. Plane\Cone intersection when we had 4 curves
  aType1=myHS1->GetType();
  aType2=myHS2->GetType();
  aNbCurves=aNewCvs.Length();
  //
  if ((aType1==GeomAbs_Plane && aType2==GeomAbs_Cone) ||
      (aType2==GeomAbs_Plane && aType1==GeomAbs_Cone)) {
    if (aNbCurves==4) {
      GeomAbs_CurveType aCType1;
      //
      aCType1=aNewCvs(1).Type();
      if (aCType1==GeomAbs_Line) {
        IntTools_SequenceOfCurves aSeqIn, aSeqOut;
        //
        for (i=1; i<=aNbCurves; ++i) {
          const IntTools_Curve& aIC=aNewCvs(i);
          aSeqIn.Append(aIC);
        }
        //
        IntTools_Tools::RejectLines(aSeqIn, aSeqOut);
        //
        aNewCvs.Clear();
        aNbCurves=aSeqOut.Length(); 
        for (i=1; i<=aNbCurves; ++i) {
          const IntTools_Curve& aIC=aSeqOut(i);
          aNewCvs.Append(aIC);
        }
      }
    }
  }// if ((aType1==GeomAbs_Plane && aType2==GeomAbs_Cone)...
  //
  // 3. Fill  mySeqOfCurve
  mySeqOfCurve.Clear();
  aNbCurves=aNewCvs.Length();
  for (i=1; i<=aNbCurves; ++i) {
    const IntTools_Curve& aIC=aNewCvs(i);
    mySeqOfCurve.Append(aIC);
  }
}
//=======================================================================
//function : CorrectSurfaceBoundaries
//purpose  : 
//=======================================================================
 void CorrectSurfaceBoundaries(const TopoDS_Face&  theFace,
                              const Standard_Real theTolerance,
                              Standard_Real&      theumin,
                              Standard_Real&      theumax, 
                              Standard_Real&      thevmin, 
                              Standard_Real&      thevmax) 
{
  Standard_Boolean enlarge, isuperiodic, isvperiodic;
  Standard_Real uinf, usup, vinf, vsup, delta;
  GeomAbs_SurfaceType aType;
  Handle(Geom_Surface) aSurface;
  //
  aSurface = BRep_Tool::Surface(theFace);
  aSurface->Bounds(uinf, usup, vinf, vsup);
  delta = theTolerance;
  enlarge = Standard_False;
  //
  GeomAdaptor_Surface anAdaptorSurface(aSurface);
  //
  if(aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_Surface) aBasisSurface = 
      (Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface))->BasisSurface();
    
    if(aBasisSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)) ||
       aBasisSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
      return;
    }
  }
  //
  if(aSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_Surface) aBasisSurface = 
      (Handle(Geom_OffsetSurface)::DownCast(aSurface))->BasisSurface();
    
    if(aBasisSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)) ||
       aBasisSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
      return;
    }
  }
  //
  isuperiodic = anAdaptorSurface.IsUPeriodic();
  isvperiodic = anAdaptorSurface.IsVPeriodic();
  //
  aType=anAdaptorSurface.GetType();
  if((aType==GeomAbs_BezierSurface) ||
     (aType==GeomAbs_BSplineSurface) ||
     (aType==GeomAbs_SurfaceOfExtrusion) ||
     (aType==GeomAbs_SurfaceOfRevolution) ||
     (aType==GeomAbs_Cylinder)) {
    enlarge=Standard_True;
  }
  //
  if(!isuperiodic && enlarge) {

    if(!Precision::IsInfinite(theumin) &&
        ((theumin - uinf) > delta))
      theumin -= delta;
    else {
      theumin = uinf;
    }

    if(!Precision::IsInfinite(theumax) &&
        ((usup - theumax) > delta))
      theumax += delta;
    else
      theumax = usup;
  }
  //
  if(!isvperiodic && enlarge) {
    if(!Precision::IsInfinite(thevmin) &&
        ((thevmin - vinf) > delta)) {
      thevmin -= delta;
    }
    else { 
      thevmin = vinf;
    }
    if(!Precision::IsInfinite(thevmax) &&
        ((vsup - thevmax) > delta)) {
      thevmax += delta;
    }
    else {
      thevmax = vsup;
    }
  }
  //
  {
    Standard_Integer aNbP;
    Standard_Real aXP, dXfact, aXmid, aX1, aX2, aTolPA;
    //
    aTolPA=Precision::Angular();
    // U
    if (isuperiodic) {
      aXP=anAdaptorSurface.UPeriod();
      dXfact=theumax-theumin;
      if (dXfact-aTolPA>aXP) {
        aXmid=0.5*(theumax+theumin);
        aNbP=RealToInt(aXmid/aXP);
        if (aXmid<0.) {
          aNbP=aNbP-1;
        }
        aX1=aNbP*aXP;
        if (theumin>aTolPA) {
          aX1=theumin+aNbP*aXP;
        }
        aX2=aX1+aXP;
        if (theumin<aX1) {
          theumin=aX1;
        }
        if (theumax>aX2) {
          theumax=aX2;
        }
      }
    }
    // V
    if (isvperiodic) {
      aXP=anAdaptorSurface.VPeriod();
      dXfact=thevmax-thevmin;
      if (dXfact-aTolPA>aXP) {
        aXmid=0.5*(thevmax+thevmin);
        aNbP=RealToInt(aXmid/aXP);
        if (aXmid<0.) {
          aNbP=aNbP-1;
        }
        aX1=aNbP*aXP;
        if (thevmin>aTolPA) {
          aX1=thevmin+aNbP*aXP;
        }
        aX2=aX1+aXP;
        if (thevmin<aX1) {
          thevmin=aX1;
        }
        if (thevmax>aX2) {
          thevmax=aX2;
        }
      }
    }
  }
  //
  if(isuperiodic || isvperiodic) {
    Standard_Boolean correct = Standard_False;
    Standard_Boolean correctU = Standard_False;
    Standard_Boolean correctV = Standard_False;
    Bnd_Box2d aBox;
    TopExp_Explorer anExp;

    for(anExp.Init(theFace, TopAbs_EDGE); anExp.More(); anExp.Next()) {
      if(BRep_Tool::IsClosed(TopoDS::Edge(anExp.Current()), theFace)) {
        correct = Standard_True;
        Standard_Real f, l;
        TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());
        
        for(Standard_Integer i = 0; i < 2; i++) {
          if(i==0) {
            anEdge.Orientation(TopAbs_FORWARD);
          }
          else {
            anEdge.Orientation(TopAbs_REVERSED);
          }
          Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface(anEdge, theFace, f, l);
          
          if(aCurve.IsNull()) {
            correct = Standard_False;
            break;
          }
          Handle(Geom2d_Line) aLine = Handle(Geom2d_Line)::DownCast(aCurve);

          if(aLine.IsNull()) {
            correct = Standard_False;
            break;
          }
          gp_Dir2d anUDir(1., 0.);
          gp_Dir2d aVDir(0., 1.);
          Standard_Real anAngularTolerance = Precision::Angular();

          correctU = correctU || aLine->Position().Direction().IsParallel(aVDir, anAngularTolerance);
          correctV = correctV || aLine->Position().Direction().IsParallel(anUDir, anAngularTolerance);
          
          gp_Pnt2d pp1 = aCurve->Value(f);
          aBox.Add(pp1);
          gp_Pnt2d pp2 = aCurve->Value(l);
          aBox.Add(pp2);
        }
        if(!correct)
          break;
      }
    }

    if(correct) {
      Standard_Real umin, vmin, umax, vmax;
      aBox.Get(umin, vmin, umax, vmax);

      if(isuperiodic && correctU) {
        
        if(theumin < umin)
          theumin = umin;
        
        if(theumax > umax) {
          theumax = umax;
        }
      }
      if(isvperiodic && correctV) {
        
        if(thevmin < vmin)
          thevmin = vmin;
        if(thevmax > vmax)
          thevmax = vmax;
      }
    }
  }
}
//
//
// The block is dedicated to determine whether WLine [ifprm, ilprm]
// crosses the degenerated zone on each given surface or not.
// If Yes -> We will not use info about surfaces during approximation
// because inside degenerated zone of the surface the approx. algo.
// uses wrong values of normal, etc., and resulting curve will have
// oscillations that we would not like to have. 
 


static
  Standard_Boolean IsDegeneratedZone(const gp_Pnt2d& aP2d,
                                     const Handle(Geom_Surface)& aS,
                                     const Standard_Integer iDir);
static
  Standard_Boolean IsPointInDegeneratedZone(const IntSurf_PntOn2S& aP2S,
                                            const TopoDS_Face& aF1,
                                            const TopoDS_Face& aF2);
//=======================================================================
//function :  NotUseSurfacesForApprox
//purpose  : 
//=======================================================================
Standard_Boolean NotUseSurfacesForApprox(const TopoDS_Face& aF1,
                                         const TopoDS_Face& aF2,
                                         const Handle(IntPatch_WLine)& WL,
                                         const Standard_Integer ifprm,
                                         const Standard_Integer ilprm)
{
  Standard_Boolean bPInDZ;

  Handle(IntSurf_LineOn2S) aLineOn2S=WL->Curve();
  
  const IntSurf_PntOn2S& aP2Sfprm=aLineOn2S->Value(ifprm);
  bPInDZ=IsPointInDegeneratedZone(aP2Sfprm, aF1, aF2);
  if (bPInDZ) {
    return bPInDZ;
  }

  const IntSurf_PntOn2S& aP2Slprm=aLineOn2S->Value(ilprm);
  bPInDZ=IsPointInDegeneratedZone(aP2Slprm, aF1, aF2);
  
  return bPInDZ;
}
//=======================================================================
//function : IsPointInDegeneratedZone
//purpose  : 
//=======================================================================
Standard_Boolean IsPointInDegeneratedZone(const IntSurf_PntOn2S& aP2S,
                                          const TopoDS_Face& aF1,
                                          const TopoDS_Face& aF2)
                                          
{
  Standard_Boolean bFlag=Standard_True;
  Standard_Real US11, US12, VS11, VS12, US21, US22, VS21, VS22;
  Standard_Real U1, V1, U2, V2, aDelta, aD;
  gp_Pnt2d aP2d;

  Handle(Geom_Surface)aS1 = BRep_Tool::Surface(aF1);
  aS1->Bounds(US11, US12, VS11, VS12);
  GeomAdaptor_Surface aGAS1(aS1);

  Handle(Geom_Surface)aS2 = BRep_Tool::Surface(aF2);
  aS1->Bounds(US21, US22, VS21, VS22);
  GeomAdaptor_Surface aGAS2(aS2);
  //
  //const gp_Pnt& aP=aP2S.Value();
  aP2S.Parameters(U1, V1, U2, V2);
  //
  aDelta=1.e-7;
  // Check on Surf 1
  aD=aGAS1.UResolution(aDelta);
  aP2d.SetCoord(U1, V1);
  if (fabs(U1-US11) < aD) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(U1-US12) < aD) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  aD=aGAS1.VResolution(aDelta);
  if (fabs(V1-VS11) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(V1-VS12) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS1, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  // Check on Surf 2
  aD=aGAS2.UResolution(aDelta);
  aP2d.SetCoord(U2, V2);
  if (fabs(U2-US21) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  if (fabs(U2-US22) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 1);
    if (bFlag) {
      return bFlag;
    }
  }
  aD=aGAS2.VResolution(aDelta);
  if (fabs(V2-VS21) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 2);
    if (bFlag) {  
      return bFlag;
    }
  }
  if (fabs(V2-VS22) < aDelta) {
    bFlag=IsDegeneratedZone(aP2d, aS2, 2);
    if (bFlag) {
      return bFlag;
    }
  }
  return !bFlag;
}

//=======================================================================
//function : IsDegeneratedZone
//purpose  : 
//=======================================================================
Standard_Boolean IsDegeneratedZone(const gp_Pnt2d& aP2d,
                                   const Handle(Geom_Surface)& aS,
                                   const Standard_Integer iDir)
{
  Standard_Boolean bFlag=Standard_True;
  Standard_Real US1, US2, VS1, VS2, dY, dX, d1, d2, dD;
  Standard_Real aXm, aYm, aXb, aYb, aXe, aYe;
  aS->Bounds(US1, US2, VS1, VS2); 

  gp_Pnt aPm, aPb, aPe;
  
  aXm=aP2d.X();
  aYm=aP2d.Y();
  
  aS->D0(aXm, aYm, aPm); 
  
  dX=1.e-5;
  dY=1.e-5;
  dD=1.e-12;

  if (iDir==1) {
    aXb=aXm;
    aXe=aXm;
    aYb=aYm-dY;
    if (aYb < VS1) {
      aYb=VS1;
    }
    aYe=aYm+dY;
    if (aYe > VS2) {
      aYe=VS2;
    }
    aS->D0(aXb, aYb, aPb);
    aS->D0(aXe, aYe, aPe);
    
    d1=aPm.Distance(aPb);
    d2=aPm.Distance(aPe);
    if (d1 < dD && d2 < dD) {
      return bFlag;
    }
    return !bFlag;
  }
  //
  else if (iDir==2) {
    aYb=aYm;
    aYe=aYm;
    aXb=aXm-dX;
    if (aXb < US1) {
      aXb=US1;
    }
    aXe=aXm+dX;
    if (aXe > US2) {
      aXe=US2;
    }
    aS->D0(aXb, aYb, aPb);
    aS->D0(aXe, aYe, aPe);
    
    d1=aPm.Distance(aPb);
    d2=aPm.Distance(aPe);
    if (d1 < dD && d2 < dD) {
      return bFlag;
    }
    return !bFlag;
  }
  return !bFlag;
}

// Check if aNextPnt lies inside of tube build on aBasePnt and aBaseVec.
// In 2d space.
static Standard_Boolean IsInsideIn2d(const gp_Pnt2d& aBasePnt,
                                     const gp_Vec2d& aBaseVec,
                                     const gp_Pnt2d& aNextPnt,
                                     const Standard_Real aSquareMaxDist)
{
  gp_Vec2d aVec2d(aBasePnt, aNextPnt);

  //d*d = (basevec^(nextpnt-basepnt))**2 / basevec**2
  Standard_Real aCross = aVec2d.Crossed(aBaseVec);
  Standard_Real aSquareDist = aCross * aCross
                            / aBaseVec.SquareMagnitude();

  return (aSquareDist <= aSquareMaxDist);
}

// Check if aNextPnt lies inside of tube build on aBasePnt and aBaseVec.
// In 3d space.
static Standard_Boolean IsInsideIn3d(const gp_Pnt& aBasePnt,
                                     const gp_Vec& aBaseVec,
                                     const gp_Pnt& aNextPnt,
                                     const Standard_Real aSquareMaxDist)
{
  gp_Vec aVec(aBasePnt, aNextPnt);

  //d*d = (basevec^(nextpnt-basepnt))**2 / basevec**2
  Standard_Real aSquareDist = aVec.CrossSquareMagnitude(aBaseVec)
                            / aBaseVec.SquareMagnitude();

  return (aSquareDist <= aSquareMaxDist);
}

//=========================================================================
// static function : ComputePurgedWLine
// purpose : Removes equal points (leave one of equal points) from theWLine
//           and recompute vertex parameters.
//           Removes exceed points using tube criteria:
//           delete 7D point if it lies near to expected lines in 2d and 3d.
//           Each task (2d, 2d, 3d) have its own tolerance and checked separately.
//           Returns new WLine or null WLine if the number
//           of the points is less than 2.
//=========================================================================
Handle(IntPatch_WLine) ComputePurgedWLine(const Handle(IntPatch_WLine)       &theWLine,
                                          const Handle(GeomAdaptor_HSurface) &theS1,
                                          const Handle(GeomAdaptor_HSurface) &theS2)
{
  Standard_Integer i, k, v, nb, nbvtx;
  Handle(IntPatch_WLine) aResult;
  nbvtx = theWLine->NbVertex();
  nb = theWLine->NbPnts();
  if (nb==2) {
    const IntSurf_PntOn2S& p1 = theWLine->Point(1);
    const IntSurf_PntOn2S& p2 = theWLine->Point(2);
    if(p1.Value().IsEqual(p2.Value(), gp::Resolution())) {
      return aResult;
    }
  }
  //
  Handle(IntPatch_WLine) aLocalWLine;
  Handle(IntPatch_WLine) aTmpWLine = theWLine;
  Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
  aLocalWLine = new IntPatch_WLine(aLineOn2S, Standard_False);
  for(i = 1; i <= nb; i++) {
    aLineOn2S->Add(theWLine->Point(i));
  }

  for(v = 1; v <= nbvtx; v++) {
    aLocalWLine->AddVertex(theWLine->Vertex(v));
  }
  for(i = 1; i <= aLineOn2S->NbPoints(); i++) {
    Standard_Integer aStartIndex = i + 1;
    Standard_Integer anEndIndex = i + 5;
    nb = aLineOn2S->NbPoints();
    anEndIndex = (anEndIndex > nb) ? nb : anEndIndex;

    if((aStartIndex > nb) || (anEndIndex <= 1)) {
      continue;
    }
    k = aStartIndex;

    while(k <= anEndIndex) {
      
      if(i != k) {
        IntSurf_PntOn2S p1 = aLineOn2S->Value(i);
        IntSurf_PntOn2S p2 = aLineOn2S->Value(k);
        
        Standard_Real UV[8];
        p1.Parameters(UV[0], UV[1], UV[2], UV[3]);
        p2.Parameters(UV[4], UV[5], UV[6], UV[7]);

        Standard_Real aMax = Abs(UV[0]);
        for(Standard_Integer anIdx = 1; anIdx < 8; anIdx++)
        {
          if (aMax < Abs(UV[anIdx]))
            aMax = Abs(UV[anIdx]);
        }

        if(p1.Value().IsEqual(p2.Value(), gp::Resolution()) ||
           Abs(UV[0] - UV[4]) + Abs(UV[1] - UV[5]) < 1.0e-16 * aMax ||
           Abs(UV[2] - UV[6]) + Abs(UV[3] - UV[7]) < 1.0e-16 * aMax )
        {
          aTmpWLine = aLocalWLine;
          aLocalWLine = new IntPatch_WLine(aLineOn2S, Standard_False);
          
          for(v = 1; v <= aTmpWLine->NbVertex(); v++) {
            IntPatch_Point aVertex = aTmpWLine->Vertex(v);
            Standard_Integer avertexindex = (Standard_Integer)aVertex.ParameterOnLine();

            if(avertexindex >= k) {
              aVertex.SetParameter(aVertex.ParameterOnLine() - 1.);
            }
            aLocalWLine->AddVertex(aVertex);
          }
          aLineOn2S->RemovePoint(k);
          anEndIndex--;
          continue;
        }
      }
      k++;
    }
  }

  if (aLineOn2S->NbPoints() <= 2)
  {
    if (aLineOn2S->NbPoints() == 2)
      return aLocalWLine;
    else
      return aResult;
  }

  const Standard_Integer aMinNbBadDistr = 15;
  const Standard_Integer aNbSingleBezier = 30;
  // Avoid purge in case of C0 continuity:
  // Intersection approximator may produce invalid curve after purge, example:
  // bugs modalg_5 bug24731,
  // Do not run purger when base number of points is too small.
  if (theS1->UContinuity() == GeomAbs_C0 ||
      theS1->VContinuity() == GeomAbs_C0 ||
      theS2->UContinuity() == GeomAbs_C0 ||
      theS2->VContinuity() == GeomAbs_C0 ||
      nb < aNbSingleBezier)
  {
    return aLocalWLine;
  }

  // 1 - Delete point.
  // 0 - Store point.
  // -1 - Vertex point (not delete).
  NCollection_Array1<Standard_Integer> aNewPointsHash(1, aLineOn2S->NbPoints());
  for(i = 1; i <= aLineOn2S->NbPoints(); i++)
    aNewPointsHash.SetValue(i, 0);

  for(v = 1; v <= aLocalWLine->NbVertex(); v++) 
  {
    IntPatch_Point aVertex = aLocalWLine->Vertex(v);
    Standard_Integer avertexindex = (Standard_Integer)aVertex.ParameterOnLine();
    aNewPointsHash.SetValue(avertexindex, -1);
  }

  // Workaround to handle case of small amount points after purge.
  // Test "boolean boptuc_complex B5" and similar.
  Standard_Integer aNbPnt = 0;

  // Inital computations.
  Standard_Real UonS1[3], VonS1[3], UonS2[3], VonS2[3];
  aLineOn2S->Value(1).ParametersOnS1(UonS1[0], VonS1[0]);
  aLineOn2S->Value(2).ParametersOnS1(UonS1[1], VonS1[1]);
  aLineOn2S->Value(1).ParametersOnS2(UonS2[0], VonS2[0]);
  aLineOn2S->Value(2).ParametersOnS2(UonS2[1], VonS2[1]);

  gp_Pnt2d aBase2dPnt1(UonS1[0], VonS1[0]);
  gp_Pnt2d aBase2dPnt2(UonS2[0], VonS2[0]);
  gp_Vec2d aBase2dVec1(UonS1[1] - UonS1[0], VonS1[1] - VonS1[0]);
  gp_Vec2d aBase2dVec2(UonS2[1] - UonS2[0], VonS2[1] - VonS2[0]);
  gp_Pnt   aBase3dPnt = aLineOn2S->Value(1).Value();
  gp_Vec   aBase3dVec(aLineOn2S->Value(1).Value(), aLineOn2S->Value(2).Value());

  // Choose base tolerance and scale it to pipe algorithm.
  const Standard_Real aBaseTolerance = Precision::Approximation();
  Standard_Real aResS1Tol = Min(theS1->UResolution(aBaseTolerance),
                                theS1->VResolution(aBaseTolerance));
  Standard_Real aResS2Tol = Min(theS2->UResolution(aBaseTolerance),
                                theS2->VResolution(aBaseTolerance));
  Standard_Real aTol1 = aResS1Tol * aResS1Tol;
  Standard_Real aTol2 = aResS2Tol * aResS2Tol;
  Standard_Real aTol3d = aBaseTolerance * aBaseTolerance;

  const Standard_Real aLimitCoeff = 0.99 * 0.99;
  for(i = 3; i <= aLineOn2S->NbPoints(); i++)
  {
    Standard_Boolean isDeleteState = Standard_False;

    aLineOn2S->Value(i).ParametersOnS1(UonS1[2], VonS1[2]);
    aLineOn2S->Value(i).ParametersOnS2(UonS2[2], VonS2[2]);
    gp_Pnt2d aPnt2dOnS1(UonS1[2], VonS1[2]);
    gp_Pnt2d aPnt2dOnS2(UonS2[2], VonS2[2]);
    const gp_Pnt& aPnt3d = aLineOn2S->Value(i).Value();

    if (aNewPointsHash(i - 1) != - 1 &&
        IsInsideIn2d(aBase2dPnt1, aBase2dVec1, aPnt2dOnS1, aTol1) &&
        IsInsideIn2d(aBase2dPnt2, aBase2dVec2, aPnt2dOnS2, aTol2) &&
        IsInsideIn3d(aBase3dPnt, aBase3dVec, aPnt3d, aTol3d) )
    {
      // Handle possible uneven parametrization on one of 2d subspaces.
      // Delete point only when expected lengths are close to each other (aLimitCoeff).
      // Example:
      // c2d1 - line
      // c3d - line
      // c2d2 - geometrically line, but have uneven parametrization -> c2d2 is bspline.
      gp_XY aPntOnS1[2]= { gp_XY(UonS1[1] - UonS1[0], VonS1[1] - VonS1[0])
                         , gp_XY(UonS1[2] - UonS1[1], VonS1[2] - VonS1[1])};
      gp_XY aPntOnS2[2]= { gp_XY(UonS2[1] - UonS2[0], VonS2[1] - VonS2[0])
                         , gp_XY(UonS2[2] - UonS2[1], VonS2[2] - VonS2[1])};

      Standard_Real aStepOnS1 = aPntOnS1[0].SquareModulus() / aPntOnS1[1].SquareModulus();
      Standard_Real aStepOnS2 = aPntOnS2[0].SquareModulus() / aPntOnS2[1].SquareModulus();

      Standard_Real aStepCoeff = Min(aStepOnS1, aStepOnS2) / Max(aStepOnS1, aStepOnS2);

      if (aStepCoeff > aLimitCoeff)
      {
        // Set hash flag to "Delete" state.
        isDeleteState = Standard_True;
        aNewPointsHash.SetValue(i - 1, 1);

        // Change middle point.
        UonS1[1] = UonS1[2];
        UonS2[1] = UonS2[2];
        VonS1[1] = VonS1[2];
        VonS2[1] = VonS2[2];
      }
    }

    if (!isDeleteState)
    {
      // Compute new pipe parameters.
      UonS1[0] = UonS1[1];
      VonS1[0] = VonS1[1];
      UonS2[0] = UonS2[1];
      VonS2[0] = VonS2[1];

      UonS1[1] = UonS1[2];
      VonS1[1] = VonS1[2];
      UonS2[1] = UonS2[2];
      VonS2[1] = VonS2[2];

      aBase2dPnt1.SetCoord(UonS1[0], VonS1[0]);
      aBase2dPnt2.SetCoord(UonS2[0], VonS2[0]);
      aBase2dVec1.SetCoord(UonS1[1] - UonS1[0], VonS1[1] - VonS1[0]);
      aBase2dVec2.SetCoord(UonS2[1] - UonS2[0], VonS2[1] - VonS2[0]);
      aBase3dPnt = aLineOn2S->Value(i - 1).Value();
      aBase3dVec = gp_Vec(aLineOn2S->Value(i - 1).Value(), aLineOn2S->Value(i).Value());

      aNbPnt++;
    }
  }

  // Workaround to handle case of small amount of points after purge.
  // Test "boolean boptuc_complex B5" and similar.
  // This is possible since there are at least two points.
  if (aNewPointsHash(1) == -1 &&
      aNewPointsHash(2) == -1 &&
      aNbPnt <= 3)
  {
    // Delete first.
    aNewPointsHash(1) = 1;
  }
  if (aNewPointsHash(aLineOn2S->NbPoints() - 1) == -1 &&
      aNewPointsHash(aLineOn2S->NbPoints()    ) == -1 &&
      aNbPnt <= 3)
  {
    // Delete last.
    aNewPointsHash(aLineOn2S->NbPoints() ) = 1;
  }

  // Purgre when too small amount of points left.
  if (aNbPnt <= 2)
  {
    for(i = aNewPointsHash.Lower(); i <= aNewPointsHash.Upper(); i++)
    {
      if (aNewPointsHash(i) != -1)
      {
        aNewPointsHash(i) = 1;
      }
    }
  }

  // Handle possible bad distribution of points, 
  // which are will converted into one single bezier curve (less than 30 points).
  // Make distribution more even:
  // max step will be nearly to 0.1 of param distance.
  if (aNbPnt + 2 > aMinNbBadDistr &&
      aNbPnt + 2 < aNbSingleBezier )
  {
    for(Standard_Integer anIdx = 1; anIdx <= 8; anIdx++)
    {
      Standard_Integer aHashIdx = 
        Standard_Integer(anIdx * aLineOn2S->NbPoints() / 9);

      //Store this point.
      aNewPointsHash(aHashIdx) = 0;
    }
  }

  aTmpWLine = aLocalWLine;
  Handle(IntSurf_LineOn2S) aPurgedLineOn2S = new IntSurf_LineOn2S();
  aLocalWLine = new IntPatch_WLine(aPurgedLineOn2S, Standard_False);
  Standard_Integer anOldLineIdx = 1, aVertexIdx = 1;
  for(i = 1; i <= aNewPointsHash.Upper(); i++)
  {
    if (aNewPointsHash(i) == 0)
    {
      // Store this point.
      aPurgedLineOn2S->Add(aLineOn2S->Value(i));
      anOldLineIdx++;
    }
    else if (aNewPointsHash(i) == -1)
    {
      // Add vertex.
      IntPatch_Point aVertex = aTmpWLine->Vertex(aVertexIdx++);
      aVertex.SetParameter(anOldLineIdx++);
      aLocalWLine->AddVertex(aVertex);
      aPurgedLineOn2S->Add(aLineOn2S->Value(i));
    }
  }

  if(aPurgedLineOn2S->NbPoints() > 1) {
    aResult = aLocalWLine;
  }
  return aResult;
}

//=======================================================================
//function : TolR3d
//purpose  : 
//=======================================================================
void TolR3d(const TopoDS_Face& aF1,
            const TopoDS_Face& aF2,
            Standard_Real& myTolReached3d)
{
  Standard_Real aTolF1, aTolF2, aTolFMax, aTolTresh;
      
  aTolTresh=2.999999e-3;
  aTolF1 = BRep_Tool::Tolerance(aF1);
  aTolF2 = BRep_Tool::Tolerance(aF2);
  aTolFMax=Max(aTolF1, aTolF2);
  
  if (aTolFMax>aTolTresh) {
    myTolReached3d=aTolFMax;
  }
}
//=======================================================================
//function : IsPointOnBoundary
//purpose  : 
//=======================================================================
Standard_Boolean IsPointOnBoundary(const Standard_Real theParameter,
                                   const Standard_Real theFirstBoundary,
                                   const Standard_Real theSecondBoundary,
                                   const Standard_Real theResolution,
                                   Standard_Boolean&   IsOnFirstBoundary) 
{
  Standard_Boolean bRet;
  Standard_Integer i;
  Standard_Real adist;
  //
  bRet=Standard_False;
  for(i = 0; i < 2; ++i) {
    IsOnFirstBoundary = (i == 0);
    if (IsOnFirstBoundary) {
      adist = fabs(theParameter - theFirstBoundary);
    }
    else {
      adist = fabs(theParameter - theSecondBoundary);
    }
    if(adist < theResolution) {
      return !bRet;
    }
  }
  return bRet;
}
// ------------------------------------------------------------------------------------------------
// static function: FindPoint
// purpose:
// ------------------------------------------------------------------------------------------------
Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
                           const gp_Pnt2d&     theLastPoint,
                           const Standard_Real theUmin, 
                           const Standard_Real theUmax,
                           const Standard_Real theVmin,
                           const Standard_Real theVmax,
                           gp_Pnt2d&           theNewPoint) {
  
  gp_Vec2d aVec(theFirstPoint, theLastPoint);
  Standard_Integer i = 0, j = 0;

  for(i = 0; i < 4; i++) {
    gp_Vec2d anOtherVec;
    gp_Vec2d anOtherVecNormal;
    gp_Pnt2d aprojpoint = theLastPoint;    

    if((i % 2) == 0) {
      anOtherVec.SetX(0.);
      anOtherVec.SetY(1.);
      anOtherVecNormal.SetX(1.);
      anOtherVecNormal.SetY(0.);

      if(i < 2)
        aprojpoint.SetX(theUmin);
      else
        aprojpoint.SetX(theUmax);
    }
    else {
      anOtherVec.SetX(1.);
      anOtherVec.SetY(0.);
      anOtherVecNormal.SetX(0.);
      anOtherVecNormal.SetY(1.);

      if(i < 2)
        aprojpoint.SetY(theVmin);
      else
        aprojpoint.SetY(theVmax);
    }
    gp_Vec2d anormvec = aVec;
    anormvec.Normalize();
    RefineVector(anormvec);
    Standard_Real adot1 = anormvec.Dot(anOtherVecNormal);

    if(fabs(adot1) < Precision::Angular())
      continue;
    Standard_Real adist = 0.;
    Standard_Boolean bIsOut = Standard_False;

    if((i % 2) == 0) {
      adist = (i < 2) ? fabs(theLastPoint.X() - theUmin) : fabs(theLastPoint.X() - theUmax);
      bIsOut = (i < 2) ? (theLastPoint.X() < theUmin) : (theLastPoint.X() > theUmax);
    }
    else {
      adist = (i < 2) ? fabs(theLastPoint.Y() - theVmin) : fabs(theLastPoint.Y() - theVmax);
      bIsOut = (i < 2) ? (theLastPoint.Y() < theVmin) : (theLastPoint.Y() > theVmax);
    }
    Standard_Real anoffset = adist * anOtherVec.Dot(anormvec) / adot1;

    for(j = 0; j < 2; j++) {
      anoffset = (j == 0) ? anoffset : -anoffset;
      gp_Pnt2d acurpoint(aprojpoint.XY() + (anOtherVec.XY()*anoffset));
      gp_Vec2d acurvec(theLastPoint, acurpoint);
      if ( bIsOut )
        acurvec.Reverse();

      Standard_Real aDotX, anAngleX;
      //
      aDotX = aVec.Dot(acurvec);
      anAngleX = aVec.Angle(acurvec);
      //
      if(aDotX > 0. && fabs(anAngleX) < Precision::PConfusion()) {
        if((i % 2) == 0) {
          if((acurpoint.Y() >= theVmin) &&
             (acurpoint.Y() <= theVmax)) {
            theNewPoint = acurpoint;
            return Standard_True;
          }
        }
        else {
          if((acurpoint.X() >= theUmin) &&
             (acurpoint.X() <= theUmax)) {
            theNewPoint = acurpoint;
            return Standard_True;
          }
        }
      }
    }
  }
  return Standard_False;
}


// ------------------------------------------------------------------------------------------------
// static function: FindPoint
// purpose: Find point on the boundary of radial tangent zone
// ------------------------------------------------------------------------------------------------
Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
                           const gp_Pnt2d&     theLastPoint,
                           const Standard_Real theUmin, 
                           const Standard_Real theUmax,
                           const Standard_Real theVmin,
                           const Standard_Real theVmax,
                           const gp_Pnt2d&     theTanZoneCenter,
                           const Standard_Real theZoneRadius,
                           Handle(GeomAdaptor_HSurface) theGASurface,
                           gp_Pnt2d&           theNewPoint) {
  theNewPoint = theLastPoint;

  if ( !IsInsideTanZone( theLastPoint, theTanZoneCenter, theZoneRadius, theGASurface) )
    return Standard_False;

  Standard_Real aUResolution = theGASurface->UResolution( theZoneRadius );
  Standard_Real aVResolution = theGASurface->VResolution( theZoneRadius );

  Standard_Real aRadius = ( aUResolution < aVResolution ) ? aUResolution : aVResolution;
  gp_Ax22d anAxis( theTanZoneCenter, gp_Dir2d(1, 0), gp_Dir2d(0, 1) );
  gp_Circ2d aCircle( anAxis, aRadius );
  
  //
  gp_Vec2d aDir( theLastPoint.XY() - theFirstPoint.XY() );
  Standard_Real aLength = aDir.Magnitude();
  if ( aLength <= gp::Resolution() )
    return Standard_False;
  gp_Lin2d aLine( theFirstPoint, aDir );

  //
  Handle(Geom2d_Line) aCLine = new Geom2d_Line( aLine );
  Handle(Geom2d_TrimmedCurve) aC1 = new Geom2d_TrimmedCurve( aCLine, 0, aLength );
  Handle(Geom2d_Circle) aC2 = new Geom2d_Circle( aCircle );

  Standard_Real aTol = aRadius * 0.001;
  aTol = ( aTol < Precision::PConfusion() ) ? Precision::PConfusion() : aTol;

  Geom2dAPI_InterCurveCurve anIntersector;
  anIntersector.Init( aC1, aC2, aTol );

  if ( anIntersector.NbPoints() == 0 )
    return Standard_False;

  Standard_Boolean aFound = Standard_False;
  Standard_Real aMinDist = aLength * aLength;
  Standard_Integer i = 0;
  for ( i = 1; i <= anIntersector.NbPoints(); i++ ) {
    gp_Pnt2d aPInt = anIntersector.Point( i );
    if ( aPInt.SquareDistance( theFirstPoint ) < aMinDist ) {
      if ( ( aPInt.X() >= theUmin ) && ( aPInt.X() <= theUmax ) &&
           ( aPInt.Y() >= theVmin ) && ( aPInt.Y() <= theVmax ) ) {
        theNewPoint = aPInt;
        aFound = Standard_True;
      }
    }
  }

  return aFound;
}

// ------------------------------------------------------------------------------------------------
// static function: IsInsideTanZone
// purpose: Check if point is inside a radial tangent zone
// ------------------------------------------------------------------------------------------------
Standard_Boolean IsInsideTanZone(const gp_Pnt2d&     thePoint,
                                 const gp_Pnt2d&     theTanZoneCenter,
                                 const Standard_Real theZoneRadius,
                                 Handle(GeomAdaptor_HSurface) theGASurface) {

  Standard_Real aUResolution = theGASurface->UResolution( theZoneRadius );
  Standard_Real aVResolution = theGASurface->VResolution( theZoneRadius );
  Standard_Real aRadiusSQR = ( aUResolution < aVResolution ) ? aUResolution : aVResolution;
  aRadiusSQR *= aRadiusSQR;
  if ( thePoint.SquareDistance( theTanZoneCenter ) <= aRadiusSQR )
    return Standard_True;
  return Standard_False;
}

// ------------------------------------------------------------------------------------------------
// static function: CheckTangentZonesExist
// purpose: Check if tangent zone exists
// ------------------------------------------------------------------------------------------------
Standard_Boolean CheckTangentZonesExist( const Handle(GeomAdaptor_HSurface)& theSurface1,
                                        const Handle(GeomAdaptor_HSurface)&  theSurface2 ) 
{
  if ( ( theSurface1->GetType() != GeomAbs_Torus ) ||
      ( theSurface2->GetType() != GeomAbs_Torus ) )
    return Standard_False;

  gp_Torus aTor1 = theSurface1->Torus();
  gp_Torus aTor2 = theSurface2->Torus();

  if ( aTor1.Location().Distance( aTor2.Location() ) > Precision::Confusion() )
    return Standard_False;

  if ( ( fabs( aTor1.MajorRadius() - aTor2.MajorRadius() ) > Precision::Confusion() ) ||
       ( fabs( aTor1.MinorRadius() - aTor2.MinorRadius() ) > Precision::Confusion() ) )
    return Standard_False;

  if ( ( aTor1.MajorRadius() < aTor1.MinorRadius() ) ||
       ( aTor2.MajorRadius() < aTor2.MinorRadius() ) )
    return Standard_False;
  return Standard_True;
}

// ------------------------------------------------------------------------------------------------
// static function: ComputeTangentZones
// purpose: 
// ------------------------------------------------------------------------------------------------
Standard_Integer ComputeTangentZones( const Handle(GeomAdaptor_HSurface)& theSurface1,
                                     const Handle(GeomAdaptor_HSurface)&  theSurface2,
                                     const TopoDS_Face&                   theFace1,
                                     const TopoDS_Face&                   theFace2,
                                     Handle(TColgp_HArray1OfPnt2d)&       theResultOnS1,
                                     Handle(TColgp_HArray1OfPnt2d)&       theResultOnS2,
                                     Handle(TColStd_HArray1OfReal)&       theResultRadius,
                                     const Handle(IntTools_Context)& aContext)
{
  Standard_Integer aResult = 0;
  if ( !CheckTangentZonesExist( theSurface1, theSurface2 ) )
    return aResult;


  TColgp_SequenceOfPnt2d aSeqResultS1, aSeqResultS2;
  TColStd_SequenceOfReal aSeqResultRad;

  gp_Torus aTor1 = theSurface1->Torus();
  gp_Torus aTor2 = theSurface2->Torus();

  gp_Ax2 anax1( aTor1.Location(), aTor1.Axis().Direction() );
  gp_Ax2 anax2( aTor2.Location(), aTor2.Axis().Direction() );
  Standard_Integer j = 0;

  for ( j = 0; j < 2; j++ ) {
    Standard_Real aCoef = ( j == 0 ) ? -1 : 1;
    Standard_Real aRadius1 = fabs(aTor1.MajorRadius() + aCoef * aTor1.MinorRadius());
    Standard_Real aRadius2 = fabs(aTor2.MajorRadius() + aCoef * aTor2.MinorRadius());

    gp_Circ aCircle1( anax1, aRadius1 );
    gp_Circ aCircle2( anax2, aRadius2 );

    // roughly compute radius of tangent zone for perpendicular case
    Standard_Real aCriteria = Precision::Confusion() * 0.5;

    Standard_Real aT1 = aCriteria;
    Standard_Real aT2 = aCriteria;
    if ( j == 0 ) {
      // internal tangency
      Standard_Real aR = ( aRadius1 > aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      //aT1 = aCriteria * aCriteria + aR * aR - ( aR - aCriteria ) * ( aR - aCriteria );
      aT1 = 2. * aR * aCriteria;
      aT2 = aT1;
    }
    else {
      // external tangency
      Standard_Real aRb = ( aRadius1 > aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      Standard_Real aRm = ( aRadius1 < aTor2.MinorRadius() ) ? aRadius1 : aTor2.MinorRadius();
      Standard_Real aDelta = aRb - aCriteria;
      aDelta *= aDelta;
      aDelta -= aRm * aRm;
      aDelta /= 2. * (aRb - aRm);
      aDelta -= 0.5 * (aRb - aRm);
      
      aT1 = 2. * aRm * (aRm - aDelta);
      aT2 = aT1;
    }
    aCriteria = ( aT1 > aT2) ? aT1 : aT2;
    if ( aCriteria > 0 )
      aCriteria = sqrt( aCriteria );

    if ( aCriteria > 0.5 * aTor1.MinorRadius() ) {
      // too big zone -> drop to minimum
      aCriteria = Precision::Confusion();
    }

    GeomAdaptor_Curve aC1( new Geom_Circle(aCircle1) );
    GeomAdaptor_Curve aC2( new Geom_Circle(aCircle2) );
    Extrema_ExtCC anExtrema(aC1, aC2, 0, 2. * M_PI, 0, 2. * M_PI, 
                            Precision::PConfusion(), Precision::PConfusion());
            
    if ( anExtrema.IsDone() ) {

      Standard_Integer i = 0;
      for ( i = 1; i <= anExtrema.NbExt(); i++ ) {
        if ( anExtrema.SquareDistance(i) > aCriteria * aCriteria )
          continue;

        Extrema_POnCurv P1, P2;
        anExtrema.Points( i, P1, P2 );

        Standard_Boolean bFoundResult = Standard_True;
        gp_Pnt2d pr1, pr2;

        Standard_Integer surfit = 0;
        for ( surfit = 0; surfit < 2; surfit++ ) {
          GeomAPI_ProjectPointOnSurf& aProjector = 
            (surfit == 0) ? aContext->ProjPS(theFace1) : aContext->ProjPS(theFace2);

          gp_Pnt aP3d = (surfit == 0) ? P1.Value() : P2.Value();
          aProjector.Perform(aP3d);

          if(!aProjector.IsDone())
            bFoundResult = Standard_False;
          else {
            if(aProjector.LowerDistance() > aCriteria) {
              bFoundResult = Standard_False;
            }
            else {
              Standard_Real foundU = 0, foundV = 0;
              aProjector.LowerDistanceParameters(foundU, foundV);
              if ( surfit == 0 )
                pr1 = gp_Pnt2d( foundU, foundV );
              else
                pr2 = gp_Pnt2d( foundU, foundV );
            }
          }
        }
        if ( bFoundResult ) {
          aSeqResultS1.Append( pr1 );
          aSeqResultS2.Append( pr2 );
          aSeqResultRad.Append( aCriteria );

          // torus is u and v periodic
          const Standard_Real twoPI = M_PI + M_PI;
          Standard_Real arr1tmp[2] = {pr1.X(), pr1.Y()};
          Standard_Real arr2tmp[2] = {pr2.X(), pr2.Y()};

          // iteration on period bounds
          for ( Standard_Integer k1 = 0; k1 < 2; k1++ ) {
            Standard_Real aBound = ( k1 == 0 ) ? 0 : twoPI;
            Standard_Real aShift = ( k1 == 0 ) ? twoPI : -twoPI;

            // iteration on surfaces
            for ( Standard_Integer k2 = 0; k2 < 2; k2++ ) {
              Standard_Real* arr1 = ( k2 == 0 ) ? arr1tmp : arr2tmp;
              Standard_Real* arr2 = ( k2 != 0 ) ? arr1tmp : arr2tmp;
              TColgp_SequenceOfPnt2d& aSeqS1 = ( k2 == 0 ) ? aSeqResultS1 : aSeqResultS2; 
              TColgp_SequenceOfPnt2d& aSeqS2 = ( k2 != 0 ) ? aSeqResultS1 : aSeqResultS2; 

              if (fabs(arr1[0] - aBound) < Precision::PConfusion()) {
                aSeqS1.Append( gp_Pnt2d( arr1[0] + aShift, arr1[1] ) );
                aSeqS2.Append( gp_Pnt2d( arr2[0], arr2[1] ) );
                aSeqResultRad.Append( aCriteria );
              }
              if (fabs(arr1[1] - aBound) < Precision::PConfusion()) {
                aSeqS1.Append( gp_Pnt2d( arr1[0], arr1[1] + aShift) );
                aSeqS2.Append( gp_Pnt2d( arr2[0], arr2[1] ) );
                aSeqResultRad.Append( aCriteria );
              }
            }
          } //
        }
      }
    }
  }
  aResult = aSeqResultRad.Length();

  if ( aResult > 0 ) {
    theResultOnS1 = new TColgp_HArray1OfPnt2d( 1, aResult );
    theResultOnS2 = new TColgp_HArray1OfPnt2d( 1, aResult );
    theResultRadius = new TColStd_HArray1OfReal( 1, aResult );

    for ( Standard_Integer i = 1 ; i <= aResult; i++ ) {
      theResultOnS1->SetValue( i, aSeqResultS1.Value(i) );
      theResultOnS2->SetValue( i, aSeqResultS2.Value(i) );
      theResultRadius->SetValue( i, aSeqResultRad.Value(i) );
    }
  }
  return aResult;
}

// ------------------------------------------------------------------------------------------------
// static function: AdjustByNeighbour
// purpose:
// ------------------------------------------------------------------------------------------------
gp_Pnt2d AdjustByNeighbour(const gp_Pnt2d&     theaNeighbourPoint,
                           const gp_Pnt2d&     theOriginalPoint,
                           Handle(GeomAdaptor_HSurface) theGASurface) {
  
  gp_Pnt2d ap1 = theaNeighbourPoint;
  gp_Pnt2d ap2 = theOriginalPoint;

  if ( theGASurface->IsUPeriodic() ) {
    Standard_Real aPeriod     = theGASurface->UPeriod();
    gp_Pnt2d aPTest = ap2;
    Standard_Real aSqDistMin = 1.e+100;

    for ( Standard_Integer pIt = -1; pIt <= 1; pIt++) {
      aPTest.SetX( theOriginalPoint.X() + aPeriod * pIt );
      Standard_Real dd = ap1.SquareDistance( aPTest );

      if ( dd < aSqDistMin ) {
        ap2 = aPTest;
        aSqDistMin = dd;
      }
    }
  }
  if ( theGASurface->IsVPeriodic() ) {
    Standard_Real aPeriod     = theGASurface->VPeriod();
    gp_Pnt2d aPTest = ap2;
    Standard_Real aSqDistMin = 1.e+100;

    for ( Standard_Integer pIt = -1; pIt <= 1; pIt++) {
      aPTest.SetY( theOriginalPoint.Y() + aPeriod * pIt );
      Standard_Real dd = ap1.SquareDistance( aPTest );

      if ( dd < aSqDistMin ) {
        ap2 = aPTest;
        aSqDistMin = dd;
      }
    }
  }
  return ap2;
}

// ------------------------------------------------------------------------------------------------
//function: DecompositionOfWLine
// purpose:
// ------------------------------------------------------------------------------------------------
Standard_Boolean DecompositionOfWLine(const Handle(IntPatch_WLine)& theWLine,
                                      const Handle(GeomAdaptor_HSurface)&            theSurface1, 
                                      const Handle(GeomAdaptor_HSurface)&            theSurface2,
                                      const TopoDS_Face&                             theFace1,
                                      const TopoDS_Face&                             theFace2,
                                      const GeomInt_LineConstructor&                 theLConstructor,
                                      const Standard_Boolean                         theAvoidLConstructor,
                                      IntPatch_SequenceOfLine&                       theNewLines,
                                      Standard_Real&                                 theReachedTol3d,
                                      const Handle(IntTools_Context)& aContext) 
{

  Standard_Boolean bRet, bAvoidLineConstructor;
  Standard_Integer aNbPnts, aNbParts;
  //
  bRet=Standard_False;
  aNbPnts=theWLine->NbPnts();
  bAvoidLineConstructor=theAvoidLConstructor;
  //
  if(!aNbPnts){
    return bRet;
  }
  if (!bAvoidLineConstructor) {
    aNbParts=theLConstructor.NbParts();
    if (!aNbParts) {
      return bRet;
    }
  }
  //
  Standard_Boolean bIsPrevPointOnBoundary, bIsPointOnBoundary, bIsCurrentPointOnBoundary;
  Standard_Integer nblines, pit, i, j;
  Standard_Real aTol;
  TColStd_Array1OfListOfInteger anArrayOfLines(1, aNbPnts); 
  TColStd_Array1OfInteger       anArrayOfLineType(1, aNbPnts);
  TColStd_ListOfInteger aListOfPointIndex;
  
  Handle(TColgp_HArray1OfPnt2d) aTanZoneS1;
  Handle(TColgp_HArray1OfPnt2d) aTanZoneS2;
  Handle(TColStd_HArray1OfReal) aTanZoneRadius;
  Standard_Integer aNbZone = ComputeTangentZones( theSurface1, theSurface2, theFace1, theFace2,
                                                 aTanZoneS1, aTanZoneS2, aTanZoneRadius, aContext);
  
  //
  nblines=0;
  aTol=Precision::Confusion();
  aTol=0.5*aTol;
  bIsPrevPointOnBoundary=Standard_False;
  bIsPointOnBoundary=Standard_False;
  //
  // 1. ...
  //
  // Points
  for(pit = 1; pit <= aNbPnts; ++pit) {
    Standard_Boolean bIsOnFirstBoundary, isperiodic;
    Standard_Real aResolution, aPeriod, alowerboundary, aupperboundary, U, V;
    Standard_Real aParameter, anoffset, anAdjustPar;
    Standard_Real umin, umax, vmin, vmax;
    //
    bIsCurrentPointOnBoundary = Standard_False;
    const IntSurf_PntOn2S& aPoint = theWLine->Point(pit);
    //
    // Surface
    for(i = 0; i < 2; ++i) {
      Handle(GeomAdaptor_HSurface) aGASurface = (!i) ? theSurface1 : theSurface2;
      aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
      if(!i) {
        aPoint.ParametersOnS1(U, V);
      }
      else {
        aPoint.ParametersOnS2(U, V);
      }
      // U, V
      for(j = 0; j < 2; j++) {
        isperiodic = (!j) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();
        if(!isperiodic){
          continue;
        }
        //
        if (!j) {
          aResolution=aGASurface->UResolution(aTol);
          aPeriod=aGASurface->UPeriod();
          alowerboundary=umin;
          aupperboundary=umax;
          aParameter=U;
        }
        else {
          aResolution=aGASurface->VResolution(aTol);
          aPeriod=aGASurface->VPeriod();
          alowerboundary=vmin;
          aupperboundary=vmax;
          aParameter=V;
        }
        
        GeomInt::AdjustPeriodic(aParameter, 
                                       alowerboundary, 
                                       aupperboundary, 
                                       aPeriod,
                                       anAdjustPar,
                                       anoffset);
        //
        bIsOnFirstBoundary = Standard_True;// ?
        bIsPointOnBoundary=
          IsPointOnBoundary(anAdjustPar, 
                            alowerboundary, 
                            aupperboundary,
                            aResolution, 
                            bIsOnFirstBoundary);
        //
        if(bIsPointOnBoundary) {
          bIsCurrentPointOnBoundary = Standard_True;
          break;
        }
        else {
          // check if a point belong to a tangent zone. Begin
          Standard_Integer zIt = 0;
          for ( zIt = 1; zIt <= aNbZone; zIt++ ) {
            gp_Pnt2d aPZone = (i == 0) ? aTanZoneS1->Value(zIt) : aTanZoneS2->Value(zIt);
            Standard_Real aZoneRadius = aTanZoneRadius->Value(zIt);

            if ( IsInsideTanZone(gp_Pnt2d( U, V ), aPZone, aZoneRadius, aGASurface ) ) {
              // set boundary flag to split the curve by a tangent zone
              bIsPointOnBoundary = Standard_True;
              bIsCurrentPointOnBoundary = Standard_True;
              if ( theReachedTol3d < aZoneRadius ) {
                theReachedTol3d = aZoneRadius;
              }
              break;
            }
          }
        }
      }//for(j = 0; j < 2; j++) {

      if(bIsCurrentPointOnBoundary){
        break;
      }
    }//for(i = 0; i < 2; ++i) {
    //
    if((bIsCurrentPointOnBoundary != bIsPrevPointOnBoundary)) {
      if(!aListOfPointIndex.IsEmpty()) {
        nblines++;
        anArrayOfLines.SetValue(nblines, aListOfPointIndex);
        anArrayOfLineType.SetValue(nblines, bIsPrevPointOnBoundary);
        aListOfPointIndex.Clear();
      }
      bIsPrevPointOnBoundary = bIsCurrentPointOnBoundary;
    }
    aListOfPointIndex.Append(pit);
  } //for(pit = 1; pit <= aNbPnts; ++pit) {
  //
  if(!aListOfPointIndex.IsEmpty()) {
    nblines++;
    anArrayOfLines.SetValue(nblines, aListOfPointIndex);
    anArrayOfLineType.SetValue(nblines, bIsPrevPointOnBoundary);
    aListOfPointIndex.Clear();
  }
  //
  if(nblines<=1) {
    return bRet; //Standard_False;
  }
  //
  // 
  // 2. Correct wlines.begin
  TColStd_Array1OfListOfInteger anArrayOfLineEnds(1, nblines);
  Handle(IntSurf_LineOn2S) aSeqOfPntOn2S = new IntSurf_LineOn2S();
  //
  for(i = 1; i <= nblines; i++) {
    if(anArrayOfLineType.Value(i) != 0) {
      continue;
    }
    const TColStd_ListOfInteger& aListOfIndex = anArrayOfLines.Value(i);
    TColStd_ListOfInteger aListOfFLIndex;

    for(j = 0; j < 2; j++) {
      Standard_Integer aneighbourindex = (j == 0) ? (i - 1) : (i + 1);

      if((aneighbourindex < 1) || (aneighbourindex > nblines))
        continue;

      if(anArrayOfLineType.Value(aneighbourindex) == 0)
        continue;
      const TColStd_ListOfInteger& aNeighbour = anArrayOfLines.Value(aneighbourindex);
      Standard_Integer anIndex = (j == 0) ? aNeighbour.Last() : aNeighbour.First();
      const IntSurf_PntOn2S& aPoint = theWLine->Point(anIndex);

      IntSurf_PntOn2S aNewP = aPoint;
      if(aListOfIndex.Extent() < 2) {
        aSeqOfPntOn2S->Add(aNewP);
        aListOfFLIndex.Append(aSeqOfPntOn2S->NbPoints());
        continue;
      }
      //
      Standard_Integer iFirst = aListOfIndex.First();
      Standard_Integer iLast  = aListOfIndex.Last();
      //
      for(Standard_Integer surfit = 0; surfit < 2; surfit++) {

        Handle(GeomAdaptor_HSurface) aGASurface = (surfit == 0) ? theSurface1 : theSurface2;
        Standard_Real umin=0., umax=0., vmin=0., vmax=0.;
        aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
        Standard_Real U=0., V=0.;

        if(surfit == 0)
          aNewP.ParametersOnS1(U, V);
        else
          aNewP.ParametersOnS2(U, V);
        Standard_Integer nbboundaries = 0;

        Standard_Boolean bIsNearBoundary = Standard_False;
        Standard_Integer aZoneIndex = 0;
        Standard_Integer bIsUBoundary = Standard_False; // use if nbboundaries == 1
        Standard_Integer bIsFirstBoundary = Standard_False; // use if nbboundaries == 1
        

        for(Standard_Integer parit = 0; parit < 2; parit++) {
          Standard_Boolean isperiodic = (parit == 0) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

          Standard_Real aResolution = (parit == 0) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
          Standard_Real alowerboundary = (parit == 0) ? umin : vmin;
          Standard_Real aupperboundary = (parit == 0) ? umax : vmax;

          Standard_Real aParameter = (parit == 0) ? U : V;
          Standard_Boolean bIsOnFirstBoundary = Standard_True;
  
          if(!isperiodic) {
            bIsPointOnBoundary=
              IsPointOnBoundary(aParameter, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary);
            if(bIsPointOnBoundary) {
              bIsUBoundary = (parit == 0);
              bIsFirstBoundary = bIsOnFirstBoundary;
              nbboundaries++;
            }
          }
          else {
            Standard_Real aPeriod     = (parit == 0) ? aGASurface->UPeriod() : aGASurface->VPeriod();
            Standard_Real anoffset, anAdjustPar;
            GeomInt::AdjustPeriodic(aParameter, alowerboundary, aupperboundary,
                                           aPeriod, anAdjustPar, anoffset);

            bIsPointOnBoundary=
              IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary);
            if(bIsPointOnBoundary) {
              bIsUBoundary = (parit == 0);
              bIsFirstBoundary = bIsOnFirstBoundary;
              nbboundaries++;
            }
            else {
            //check neighbourhood of boundary
            Standard_Real anEpsilon = aResolution * 100.;
            Standard_Real aPart = ( aupperboundary - alowerboundary ) * 0.1;
            anEpsilon = ( anEpsilon > aPart ) ? aPart : anEpsilon;
            
              bIsNearBoundary = IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, 
                                                  anEpsilon, bIsOnFirstBoundary);

            }
          }
        }

        // check if a point belong to a tangent zone. Begin
        for ( Standard_Integer zIt = 1; zIt <= aNbZone; zIt++ ) {
          gp_Pnt2d aPZone = (surfit == 0) ? aTanZoneS1->Value(zIt) : aTanZoneS2->Value(zIt);
          Standard_Real aZoneRadius = aTanZoneRadius->Value(zIt);

          Standard_Integer aneighbourpointindex1 = (j == 0) ? iFirst : iLast;
          const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
          Standard_Real nU1, nV1;
            
          if(surfit == 0)
            aNeighbourPoint.ParametersOnS1(nU1, nV1);
          else
            aNeighbourPoint.ParametersOnS2(nU1, nV1);
          gp_Pnt2d ap1(nU1, nV1);
          gp_Pnt2d ap2 = AdjustByNeighbour( ap1, gp_Pnt2d( U, V ), aGASurface );


          if ( IsInsideTanZone( ap2, aPZone, aZoneRadius, aGASurface ) ) {
            aZoneIndex = zIt;
            bIsNearBoundary = Standard_True;
            if ( theReachedTol3d < aZoneRadius ) {
              theReachedTol3d = aZoneRadius;
            }
          }
        }
        // check if a point belong to a tangent zone. End
        Standard_Boolean bComputeLineEnd = Standard_False;

        if(nbboundaries == 2) {
          //xf
          bComputeLineEnd = Standard_True;
          //xt
        }
        else if(nbboundaries == 1) {
          Standard_Boolean isperiodic = (bIsUBoundary) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

          if(isperiodic) {
            Standard_Real alowerboundary = (bIsUBoundary) ? umin : vmin;
            Standard_Real aupperboundary = (bIsUBoundary) ? umax : vmax;
            Standard_Real aPeriod     = (bIsUBoundary) ? aGASurface->UPeriod() : aGASurface->VPeriod();
            Standard_Real aParameter = (bIsUBoundary) ? U : V;
            Standard_Real anoffset, anAdjustPar;
            GeomInt::AdjustPeriodic(aParameter, alowerboundary, aupperboundary, 
                                           aPeriod, anAdjustPar, anoffset);

            Standard_Real adist = (bIsFirstBoundary) ? fabs(anAdjustPar - alowerboundary) : fabs(anAdjustPar - aupperboundary);
            Standard_Real anotherPar = (bIsFirstBoundary) ? (aupperboundary - adist) : (alowerboundary + adist);
            anotherPar += anoffset;
            Standard_Integer aneighbourpointindex = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
            const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex);
            Standard_Real nU1, nV1;

            if(surfit == 0)
              aNeighbourPoint.ParametersOnS1(nU1, nV1);
            else
              aNeighbourPoint.ParametersOnS2(nU1, nV1);
            
            Standard_Real adist1 = (bIsUBoundary) ? fabs(nU1 - U) : fabs(nV1 - V);
            Standard_Real adist2 = (bIsUBoundary) ? fabs(nU1 - anotherPar) : fabs(nV1 - anotherPar);
            bComputeLineEnd = Standard_True;
            Standard_Boolean bCheckAngle1 = Standard_False;
            Standard_Boolean bCheckAngle2 = Standard_False;
            gp_Vec2d aNewVec;
            Standard_Real anewU = (bIsUBoundary) ? anotherPar : U;
            Standard_Real anewV = (bIsUBoundary) ? V : anotherPar;

            if(((adist1 - adist2) > Precision::PConfusion()) && 
               (adist2 < (aPeriod / 4.))) {
              bCheckAngle1 = Standard_True;
              aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(anewU, anewV));

              if(aNewVec.SquareMagnitude() < gp::Resolution()) {
                aNewP.SetValue((surfit == 0), anewU, anewV);
                bCheckAngle1 = Standard_False;
              }
            }
            else if(adist1 < (aPeriod / 4.)) {
              bCheckAngle2 = Standard_True;
              aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(U, V));

              if(aNewVec.SquareMagnitude() < gp::Resolution()) {
                bCheckAngle2 = Standard_False;
              }
            }

            if(bCheckAngle1 || bCheckAngle2) {
              // assume there are at least two points in line (see "if" above)
              Standard_Integer anindexother = aneighbourpointindex;

              while((anindexother <= iLast) && (anindexother >= iFirst)) {
                anindexother = (j == 0) ? (anindexother + 1) : (anindexother - 1);
                const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(anindexother);
                Standard_Real nU2, nV2;
                
                if(surfit == 0)
                  aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
                else
                  aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
                gp_Vec2d aVecOld(gp_Pnt2d(nU2, nV2), gp_Pnt2d(nU1, nV1));

                if(aVecOld.SquareMagnitude() <= gp::Resolution()) {
                  continue;
                }
                else {
                  Standard_Real anAngle = aNewVec.Angle(aVecOld);

                  if((fabs(anAngle) < (M_PI * 0.25)) && (aNewVec.Dot(aVecOld) > 0.)) {

                    if(bCheckAngle1) {
                      Standard_Real U1, U2, V1, V2;
                      IntSurf_PntOn2S atmppoint = aNewP;
                      atmppoint.SetValue((surfit == 0), anewU, anewV);
                      atmppoint.Parameters(U1, V1, U2, V2);
                      gp_Pnt P1 = theSurface1->Value(U1, V1);
                      gp_Pnt P2 = theSurface2->Value(U2, V2);
                      gp_Pnt P0 = aPoint.Value();

                      if(P0.IsEqual(P1, aTol) &&
                         P0.IsEqual(P2, aTol) &&
                         P1.IsEqual(P2, aTol)) {
                        bComputeLineEnd = Standard_False;
                        aNewP.SetValue((surfit == 0), anewU, anewV);
                      }
                    }

                    if(bCheckAngle2) {
                      bComputeLineEnd = Standard_False;
                    }
                  }
                  break;
                }
              } // end while(anindexother...)
            }
          }
        }
        else if ( bIsNearBoundary ) {
          bComputeLineEnd = Standard_True;
        }

        if(bComputeLineEnd) {

          gp_Pnt2d anewpoint;
          Standard_Boolean found = Standard_False;

          if ( bIsNearBoundary ) {
            // re-compute point near natural boundary or near tangent zone
            Standard_Real u1, v1, u2, v2;
            aNewP.Parameters( u1, v1, u2, v2 );
            if(surfit == 0)
              anewpoint = gp_Pnt2d( u1, v1 );
            else
              anewpoint = gp_Pnt2d( u2, v2 );
            
            Standard_Integer aneighbourpointindex1 = (j == 0) ? iFirst : iLast;
            const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
            Standard_Real nU1, nV1;
            
            if(surfit == 0)
              aNeighbourPoint.ParametersOnS1(nU1, nV1);
            else
              aNeighbourPoint.ParametersOnS2(nU1, nV1);
            gp_Pnt2d ap1(nU1, nV1);
            gp_Pnt2d ap2;


            if ( aZoneIndex ) {
              // exclude point from a tangent zone
              anewpoint = AdjustByNeighbour( ap1, anewpoint, aGASurface );
              gp_Pnt2d aPZone = (surfit == 0) ? aTanZoneS1->Value(aZoneIndex) : aTanZoneS2->Value(aZoneIndex);
              Standard_Real aZoneRadius = aTanZoneRadius->Value(aZoneIndex);

              if ( FindPoint(ap1, anewpoint, umin, umax, vmin, vmax, 
                             aPZone, aZoneRadius, aGASurface, ap2) ) {
                anewpoint = ap2;
                found = Standard_True;
              }
            }
            else if ( aGASurface->IsUPeriodic() || aGASurface->IsVPeriodic() ) {
              // re-compute point near boundary if shifted on a period
              ap2 = AdjustByNeighbour( ap1, anewpoint, aGASurface );

              if ( ( ap2.X() < umin ) || ( ap2.X() > umax ) ||
                  ( ap2.Y() < vmin ) || ( ap2.Y() > vmax ) ) {
                found = FindPoint(ap1, ap2, umin, umax, vmin, vmax, anewpoint);
              }
              else {
                anewpoint = ap2;
                aNewP.SetValue( (surfit == 0), anewpoint.X(), anewpoint.Y() );
              }
            }
          }
          else {

            Standard_Integer aneighbourpointindex1 = (j == 0) ? iFirst : iLast;
            const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
            Standard_Real nU1, nV1;

            if(surfit == 0)
              aNeighbourPoint.ParametersOnS1(nU1, nV1);
            else
              aNeighbourPoint.ParametersOnS2(nU1, nV1);
            gp_Pnt2d ap1(nU1, nV1);
            gp_Pnt2d ap2(nU1, nV1);
            Standard_Integer aneighbourpointindex2 = aneighbourpointindex1;

            while((aneighbourpointindex2 <= iLast) && (aneighbourpointindex2 >= iFirst)) {
              aneighbourpointindex2 = (j == 0) ? (aneighbourpointindex2 + 1) : (aneighbourpointindex2 - 1);
              const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(aneighbourpointindex2);
              Standard_Real nU2, nV2;

              if(surfit == 0)
                aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
              else
                aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
              ap2.SetX(nU2);
              ap2.SetY(nV2);

              if(ap1.SquareDistance(ap2) > gp::Resolution()) {
                break;
              }
            }  
            found = FindPoint(ap2, ap1, umin, umax, vmin, vmax, anewpoint);
          }

          if(found) {
            // check point
            Standard_Real aCriteria = BRep_Tool::Tolerance(theFace1) + BRep_Tool::Tolerance(theFace2);
            GeomAPI_ProjectPointOnSurf& aProjector = 
              (surfit == 0) ? aContext->ProjPS(theFace2) : aContext->ProjPS(theFace1);
            Handle(GeomAdaptor_HSurface) aSurface = (surfit == 0) ? theSurface1 : theSurface2;

            Handle(GeomAdaptor_HSurface) aSurfaceOther = (surfit == 0) ? theSurface2 : theSurface1;

            gp_Pnt aP3d = aSurface->Value(anewpoint.X(), anewpoint.Y());
            aProjector.Perform(aP3d);

            if(aProjector.IsDone()) {
              if(aProjector.LowerDistance() < aCriteria) {
                Standard_Real foundU = U, foundV = V;
                aProjector.LowerDistanceParameters(foundU, foundV);

                //Correction of projected coordinates. Begin
                //Note, it may be shifted on a period
                Standard_Integer aneindex1 = (j == 0) ? iFirst : iLast;
                const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneindex1);
                Standard_Real nUn, nVn;
                
                if(surfit == 0)
                  aNeighbourPoint.ParametersOnS2(nUn, nVn);
                else
                  aNeighbourPoint.ParametersOnS1(nUn, nVn);
                gp_Pnt2d aNeighbour2d(nUn, nVn);
                gp_Pnt2d anAdjustedPoint = AdjustByNeighbour( aNeighbour2d, gp_Pnt2d(foundU, foundV), aSurfaceOther );
                foundU = anAdjustedPoint.X();
                foundV = anAdjustedPoint.Y();

                if ( ( anAdjustedPoint.X() < umin ) && ( anAdjustedPoint.X() > umax ) &&
                    ( anAdjustedPoint.Y() < vmin ) && ( anAdjustedPoint.Y() > vmax ) ) {
                  // attempt to roughly re-compute point
                  foundU = ( foundU < umin ) ? umin : foundU;
                  foundU = ( foundU > umax ) ? umax : foundU;
                  foundV = ( foundV < vmin ) ? vmin : foundV;
                  foundV = ( foundV > vmax ) ? vmax : foundV;

                  GeomAPI_ProjectPointOnSurf& aProjector2 = 
                    (surfit == 0) ? aContext->ProjPS(theFace1) : aContext->ProjPS(theFace2);

                  aP3d = aSurfaceOther->Value(foundU, foundV);
                  aProjector2.Perform(aP3d);
                  
                  if(aProjector2.IsDone()) {
                    if(aProjector2.LowerDistance() < aCriteria) {
                      Standard_Real foundU2 = anewpoint.X(), foundV2 = anewpoint.Y();
                      aProjector2.LowerDistanceParameters(foundU2, foundV2);
                      anewpoint.SetX(foundU2);
                      anewpoint.SetY(foundV2);
                    }
                  }
                }
                //Correction of projected coordinates. End

                if(surfit == 0)
                  aNewP.SetValue(aP3d, anewpoint.X(), anewpoint.Y(), foundU, foundV);
                else
                  aNewP.SetValue(aP3d, foundU, foundV, anewpoint.X(), anewpoint.Y());
              }
            }
          }
        }
      }
      aSeqOfPntOn2S->Add(aNewP);
      aListOfFLIndex.Append(aSeqOfPntOn2S->NbPoints());
    }
    anArrayOfLineEnds.SetValue(i, aListOfFLIndex);
  }
  // Correct wlines.end

  // Split wlines.begin
  Standard_Integer nbiter;
  //
  nbiter=1;
  if (!bAvoidLineConstructor) {
    nbiter=theLConstructor.NbParts();
  }
  //
  for(j = 1; j <= nbiter; ++j) {
    Standard_Real fprm, lprm;
    Standard_Integer ifprm, ilprm;
    //
    if(bAvoidLineConstructor) {
      ifprm = 1;
      ilprm = theWLine->NbPnts();
    }
    else {
      theLConstructor.Part(j, fprm, lprm);
      ifprm = (Standard_Integer)fprm;
      ilprm = (Standard_Integer)lprm;
    }

    Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
    //
    for(i = 1; i <= nblines; i++) {
      if(anArrayOfLineType.Value(i) != 0) {
        continue;
      }
      const TColStd_ListOfInteger& aListOfIndex = anArrayOfLines.Value(i);
      const TColStd_ListOfInteger& aListOfFLIndex = anArrayOfLineEnds.Value(i);
      Standard_Boolean bhasfirstpoint = (aListOfFLIndex.Extent() == 2);
      Standard_Boolean bhaslastpoint = (aListOfFLIndex.Extent() == 2);

      if(!bhasfirstpoint && !aListOfFLIndex.IsEmpty()) {
        bhasfirstpoint = (i != 1);
      }

      if(!bhaslastpoint && !aListOfFLIndex.IsEmpty()) {
        bhaslastpoint = (i != nblines);
      }
      
      Standard_Integer iFirst = aListOfIndex.First();
      Standard_Integer iLast  = aListOfIndex.Last();
      Standard_Boolean bIsFirstInside = ((ifprm >= iFirst) && (ifprm <= iLast));
      Standard_Boolean bIsLastInside =  ((ilprm >= iFirst) && (ilprm <= iLast));

      if(!bIsFirstInside && !bIsLastInside) {
        if((ifprm < iFirst) && (ilprm > iLast)) {
          // append whole line, and boundaries if neccesary
          if(bhasfirstpoint) {
            pit = aListOfFLIndex.First();
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(pit);
            aLineOn2S->Add(aP);
          }
          TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

          for(; anIt.More(); anIt.Next()) {
            pit = anIt.Value();
            const IntSurf_PntOn2S& aP = theWLine->Point(pit);
            aLineOn2S->Add(aP);
          }

          if(bhaslastpoint) {
            pit = aListOfFLIndex.Last();
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(pit);
            aLineOn2S->Add(aP);
          }

          // check end of split line (end is almost always)
          Standard_Integer aneighbour = i + 1;
          Standard_Boolean bIsEndOfLine = Standard_True;

          if(aneighbour <= nblines) {
            const TColStd_ListOfInteger& aListOfNeighbourIndex = anArrayOfLines.Value(aneighbour);

            if((anArrayOfLineType.Value(aneighbour) != 0) &&
               (aListOfNeighbourIndex.IsEmpty())) {
              bIsEndOfLine = Standard_False;
            }
          }

          if(bIsEndOfLine) {
            if(aLineOn2S->NbPoints() > 1) {
              Handle(IntPatch_WLine) aNewWLine = 
                new IntPatch_WLine(aLineOn2S, Standard_False);
              theNewLines.Append(aNewWLine);
            }
            aLineOn2S = new IntSurf_LineOn2S();
          }
        }
        continue;
      }
      // end if(!bIsFirstInside && !bIsLastInside)

      if(bIsFirstInside && bIsLastInside) {
        // append inside points between ifprm and ilprm
        TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

        for(; anIt.More(); anIt.Next()) {
          pit = anIt.Value();
          if((pit < ifprm) || (pit > ilprm))
            continue;
          const IntSurf_PntOn2S& aP = theWLine->Point(pit);
          aLineOn2S->Add(aP);
        }
      }
      else {

        if(bIsFirstInside) {
          // append points from ifprm to last point + boundary point
          TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

          for(; anIt.More(); anIt.Next()) {
            pit = anIt.Value();
            if(pit < ifprm)
              continue;
            const IntSurf_PntOn2S& aP = theWLine->Point(pit);
            aLineOn2S->Add(aP);
          }

          if(bhaslastpoint) {
            pit = aListOfFLIndex.Last();
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(pit);
            aLineOn2S->Add(aP);
          }
          // check end of split line (end is almost always)
          Standard_Integer aneighbour = i + 1;
          Standard_Boolean bIsEndOfLine = Standard_True;

          if(aneighbour <= nblines) {
            const TColStd_ListOfInteger& aListOfNeighbourIndex = anArrayOfLines.Value(aneighbour);

            if((anArrayOfLineType.Value(aneighbour) != 0) &&
               (aListOfNeighbourIndex.IsEmpty())) {
              bIsEndOfLine = Standard_False;
            }
          }

          if(bIsEndOfLine) {
            if(aLineOn2S->NbPoints() > 1) {
              Handle(IntPatch_WLine) aNewWLine = 
                new IntPatch_WLine(aLineOn2S, Standard_False);
              theNewLines.Append(aNewWLine);
            }
            aLineOn2S = new IntSurf_LineOn2S();
          }
        }
        // end if(bIsFirstInside)

        if(bIsLastInside) {
          // append points from first boundary point to ilprm
          if(bhasfirstpoint) {
            pit = aListOfFLIndex.First();
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(pit);
            aLineOn2S->Add(aP);
          }
          TColStd_ListIteratorOfListOfInteger anIt(aListOfIndex);

          for(; anIt.More(); anIt.Next()) {
            pit = anIt.Value();
            if(pit > ilprm)
              continue;
            const IntSurf_PntOn2S& aP = theWLine->Point(pit);
            aLineOn2S->Add(aP);
          }
        }
        //end if(bIsLastInside)
      }
    }

    if(aLineOn2S->NbPoints() > 1) {
      Handle(IntPatch_WLine) aNewWLine = 
        new IntPatch_WLine(aLineOn2S, Standard_False);
      theNewLines.Append(aNewWLine);
    }
  }
  // Split wlines.end

  return Standard_True;
}

// ------------------------------------------------------------------------------------------------
// static function: ParameterOutOfBoundary
// purpose:         Computes a new parameter for given curve. The corresponding 2d points 
//                  does not lay on any boundary of given faces
// ------------------------------------------------------------------------------------------------
Standard_Boolean ParameterOutOfBoundary(const Standard_Real       theParameter, 
                                        const Handle(Geom_Curve)& theCurve, 
                                        const TopoDS_Face&        theFace1, 
                                        const TopoDS_Face&        theFace2,
                                        const Standard_Real       theOtherParameter,
                                        const Standard_Boolean    bIncreasePar,
                                        Standard_Real&            theNewParameter,
                                        const Handle(IntTools_Context)& aContext)
{
  Standard_Boolean bIsComputed = Standard_False;
  theNewParameter = theParameter;

  Standard_Real acurpar = theParameter;
  TopAbs_State aState = TopAbs_ON;
  Standard_Integer iter = 0;
  Standard_Real asumtol = BRep_Tool::Tolerance(theFace1) + BRep_Tool::Tolerance(theFace2);
  Standard_Real adelta = asumtol * 0.1;
  adelta = (adelta < Precision::Confusion()) ? Precision::Confusion() : adelta;
  Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(theFace1);
  Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(theFace2);

  Standard_Real u1, u2, v1, v2;

  GeomAPI_ProjectPointOnSurf aPrj1;
  aSurf1->Bounds(u1, u2, v1, v2);
  aPrj1.Init(aSurf1, u1, u2, v1, v2);

  GeomAPI_ProjectPointOnSurf aPrj2;
  aSurf2->Bounds(u1, u2, v1, v2);
  aPrj2.Init(aSurf2, u1, u2, v1, v2);

  while(aState == TopAbs_ON) {
    if(bIncreasePar)
      acurpar += adelta;
    else
      acurpar -= adelta;
    gp_Pnt aPCurrent = theCurve->Value(acurpar);
    aPrj1.Perform(aPCurrent);
    Standard_Real U=0., V=0.;

    if(aPrj1.IsDone()) {
      aPrj1.LowerDistanceParameters(U, V);
      aState = aContext->StatePointFace(theFace1, gp_Pnt2d(U, V));
    }

    if(aState != TopAbs_ON) {
      aPrj2.Perform(aPCurrent);
                
      if(aPrj2.IsDone()) {
        aPrj2.LowerDistanceParameters(U, V);
        aState = aContext->StatePointFace(theFace2, gp_Pnt2d(U, V));
      }
    }

    if(iter > 11) {
      break;
    }
    iter++;
  }

  if(iter <= 11) {
    theNewParameter = acurpar;
    bIsComputed = Standard_True;

    if(bIncreasePar) {
      if(acurpar >= theOtherParameter)
        theNewParameter = theOtherParameter;
    }
    else {
      if(acurpar <= theOtherParameter)
        theNewParameter = theOtherParameter;
    }
  }
  return bIsComputed;
}

//=======================================================================
//function : IsCurveValid
//purpose  : 
//=======================================================================
Standard_Boolean IsCurveValid(Handle(Geom2d_Curve)& thePCurve)
{
  if(thePCurve.IsNull())
    return Standard_False;

  Standard_Real tolint = 1.e-10;
  Geom2dAdaptor_Curve PCA;
  IntRes2d_Domain PCD;
  Geom2dInt_GInter PCI;

  Standard_Real pf = 0., pl = 0.;
  gp_Pnt2d pntf, pntl;

  if(!thePCurve->IsClosed() && !thePCurve->IsPeriodic()) {
    pf = thePCurve->FirstParameter();
    pl = thePCurve->LastParameter();
    pntf = thePCurve->Value(pf);
    pntl = thePCurve->Value(pl);
    PCA.Load(thePCurve);
    if(!PCA.IsPeriodic()) {
      if(PCA.FirstParameter() > pf) pf = PCA.FirstParameter();
      if(PCA.LastParameter()  < pl) pl = PCA.LastParameter();
    }
    PCD.SetValues(pntf,pf,tolint,pntl,pl,tolint);
    PCI.Perform(PCA,PCD,tolint,tolint);
    if(PCI.IsDone())
      if(PCI.NbPoints() > 0) {
        return Standard_False;
      }
  }

  return Standard_True;
}

//=======================================================================
//static function : ApproxWithPCurves
//purpose  : for bug 20964 only
//=======================================================================
Standard_Boolean ApproxWithPCurves(const gp_Cylinder& theCyl, 
                                   const gp_Sphere& theSph)
{
  Standard_Boolean bRes = Standard_True;
  Standard_Real R1 = theCyl.Radius(), R2 = theSph.Radius();
  //
  {
    Standard_Real aD2, aRc2, aEps;
    gp_Pnt aApexSph;
    //
    aEps=1.E-7;
    aRc2=R1*R1;
    //
    const gp_Ax3& aAx3Sph=theSph.Position();
    const gp_Pnt& aLocSph=aAx3Sph.Location();
    const gp_Dir& aDirSph=aAx3Sph.Direction();
    //
    const gp_Ax1& aAx1Cyl=theCyl.Axis();
    gp_Lin aLinCyl(aAx1Cyl);
    //
    aApexSph.SetXYZ(aLocSph.XYZ()+R2*aDirSph.XYZ());
    aD2=aLinCyl.SquareDistance(aApexSph);
    if (fabs(aD2-aRc2)<aEps) {
      return !bRes;
    }
    //
    aApexSph.SetXYZ(aLocSph.XYZ()-R2*aDirSph.XYZ());
    aD2=aLinCyl.SquareDistance(aApexSph);
    if (fabs(aD2-aRc2)<aEps) {
      return !bRes;
    }
  }
  //
    
  if(R1 < 2.*R2) {
    return bRes;
  }
  gp_Lin anCylAx(theCyl.Axis());

  Standard_Real aDist = anCylAx.Distance(theSph.Location());
  Standard_Real aDRel = Abs(aDist - R1)/R2;

  if(aDRel > .2) return bRes;

  Standard_Real par = ElCLib::Parameter(anCylAx, theSph.Location());
  gp_Pnt aP = ElCLib::Value(par, anCylAx);
  gp_Vec aV(aP, theSph.Location());

  Standard_Real dd = aV.Dot(theSph.Position().XDirection());

  if(aDist < R1 && dd > 0.) return Standard_False;
  if(aDist > R1 && dd < 0.) return Standard_False;

  
  return bRes;
}
//=======================================================================
//function : PerformPlanes
//purpose  : 
//=======================================================================
void  PerformPlanes(const Handle(GeomAdaptor_HSurface)& theS1, 
                    const Handle(GeomAdaptor_HSurface)& theS2, 
                    const Standard_Real TolAng, 
                    const Standard_Real TolTang, 
                    const Standard_Boolean theApprox1,
                    const Standard_Boolean theApprox2,
                    IntTools_SequenceOfCurves& theSeqOfCurve, 
                    Standard_Boolean& theTangentFaces)
{

  gp_Pln aPln1 = theS1->Surface().Plane();
  gp_Pln aPln2 = theS2->Surface().Plane();

  IntAna_QuadQuadGeo aPlnInter(aPln1, aPln2, TolAng, TolTang);

  if(!aPlnInter.IsDone()) {
    theTangentFaces = Standard_False;
    return;
  }

  IntAna_ResultType aResType = aPlnInter.TypeInter();

  if(aResType == IntAna_Same) {
    theTangentFaces = Standard_True;
    return;
  }

  theTangentFaces = Standard_False;

  if(aResType == IntAna_Empty) {
    return;
  }

  gp_Lin aLin = aPlnInter.Line(1);

  ProjLib_Plane aProj;

  aProj.Init(aPln1);
  aProj.Project(aLin);
  gp_Lin2d aLin2d1 = aProj.Line();
  //
  aProj.Init(aPln2);
  aProj.Project(aLin);
  gp_Lin2d aLin2d2 = aProj.Line();
  //
  //classify line2d1 relatively first plane
  Standard_Real P11, P12;
  Standard_Boolean IsCrossed = ClassifyLin2d(theS1, aLin2d1, TolTang, P11, P12);
  if(!IsCrossed) return;
  //classify line2d2 relatively second plane
  Standard_Real P21, P22;
  IsCrossed = ClassifyLin2d(theS2, aLin2d2, TolTang, P21, P22);
  if(!IsCrossed) return;

  //Analysis of parametric intervals: must have common part

  if(P21 >= P12) return;
  if(P22 <= P11) return;

  Standard_Real pmin, pmax;
  pmin = Max(P11, P21);
  pmax = Min(P12, P22);

  if(pmax - pmin <= TolTang) return;

  Handle(Geom_Line) aGLin = new Geom_Line(aLin);

  IntTools_Curve aCurve;
  Handle(Geom_TrimmedCurve) aGTLin = new Geom_TrimmedCurve(aGLin, pmin, pmax);

  aCurve.SetCurve(aGTLin);

  if(theApprox1) { 
    Handle(Geom2d_Line) C2d = new Geom2d_Line(aLin2d1);
    aCurve.SetFirstCurve2d(new Geom2d_TrimmedCurve(C2d, pmin, pmax));
  }
  else { 
    Handle(Geom2d_Curve) H1;
    aCurve.SetFirstCurve2d(H1);
  }
  if(theApprox2) { 
    Handle(Geom2d_Line) C2d = new Geom2d_Line(aLin2d2);
    aCurve.SetSecondCurve2d(new Geom2d_TrimmedCurve(C2d, pmin, pmax));
  }
  else { 
    Handle(Geom2d_Curve) H1;
    aCurve.SetFirstCurve2d(H1);
  }

  theSeqOfCurve.Append(aCurve);
 
}

//=======================================================================
//function : ClassifyLin2d
//purpose  : 
//=======================================================================
static inline Standard_Boolean INTER(const Standard_Real d1, 
                                     const Standard_Real d2, 
                                     const Standard_Real tol) 
{
  return (d1 > tol && d2 < -tol) || 
         (d1 < -tol && d2 > tol) ||
         ((d1 <= tol && d1 >= -tol) && (d2 > tol || d2 < -tol)) || 
         ((d2 <= tol && d2 >= -tol) && (d1 > tol || d1 < -tol));
}
static inline Standard_Boolean COINC(const Standard_Real d1, 
                                     const Standard_Real d2, 
                                     const Standard_Real tol) 
{
  return (d1 <= tol && d1 >= -tol) && (d2 <= tol && d2 >= -tol);
}
Standard_Boolean ClassifyLin2d(const Handle(GeomAdaptor_HSurface)& theS, 
                               const gp_Lin2d& theLin2d, 
                               const Standard_Real theTol,
                               Standard_Real& theP1, 
                               Standard_Real& theP2)

{
  Standard_Real xmin, xmax, ymin, ymax, d1, d2, A, B, C;
  Standard_Real par[2];
  Standard_Integer nbi = 0;

  xmin = theS->Surface().FirstUParameter();
  xmax = theS->Surface().LastUParameter();
  ymin = theS->Surface().FirstVParameter();
  ymax = theS->Surface().LastVParameter();

  theLin2d.Coefficients(A, B, C);

  //xmin, ymin <-> xmin, ymax
  d1 = A*xmin + B*ymin + C;
  d2 = A*xmin + B*ymax + C;

  if(INTER(d1, d2, theTol)) {
    //Intersection with boundary
    Standard_Real y = -(C + A*xmin)/B;
    par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, y));
    nbi++;
  }
  else if (COINC(d1, d2, theTol)) {
    //Coincidence with boundary
    par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymin));
    par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymax));
    nbi = 2;
  }

  if(nbi == 2) {

    if(fabs(par[0]-par[1]) > theTol) {
      theP1 = Min(par[0], par[1]);
      theP2 = Max(par[0], par[1]);
      return Standard_True;
    }
    else return Standard_False;

  }

  //xmin, ymax <-> xmax, ymax
  d1 = d2;
  d2 = A*xmax + B*ymax + C;

  if(d1 > theTol || d1 < -theTol) {//to avoid checking of
                                   //coincidence with the same point
    if(INTER(d1, d2, theTol)) {
      Standard_Real x = -(C + B*ymax)/A;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(x, ymax));
      nbi++;
    }
    else if (COINC(d1, d2, theTol)) {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymax));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymax));
      nbi = 2;
    }
  }

  if(nbi == 2) {

    if(fabs(par[0]-par[1]) > theTol) {
      theP1 = Min(par[0], par[1]);
      theP2 = Max(par[0], par[1]);
      return Standard_True;
    }
    else return Standard_False;

  }

  //xmax, ymax <-> xmax, ymin
  d1 = d2;
  d2 = A*xmax + B*ymin + C;

  if(d1 > theTol || d1 < -theTol) {
    if(INTER(d1, d2, theTol)) {
      Standard_Real y = -(C + A*xmax)/B;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, y));
      nbi++;
    }
    else if (COINC(d1, d2, theTol)) {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymax));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymin));
      nbi = 2;
    }
  }

  if(nbi == 2) {
    if(fabs(par[0]-par[1]) > theTol) {
      theP1 = Min(par[0], par[1]);
      theP2 = Max(par[0], par[1]);
      return Standard_True;
    }
    else return Standard_False;
  }

  //xmax, ymin <-> xmin, ymin 
  d1 = d2;
  d2 = A*xmin + B*ymin + C;

  if(d1 > theTol || d1 < -theTol) {
    if(INTER(d1, d2, theTol)) {
      Standard_Real x = -(C + B*ymin)/A;
      par[nbi] = ElCLib::Parameter(theLin2d, gp_Pnt2d(x, ymin));
      nbi++;
    }
    else if (COINC(d1, d2, theTol)) {
      par[0] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmax, ymin));
      par[1] = ElCLib::Parameter(theLin2d, gp_Pnt2d(xmin, ymin));
      nbi = 2;
    }
  }

  if(nbi == 2) {
    if(fabs(par[0]-par[1]) > theTol) {
      theP1 = Min(par[0], par[1]);
      theP2 = Max(par[0], par[1]);
      return Standard_True;
    }
    else return Standard_False;
  }

  return Standard_False;

}
//
//=======================================================================
//function : ApproxParameters
//purpose  : 
//=======================================================================
void ApproxParameters(const Handle(GeomAdaptor_HSurface)& aHS1,
                      const Handle(GeomAdaptor_HSurface)& aHS2,
                      Standard_Integer& iDegMin,
                      Standard_Integer& iDegMax,
                      Standard_Integer& iNbIter)

{
  GeomAbs_SurfaceType aTS1, aTS2;
  
  //
  iNbIter=0;
  iDegMin=4;
  iDegMax=8;
  //
  aTS1=aHS1->Surface().GetType();
  aTS2=aHS2->Surface().GetType();
  //
  // Cylinder/Torus
  if ((aTS1==GeomAbs_Cylinder && aTS2==GeomAbs_Torus) ||
      (aTS2==GeomAbs_Cylinder && aTS1==GeomAbs_Torus)) {
    Standard_Real aRC, aRT, dR, aPC;
    gp_Cylinder aCylinder;
    gp_Torus aTorus;
    //
    aPC=Precision::Confusion();
    //
    aCylinder=(aTS1==GeomAbs_Cylinder)? aHS1->Surface().Cylinder() : aHS2->Surface().Cylinder();
    aTorus=(aTS1==GeomAbs_Torus)? aHS1->Surface().Torus() : aHS2->Surface().Torus();
    //
    aRC=aCylinder.Radius();
    aRT=aTorus.MinorRadius();
    dR=aRC-aRT;
    if (dR<0.) {
      dR=-dR;
    }
    //
    if (dR<aPC) {
      iDegMax=6;
    }
  }
  if (aTS1==GeomAbs_Cylinder && aTS2==GeomAbs_Cylinder) {
    iNbIter=1; 
  }
}
//=======================================================================
//function : Tolerances
//purpose  : 
//=======================================================================
void Tolerances(const Handle(GeomAdaptor_HSurface)& aHS1,
                const Handle(GeomAdaptor_HSurface)& aHS2,
                Standard_Real& aTolTang)
{
  GeomAbs_SurfaceType aTS1, aTS2;
  //
  aTS1=aHS1->Surface().GetType();
  aTS2=aHS2->Surface().GetType();
  //
  // Cylinder/Torus
  if ((aTS1==GeomAbs_Cylinder && aTS2==GeomAbs_Torus) ||
      (aTS2==GeomAbs_Cylinder && aTS1==GeomAbs_Torus)) {
    Standard_Real aRC, aRT, dR, aPC;
    gp_Cylinder aCylinder;
    gp_Torus aTorus;
    //
    aPC=Precision::Confusion();
    //
    aCylinder=(aTS1==GeomAbs_Cylinder)? aHS1->Surface().Cylinder() : aHS2->Surface().Cylinder();
    aTorus=(aTS1==GeomAbs_Torus)? aHS1->Surface().Torus() : aHS2->Surface().Torus();
    //
    aRC=aCylinder.Radius();
    aRT=aTorus.MinorRadius();
    dR=aRC-aRT;
    if (dR<0.) {
      dR=-dR;
    }
    //
    if (dR<aPC) {
      aTolTang=0.1*aTolTang;
    }
  }
}
//=======================================================================
//function : SortTypes
//purpose  : 
//=======================================================================
Standard_Boolean SortTypes(const GeomAbs_SurfaceType aType1,
                           const GeomAbs_SurfaceType aType2)
{
  Standard_Boolean bRet;
  Standard_Integer aI1, aI2;
  //
  bRet=Standard_False;
  //
  aI1=IndexType(aType1);
  aI2=IndexType(aType2);
  if (aI1<aI2){
    bRet=!bRet;
  }
  return bRet;
}
//=======================================================================
//function : IndexType
//purpose  : 
//=======================================================================
Standard_Integer IndexType(const GeomAbs_SurfaceType aType)
{
  Standard_Integer aIndex;
  //
  aIndex=11;
  //
  if (aType==GeomAbs_Plane) {
    aIndex=0;
  }
  else if (aType==GeomAbs_Cylinder) {
    aIndex=1;
  } 
  else if (aType==GeomAbs_Cone) {
    aIndex=2;
  } 
  else if (aType==GeomAbs_Sphere) {
    aIndex=3;
  } 
  else if (aType==GeomAbs_Torus) {
    aIndex=4;
  } 
  else if (aType==GeomAbs_BezierSurface) {
    aIndex=5;
  } 
  else if (aType==GeomAbs_BSplineSurface) {
    aIndex=6;
  } 
  else if (aType==GeomAbs_SurfaceOfRevolution) {
    aIndex=7;
  } 
  else if (aType==GeomAbs_SurfaceOfExtrusion) {
    aIndex=8;
  } 
  else if (aType==GeomAbs_OffsetSurface) {
    aIndex=9;
  } 
  else if (aType==GeomAbs_OtherSurface) {
    aIndex=10;
  } 
  return aIndex;
}
#ifdef OCCT_DEBUG_DUMPWLINE
//=======================================================================
//function : DumpWLine
//purpose  : 
//=======================================================================
void DumpWLine(const Handle(IntPatch_WLine)& aWLine)
{
  Standard_Integer i, aNbPnts; 
  Standard_Real aX, aY, aZ, aU1, aV1, aU2, aV2;
  //
  printf(" *WLine\n");
  aNbPnts=aWLine->NbPnts();
  for (i=1; i<=aNbPnts; ++i) {
    const IntSurf_PntOn2S aPntOn2S=aWLine->Point(i);
    const gp_Pnt& aP3D=aPntOn2S.Value();
    aP3D.Coord(aX, aY, aZ);
    aPntOn2S.Parameters(aU1, aV1, aU2, aV2);
    //
    printf("point p_%d %lf %lf %lf\n", i, aX, aY, aZ);
    //printf("point p_%d %20.15lf %20.15lf %20.15lf %20.15lf %20.15lf %20.15lf %20.15lf\n",
        //   i, aX, aY, aZ, aU1, aV1, aU2, aV2);
  }
}
#endif
//=======================================================================
//function : RefineVector
//purpose  : 
//=======================================================================
void RefineVector(gp_Vec2d& aV2D)
{
  Standard_Integer k,m;
  Standard_Real aC[2], aEps, aR1, aR2, aNum;
  //
  aEps=RealEpsilon();
  aR1=1.-aEps;
  aR2=1.+aEps;
  //
  aV2D.Coord(aC[0], aC[1]);
  //
  for (k=0; k<2; ++k) {
    m=(k+1)%2;
    aNum=fabs(aC[k]);
    if (aNum>aR1 && aNum<aR2) {
      if (aC[k]<0.) {
        aC[k]=-1.;
      }          
      else {
        aC[k]=1.;
      }
      aC[m]=0.;
      break;
    }
  }
  aV2D.SetCoord(aC[0], aC[1]);
}

//=======================================================================
// Function : FindMaxDistance
// purpose : 
//=======================================================================
Standard_Real FindMaxDistance(const Handle(Geom_Curve)& theCurve,
                              const Standard_Real theFirst,
                              const Standard_Real theLast,
                              const TopoDS_Face& theFace,
                              const Handle(IntTools_Context)& theContext)
{
  Standard_Integer aNbS;
  Standard_Real aT1, aT2, aDt, aD, aDMax, anEps;
  //
  aNbS = 11;
  aDt = (theLast - theFirst) / aNbS;
  aDMax = 0.;
  anEps = 1.e-4 * aDt;
  //
  GeomAPI_ProjectPointOnSurf& aProjPS = theContext->ProjPS(theFace);
  aT2 = theFirst;
  for (;;) {
    aT1 = aT2;
    aT2 += aDt;
    //
    if (aT2 > theLast) {
      break;
    }
    //
    aD = FindMaxDistance(theCurve, aT1, aT2, aProjPS, anEps);
    if (aD > aDMax) {
      aDMax = aD;
    }
  }
  //
  return aDMax;
}

//=======================================================================
// Function : FindMaxDistance
// purpose : 
//=======================================================================
Standard_Real FindMaxDistance(const Handle(Geom_Curve)& theC,
                              const Standard_Real theFirst,
                              const Standard_Real theLast,
                              GeomAPI_ProjectPointOnSurf& theProjPS,
                              const Standard_Real theEps)
{
  Standard_Real aA, aB, aCf, aX, aX1, aX2, aF1, aF2, aF;
  //
  aCf = 0.61803398874989484820458683436564;//(sqrt(5.)-1)/2.;
  aA = theFirst;
  aB = theLast;
  //
  aX1 = aB - aCf * (aB - aA);
  aF1 = MaxDistance(theC, aX1, theProjPS);
  aX2 = aA + aCf * (aB - aA);
  aF2 = MaxDistance(theC, aX2, theProjPS);
  //
  for (;;) {
    if ((aB - aA) < theEps) {
      break;
    }
    //
    if (aF1 > aF2) {
      aB = aX2;
      aX2 = aX1;
      aF2 = aF1;
      aX1 = aB - aCf * (aB - aA); 
      aF1 = MaxDistance(theC, aX1, theProjPS);
    }
    else {
      aA = aX1;
      aX1 = aX2;
      aF1 = aF2;
      aX2 = aA + aCf * (aB - aA);
      aF2 = MaxDistance(theC, aX2, theProjPS);
    }
  }
  //
  aX = 0.5 * (aA + aB);
  aF = MaxDistance(theC, aX, theProjPS);
  //
  if (aF1 > aF) {
    aF = aF1;
  }
  //
  if (aF2 > aF) {
    aF = aF2;
  }
  //
  return aF;
}

//=======================================================================
// Function : MaxDistance
// purpose : 
//=======================================================================
Standard_Real MaxDistance(const Handle(Geom_Curve)& theC,
                          const Standard_Real aT,
                          GeomAPI_ProjectPointOnSurf& theProjPS)
{
  Standard_Real aD;
  gp_Pnt aP;
  //
  theC->D0(aT, aP);
  theProjPS.Perform(aP);
  aD = theProjPS.NbPoints() ? theProjPS.LowerDistance() : 0.;
  //
  return aD;
}

//=======================================================================
//function : CheckPCurve
//purpose  : Checks if points of the pcurve are out of the face bounds.
//=======================================================================
  Standard_Boolean CheckPCurve(const Handle(Geom2d_Curve)& aPC, 
                               const TopoDS_Face& aFace) 
{
  const Standard_Integer NPoints = 23;
  Standard_Integer i;
  Standard_Real umin,umax,vmin,vmax;

  BRepTools::UVBounds(aFace, umin, umax, vmin, vmax);
  Standard_Real tolU = Max ((umax-umin)*0.01, Precision::Confusion());
  Standard_Real tolV = Max ((vmax-vmin)*0.01, Precision::Confusion());
  Standard_Real fp = aPC->FirstParameter();
  Standard_Real lp = aPC->LastParameter();


  // adjust domain for periodic surfaces
  TopLoc_Location aLoc;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace, aLoc);
  if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    aSurf = (Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf))->BasisSurface();
  }
  gp_Pnt2d pnt = aPC->Value((fp+lp)/2);
  Standard_Real u,v;
  pnt.Coord(u,v);
  //
  if (aSurf->IsUPeriodic()) {
    Standard_Real aPer = aSurf->UPeriod();
    Standard_Integer nshift = (Standard_Integer) ((u-umin)/aPer);
    if (u < umin+aPer*nshift) nshift--;
    umin += aPer*nshift;
    umax += aPer*nshift;
  }
  if (aSurf->IsVPeriodic()) {
    Standard_Real aPer = aSurf->VPeriod();
    Standard_Integer nshift = (Standard_Integer) ((v-vmin)/aPer);
    if (v < vmin+aPer*nshift) nshift--;
    vmin += aPer*nshift;
    vmax += aPer*nshift;
  }
  //
  //--------------------------------------------------------
  Standard_Boolean bRet;
  Standard_Integer j, aNbIntervals;
  Standard_Real aT, dT;
  gp_Pnt2d aP2D; 
  //
  Geom2dAdaptor_Curve aGAC(aPC);
  aNbIntervals=aGAC.NbIntervals(GeomAbs_CN);
  //
  TColStd_Array1OfReal aTI(1, aNbIntervals+1);
  aGAC.Intervals(aTI,GeomAbs_CN);
  //
  bRet=Standard_False;
  //
  aT=aGAC.FirstParameter();
  for (j=1; j<=aNbIntervals; ++j) {
    dT=(aTI(j+1)-aTI(j))/NPoints;
    //
    for (i=1; i<NPoints; i++) {
      aT=aT+dT;
      aGAC.D0(aT, aP2D);
      aP2D.Coord(u,v);
    if (umin-u > tolU || u-umax > tolU ||
          vmin-v > tolV || v-vmax > tolV) {
        return bRet;
  }
}
  }
  return !bRet;
}
//=======================================================================
//function : CorrectPlaneBoundaries
//purpose  : 
//=======================================================================
 void CorrectPlaneBoundaries(Standard_Real& aUmin,
                             Standard_Real& aUmax, 
                             Standard_Real& aVmin, 
                             Standard_Real& aVmax) 
{
  if (!(Precision::IsInfinite(aUmin) ||
        Precision::IsInfinite(aUmax))) { 
    Standard_Real dU;
    //
    dU=0.1*(aUmax-aUmin);
    aUmin=aUmin-dU;
    aUmax=aUmax+dU;
  }
  if (!(Precision::IsInfinite(aVmin) ||
        Precision::IsInfinite(aVmax))) { 
    Standard_Real dV;
    //
    dV=0.1*(aVmax-aVmin);
    aVmin=aVmin-dV;
    aVmax=aVmax+dV;
  }
}
