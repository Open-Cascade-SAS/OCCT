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

#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomInt_IntSS.hxx>
#include <GeomInt_WLApprox.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_TopolTool.hxx>
#include <IntTools_WLineTool.hxx>
#include <ProjLib_Plane.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Elips.hxx>

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
  Standard_Boolean ParameterOutOfBoundary(const Standard_Real       theParameter, 
                                          const Handle(Geom_Curve)& theCurve, 
                                          const TopoDS_Face&        theFace1, 
                                          const TopoDS_Face&        theFace2,
                                          const Standard_Real       theOtherParameter,
                                          const Standard_Boolean    bIncreasePar,
                                          const Standard_Real       theTol,
                                          Standard_Real&            theNewParameter,
                                          const Handle(IntTools_Context)& );

static 
  Standard_Boolean IsCurveValid(const Handle(Geom2d_Curve)& thePCurve);

static
  Standard_Boolean  ApproxWithPCurves(const gp_Cylinder& theCyl, 
                                      const gp_Sphere& theSph);

static void  PerformPlanes(const Handle(GeomAdaptor_HSurface)& theS1,
                           const Handle(GeomAdaptor_HSurface)& theS2,
                           const Standard_Real TolF1,
                           const Standard_Real TolF2,
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
                               const TopoDS_Face& aFace,
                               const Handle(IntTools_Context)& theCtx);

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
  myTolF1 = 0.;
  myTolF2 = 0.;
  myTol = 0.;
  myFuzzyValue = Precision::Confusion();
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
//function : SetFuzzyValue
//purpose  : 
//=======================================================================
void IntTools_FaceFace::SetFuzzyValue(const Standard_Real theFuzz)
{
  myFuzzyValue = Max(theFuzz, Precision::Confusion());
}
//=======================================================================
//function : FuzzyValue
//purpose  : 
//=======================================================================
Standard_Real IntTools_FaceFace::FuzzyValue() const
{
  return myFuzzyValue;
}

//=======================================================================
//function : SetList
//purpose  : 
//=======================================================================
void IntTools_FaceFace::SetList(IntSurf_ListOfPntOn2S& aListOfPnts)
{
  myListOfPnts = aListOfPnts;  
}


static Standard_Boolean isTreatAnalityc(const BRepAdaptor_Surface& theBAS1,
                                        const BRepAdaptor_Surface& theBAS2,
                                        const Standard_Real theTol)
{
  const Standard_Real Tolang = 1.e-8;
  Standard_Real aHigh = 0.0;

  const GeomAbs_SurfaceType aType1=theBAS1.GetType();
  const GeomAbs_SurfaceType aType2=theBAS2.GetType();
  
  gp_Pln aS1;
  gp_Cylinder aS2;
  if(aType1 == GeomAbs_Plane)
  {
    aS1=theBAS1.Plane();
  }
  else if(aType2 == GeomAbs_Plane)
  {
    aS1=theBAS2.Plane();
  }
  else
  {
    return Standard_True;
  }

  if(aType1 == GeomAbs_Cylinder)
  {
    aS2=theBAS1.Cylinder();
    const Standard_Real VMin = theBAS1.FirstVParameter();
    const Standard_Real VMax = theBAS1.LastVParameter();

    if( Precision::IsNegativeInfinite(VMin) ||
        Precision::IsPositiveInfinite(VMax))
          return Standard_True;
    else
      aHigh = VMax - VMin;
  }
  else if(aType2 == GeomAbs_Cylinder)
  {
    aS2=theBAS2.Cylinder();

    const Standard_Real VMin = theBAS2.FirstVParameter();
    const Standard_Real VMax = theBAS2.LastVParameter();

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
  inter.Perform(aS1,aS2,Tolang,theTol, aHigh);
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
  Standard_Boolean RestrictLine = Standard_False;
  
  if (myContext.IsNull()) {
    myContext=new IntTools_Context;
  }

  mySeqOfCurve.Clear();
  myIsDone = Standard_False;
  myNbrestr=0;//?

  myFace1=aF1;
  myFace2=aF2;

  const BRepAdaptor_Surface& aBAS1 = myContext->SurfaceAdaptor(myFace1);
  const BRepAdaptor_Surface& aBAS2 = myContext->SurfaceAdaptor(myFace2);
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

  Standard_Real aFuzz = myFuzzyValue / 2.;
  myTolF1 = BRep_Tool::Tolerance(myFace1) + aFuzz;
  myTolF2 = BRep_Tool::Tolerance(myFace2) + aFuzz;
  myTol = myTolF1 + myTolF2;

  Standard_Real TolArc = myTol;
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
    myContext->UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    //
    myContext->UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
    //
    Standard_Real TolAng = 1.e-8;
    //
    PerformPlanes(myHS1, myHS2,
                  myTolF1, myTolF2, TolAng, TolTang,
                  myApprox1, myApprox2,
                  mySeqOfCurve, myTangentFaces);
    //
    myIsDone = Standard_True;
    //
    if (!myTangentFaces) {
      const Standard_Integer NbLinPP = mySeqOfCurve.Length();
      if (NbLinPP && bReverse) {
        Handle(Geom2d_Curve) aC2D1, aC2D2;
        const Standard_Integer aNbLin = mySeqOfCurve.Length();
        for (Standard_Integer i = 1; i <= aNbLin; ++i) {
          IntTools_Curve& aIC = mySeqOfCurve(i);
          aC2D1 = aIC.FirstCurve2d();
          aC2D2 = aIC.SecondCurve2d();
          aIC.SetFirstCurve2d(aC2D2);
          aIC.SetSecondCurve2d(aC2D1);
        }
      }
    }
    return;
  }//if(aType1==GeomAbs_Plane && aType2==GeomAbs_Plane){

  if ((aType1==GeomAbs_Plane) && isFace2Quad)
  {
    Standard_Real umin, umax, vmin, vmax;
    // F1
    myContext->UVBounds(myFace1, umin, umax, vmin, vmax); 
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    // F2
    myContext->UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace2, myTol * 2., umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
  }
  else if ((aType2==GeomAbs_Plane) && isFace1Quad)
  {
    Standard_Real umin, umax, vmin, vmax;
    //F1
    myContext->UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace1, myTol * 2., umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    // F2
    myContext->UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectPlaneBoundaries(umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
  }
  else
  {
    Standard_Real umin, umax, vmin, vmax;
    myContext->UVBounds(myFace1, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace1, myTol * 2., umin, umax, vmin, vmax);
    myHS1->ChangeSurface().Load(S1, umin, umax, vmin, vmax);
    myContext->UVBounds(myFace2, umin, umax, vmin, vmax);
    CorrectSurfaceBoundaries(myFace2, myTol * 2., umin, umax, vmin, vmax);
    myHS2->ChangeSurface().Load(S2, umin, umax, vmin, vmax);
  }

  const Handle(IntTools_TopolTool) dom1 = new IntTools_TopolTool(myHS1);
  const Handle(IntTools_TopolTool) dom2 = new IntTools_TopolTool(myHS2);

  myLConstruct.Load(dom1, dom2, myHS1, myHS2);
  

  Tolerances(myHS1, myHS2, TolTang);

  {
    const Standard_Real UVMaxStep = 0.001;
    const Standard_Real Deflection = 0.1;
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

#ifdef INTTOOLS_FACEFACE_DEBUG
    if(!myListOfPnts.IsEmpty()) {
      char aBuff[10000];
      const IntSurf_PntOn2S& aPt = myListOfPnts.First();
      Standard_Real u1, v1, u2, v2;
      aPt.Parameters(u1, v1, u2, v2);

      Sprintf(aBuff,"bopcurves <face1 face2> -2d");
      IntSurf_ListIteratorOfListOfPntOn2S IterLOP1(myListOfPnts);
      for(;IterLOP1.More(); IterLOP1.Next())
      {
        const IntSurf_PntOn2S& aPt = IterLOP1.Value();
        Standard_Real u1, v1, u2, v2;
        aPt.Parameters(u1, v1, u2, v2);

        Sprintf(aBuff, "%s -p %+10.20f %+10.20f %+10.20f %+10.20f", aBuff, u1, v1, u2, v2);
      }

      cout << aBuff << endl;
    }
#endif

  const Standard_Boolean isGeomInt = isTreatAnalityc(aBAS1, aBAS2, myTol);
  if (aF1.IsSame(aF2))
    myIntersector.Perform(myHS1, dom1, TolArc, TolTang);
  else
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
      MakeCurve(i, dom1, dom2, TolArc);
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
//function :ComputeTolReached3d 
//purpose  : 
//=======================================================================
void IntTools_FaceFace::ComputeTolReached3d()
{
  Standard_Integer i, j, aNbLin = mySeqOfCurve.Length();
  if (!aNbLin) {
    return;
  }
  //
  // Minimal tangential tolerance for the curve
  Standard_Real aTolFMax = Max(myTolF1, myTolF2);
  //
  const Handle(Geom_Surface)& aS1 = myHS1->ChangeSurface().Surface();
  const Handle(Geom_Surface)& aS2 = myHS2->ChangeSurface().Surface();
  //
  for (i = 1; i <= aNbLin; ++i)
  {
    IntTools_Curve& aIC = mySeqOfCurve(i);
    const Handle(Geom_Curve)& aC3D = aIC.Curve();
    if (aC3D.IsNull())
    {
      continue;
    }
    //
    Standard_Real aTolC = aIC.Tolerance();
    Standard_Real aFirst = aC3D->FirstParameter();
    Standard_Real aLast  = aC3D->LastParameter();
    //
    // Compute the tolerance for the curve
    const Handle(Geom2d_Curve)& aC2D1 = aIC.FirstCurve2d();
    const Handle(Geom2d_Curve)& aC2D2 = aIC.SecondCurve2d();
    //
    for (j = 0; j < 2; ++j)
    {
      const Handle(Geom2d_Curve)& aC2D = !j ? aC2D1 : aC2D2;
      if (!aC2D.IsNull())
      {
        // Look for the maximal deviation between 3D and 2D curves
        Standard_Real aD, aT;
        const Handle(Geom_Surface)& aS = !j ? aS1 : aS2;
        if (IntTools_Tools::ComputeTolerance
            (aC3D, aC2D, aS, aFirst, aLast, aD, aT))
        {
          if (aD > aTolC)
          {
            aTolC = aD;
          }
        }
      }
      else
      {
        // Look for the maximal deviation between 3D curve and surface
        const TopoDS_Face& aF = !j ? myFace1 : myFace2;
        Standard_Real aD = FindMaxDistance(aC3D, aFirst, aLast, aF, myContext);
        if (aD > aTolC)
        {
          aTolC = aD;
        }
      }
    }
    // Set the valid tolerance for the curve
    aIC.SetTolerance(aTolC);
    //
    // Set the tangential tolerance for the curve.
    // Note, that, currently, computation of the tangential tolerance is
    // implemented for the Plane/Plane case only.
    // Thus, set the tangential tolerance equal to maximal tolerance of faces.
    if (aIC.TangentialTolerance() < aTolFMax) {
      aIC.SetTangentialTolerance(aTolFMax);
    }
  }
}

//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================
void IntTools_FaceFace::MakeCurve(const Standard_Integer Index,
                                  const Handle(Adaptor3d_TopolTool)& dom1,
                                  const Handle(Adaptor3d_TopolTool)& dom2,
                                  const Standard_Real theToler) 
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
    Handle(IntPatch_WLine) aWLine (Handle(IntPatch_WLine)::DownCast(L));
    if(aWLine.IsNull()) {
      return;
    }
    L = aWLine;

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

  if(typl == IntPatch_Restriction)
    bAvoidLineConstructor = Standard_True;

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
          Standard_Real aTolC = IntTools_Tools::CurveTolerance(aCT3D, myTol);
          aCurve.SetTolerance(aTolC);
        }
        //
        aCurve.SetCurve(new Geom_TrimmedCurve(newc, fprm, lprm));
        if(myApprox1) { 
          Handle (Geom2d_Curve) C2d;
          GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc,
                myHS1->ChangeSurface().Surface(), newc, C2d);
          aCurve.SetFirstCurve2d(new Geom2d_TrimmedCurve(C2d, fprm, lprm));
        }
        //
        if(myApprox2) { 
          Handle (Geom2d_Curve) C2d;
          GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc,
                    myHS2->ChangeSurface().Surface(), newc, C2d);
          aCurve.SetSecondCurve2d(new Geom2d_TrimmedCurve(C2d, fprm, lprm));
        }
        //
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

          if(P1.Distance(P2) > myTol) {
            Standard_Real anewpar = fprm;

            if(ParameterOutOfBoundary(fprm, newc, myFace1, myFace2, 
                                      lprm, Standard_False, myTol, anewpar, myContext)) {
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

          if(P1.Distance(P2) > myTol) {
            Standard_Real anewpar = lprm;

            if(ParameterOutOfBoundary(lprm, newc, myFace1, myFace2, 
                                      fprm, Standard_True, myTol, anewpar, myContext)) {
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
            aCurve.SetFirstCurve2d(C2d);
          }

          if(myApprox2) { 
            Handle (Geom2d_Curve) C2d;
            GeomInt_IntSS::BuildPCurves(fprm,lprm,Tolpc,
                    myHS2->ChangeSurface().Surface(),newc,C2d);
            aCurve.SetSecondCurve2d(C2d);
          }
        }
        //
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
              aCurve.SetFirstCurve2d(C2d);
            }

            if(myApprox2) { 
              Handle (Geom2d_Curve) C2d;
              GeomInt_IntSS::BuildPCurves(fprm,lprm,Tolpc,
                    myHS2->ChangeSurface().Surface(),newc,C2d);
              aCurve.SetSecondCurve2d(C2d);
            }
            //
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
                aCurve.SetFirstCurve2d(C2d);
              }

              if(myApprox2) { 
                Handle (Geom2d_Curve) C2d;
                GeomInt_IntSS::BuildPCurves(fprm, lprm, Tolpc,
                        myHS2->ChangeSurface().Surface(), newc, C2d);
                aCurve.SetSecondCurve2d(C2d);
              }
            }//  end of if (typl == IntPatch_Circle || typl == IntPatch_Ellipse)
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
    
  case IntPatch_Analytic:
    //This case was processed earlier (in IntPatch_Intersection)
    break;

  case IntPatch_Walking:{
    Handle(IntPatch_WLine) WL = 
      Handle(IntPatch_WLine)::DownCast(L);

#ifdef INTTOOLS_FACEFACE_DEBUG
    WL->Dump(0);
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
          H1 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
        }
        //
        if(myApprox2) {
          H2 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
        }
        //           
        aBSp=GeomInt_IntSS::MakeBSpline(WL, ifprm, ilprm);
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
        theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30, Standard_False, aParType);
        rejectSurface = Standard_True;
      }
      else { 
        if(reApprox && !rejectSurface)
          theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30, Standard_False, aParType);
        else {
          Standard_Integer iDegMax, iDegMin, iNbIter;
          //
          ApproxParameters(myHS1, myHS2, iDegMin, iDegMax, iNbIter);
          theapp3d.SetParameters(myTolApprox, tol2d, iDegMin, iDegMax,
                                                  iNbIter, 30, Standard_True, aParType);
        }
      }
      //
      Standard_Real aReachedTol = Precision::Confusion();
      bIsDecomposited = IntTools_WLineTool::
        DecompositionOfWLine(WL,
                             myHS1, 
                             myHS2, 
                             myFace1, 
                             myFace2, 
                             myLConstruct, 
                             bAvoidLineConstructor, 
                             myTol,
                             aSeqOfL, 
                             aReachedTol,
                             myContext);
      //
      aNbSeqOfL=aSeqOfL.Length();
      //
      Standard_Real aTolC = 0.;
      if (bIsDecomposited) {
        nbiter=aNbSeqOfL;
        aTolC = aReachedTol;
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
             
              theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30,
                                                                Standard_True, aParType);
              
              Standard_Boolean bUseSurfaces;
              bUseSurfaces = IntTools_WLineTool::NotUseSurfacesForApprox(myFace1, myFace2, WL, ifprm,  ilprm);
              if (bUseSurfaces) {
                // ######
                rejectSurface = Standard_True;
                // ######
                theapp3d.SetParameters(myTolApprox, tol2d, 4, 8, 0, 30,
                                                                Standard_False, aParType);
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
          Handle(Geom_Curve) aBSp=GeomInt_IntSS::MakeBSpline(WL,ifprm, ilprm);
          //
          if(myApprox1) {
            H1 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
          }
          //
          if(myApprox2) {
            H2 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
          }
          //           
          IntTools_Curve aIC(aBSp, H1, H2);
          mySeqOfCurve.Append(aIC);
        }
        else {
          if (typs1 == GeomAbs_Plane || typs2 == GeomAbs_Plane) {
            //
            if (typs1 == GeomAbs_Torus || typs2 == GeomAbs_Torus) {
              if (aTolC < 1.e-6) {
                aTolC = 1.e-6;
              }
            }
          }
          //
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
              //
              aCurve.SetTolerance(aTolC);
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
              aCurve.SetTolerance(aTolC);

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
                bPCurvesOk = CheckPCurve(BS1, myFace2, myContext);
                aCurve.SetSecondCurve2d(BS1);
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
                bPCurvesOk = bPCurvesOk && CheckPCurve(BS2, myFace1, myContext);
                aCurve.SetFirstCurve2d(BS2);
              }
              //
              //if points of the pcurves are out of the faces bounds
              //create 3d and 2d curves without approximation
              if (!bPCurvesOk) {
                Handle(Geom2d_BSplineCurve) H1, H2;
                bPCurvesOk = Standard_True;
                //           
                Handle(Geom_Curve) aBSp=GeomInt_IntSS::MakeBSpline(WL,ifprm, ilprm);
                
                if(myApprox1) {
                  H1 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, Standard_True);
                  bPCurvesOk = CheckPCurve(H1, myFace1, myContext);
                }
                
                if(myApprox2) {
                  H2 = GeomInt_IntSS::MakeBSpline2d(WL, ifprm, ilprm, Standard_False);
                  bPCurvesOk = bPCurvesOk && CheckPCurve(H2, myFace2, myContext);
                }
                //
                //if pcurves created without approximation are out of the 
                //faces bounds, use approximated 3d and 2d curves
                if (bPCurvesOk) {
                  IntTools_Curve aIC(aBSp, H1, H2, aTolC);
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
              aCurve.SetTolerance(aTolC);
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
                    bIsValid1=CheckPCurve(BS1, myFace1, myContext);
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
                    bIsValid2=CheckPCurve(BS2, myFace2, myContext);
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

#ifdef INTTOOLS_FACEFACE_DEBUG
    RL->Dump(0);
#endif

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

      //Intersect with true boundaries. After that, enlarge bounding-boxes in order to 
      //correct definition, if point on curve is inscribed in the box.
      aBox1.Enlarge(theToler);
      aBox2.Enlarge(theToler);

      const Standard_Integer aNbIntersSolutionsm1 = anArrayOfParameters.Length() - 1;

      //Trim RLine found.
      for(Standard_Integer anInd = 0; anInd < aNbIntersSolutionsm1; anInd++)
      {
        Standard_Real &aParF = anArrayOfParameters(anInd),
                      &aParL = anArrayOfParameters(anInd+1);

        if((aParL - aParF) <= Precision::PConfusion())
        {
          //In order to more precise extending to the boundaries of source curves.
          if(anInd < aNbIntersSolutionsm1-1)
            aParL = aParF;

          continue;
        }

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
    throw Standard_ConstructionError("GeomInt_IntSS::MakeCurve");
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
    throw Standard_ConstructionError("GeomInt_IntSS::MakeCurve");
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
                                        const Standard_Real       theTol,
                                        Standard_Real&            theNewParameter,
                                        const Handle(IntTools_Context)& aContext)
{
  Standard_Boolean bIsComputed = Standard_False;
  theNewParameter = theParameter;

  Standard_Real acurpar = theParameter;
  TopAbs_State aState = TopAbs_ON;
  Standard_Integer iter = 0;
  Standard_Real asumtol = theTol;
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
Standard_Boolean IsCurveValid (const Handle(Geom2d_Curve)& thePCurve)
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
                    const Standard_Real TolF1,
                    const Standard_Real TolF2,
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
  //
  // Valid tolerance for the intersection curve between planar faces
  // is the maximal tolerance between tolerances of faces
  Standard_Real aTolC = Max(TolF1, TolF2);
  aCurve.SetTolerance(aTolC);
  //
  // Computation of the tangential tolerance
  Standard_Real anAngle, aDt;
  gp_Dir aD1, aD2;
  //
  aD1 = aPln1.Position().Direction();
  aD2 = aPln2.Position().Direction();
  anAngle = aD1.Angle(aD2);
  //
  aDt = IntTools_Tools::ComputeIntRange(TolF1, TolF2, anAngle);
  Standard_Real aTangTol = sqrt(aDt*aDt + TolF1*TolF1);
  //
  aCurve.SetTangentialTolerance(aTangTol);
  //
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
  aX1=aB - aCf*(aB-aA);
  aF1 = MaxDistance(theC, aX1, theProjPS);
  aX2 = aA + aCf * (aB - aA);
  aF2 = MaxDistance(theC, aX2, theProjPS);

  while (Abs(aX1-aX2) > theEps)
  {
    if (aF1 > aF2) {
      aB = aX2;
      aX2 = aX1;
      aF2 = aF1;
      aX1 = aB-aCf*(aB-aA);
      aF1 = MaxDistance(theC, aX1, theProjPS);
    }
    else {
      aA = aX1;
      aX1 = aX2;
      aF1 = aF2;
      aX2=aA+aCf*(aB-aA);
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
                               const TopoDS_Face& aFace,
                               const Handle(IntTools_Context)& theCtx)
{
  const Standard_Integer NPoints = 23;
  Standard_Integer i;
  Standard_Real umin,umax,vmin,vmax;

  theCtx->UVBounds(aFace, umin, umax, vmin, vmax);
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
