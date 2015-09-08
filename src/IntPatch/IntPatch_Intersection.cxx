// Created by: Modelization
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


#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_ALineToWLine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_ImpImpIntersection.hxx>
#include <IntPatch_ImpPrmIntersection.hxx>
#include <IntPatch_Intersection.hxx>
#include <IntPatch_Line.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_PrmPrmIntersection.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_Quadric.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

#include <stdio.h>
#define DEBUG 0 
static const Standard_Integer aNbPointsInALine = 200;

//=======================================================================
//function : MinMax
//purpose  : Replaces theParMIN = MIN(theParMIN, theParMAX),
//                    theParMAX = MAX(theParMIN, theParMAX).
//=======================================================================
static inline void MinMax(Standard_Real& theParMIN, Standard_Real& theParMAX)
{
  if(theParMIN > theParMAX)
  {
    const Standard_Real aTmp = theParMAX;
    theParMAX = theParMIN;
    theParMIN = aTmp;
  }
}

//=======================================================================
//function : IsSeam
//purpose  : Returns:
//            0 - if interval [theU1, theU2] does not intersect the "seam-edge"
//                or if "seam-edge" do not exist;
//            1 - if interval (theU1, theU2) intersect the "seam-edge".
//            2 - if theU1 or/and theU2 lie ON the "seam-edge"
//
//ATTENTION!!!
//  If (theU1 == theU2) then this function will return only both 0 or 2.
//=======================================================================
static Standard_Integer IsSeam( const Standard_Real theU1,
                                const Standard_Real theU2,
                                const Standard_Real thePeriod)
{
  if(IsEqual(thePeriod, 0.0))
    return 0;

  //If interval [theU1, theU2] intersect seam-edge then there exists an integer
  //number N such as 
  //    (theU1 <= T*N <= theU2) <=> (theU1/T <= N <= theU2/T),
  //where T is the period.
  //I.e. the inerval [theU1/T, theU2/T] must contain at least one
  //integer number. In this case, Floor(theU1/T) and Floor(theU2/T)
  //return different values or theU1/T is strictly integer number.
  //Examples:
  //  1. theU1/T==2.8, theU2/T==3.5 => Floor(theU1/T) == 2, Floor(theU2/T) == 3.
  //  2. theU1/T==2.0, theU2/T==2.6 => Floor(theU1/T) == Floor(theU2/T) == 2.

  const Standard_Real aVal1 = theU1/thePeriod,
                      aVal2 = theU2/thePeriod;
  const Standard_Integer aPar1 = static_cast<Standard_Integer>(Floor(aVal1));
  const Standard_Integer aPar2 = static_cast<Standard_Integer>(Floor(aVal2));
  if(aPar1 != aPar2)
  {//Interval (theU1, theU2] intersects seam-edge
    if(IsEqual(aVal2, static_cast<Standard_Real>(aPar2)))
    {//aVal2 is an integer number => theU2 lies ON the "seam-edge"
      return 2;
    }

    return 1;
  }

  //Here, aPar1 == aPar2. 

  if(IsEqual(aVal1, static_cast<Standard_Real>(aPar1)))
  {//aVal1 is an integer number => theU1 lies ON the "seam-edge"
    return 2;
  }

  //If aVal2 is a true integer number then always (aPar1 != aPar2).

  return 0;
}

//=======================================================================
//function : IsSeamOrBound
//purpose  : Returns TRUE if segment [thePtf, thePtl] intersects "seam-edge"
//            (if it exist) or surface boundaries and both thePtf and thePtl do
//            not match "seam-edge" or boundaries.
//           Point thePtmid lies in this segment. If thePtmid match
//            "seam-edge" or boundaries strictly (without any tolerance) then
//            the function will return TRUE.
//            See comments in function body for detail information.
//=======================================================================
static Standard_Boolean IsSeamOrBound(const IntSurf_PntOn2S& thePtf,
                                      const IntSurf_PntOn2S& thePtl,
                                      const IntSurf_PntOn2S& thePtmid,
                                      const Standard_Real theU1Period,
                                      const Standard_Real theU2Period,
                                      const Standard_Real theV1Period,
                                      const Standard_Real theV2Period,
                                      const Standard_Real theUfSurf1,
                                      const Standard_Real theUlSurf1,
                                      const Standard_Real theVfSurf1,
                                      const Standard_Real theVlSurf1,
                                      const Standard_Real theUfSurf2,
                                      const Standard_Real theUlSurf2,
                                      const Standard_Real theVfSurf2,
                                      const Standard_Real theVlSurf2)
{
  Standard_Real aU11 = 0.0, aU12 = 0.0, aV11 = 0.0, aV12 = 0.0;
  Standard_Real aU21 = 0.0, aU22 = 0.0, aV21 = 0.0, aV22 = 0.0;
  thePtf.Parameters(aU11, aV11, aU12, aV12);
  thePtl.Parameters(aU21, aV21, aU22, aV22);

  MinMax(aU11, aU21);
  MinMax(aV11, aV21);
  MinMax(aU12, aU22);
  MinMax(aV12, aV22);

  if((aU11 - theUfSurf1)*(aU21 - theUfSurf1) < 0.0)
  {//Interval [aU11, aU21] intersects theUfSurf1
    return Standard_True;
  }

  if((aU11 - theUlSurf1)*(aU21 - theUlSurf1) < 0.0)
  {//Interval [aU11, aU21] intersects theUlSurf1
    return Standard_True;
  }

  if((aV11 - theVfSurf1)*(aV21 - theVfSurf1) < 0.0)
  {//Interval [aV11, aV21] intersects theVfSurf1
    return Standard_True;
  }

  if((aV11 - theVlSurf1)*(aV21 - theVlSurf1) < 0.0)
  {//Interval [aV11, aV21] intersects theVlSurf1
    return Standard_True;
  }

  if((aU12 - theUfSurf2)*(aU22 - theUfSurf2) < 0.0)
  {//Interval [aU12, aU22] intersects theUfSurf2
    return Standard_True;
  }

  if((aU12 - theUlSurf2)*(aU22 - theUlSurf2) < 0.0)
  {//Interval [aU12, aU22] intersects theUlSurf2
    return Standard_True;
  }

  if((aV12 - theVfSurf2)*(aV22 - theVfSurf2) < 0.0)
  {//Interval [aV12, aV22] intersects theVfSurf2
    return Standard_True;
  }

  if((aV12 - theVlSurf2)*(aV22 - theVlSurf2) < 0.0)
  {//Interval [aV12, aV22] intersects theVlSurf2
    return Standard_True;
  }

  if(IsSeam(aU11, aU21, theU1Period))
    return Standard_True;

  if(IsSeam(aV11, aV21, theV1Period))
    return Standard_True;

  if(IsSeam(aU12, aU22, theU2Period))
    return Standard_True;

  if(IsSeam(aV12, aV22, theV2Period))
    return Standard_True;

  /*
    The segment [thePtf, thePtl] does not intersect the boundaries and
    the seam-edge of the surfaces.
    Nevertheless, following situation is possible:

                  seam or
                   bound
                     |
        thePtf  *    |
                     |
                     * thePtmid
          thePtl  *  |
                     |

    This case must be processed, too.
  */

  Standard_Real aU1 = 0.0, aU2 = 0.0, aV1 = 0.0, aV2 = 0.0;
  thePtmid.Parameters(aU1, aV1, aU2, aV2);

  if(IsEqual(aU1, theUfSurf1) || IsEqual(aU1, theUlSurf1))
    return Standard_True;

  if(IsEqual(aU2, theUfSurf2) || IsEqual(aU2, theUlSurf2))
    return Standard_True;

  if(IsEqual(aV1, theVfSurf1) || IsEqual(aV1, theVlSurf1))
    return Standard_True;

  if(IsEqual(aV2, theVfSurf2) || IsEqual(aV2, theVlSurf2))
    return Standard_True;

  if(IsSeam(aU1, aU1, theU1Period))
    return Standard_True;

  if(IsSeam(aU2, aU2, theU2Period))
    return Standard_True;

  if(IsSeam(aV1, aV1, theV1Period))
    return Standard_True;

  if(IsSeam(aV2, aV2, theV2Period))
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : JoinWLines
//purpose  : joins all WLines from theSlin to one if it is possible and
//            records the result into theSlin again.
//            Lines will be kept to be splitted if:
//              a) they are separated (has no common points);
//              b) resulted line (after joining) go through
//                 seam-edges or surface boundaries.
//
//          In addition, if points in theSPnt lies at least in one of 
//          the line in theSlin, this point will be deleted.
//=======================================================================
static void JoinWLines(IntPatch_SequenceOfLine& theSlin,
                IntPatch_SequenceOfPoint& theSPnt,
                const Standard_Real theTol3D,
                const Standard_Real theU1Period,
                const Standard_Real theU2Period,
                const Standard_Real theV1Period,
                const Standard_Real theV2Period,
                const Standard_Real theUfSurf1,
                const Standard_Real theUlSurf1,
                const Standard_Real theVfSurf1,
                const Standard_Real theVlSurf1,
                const Standard_Real theUfSurf2,
                const Standard_Real theUlSurf2,
                const Standard_Real theVfSurf2,
                const Standard_Real theVlSurf2)
{
  if(theSlin.Length() == 0)
    return;

  for(Standard_Integer aNumOfLine1 = 1; aNumOfLine1 <= theSlin.Length(); aNumOfLine1++)
  {
    Handle(IntPatch_WLine) aWLine1 (Handle(IntPatch_WLine)::DownCast(theSlin.Value(aNumOfLine1)));

    if(aWLine1.IsNull())
    {//We must have failed to join not-point-lines
      return;
    }

    const Standard_Integer aNbPntsWL1 = aWLine1->NbPnts();
    const IntSurf_PntOn2S& aPntFW1 = aWLine1->Point(1);
    const IntSurf_PntOn2S& aPntLW1 = aWLine1->Point(aNbPntsWL1);

    for(Standard_Integer aNPt = 1; aNPt <= theSPnt.Length(); aNPt++)
    {
      const IntSurf_PntOn2S aPntCur = theSPnt.Value(aNPt).PntOn2S();

      if( aPntCur.IsSame(aPntFW1, Precision::Confusion()) ||
        aPntCur.IsSame(aPntLW1, Precision::Confusion()))
      {
        theSPnt.Remove(aNPt);
        aNPt--;
      }
    }

    Standard_Boolean hasBeenRemoved = Standard_False;
    for(Standard_Integer aNumOfLine2 = aNumOfLine1 + 1; aNumOfLine2 <= theSlin.Length(); aNumOfLine2++)
    {
      Handle(IntPatch_WLine) aWLine2 (Handle(IntPatch_WLine)::DownCast(theSlin.Value(aNumOfLine2)));

      const Standard_Integer aNbPntsWL2 = aWLine2->NbPnts();

      const IntSurf_PntOn2S& aPntFWL1 = aWLine1->Point(1);
      const IntSurf_PntOn2S& aPntLWL1 = aWLine1->Point(aNbPntsWL1);

      const IntSurf_PntOn2S& aPntFWL2 = aWLine2->Point(1);
      const IntSurf_PntOn2S& aPntLWL2 = aWLine2->Point(aNbPntsWL2);

      if(aPntFWL1.IsSame(aPntFWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(2);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(2);
        if(!IsSeamOrBound(aPt1, aPt2, aPntFWL1, theU1Period, theU2Period,
                          theV1Period, theV2Period, theUfSurf1, theUlSurf1,
                          theVfSurf1, theVlSurf1, theUfSurf2, theUlSurf2,
                          theVfSurf2, theVlSurf2))
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = 1; aNPt <= aNbPntsWL2; aNPt++)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->InsertBefore(1, aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntFWL1.IsSame(aPntLWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(2);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(aNbPntsWL2-1);
        if(!IsSeamOrBound(aPt1, aPt2, aPntFWL1, theU1Period, theU2Period,
                          theV1Period, theV2Period, theUfSurf1, theUlSurf1,
                          theVfSurf1, theVlSurf1, theUfSurf2, theUlSurf2,
                          theVfSurf2, theVlSurf2))
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = aNbPntsWL2; aNPt >= 1; aNPt--)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->InsertBefore(1, aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntLWL1.IsSame(aPntFWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(aNbPntsWL1-1);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(2);
        if(!IsSeamOrBound(aPt1, aPt2, aPntLWL1, theU1Period, theU2Period,
                          theV1Period, theV2Period, theUfSurf1, theUlSurf1,
                          theVfSurf1, theVlSurf1, theUfSurf2, theUlSurf2,
                          theVfSurf2, theVlSurf2))
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = 1; aNPt <= aNbPntsWL2; aNPt++)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->Add(aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }

      if(aPntLWL1.IsSame(aPntLWL2, Precision::Confusion()))
      {
        const IntSurf_PntOn2S& aPt1 = aWLine1->Point(aNbPntsWL1-1);
        const IntSurf_PntOn2S& aPt2 = aWLine2->Point(aNbPntsWL2-1);
        if(!IsSeamOrBound(aPt1, aPt2, aPntLWL1, theU1Period, theU2Period,
                          theV1Period, theV2Period, theUfSurf1, theUlSurf1,
                          theVfSurf1, theVlSurf1, theUfSurf2, theUlSurf2,
                          theVfSurf2, theVlSurf2))
        {
          aWLine1->ClearVertexes();
          for(Standard_Integer aNPt = aNbPntsWL2; aNPt >= 1; aNPt--)
          {
            const IntSurf_PntOn2S& aPt = aWLine2->Point(aNPt);
            aWLine1->Curve()->Add(aPt);
          }

          aWLine1->ComputeVertexParameters(theTol3D);

          theSlin.Remove(aNumOfLine2);
          aNumOfLine2--;
          hasBeenRemoved = Standard_True;

          continue;
        }
      }
    }

    if(hasBeenRemoved)
      aNumOfLine1--;
  }
}

//======================================================================
// function: SequenceOfLine
//======================================================================
const IntPatch_SequenceOfLine& IntPatch_Intersection::SequenceOfLine() const { return(slin); }

//======================================================================
// function: IntPatch_Intersection
//======================================================================
IntPatch_Intersection::IntPatch_Intersection ()
 : done(Standard_False),
   //empt, tgte, oppo,
   myTolArc(0.0), myTolTang(0.0),
   myUVMaxStep(0.0), myFleche(0.0),
   myIsStartPnt(Standard_False)
   //myU1Start, myV1Start, myU2Start, myV2Start
{
}

//======================================================================
// function: IntPatch_Intersection
//======================================================================
IntPatch_Intersection::IntPatch_Intersection(const Handle(Adaptor3d_HSurface)&  S1,
                                             const Handle(Adaptor3d_TopolTool)& D1,
                                             const Handle(Adaptor3d_HSurface)&  S2,
                                             const Handle(Adaptor3d_TopolTool)& D2,
                                             const Standard_Real TolArc,
                                             const Standard_Real TolTang)
 : done(Standard_False),
   //empt, tgte, oppo,
   myTolArc(TolArc), myTolTang(TolTang),
   myUVMaxStep(0.0), myFleche(0.0),
   myIsStartPnt(Standard_False)
   //myU1Start, myV1Start, myU2Start, myV2Start
{
  if(myTolArc<1e-8) myTolArc=1e-8;
  if(myTolTang<1e-8) myTolTang=1e-8;
  if(myTolArc>0.5) myTolArc=0.5;
  if(myTolTang>0.5) myTolTang=0.5;
  Perform(S1,D1,S2,D2,TolArc,TolTang);
}

//======================================================================
// function: IntPatch_Intersection
//======================================================================
IntPatch_Intersection::IntPatch_Intersection(const Handle(Adaptor3d_HSurface)&  S1,
                                             const Handle(Adaptor3d_TopolTool)& D1,
                                             const Standard_Real TolArc,
                                             const Standard_Real TolTang)
 : done(Standard_False),
   //empt, tgte, oppo,
   myTolArc(TolArc), myTolTang(TolTang),
   myUVMaxStep(0.0), myFleche(0.0),
   myIsStartPnt(Standard_False)
   //myU1Start, myV1Start, myU2Start, myV2Start
{
  Perform(S1,D1,TolArc,TolTang);
}

//======================================================================
// function: SetTolerances
//======================================================================
void IntPatch_Intersection::SetTolerances(const Standard_Real TolArc,
                                          const Standard_Real TolTang,
                                          const Standard_Real UVMaxStep,
                                          const Standard_Real Fleche)
{ 
  myTolArc     = TolArc;
  myTolTang    = TolTang;
  myUVMaxStep  = UVMaxStep;
  myFleche     = Fleche;
  if(myTolArc<1e-8) myTolArc=1e-8;
  if(myTolTang<1e-8) myTolTang=1e-8;
  if(myTolArc>0.5) myTolArc=0.5;
  if(myTolTang>0.5) myTolTang=0.5;  
  if(myFleche<1.0e-3) myFleche=1e-3;
  if(myUVMaxStep<1.0e-3) myUVMaxStep=1e-3;
  if(myFleche>10) myFleche=10;
  if(myUVMaxStep>0.5) myUVMaxStep=0.5;
}

//======================================================================
// function: Perform
//======================================================================
void IntPatch_Intersection::Perform(const Handle(Adaptor3d_HSurface)&  S1,
                                    const Handle(Adaptor3d_TopolTool)& D1,
                                    const Standard_Real TolArc,
                                    const Standard_Real TolTang)
{
  myTolArc = TolArc;
  myTolTang = TolTang;
  if(myFleche == 0.0)  myFleche = 0.01;
  if(myUVMaxStep==0.0) myUVMaxStep = 0.01;

  done = Standard_True;
  spnt.Clear();
  slin.Clear();
  
  empt = Standard_True;
  tgte = Standard_False;
  oppo = Standard_False;

  switch (S1->GetType())
  { 
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone:
    case GeomAbs_Torus: break;
    default:
    {
      IntPatch_PrmPrmIntersection interpp;
      interpp.Perform(S1,D1,TolArc,TolTang,myFleche,myUVMaxStep);
      if (interpp.IsDone())
      {
        done = Standard_True;
        tgte = Standard_False;
        empt = interpp.IsEmpty();
        const Standard_Integer nblm = interpp.NbLines();
        for (Standard_Integer i=1; i<=nblm; i++) slin.Append(interpp.Line(i));
      }
    }
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////
//  These several support functions provide methods which can help basic   //
//  algorithm to intersect infinite surfaces of the following types:       //
//                                                                         //
//  a.) SurfaceOfExtrusion;                                                //
//  b.) SurfaceOfRevolution;                                               //
//  c.) OffsetSurface.                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include <TColgp_Array1OfXYZ.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <GeomAdaptor.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Geom_Plane.hxx>
#include <ProjLib_ProjectOnPlane.hxx>
#include <GeomProjLib.hxx>
#include <ElCLib.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

//===============================================================
//function: FUN_GetMinMaxXYZPnt
//===============================================================
static void FUN_GetMinMaxXYZPnt( const Handle(Adaptor3d_HSurface)& S,
                                 gp_Pnt& pMin, gp_Pnt& pMax )
{
  const Standard_Real DU = 0.25 * Abs(S->LastUParameter() - S->FirstUParameter());
  const Standard_Real DV = 0.25 * Abs(S->LastVParameter() - S->FirstVParameter());
  Standard_Real tMinXYZ = RealLast();
  Standard_Real tMaxXYZ = -tMinXYZ;
  gp_Pnt PUV, ptMax, ptMin;
  for(Standard_Real U = S->FirstUParameter(); U <= S->LastUParameter(); U += DU)
  {
    for(Standard_Real V = S->FirstVParameter(); V <= S->LastVParameter(); V += DV)
    {
      S->D0(U,V,PUV);
      const Standard_Real cXYZ = PUV.XYZ().Modulus();
      if(cXYZ > tMaxXYZ) { tMaxXYZ = cXYZ; ptMax = PUV; }
      if(cXYZ < tMinXYZ) { tMinXYZ = cXYZ; ptMin = PUV; }
    }
  }
  pMin = ptMin;
  pMax = ptMax;
}
//==========================================================================
//function: FUN_TrimInfSurf
//==========================================================================
static void FUN_TrimInfSurf(const gp_Pnt& Pmin,
                            const gp_Pnt& Pmax,
                            const Handle(Adaptor3d_HSurface)& InfSurf,
                            const Standard_Real& AlternativeTrimPrm,
                            Handle(Adaptor3d_HSurface)& TrimS)
{
  Standard_Real TP = AlternativeTrimPrm;
  Extrema_ExtPS ext1(Pmin, InfSurf->Surface(), 1.e-7, 1.e-7);
  Extrema_ExtPS ext2(Pmax, InfSurf->Surface(), 1.e-7, 1.e-7);
  if(ext1.IsDone() || ext2.IsDone())
  {
    Standard_Real Umax = -1.e+100, Umin = 1.e+100, Vmax = -1.e+100, Vmin = 1.e+100, cU, cV;
    if(ext1.IsDone())
    {
      for(Standard_Integer i = 1; i <= ext1.NbExt(); i++)
      {
        const Extrema_POnSurf & pons = ext1.Point(i);
        pons.Parameter(cU,cV);
        if(cU > Umax) Umax = cU;
        if(cU < Umin) Umin = cU;
        if(cV > Vmax) Vmax = cV;
        if(cV < Vmin) Vmin = cV;
      }
    }
    if(ext2.IsDone())
    {
      for(Standard_Integer i = 1; i <= ext2.NbExt(); i++)
      {
        const Extrema_POnSurf & pons = ext2.Point(i);
        pons.Parameter(cU,cV);
        if(cU > Umax) Umax = cU;
        if(cU < Umin) Umin = cU;
        if(cV > Vmax) Vmax = cV;
        if(cV < Vmin) Vmin = cV;
      }
    }
    TP = Max(Abs(Umin),Max(Abs(Umax),Max(Abs(Vmin),Abs(Vmax))));
  }
  if(TP == 0.) { TrimS = InfSurf; return; }
  else
  {
    const Standard_Boolean Uinf = Precision::IsNegativeInfinite(InfSurf->FirstUParameter()); 
    const Standard_Boolean Usup = Precision::IsPositiveInfinite(InfSurf->LastUParameter());
    const Standard_Boolean Vinf = Precision::IsNegativeInfinite(InfSurf->FirstVParameter()); 
    const Standard_Boolean Vsup = Precision::IsPositiveInfinite(InfSurf->LastVParameter());
    Handle(Adaptor3d_HSurface) TmpSS;
    Standard_Integer IsTrimed = 0;
    const Standard_Real tp = 1000.0 * TP;
    if(Vinf && Vsup) { TrimS = InfSurf->VTrim(-tp, tp, 1.0e-7); IsTrimed = 1; }
    if(Vinf && !Vsup){ TrimS = InfSurf->VTrim(-tp, InfSurf->LastVParameter(), 1.0e-7); IsTrimed = 1; }
    if(!Vinf && Vsup){ TrimS = InfSurf->VTrim(InfSurf->FirstVParameter(), tp, 1.0e-7); IsTrimed = 1; }
    if(IsTrimed)
    {
      TmpSS = TrimS;
      if(Uinf && Usup)  TrimS = TmpSS->UTrim(-tp, tp, 1.0e-7);
      if(Uinf && !Usup) TrimS = TmpSS->UTrim(-tp, InfSurf->LastUParameter(), 1.0e-7);
      if(!Uinf && Usup) TrimS = TmpSS->UTrim(InfSurf->FirstUParameter(), tp, 1.0e-7);
    }
    else
    {
      if(Uinf && Usup)  TrimS = InfSurf->UTrim(-tp, tp, 1.0e-7);
      if(Uinf && !Usup) TrimS = InfSurf->UTrim(-tp, InfSurf->LastUParameter(), 1.0e-7);
      if(!Uinf && Usup) TrimS = InfSurf->UTrim(InfSurf->FirstUParameter(), tp, 1.0e-7);
    }
  }
}
//================================================================================
//function: FUN_GetUiso
//================================================================================
static void FUN_GetUiso(const Handle(Geom_Surface)& GS,
                        const GeomAbs_SurfaceType&  T,
                        const Standard_Real&        FirstV,
                        const Standard_Real&        LastV,
                        const Standard_Boolean&     IsVC,
                        const Standard_Boolean&     IsVP,
                        const Standard_Real&        U,
                        Handle(Geom_Curve)&         I)
{
  if(T !=  GeomAbs_OffsetSurface)
  {
    Handle(Geom_Curve) gc = GS->UIso(U);
    if(IsVP && (FirstV == 0.0 && LastV == (2.*M_PI))) I = gc;
    else
    {
      Handle(Geom_TrimmedCurve) gtc = new Geom_TrimmedCurve(gc,FirstV,LastV);
      //szv:I = Handle(Geom_Curve)::DownCast(gtc);
      I = gtc;
    }
  }
  else//OffsetSurface
  {
    const Handle(Geom_OffsetSurface) gos = Handle(Geom_OffsetSurface)::DownCast (GS);
    const Handle(Geom_Surface) bs = gos->BasisSurface();
    Handle(Geom_Curve) gcbs = bs->UIso(U);
    GeomAdaptor_Curve gac(gcbs);
    const GeomAbs_CurveType GACT = gac.GetType();
    if(IsVP || IsVC || GACT == GeomAbs_BSplineCurve || GACT == GeomAbs_BezierCurve || Abs(LastV - FirstV) < 1.e+5)
    {
      Handle(Geom_Curve) gc = gos->UIso(U);
      if(IsVP && (FirstV == 0.0 && LastV == (2*M_PI))) I = gc;
      else
      {
        Handle(Geom_TrimmedCurve) gtc = new Geom_TrimmedCurve(gc,FirstV,LastV);
        //szv:I = Handle(Geom_Curve)::DownCast(gtc);
        I = gtc;
      }
    }
    else//Offset Line, Parab, Hyperb
    {
      Standard_Real VmTr, VMTr;
      if(GACT != GeomAbs_Hyperbola)
      {
        if(FirstV >= 0. && LastV >= 0.){ VmTr = FirstV; VMTr = ((LastV - FirstV) > 1.e+4) ? (FirstV + 1.e+4) : LastV; }
        else if(FirstV < 0. && LastV < 0.){ VMTr = LastV; VmTr = ((FirstV - LastV) < -1.e+4) ? (LastV - 1.e+4) : FirstV; }
        else { VmTr = (FirstV < -1.e+4) ? -1.e+4 : FirstV; VMTr = (LastV > 1.e+4) ? 1.e+4 : LastV; }
      }
      else//Hyperbola
      {
        if(FirstV >= 0. && LastV >= 0.)
        {
          if(FirstV > 4.) return;
          VmTr = FirstV; VMTr = (LastV > 4.) ? 4. : LastV;
        }
        else if(FirstV < 0. && LastV < 0.)
        {
          if(LastV < -4.) return;
          VMTr = LastV; VmTr = (FirstV < -4.) ? -4. : FirstV;
        }
        else { VmTr = (FirstV < -4.) ? -4. : FirstV; VMTr = (LastV > 4.) ? 4. : LastV; }
      }
      //Make trimmed surface
      Handle(Geom_RectangularTrimmedSurface) rts = new Geom_RectangularTrimmedSurface(gos,VmTr,VMTr,Standard_True);
      I = rts->UIso(U);
    }
  }
}
//================================================================================
//function: FUN_GetViso
//================================================================================
static void FUN_GetViso(const Handle(Geom_Surface)& GS,
                        const GeomAbs_SurfaceType&  T,
                        const Standard_Real&        FirstU,
                        const Standard_Real&        LastU,
                        const Standard_Boolean&     IsUC,
                        const Standard_Boolean&     IsUP,
                        const Standard_Real&        V,
                        Handle(Geom_Curve)&         I)
{
  if(T !=  GeomAbs_OffsetSurface)
  {
    Handle(Geom_Curve) gc = GS->VIso(V);
    if(IsUP && (FirstU == 0.0 && LastU == (2*M_PI))) I = gc;
    else
    {
      Handle(Geom_TrimmedCurve) gtc = new Geom_TrimmedCurve(gc,FirstU,LastU);
      //szv:I = Handle(Geom_Curve)::DownCast(gtc);
      I = gtc;
    }
  }
  else//OffsetSurface
  {
    const Handle(Geom_OffsetSurface) gos = Handle(Geom_OffsetSurface)::DownCast (GS);
    const Handle(Geom_Surface) bs = gos->BasisSurface();
    Handle(Geom_Curve) gcbs = bs->VIso(V);
    GeomAdaptor_Curve gac(gcbs);
    const GeomAbs_CurveType GACT = gac.GetType();
    if(IsUP || IsUC || GACT == GeomAbs_BSplineCurve || GACT == GeomAbs_BezierCurve || Abs(LastU - FirstU) < 1.e+5)
    {
      Handle(Geom_Curve) gc = gos->VIso(V);
      if(IsUP && (FirstU == 0.0 && LastU == (2*M_PI))) I = gc;
      else
      {
        Handle(Geom_TrimmedCurve) gtc = new Geom_TrimmedCurve(gc,FirstU,LastU);
        //szv:I = Handle(Geom_Curve)::DownCast(gtc);
        I = gtc;
      }
    }
    else//Offset Line, Parab, Hyperb
    {
      Standard_Real UmTr, UMTr;
      if(GACT != GeomAbs_Hyperbola)
      {
        if(FirstU >= 0. && LastU >= 0.){ UmTr = FirstU; UMTr = ((LastU - FirstU) > 1.e+4) ? (FirstU + 1.e+4) : LastU; }
        else if(FirstU < 0. && LastU < 0.){ UMTr = LastU; UmTr = ((FirstU - LastU) < -1.e+4) ? (LastU - 1.e+4) : FirstU; }
        else { UmTr = (FirstU < -1.e+4) ? -1.e+4 : FirstU; UMTr = (LastU > 1.e+4) ? 1.e+4 : LastU; }
      }
      else//Hyperbola
      {
        if(FirstU >= 0. && LastU >= 0.)
        {
          if(FirstU > 4.) return;
          UmTr = FirstU; UMTr = (LastU > 4.) ? 4. : LastU;
        }
        else if(FirstU < 0. && LastU < 0.)
        {
          if(LastU < -4.) return;
          UMTr = LastU; UmTr = (FirstU < -4.) ? -4. : FirstU;
        }
        else { UmTr = (FirstU < -4.) ? -4. : FirstU; UMTr = (LastU > 4.) ? 4. : LastU; }
      }
      //Make trimmed surface
      Handle(Geom_RectangularTrimmedSurface) rts = new Geom_RectangularTrimmedSurface(gos,UmTr,UMTr,Standard_True);
      I = rts->VIso(V);
    }
  }
}
//================================================================================
//function: FUN_PL_Intersection
//================================================================================
static void FUN_PL_Intersection(const Handle(Adaptor3d_HSurface)& S1,
                                const GeomAbs_SurfaceType&        T1,
                                const Handle(Adaptor3d_HSurface)& S2,
                                const GeomAbs_SurfaceType&        T2,
                                Standard_Boolean&                 IsOk,
                                TColgp_SequenceOfPnt&             SP,
                                gp_Vec&                           DV)
{
  IsOk = Standard_False;
  // 1. Check: both surfaces have U(V)isos - lines.
  DV = gp_Vec(0.,0.,1.);
  Standard_Boolean isoS1isLine[2] = {0, 0};
  Standard_Boolean isoS2isLine[2] = {0, 0};
  Handle(Geom_Curve) C1, C2;
  const GeomAdaptor_Surface & gas1 = *(GeomAdaptor_Surface*)(&(S1->Surface()));
  const GeomAdaptor_Surface & gas2 = *(GeomAdaptor_Surface*)(&(S2->Surface()));
  const Handle(Geom_Surface) gs1 = gas1.Surface();
  const Handle(Geom_Surface) gs2 = gas2.Surface();
  Standard_Real MS1[2], MS2[2];
  MS1[0] = 0.5 * (S1->LastUParameter() + S1->FirstUParameter());
  MS1[1] = 0.5 * (S1->LastVParameter() + S1->FirstVParameter());
  MS2[0] = 0.5 * (S2->LastUParameter() + S2->FirstUParameter());
  MS2[1] = 0.5 * (S2->LastVParameter() + S2->FirstVParameter());
  if(T1 == GeomAbs_SurfaceOfExtrusion) isoS1isLine[0] = Standard_True;
  else if(!S1->IsVPeriodic() && !S1->IsVClosed()) {
    if(T1 != GeomAbs_OffsetSurface) C1 = gs1->UIso(MS1[0]);
    else {
      const Handle(Geom_OffsetSurface) gos = Handle(Geom_OffsetSurface)::DownCast (gs1);
      const Handle(Geom_Surface) bs = gos->BasisSurface();
      C1 = bs->UIso(MS1[0]);
    }
    GeomAdaptor_Curve gac(C1);
    if(gac.GetType() == GeomAbs_Line) isoS1isLine[0] = Standard_True;
  }
  if(!S1->IsUPeriodic() && !S1->IsUClosed()) {
    if(T1 != GeomAbs_OffsetSurface) C1 = gs1->VIso(MS1[1]);
    else {
      const Handle(Geom_OffsetSurface) gos = Handle(Geom_OffsetSurface)::DownCast (gs1);
      const Handle(Geom_Surface) bs = gos->BasisSurface();
      C1 = bs->VIso(MS1[1]);
    }
    GeomAdaptor_Curve gac(C1);
    if(gac.GetType() == GeomAbs_Line) isoS1isLine[1] = Standard_True;
  }
  if(T2 == GeomAbs_SurfaceOfExtrusion) isoS2isLine[0] = Standard_True;
  else if(!S2->IsVPeriodic() && !S2->IsVClosed()) {
    if(T2 != GeomAbs_OffsetSurface) C2 = gs2->UIso(MS2[0]);
    else {
      const Handle(Geom_OffsetSurface) gos = Handle(Geom_OffsetSurface)::DownCast (gs2);
      const Handle(Geom_Surface) bs = gos->BasisSurface();
      C2 = bs->UIso(MS2[0]);
    }
    GeomAdaptor_Curve gac(C2);
    if(gac.GetType() == GeomAbs_Line) isoS2isLine[0] = Standard_True;
  }
  if(!S2->IsUPeriodic() && !S2->IsUClosed()) {
    if(T2 != GeomAbs_OffsetSurface) C2 = gs2->VIso(MS2[1]);
    else {
      const Handle(Geom_OffsetSurface) gos = Handle(Geom_OffsetSurface)::DownCast (gs2);
      const Handle(Geom_Surface) bs = gos->BasisSurface();
      C2 = bs->VIso(MS2[1]);
    }
    GeomAdaptor_Curve gac(C2);
    if(gac.GetType() == GeomAbs_Line) isoS2isLine[1] = Standard_True;
  }
  Standard_Boolean IsBothLines = ((isoS1isLine[0] || isoS1isLine[1]) &&
                                  (isoS2isLine[0] || isoS2isLine[1]));
  if(!IsBothLines){
    return;
  }
  // 2. Check: Uiso lines of both surfaces are collinear.
  gp_Pnt puvS1, puvS2;
  gp_Vec derS1[2], derS2[2];
  S1->D1(MS1[0], MS1[1], puvS1, derS1[0], derS1[1]);
  S2->D1(MS2[0], MS2[1], puvS2, derS2[0], derS2[1]);
  C1.Nullify(); C2.Nullify();
  Standard_Integer iso = 0;
  if(isoS1isLine[0] && isoS2isLine[0] &&
     derS1[1].IsParallel(derS2[1],Precision::Angular())) {
    iso = 1;
    FUN_GetViso(gs1,T1,S1->FirstUParameter(),S1->LastUParameter(),
                S1->IsUClosed(),S1->IsUPeriodic(),MS1[1],C1);
    FUN_GetViso(gs2,T2,S2->FirstUParameter(),S2->LastUParameter(),
                S2->IsUClosed(),S2->IsUPeriodic(),MS2[1],C2);
  }
  else if(isoS1isLine[0] && isoS2isLine[1] &&
          derS1[1].IsParallel(derS2[0],Precision::Angular())) {
    iso = 1;
    FUN_GetViso(gs1,T1,S1->FirstUParameter(),S1->LastUParameter(),
                S1->IsUClosed(),S1->IsUPeriodic(),MS1[1],C1);
    FUN_GetUiso(gs2,T2,S2->FirstVParameter(),S2->LastVParameter(),
                S2->IsVClosed(),S2->IsVPeriodic(),MS2[0],C2);
  }
  else if(isoS1isLine[1] && isoS2isLine[0] &&
          derS1[0].IsParallel(derS2[1],Precision::Angular())) {
    iso = 0;
    FUN_GetUiso(gs1,T1,S1->FirstVParameter(),S1->LastVParameter(),
                S1->IsVClosed(),S1->IsVPeriodic(),MS1[0],C1);
    FUN_GetViso(gs2,T2,S2->FirstUParameter(),S2->LastUParameter(),
                S2->IsUClosed(),S2->IsUPeriodic(),MS2[1],C2);
  }
  else if(isoS1isLine[1] && isoS2isLine[1] &&
          derS1[0].IsParallel(derS2[0],Precision::Angular())) {
    iso = 0;
    FUN_GetUiso(gs1,T1,S1->FirstVParameter(),S1->LastVParameter(),
                S1->IsVClosed(),S1->IsVPeriodic(),MS1[0],C1);
    FUN_GetUiso(gs2,T2,S2->FirstVParameter(),S2->LastVParameter(),
                S2->IsVClosed(),S2->IsVPeriodic(),MS2[0],C2);
  }
  else {
    IsOk = Standard_False;
    return;
  }
  IsOk = Standard_True;
  // 3. Make intersections of V(U)isos
  if(C1.IsNull() || C2.IsNull()) return;
  DV = derS1[iso];
  Handle(Geom_Plane) GPln = new Geom_Plane(gp_Pln(puvS1,gp_Dir(DV)));
  Handle(Geom_Curve) C1Prj =
    GeomProjLib::ProjectOnPlane(C1,GPln,gp_Dir(DV),Standard_True);
  Handle(Geom_Curve) C2Prj =
    GeomProjLib::ProjectOnPlane(C2,GPln,gp_Dir(DV),Standard_True);
  if(C1Prj.IsNull() || C2Prj.IsNull()) return;
  Handle(Geom2d_Curve) C1Prj2d =
    GeomProjLib::Curve2d(C1Prj,Handle(Geom_Surface)::DownCast (GPln));
  Handle(Geom2d_Curve) C2Prj2d =
    GeomProjLib::Curve2d(C2Prj,Handle(Geom_Surface)::DownCast (GPln));
  Geom2dAPI_InterCurveCurve ICC(C1Prj2d,C2Prj2d,1.0e-7);
  if(ICC.NbPoints() > 0 )
  {
    for(Standard_Integer ip = 1; ip <= ICC.NbPoints(); ip++)
    {
      gp_Pnt2d P = ICC.Point(ip);
      gp_Pnt P3d = ElCLib::To3d(gp_Ax2(puvS1,gp_Dir(DV)),P);
      SP.Append(P3d);
    }
  }
}
//================================================================================
//function: FUN_NewFirstLast
//================================================================================
static void FUN_NewFirstLast(const GeomAbs_CurveType& ga_ct,
                             const Standard_Real&     Fst,
                             const Standard_Real&     Lst,
                             const Standard_Real&     TrVal,
                             Standard_Real&           NewFst,
                             Standard_Real&           NewLst,
                             Standard_Boolean&        NeedTr)
{
  NewFst = Fst; NewLst = Lst; NeedTr = Standard_False;
  switch (ga_ct)
  {
    case GeomAbs_Line:
    case GeomAbs_Parabola:
    {
      if(Abs(Lst - Fst) > TrVal)
      {
        if(Fst >= 0. && Lst >= 0.)
        {
          NewFst = Fst;
          NewLst = ((Fst + TrVal) < Lst) ? (Fst + TrVal) : Lst;
        }
        if(Fst < 0. && Lst < 0.)
        {
          NewLst = Lst;
          NewFst = ((Lst - TrVal) > Fst) ? (Lst - TrVal) : Fst;
        }
        else
        {
          NewFst = (Fst < -TrVal) ? -TrVal : Fst;
          NewLst = (Lst > TrVal) ? TrVal : Lst;
        }
        NeedTr = Standard_True;
      }
      break;
    }
    case GeomAbs_Hyperbola:
    {
      if(Abs(Lst - Fst) > 10.)
      { 
        if(Fst >= 0. && Lst >= 0.)
        {
          if(Fst > 4.) return;
          NewFst = Fst;
          NewLst = (Lst > 4.) ? 4. : Lst;
        }
        if(Fst < 0. && Lst < 0.)
        {
          if(Lst < -4.) return;
          NewLst = Lst;
          NewFst = (Fst < -4.) ? -4. : Fst;
        }
        else
        {
          NewFst = (Fst < -4.) ? -4. : Fst;
          NewLst = (Lst > 4.) ? 4. : Lst;
        }
        NeedTr = Standard_True;
      }
      break;
    }
    default:
    break;
  }
}
//================================================================================
//function: FUN_TrimBothSurf
//================================================================================
static void FUN_TrimBothSurf(const Handle(Adaptor3d_HSurface)& S1,
                             const GeomAbs_SurfaceType&        T1,
                             const Handle(Adaptor3d_HSurface)& S2,
                             const GeomAbs_SurfaceType&        T2,
                             const Standard_Real&              TV,
                             Handle(Adaptor3d_HSurface)&       NS1,
                             Handle(Adaptor3d_HSurface)&       NS2)
{
  const GeomAdaptor_Surface & gas1 = *(GeomAdaptor_Surface*)(&(S1->Surface()));
  const GeomAdaptor_Surface & gas2 = *(GeomAdaptor_Surface*)(&(S2->Surface()));
  const Handle(Geom_Surface) gs1 = gas1.Surface();
  const Handle(Geom_Surface) gs2 = gas2.Surface();
  const Standard_Real UM1 = 0.5 * (S1->LastUParameter() + S1->FirstUParameter());
  const Standard_Real UM2 = 0.5 * (S2->LastUParameter() + S2->FirstUParameter());
  const Standard_Real VM1 = 0.5 * (S1->LastVParameter() + S1->FirstVParameter());
  const Standard_Real VM2 = 0.5 * (S2->LastVParameter() + S2->FirstVParameter());
  Handle(Geom_Curve) visoS1, visoS2, uisoS1, uisoS2;
  if(T1 != GeomAbs_OffsetSurface){ visoS1 = gs1->VIso(VM1); uisoS1 = gs1->UIso(UM1); }
  else
  {
    const Handle(Geom_OffsetSurface) gos = Handle(Geom_OffsetSurface)::DownCast (gs1);
    const Handle(Geom_Surface) bs = gos->BasisSurface();
    visoS1 = bs->VIso(VM1); uisoS1 = bs->UIso(UM1);
  }
  if(T2 != GeomAbs_OffsetSurface){ visoS2 = gs2->VIso(VM2); uisoS2 = gs2->UIso(UM2); }
  else
  {
    const Handle(Geom_OffsetSurface) gos = Handle(Geom_OffsetSurface)::DownCast (gs2);
    const Handle(Geom_Surface) bs = gos->BasisSurface();
    visoS2 = bs->VIso(VM2); uisoS2 = bs->UIso(UM2);
  }
  if(uisoS1.IsNull() || uisoS2.IsNull() || visoS1.IsNull() || visoS2.IsNull()){ NS1 = S1; NS2 = S2; return; }
  GeomAdaptor_Curve gau1(uisoS1);
  GeomAdaptor_Curve gav1(visoS1);
  GeomAdaptor_Curve gau2(uisoS2);
  GeomAdaptor_Curve gav2(visoS2);
  GeomAbs_CurveType GA_U1 = gau1.GetType();
  GeomAbs_CurveType GA_V1 = gav1.GetType();
  GeomAbs_CurveType GA_U2 = gau2.GetType();
  GeomAbs_CurveType GA_V2 = gav2.GetType();
  Standard_Boolean TrmU1 = Standard_False;
  Standard_Boolean TrmV1 = Standard_False;
  Standard_Boolean TrmU2 = Standard_False;
  Standard_Boolean TrmV2 = Standard_False;
  Standard_Real V1S1,V2S1,U1S1,U2S1, V1S2,V2S2,U1S2,U2S2;
  FUN_NewFirstLast(GA_U1,S1->FirstVParameter(),S1->LastVParameter(),TV,V1S1,V2S1,TrmV1);
  FUN_NewFirstLast(GA_V1,S1->FirstUParameter(),S1->LastUParameter(),TV,U1S1,U2S1,TrmU1);
  FUN_NewFirstLast(GA_U2,S2->FirstVParameter(),S2->LastVParameter(),TV,V1S2,V2S2,TrmV2);
  FUN_NewFirstLast(GA_V2,S2->FirstUParameter(),S2->LastUParameter(),TV,U1S2,U2S2,TrmU2);
  if(TrmV1) NS1 = S1->VTrim(V1S1, V2S1, 1.0e-7);
  if(TrmV2) NS2 = S2->VTrim(V1S2, V2S2, 1.0e-7);
  if(TrmU1)
  {
    if(TrmV1)
    {
      Handle(Adaptor3d_HSurface) TS = NS1;
      NS1 = TS->UTrim(U1S1, U2S1, 1.0e-7);
    }
    else NS1 = S1->UTrim(U1S1, U2S1, 1.0e-7);
  }
  if(TrmU2)
  {
    if(TrmV2)
    {
      Handle(Adaptor3d_HSurface) TS = NS2;
      NS2 = TS->UTrim(U1S2, U2S2, 1.0e-7);
    }
    else NS2 = S2->UTrim(U1S2, U2S2, 1.0e-7);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntPatch_Intersection::Perform(const Handle(Adaptor3d_HSurface)&  theS1,
                                    const Handle(Adaptor3d_TopolTool)& theD1,
                                    const Handle(Adaptor3d_HSurface)&  theS2,
                                    const Handle(Adaptor3d_TopolTool)& theD2,
                                    const Standard_Real TolArc,
                                    const Standard_Real TolTang,
                                    const Standard_Boolean isGeomInt,
                                    const Standard_Boolean theIsReqToKeepRLine)
{
  myTolArc = TolArc;
  myTolTang = TolTang;
  if(myFleche <= Precision::PConfusion())
    myFleche = 0.01;
  if(myUVMaxStep <= Precision::PConfusion())
    myUVMaxStep = 0.01;

  done = Standard_False;
  spnt.Clear();
  slin.Clear();
  empt = Standard_True;
  tgte = Standard_False;
  oppo = Standard_False;

  GeomAbs_SurfaceType typs1 = theS1->GetType();
  GeomAbs_SurfaceType typs2 = theS2->GetType();
  
  //treatment of the cases with cone or torus
  Standard_Boolean TreatAsBiParametric = Standard_False;
  Standard_Integer bGeomGeom = 0;
  //
  if (typs1 == GeomAbs_Cone  || typs2 == GeomAbs_Cone ||
      typs1 == GeomAbs_Torus || typs2 == GeomAbs_Torus) {
    gp_Ax1 aCTAx, aGeomAx;
    GeomAbs_SurfaceType aCTType;
    Standard_Boolean bToCheck;
    //
    const Handle(Adaptor3d_HSurface)& aCTSurf = 
      (typs1 == GeomAbs_Cone || typs1 == GeomAbs_Torus) ? theS1 : theS2;
    const Handle(Adaptor3d_HSurface)& aGeomSurf = 
      (typs1 == GeomAbs_Cone || typs1 == GeomAbs_Torus) ? theS2 : theS1;
    //
    aCTType = aCTSurf->GetType();
    bToCheck = Standard_False;
    //
    if (typs1 == GeomAbs_Cone  || typs2 == GeomAbs_Cone) {
      const gp_Cone aCon1 = (aCTType == GeomAbs_Cone) ? 
        aCTSurf->Cone() : aGeomSurf->Cone();
      Standard_Real a1 = Abs(aCon1.SemiAngle());
      bToCheck = (a1 < 0.02) || (a1 > 1.55);
      //
      if (typs1 == typs2) {
        const gp_Cone aCon2 = aGeomSurf->Cone();
        Standard_Real a2 = Abs(aCon2.SemiAngle());
        bToCheck = bToCheck || (a2 < 0.02) || (a2 > 1.55);
        //
        if (a1 > 1.55 && a2 > 1.55) {//quasi-planes: if same domain, treat as canonic
          const gp_Ax1 A1 = aCon1.Axis(), A2 = aCon2.Axis();
          if (A1.IsParallel(A2,Precision::Angular())) {
            const gp_Pnt Apex1 = aCon1.Apex(), Apex2 = aCon2.Apex();
            const gp_Pln Plan1( Apex1, A1.Direction() );
            if (Plan1.Distance( Apex2 ) <= Precision::Confusion()) {
              bToCheck = Standard_False;
            }
          }
        }
      }
      //
      TreatAsBiParametric = bToCheck;
      if (aCTType == GeomAbs_Cone) {
        aCTAx = aCon1.Axis();
      }
    }
    //
    if (typs1 == GeomAbs_Torus || typs2 == GeomAbs_Torus) {
      const gp_Torus aTor1 = (aCTType == GeomAbs_Torus) ? 
        aCTSurf->Torus() : aGeomSurf->Torus();
      bToCheck = aTor1.MajorRadius() > aTor1.MinorRadius();
      if (typs1 == typs2) {
        const gp_Torus aTor2 = aGeomSurf->Torus();
        bToCheck = aTor2.MajorRadius() > aTor2.MinorRadius();
      }
      //
      if (aCTType == GeomAbs_Torus) {
        aCTAx = aTor1.Axis();
      }
    }
    //
    if (bToCheck) {
      const gp_Lin aL1(aCTAx);
      //
      switch (aGeomSurf->GetType()) {
      case GeomAbs_Plane: {
        aGeomAx = aGeomSurf->Plane().Axis();
        if (aCTType == GeomAbs_Cone) {
          bGeomGeom = 1;
          if (Abs(aCTSurf->Cone().SemiAngle()) < 0.02) {
            Standard_Real ps = Abs(aCTAx.Direction().Dot(aGeomAx.Direction()));
            if(ps < 0.015) {
              bGeomGeom = 0;
            }
          }
        }
        else {
          if (aCTAx.IsParallel(aGeomAx, Precision::Angular()) ||
              (aCTAx.IsNormal(aGeomAx, Precision::Angular()) && 
               (aGeomSurf->Plane().Distance(aCTAx.Location()) < Precision::Confusion()))) {
            bGeomGeom = 1;
          }
        }
        bToCheck = Standard_False;
        break;
      }
      case GeomAbs_Sphere: {
        if (aL1.Distance(aGeomSurf->Sphere().Location()) < Precision::Confusion()) {
          bGeomGeom = 1;
        }
        bToCheck = Standard_False;
        break;
      }
      case GeomAbs_Cylinder:
        aGeomAx = aGeomSurf->Cylinder().Axis();
        break;
      case GeomAbs_Cone: 
        aGeomAx = aGeomSurf->Cone().Axis();
        break;
      case GeomAbs_Torus: 
        aGeomAx = aGeomSurf->Torus().Axis();
        break;
      default: 
        bToCheck = Standard_False;
        break;
      }
      //
      if (bToCheck) {
        if (aCTAx.IsParallel(aGeomAx, Precision::Angular()) &&
            (aL1.Distance(aGeomAx.Location()) <= Precision::Confusion())) {
          bGeomGeom = 1;
        }
      }
      //
      if (bGeomGeom == 1) {
        TreatAsBiParametric = Standard_False;
      }
    }
  }
  //

  if(theD1->DomainIsInfinite() || theD2->DomainIsInfinite()) {
    TreatAsBiParametric= Standard_False;
  }

//  Modified by skv - Mon Sep 26 14:58:30 2005 Begin
//   if(TreatAsBiParametric) { typs1 = typs2 = GeomAbs_BezierSurface; }
  if(TreatAsBiParametric)
  {
    if (typs1 == GeomAbs_Cone && typs2 == GeomAbs_Plane)
      typs1 = GeomAbs_BezierSurface; // Using Imp-Prm Intersector
    else if (typs1 == GeomAbs_Plane && typs2 == GeomAbs_Cone)
      typs2 = GeomAbs_BezierSurface; // Using Imp-Prm Intersector
    else {
      // Using Prm-Prm Intersector
      typs1 = GeomAbs_BezierSurface;
      typs2 = GeomAbs_BezierSurface;
    }
  }
//  Modified by skv - Mon Sep 26 14:58:30 2005 End

  // Surface type definition
  Standard_Integer ts1 = 0;
  switch (typs1)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone: ts1 = 1; break;
    case GeomAbs_Torus: ts1 = bGeomGeom; break;
    default: break;
  }

  Standard_Integer ts2 = 0;
  switch (typs2)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone: ts2 = 1; break;
    case GeomAbs_Torus: ts2 = bGeomGeom; break;
    default: break;
  }
  //
  // treatment of the cases with torus and any other geom surface
  //
  // Possible intersection types: 1. ts1 == ts2 == 1 <Geom-Geom>
  //                              2. ts1 != ts2      <Geom-Param>
  //                              3. ts1 == ts2 == 0 <Param-Param>

  // Geom - Geom
  if(ts1 == ts2 && ts1 == 1)
  {
    const Standard_Boolean RestrictLine = Standard_True;
    IntSurf_ListOfPntOn2S ListOfPnts;
    ListOfPnts.Clear();
    if(isGeomInt)
    {
      if(theD1->DomainIsInfinite() || theD2->DomainIsInfinite())
      {
        GeomGeomPerfom( theS1, theD1, theS2, theD2, TolArc, 
                        TolTang, ListOfPnts, RestrictLine,
                        typs1, typs2, theIsReqToKeepRLine);
      }
      else
      {
        GeomGeomPerfomTrimSurf( theS1, theD1, theS2, theD2,
                                TolArc, TolTang, ListOfPnts, RestrictLine,
                                typs1, typs2, theIsReqToKeepRLine);
      }
    }
    else
    {
      ParamParamPerfom(theS1, theD1, theS2, theD2, 
              TolArc, TolTang, ListOfPnts, RestrictLine, typs1, typs2);
    }
  }

  // Geom - Param
  if(ts1 != ts2)
  {
    GeomParamPerfom(theS1, theD1, theS2, theD2, ts1 == 0, typs1, typs2);
  }

  // Param - Param 
  if(ts1 == ts2 && ts1 == 0)
  {
    const Standard_Boolean RestrictLine = Standard_True;
    IntSurf_ListOfPntOn2S ListOfPnts;
    ListOfPnts.Clear();

    ParamParamPerfom(theS1, theD1, theS2, theD2, TolArc,
                        TolTang, ListOfPnts, RestrictLine, typs1, typs2);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntPatch_Intersection::Perform(const Handle(Adaptor3d_HSurface)&  theS1,
                                    const Handle(Adaptor3d_TopolTool)& theD1,
                                    const Handle(Adaptor3d_HSurface)&  theS2,
                                    const Handle(Adaptor3d_TopolTool)& theD2,
                                    const Standard_Real TolArc,
                                    const Standard_Real TolTang,
                                    IntSurf_ListOfPntOn2S& ListOfPnts,
                                    const Standard_Boolean RestrictLine,
                                    const Standard_Boolean isGeomInt)
{
  myTolArc = TolArc;
  myTolTang = TolTang;
  if(myFleche <= Precision::PConfusion())
    myFleche = 0.01;
  if(myUVMaxStep <= Precision::PConfusion())
    myUVMaxStep = 0.01;
    
  done = Standard_False;
  spnt.Clear();
  slin.Clear();
  empt = Standard_True;
  tgte = Standard_False;
  oppo = Standard_False;

  GeomAbs_SurfaceType typs1 = theS1->GetType();
  GeomAbs_SurfaceType typs2 = theS2->GetType();
  //
  //treatment of the cases with cone or torus
  Standard_Boolean TreatAsBiParametric = Standard_False;
  Standard_Integer bGeomGeom = 0;
  //
  if (typs1 == GeomAbs_Cone  || typs2 == GeomAbs_Cone ||
      typs1 == GeomAbs_Torus || typs2 == GeomAbs_Torus) {
    gp_Ax1 aCTAx, aGeomAx;
    GeomAbs_SurfaceType aCTType;
    Standard_Boolean bToCheck;
    //
    const Handle(Adaptor3d_HSurface)& aCTSurf = 
      (typs1 == GeomAbs_Cone || typs1 == GeomAbs_Torus) ? theS1 : theS2;
    const Handle(Adaptor3d_HSurface)& aGeomSurf = 
      (typs1 == GeomAbs_Cone || typs1 == GeomAbs_Torus) ? theS2 : theS1;
    //
    aCTType = aCTSurf->GetType();
    bToCheck = Standard_False;
    //
    if (typs1 == GeomAbs_Cone  || typs2 == GeomAbs_Cone) {
      const gp_Cone aCon1 = (aCTType == GeomAbs_Cone) ? 
        aCTSurf->Cone() : aGeomSurf->Cone();
      Standard_Real a1 = Abs(aCon1.SemiAngle());
      bToCheck = (a1 < 0.02) || (a1 > 1.55);
      //
      if (typs1 == typs2) {
        const gp_Cone aCon2 = aGeomSurf->Cone();
        Standard_Real a2 = Abs(aCon2.SemiAngle());
        bToCheck = bToCheck || (a2 < 0.02) || (a2 > 1.55);
        //
        if (a1 > 1.55 && a2 > 1.55) {//quasi-planes: if same domain, treat as canonic
          const gp_Ax1 A1 = aCon1.Axis(), A2 = aCon2.Axis();
          if (A1.IsParallel(A2,Precision::Angular())) {
            const gp_Pnt Apex1 = aCon1.Apex(), Apex2 = aCon2.Apex();
            const gp_Pln Plan1( Apex1, A1.Direction() );
            if (Plan1.Distance( Apex2 ) <= Precision::Confusion()) {
              bToCheck = Standard_False;
            }
          }
        }
      }
      //
      TreatAsBiParametric = bToCheck;
      if (aCTType == GeomAbs_Cone) {
        aCTAx = aCon1.Axis();
      }
    }
    //
    if (typs1 == GeomAbs_Torus || typs2 == GeomAbs_Torus) {
      const gp_Torus aTor1 = (aCTType == GeomAbs_Torus) ? 
        aCTSurf->Torus() : aGeomSurf->Torus();
      bToCheck = aTor1.MajorRadius() > aTor1.MinorRadius();
      if (typs1 == typs2) {
        const gp_Torus aTor2 = aGeomSurf->Torus();
        bToCheck = aTor2.MajorRadius() > aTor2.MinorRadius();
      }
      //
      if (aCTType == GeomAbs_Torus) {
        aCTAx = aTor1.Axis();
      }
    }
    //
    if (bToCheck) {
      const gp_Lin aL1(aCTAx);
      //
      switch (aGeomSurf->GetType()) {
      case GeomAbs_Plane: {
        aGeomAx = aGeomSurf->Plane().Axis();
        if (aCTType == GeomAbs_Cone) {
          bGeomGeom = 1;
          if (Abs(aCTSurf->Cone().SemiAngle()) < 0.02) {
            Standard_Real ps = Abs(aCTAx.Direction().Dot(aGeomAx.Direction()));
            if(ps < 0.015) {
              bGeomGeom = 0;
            }
          }
        }
        else {
          if (aCTAx.IsParallel(aGeomAx, Precision::Angular()) ||
              (aCTAx.IsNormal(aGeomAx, Precision::Angular()) && 
               (aGeomSurf->Plane().Distance(aCTAx.Location()) < Precision::Confusion()))) {
            bGeomGeom = 1;
          }
        }
        bToCheck = Standard_False;
        break;
      }
      case GeomAbs_Sphere: {
        if (aL1.Distance(aGeomSurf->Sphere().Location()) < Precision::Confusion()) {
          bGeomGeom = 1;
        }
        bToCheck = Standard_False;
        break;
      }
      case GeomAbs_Cylinder:
        aGeomAx = aGeomSurf->Cylinder().Axis();
        break;
      case GeomAbs_Cone: 
        aGeomAx = aGeomSurf->Cone().Axis();
        break;
      case GeomAbs_Torus: 
        aGeomAx = aGeomSurf->Torus().Axis();
        break;
      default: 
        bToCheck = Standard_False;
        break;
      }
      //
      if (bToCheck) {
        if (aCTAx.IsParallel(aGeomAx, Precision::Angular()) &&
            (aL1.Distance(aGeomAx.Location()) <= Precision::Confusion())) {
          bGeomGeom = 1;
        }
      }
      //
      if (bGeomGeom == 1) {
        TreatAsBiParametric = Standard_False;
      }
    }
  }
  //

  if(theD1->DomainIsInfinite() || theD2->DomainIsInfinite()) {
    TreatAsBiParametric= Standard_False;
  }

  if(TreatAsBiParametric)
  {
    // Using Prm-Prm Intersector
    typs1 = GeomAbs_BezierSurface;
    typs2 = GeomAbs_BezierSurface;
  }

  // Surface type definition
  Standard_Integer ts1 = 0;
  switch (typs1)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone: ts1 = 1; break;
    case GeomAbs_Torus: ts1 = bGeomGeom; break;
    default: break;
  }

  Standard_Integer ts2 = 0;
  switch (typs2)
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Sphere:
    case GeomAbs_Cone: ts2 = 1; break;
    case GeomAbs_Torus: ts2 = bGeomGeom; break;
    default: break;
  }
  //
  // Possible intersection types: 1. ts1 == ts2 == 1 <Geom-Geom>
  //                              2. ts1 != ts2      <Geom-Param>
  //                              3. ts1 == ts2 == 0 <Param-Param>

  if(!isGeomInt)
  {
    ParamParamPerfom(theS1, theD1, theS2, theD2, 
                TolArc, TolTang, ListOfPnts, RestrictLine, typs1, typs2);
  }
  else if(ts1 != ts2)
  {
    GeomParamPerfom(theS1, theD1, theS2, theD2, ts1 == 0, typs1, typs2);
  }
  else if (ts1 == 0)
  {
    ParamParamPerfom(theS1, theD1, theS2, theD2,
                TolArc, TolTang, ListOfPnts, RestrictLine, typs1, typs2);
  }
  else if(ts1 == 1)
  {
    if(theD1->DomainIsInfinite() || theD2->DomainIsInfinite())
    {
      GeomGeomPerfom(theS1, theD1, theS2, theD2, TolArc, 
                      TolTang, ListOfPnts, RestrictLine, typs1, typs2);
    }
    else
    {
      GeomGeomPerfomTrimSurf(theS1, theD1, theS2, theD2,
              TolArc, TolTang, ListOfPnts, RestrictLine, typs1, typs2);
    }
  }
}

//=======================================================================
//function : ParamParamPerfom
//purpose  : 
//=======================================================================
void IntPatch_Intersection::ParamParamPerfom(const Handle(Adaptor3d_HSurface)&  theS1,
                                             const Handle(Adaptor3d_TopolTool)& theD1,
                                             const Handle(Adaptor3d_HSurface)&  theS2,
                                             const Handle(Adaptor3d_TopolTool)& theD2,
                                             const Standard_Real TolArc,
                                             const Standard_Real TolTang,
                                             IntSurf_ListOfPntOn2S& ListOfPnts,
                                             const Standard_Boolean RestrictLine,
                                             const GeomAbs_SurfaceType typs1,
                                             const GeomAbs_SurfaceType typs2)
{
  IntPatch_PrmPrmIntersection interpp;
  if(!theD1->DomainIsInfinite() && !theD2->DomainIsInfinite())
  {
    Standard_Boolean ClearFlag = Standard_True;
    if(!ListOfPnts.IsEmpty())
    {
      interpp.Perform(theS1,theD1,theS2,theD2,TolArc,TolTang,myFleche,myUVMaxStep, ListOfPnts, RestrictLine);
      ClearFlag = Standard_False;
    }
    interpp.Perform(theS1,theD1,theS2,theD2,TolArc,TolTang,myFleche,myUVMaxStep,ClearFlag);   //double call!!!!!!!
  }
  else if((theD1->DomainIsInfinite()) ^ (theD2->DomainIsInfinite()))
  {
    gp_Pnt pMaxXYZ, pMinXYZ;
    if(theD1->DomainIsInfinite())
    {
      FUN_GetMinMaxXYZPnt( theS2, pMinXYZ, pMaxXYZ );
      const Standard_Real MU = Max(Abs(theS2->FirstUParameter()),Abs(theS2->LastUParameter()));
      const Standard_Real MV = Max(Abs(theS2->FirstVParameter()),Abs(theS2->LastVParameter()));
      const Standard_Real AP = Max(MU, MV);
      Handle(Adaptor3d_HSurface) SS;
      FUN_TrimInfSurf(pMinXYZ, pMaxXYZ, theS1, AP, SS);
      interpp.Perform(SS,theD1,theS2,theD2,TolArc,TolTang,myFleche,myUVMaxStep);
    }
    else
    {
      FUN_GetMinMaxXYZPnt( theS1, pMinXYZ, pMaxXYZ );
      const Standard_Real MU = Max(Abs(theS1->FirstUParameter()),Abs(theS1->LastUParameter()));
      const Standard_Real MV = Max(Abs(theS1->FirstVParameter()),Abs(theS1->LastVParameter()));
      const Standard_Real AP = Max(MU, MV);
      Handle(Adaptor3d_HSurface) SS;
      FUN_TrimInfSurf(pMinXYZ, pMaxXYZ, theS2, AP, SS);
      interpp.Perform(theS1, theD1, SS, theD2,TolArc,TolTang,myFleche,myUVMaxStep);
    }
  }//(theD1->DomainIsInfinite()) ^ (theD2->DomainIsInfinite())
  else
  {
    if(typs1 == GeomAbs_OtherSurface || typs2 == GeomAbs_OtherSurface)
    {
      done = Standard_False;
      return;
    }

    Standard_Boolean IsPLInt = Standard_False;
    TColgp_SequenceOfPnt sop;
    gp_Vec v;
    FUN_PL_Intersection(theS1,typs1,theS2,typs2,IsPLInt,sop,v);

    if(IsPLInt)
    {
      if(sop.Length() > 0)
      {
        for(Standard_Integer ip = 1; ip <= sop.Length(); ip++)
        {
          gp_Lin lin(sop.Value(ip),gp_Dir(v));
          Handle(IntPatch_GLine) gl = new IntPatch_GLine(lin,Standard_False);
          slin.Append(Handle(IntPatch_Line)::DownCast (gl));
        }

        done = Standard_True;
      }
      else
        done = Standard_False;

      return;
    }// 'COLLINEAR LINES'
    else
    {
      Handle(Adaptor3d_HSurface) nS1 = theS1;
      Handle(Adaptor3d_HSurface) nS2 = theS2;
      FUN_TrimBothSurf(theS1,typs1,theS2,typs2,1.e+8,nS1,nS2);
      interpp.Perform(nS1,theD1,nS2,theD2,TolArc,TolTang,myFleche,myUVMaxStep);
    }// 'NON - COLLINEAR LINES'
  }// both domains are infinite

  if (interpp.IsDone())
  {
    done = Standard_True;
    tgte = Standard_False;
    empt = interpp.IsEmpty();

    for(Standard_Integer i = 1; i <= interpp.NbLines(); i++)
    {
      if(interpp.Line(i)->ArcType() != IntPatch_Walking)
        slin.Append(interpp.Line(i));
    }

    for (Standard_Integer i = 1; i <= interpp.NbLines(); i++)
    {
      if(interpp.Line(i)->ArcType() == IntPatch_Walking)
        slin.Append(interpp.Line(i));
    }
  }
}

//=======================================================================
////function : GeomGeomPerfom
//purpose  : 
//=======================================================================
void IntPatch_Intersection::GeomGeomPerfom(const Handle(Adaptor3d_HSurface)& theS1,
                                           const Handle(Adaptor3d_TopolTool)& theD1,
                                           const Handle(Adaptor3d_HSurface)& theS2,
                                           const Handle(Adaptor3d_TopolTool)& theD2,
                                           const Standard_Real TolArc,
                                           const Standard_Real TolTang,
                                           IntSurf_ListOfPntOn2S& ListOfPnts,
                                           const Standard_Boolean RestrictLine,
                                           const GeomAbs_SurfaceType theTyps1,
                                           const GeomAbs_SurfaceType theTyps2,
                                           const Standard_Boolean theIsReqToKeepRLine)
{
  IntPatch_ImpImpIntersection interii(theS1,theD1,theS2,theD2,
                                      myTolArc,myTolTang, theIsReqToKeepRLine);
  const Standard_Boolean anIS = interii.IsDone();
  if (anIS)
  {
    done = anIS;
    empt = interii.IsEmpty();
    if (!empt)
    {
      tgte = interii.TangentFaces();
      if (tgte)
        oppo = interii.OppositeFaces();

      for (Standard_Integer i = 1; i <= interii.NbLines(); i++)
      {
        const Handle(IntPatch_Line)& line = interii.Line(i);
        if (line->ArcType() == IntPatch_Analytic)
        {
          const GeomAbs_SurfaceType aTyps1 = theS1->GetType();
          const GeomAbs_SurfaceType aTyps2 = theS2->GetType();
          IntSurf_Quadric Quad1,Quad2;
          
          switch(aTyps1)
          {
          case GeomAbs_Plane:
            Quad1.SetValue(theS1->Plane());
            break;

          case GeomAbs_Cylinder:
            Quad1.SetValue(theS1->Cylinder());
            break;

          case GeomAbs_Sphere:
            Quad1.SetValue(theS1->Sphere());
            break;

          case GeomAbs_Cone:
            Quad1.SetValue(theS1->Cone());
            break;

          case GeomAbs_Torus:
            Quad1.SetValue(theS1->Torus());
            break;

          default:
            break;
          }

          switch(aTyps2)
          {
          case GeomAbs_Plane:
            Quad2.SetValue(theS2->Plane());
            break;
          case GeomAbs_Cylinder:
            Quad2.SetValue(theS2->Cylinder());
            break;

          case GeomAbs_Sphere:
            Quad2.SetValue(theS2->Sphere());
            break;

          case GeomAbs_Cone:
            Quad2.SetValue(theS2->Cone());
            break;

          case GeomAbs_Torus:
            Quad2.SetValue(theS2->Torus());
            break;

          default:
            break;
          }

          IntPatch_ALineToWLine AToW(Quad1,Quad2,0.01,0.05,aNbPointsInALine);
          Handle(IntPatch_Line) wlin=AToW.MakeWLine((*((Handle(IntPatch_ALine) *)(&line))));
          slin.Append(wlin);
        }
        else
          slin.Append(interii.Line(i));
      }

      for (Standard_Integer i = 1; i <= interii.NbPnts(); i++)
      {
        spnt.Append(interii.Point(i));
      }
    }
  }
  else
    ParamParamPerfom(theS1, theD1, theS2, theD2, 
                TolArc, TolTang, ListOfPnts, RestrictLine, theTyps1, theTyps2);
}

//=======================================================================
//function : GeomParamPerfom
//purpose  : 
//=======================================================================
void IntPatch_Intersection::
  GeomParamPerfom(const Handle(Adaptor3d_HSurface)&  theS1,
                  const Handle(Adaptor3d_TopolTool)& theD1,
                  const Handle(Adaptor3d_HSurface)&  theS2,
                  const Handle(Adaptor3d_TopolTool)& theD2,
                  const Standard_Boolean isNotAnalitical,
                  const GeomAbs_SurfaceType typs1,
                  const GeomAbs_SurfaceType typs2)
{
  IntPatch_ImpPrmIntersection interip;
  if (myIsStartPnt)
  {
    if (isNotAnalitical/*ts1 == 0*/)
      interip.SetStartPoint(myU1Start,myV1Start);
    else
      interip.SetStartPoint(myU2Start,myV2Start);
  }

  if(theD1->DomainIsInfinite() && theD2->DomainIsInfinite())
  {
    Standard_Boolean IsPLInt = Standard_False;
    TColgp_SequenceOfPnt sop;
    gp_Vec v;
    FUN_PL_Intersection(theS1,typs1,theS2,typs2,IsPLInt,sop,v);
    
    if(IsPLInt)
    {
      if(sop.Length() > 0)
      {
        for(Standard_Integer ip = 1; ip <= sop.Length(); ip++)
        {
          gp_Lin lin(sop.Value(ip),gp_Dir(v));
          Handle(IntPatch_GLine) gl = new IntPatch_GLine(lin,Standard_False);
          slin.Append(Handle(IntPatch_Line)::DownCast (gl));
        }

        done = Standard_True;
      }
      else
        done = Standard_False;

      return;
    }
    else
    {
      Handle(Adaptor3d_HSurface) nS1 = theS1;
      Handle(Adaptor3d_HSurface) nS2 = theS2;
      FUN_TrimBothSurf(theS1,typs1,theS2,typs2,1.e+5,nS1,nS2);
      interip.Perform(nS1,theD1,nS2,theD2,myTolArc,myTolTang,myFleche,myUVMaxStep);
    }
  }
  else
    interip.Perform(theS1,theD1,theS2,theD2,myTolArc,myTolTang,myFleche,myUVMaxStep);

  if (interip.IsDone()) 
  {
    done = Standard_True;
    empt = interip.IsEmpty();

    if (!empt)
    {
      const Standard_Integer aNbLines = interip.NbLines();
      for(Standard_Integer i = 1; i <= aNbLines; i++)
      {
        if(interip.Line(i)->ArcType() != IntPatch_Walking)
          slin.Append(interip.Line(i));
      }

      for(Standard_Integer i = 1; i <= aNbLines; i++)
      {
        if(interip.Line(i)->ArcType() == IntPatch_Walking)
          slin.Append(interip.Line(i));
      }

      for (Standard_Integer i = 1; i <= interip.NbPnts(); i++)
        spnt.Append(interip.Point(i));
    }
  }
}

//=======================================================================
//function : GeomGeomPerfomTrimSurf
//purpose  : This function returns ready walking-line (which is not need
//            in convertation) as an intersection line between two
//            trimmed surfaces.
//=======================================================================
void IntPatch_Intersection::
  GeomGeomPerfomTrimSurf( const Handle(Adaptor3d_HSurface)& theS1,
                          const Handle(Adaptor3d_TopolTool)& theD1,
                          const Handle(Adaptor3d_HSurface)& theS2,
                          const Handle(Adaptor3d_TopolTool)& theD2,
                          const Standard_Real theTolArc,
                          const Standard_Real theTolTang,
                          IntSurf_ListOfPntOn2S& theListOfPnts,
                          const Standard_Boolean RestrictLine,
                          const GeomAbs_SurfaceType theTyps1,
                          const GeomAbs_SurfaceType theTyps2,
                          const Standard_Boolean theIsReqToKeepRLine)
{
  IntSurf_Quadric Quad1,Quad2;

  if((theTyps1 == GeomAbs_Cylinder) && (theTyps2 == GeomAbs_Cylinder))
  {
    IntPatch_ImpImpIntersection anInt;
    anInt.Perform(theS1, theD1, theS2, theD2, myTolArc,
                  myTolTang, Standard_True, theIsReqToKeepRLine);

    done = anInt.IsDone();

    if(done)
    {
      empt = anInt.IsEmpty();
      if (!empt)
      {
        tgte = anInt.TangentFaces();
        if (tgte)
          oppo = anInt.OppositeFaces();

        const Standard_Integer aNbLin = anInt.NbLines();
        const Standard_Integer aNbPts = anInt.NbPnts();

        for(Standard_Integer aLID = 1; aLID <= aNbLin; aLID++)
        {
          const Handle(IntPatch_Line)& aLine = anInt.Line(aLID);
          slin.Append(aLine);
        }

        for(Standard_Integer aPID = 1; aPID <= aNbPts; aPID++)
        {
          const IntPatch_Point& aPoint = anInt.Point(aPID);
          spnt.Append(aPoint);
        }

        JoinWLines( slin, spnt, theTolTang,
                    theS1->IsUPeriodic()? theS1->UPeriod() : 0.0,
                    theS2->IsUPeriodic()? theS2->UPeriod() : 0.0,
                    theS1->IsVPeriodic()? theS1->VPeriod() : 0.0,
                    theS2->IsVPeriodic()? theS2->VPeriod() : 0.0,
                    theS1->FirstUParameter(), theS1->LastUParameter(),
                    theS1->FirstVParameter(), theS1->LastVParameter(),
                    theS2->FirstUParameter(), theS2->LastUParameter(),
                    theS2->FirstVParameter(), theS2->LastVParameter());
      }
    }
  }
  else
  {
    GeomGeomPerfom(theS1, theD1, theS2, theD2,
            theTolArc, theTolTang, theListOfPnts,
            RestrictLine, theTyps1, theTyps2, theIsReqToKeepRLine);
  }
}


void IntPatch_Intersection::Perform(const Handle(Adaptor3d_HSurface)&  S1,
                                    const Handle(Adaptor3d_TopolTool)& D1,
                                    const Handle(Adaptor3d_HSurface)&  S2,
                                    const Handle(Adaptor3d_TopolTool)& D2,
                                    const Standard_Real U1,
                                    const Standard_Real V1,
                                    const Standard_Real U2,
                                    const Standard_Real V2,
                                    const Standard_Real TolArc,
                                    const Standard_Real TolTang)
{
  myTolArc = TolArc;
  myTolTang = TolTang;
  if(myFleche == 0.0) {
#if DEBUG
    //cout<<" -- IntPatch_Intersection::myFleche fixe par defaut a 0.01 --"<<endl;
    //cout<<" -- Utiliser la Methode SetTolerances( ... ) "<<endl;
#endif
    myFleche = 0.01;
  }
  if(myUVMaxStep==0.0) {
#if DEBUG
    //cout<<" -- IntPatch_Intersection::myUVMaxStep fixe par defaut a 0.01 --"<<endl;
    //cout<<" -- Utiliser la Methode SetTolerances( ... ) "<<endl;
#endif
    myUVMaxStep = 0.01;
  }

  done = Standard_False;
  spnt.Clear();
  slin.Clear();

  empt = Standard_True;
  tgte = Standard_False;
  oppo = Standard_False;

  const GeomAbs_SurfaceType typs1 = S1->GetType();
  const GeomAbs_SurfaceType typs2 = S2->GetType();
  
  if(   typs1==GeomAbs_Plane 
     || typs1==GeomAbs_Cylinder
     || typs1==GeomAbs_Sphere
     || typs1==GeomAbs_Cone
     || typs2==GeomAbs_Plane 
     || typs2==GeomAbs_Cylinder
     || typs2==GeomAbs_Sphere
     || typs2==GeomAbs_Cone)
  {
    myIsStartPnt = Standard_True;
    myU1Start = U1; myV1Start = V1; myU2Start = U2; myV2Start = V2;
    Perform(S1,D1,S2,D2,TolArc,TolTang);
    myIsStartPnt = Standard_False;
  }
  else
  {
    IntPatch_PrmPrmIntersection interpp;
    interpp.Perform(S1,D1,S2,D2,U1,V1,U2,V2,TolArc,TolTang,myFleche,myUVMaxStep);
    if (interpp.IsDone())
    {
      done = Standard_True;
      tgte = Standard_False;
      empt = interpp.IsEmpty();
      const Standard_Integer nblm = interpp.NbLines();
      Standard_Integer i = 1;
      for (; i<=nblm; i++) slin.Append(interpp.Line(i));
    }
  }
}
//======================================================================
#include <IntPatch_IType.hxx>
#include <IntPatch_LineConstructor.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <Geom_Curve.hxx>
#define MAXR 200


//void IntPatch_Intersection__MAJ_R(Handle(Adaptor2d_HCurve2d) *R1,
//                                     Handle(Adaptor2d_HCurve2d) *R2,
//                                     int *NR1,
//                                     int *NR2,
//                                     Standard_Integer nbR1,
//                                     Standard_Integer nbR2,
//                                     const IntPatch_Point& VTX)
void IntPatch_Intersection__MAJ_R(Handle(Adaptor2d_HCurve2d) *,
                                  Handle(Adaptor2d_HCurve2d) *,
                                  int *,
                                  int *,
                                  Standard_Integer ,
                                  Standard_Integer ,
                                  const IntPatch_Point& )
{ 
  /*
  if(VTX.IsOnDomS1()) { 
    
    //-- long unsigned ptr= *((long unsigned *)(((Handle(Standard_Transient) *)(&(VTX.ArcOnS1())))));
    for(Standard_Integer i=0; i<nbR1;i++) { 
      if(VTX.ArcOnS1()==R1[i]) { 
        NR1[i]++;
        printf("\n ******************************");
        return;
      }
    }
    printf("\n R Pas trouvee  (IntPatch)\n");
    
  }
  */
}


//void IntPatch_Intersection::Dump(const Standard_Integer Mode,
void IntPatch_Intersection::Dump(const Standard_Integer ,
                                 const Handle(Adaptor3d_HSurface)&  S1,
                                 const Handle(Adaptor3d_TopolTool)& D1,
                                 const Handle(Adaptor3d_HSurface)&  S2,
                                 const Handle(Adaptor3d_TopolTool)& D2) const 
{ 
  
  //-- ----------------------------------------------------------------------
  //--  construction de la liste des restrictions & vertex 
  //--
  int NR1[MAXR],NR2[MAXR];
  Handle(Adaptor2d_HCurve2d) R1[MAXR],R2[MAXR];
  Standard_Integer nbR1=0,nbR2=0;
  for(D1->Init();D1->More() && nbR1<MAXR; D1->Next()) { 
    R1[nbR1]=D1->Value(); 
    NR1[nbR1]=0;
    nbR1++;
  }
  for(D2->Init();D2->More() && nbR2<MAXR; D2->Next()) { 
    R2[nbR2]=D2->Value();
    NR2[nbR2]=0;
    nbR2++;
  }
  
  printf("\nDUMP_INT:  ----empt:%2ud  tgte:%2ud  oppo:%2ud ---------------------------------",empt,tgte,empt);
  Standard_Integer i,j,nbr1,nbr2,nbgl,nbgc,nbge,nbgp,nbgh,nbl,nbr,nbg,nbw,nba;
  nbl=nbr=nbg=nbw=nba=nbgl=nbge=nbr1=nbr2=nbgc=nbgp=nbgh=0;
  nbl=NbLines();
  for(i=1;i<=nbl;i++) { 
    const Handle(IntPatch_Line)& line=Line(i);
    const IntPatch_IType IType=line->ArcType();
    if(IType == IntPatch_Walking) nbw++;
    else     if(IType == IntPatch_Restriction) { 
      nbr++;
      Handle(IntPatch_RLine) rlin (Handle(IntPatch_RLine)::DownCast (line));
      if(rlin->IsArcOnS1()) nbr1++;
      if(rlin->IsArcOnS2()) nbr2++;
    }
    else     if(IType == IntPatch_Analytic) nba++;
    else     { 
      nbg++; 
      if(IType == IntPatch_Lin) nbgl++;
      else if(IType == IntPatch_Circle) nbgc++;
      else if(IType == IntPatch_Parabola) nbgp++;
      else if(IType == IntPatch_Hyperbola) nbgh++;
      else if(IType == IntPatch_Ellipse) nbge++;
    }
  }
  
  
  printf("\nDUMP_INT:Lines:%2d Wlin:%2d Restr:%2d(On1:%2d On2:%2d) Ana:%2d Geom:%2d(L:%2d C:%2d E:%2d H:%2d P:%2d)",
         nbl,nbw,nbr,nbr1,nbr2,nba,nbg,nbgl,nbgc,nbge,nbgh,nbgp);
  
  IntPatch_LineConstructor LineConstructor(2);
  
  Standard_Integer nbllc=0;
  nbw=nbr=nbg=nba=0;
  Standard_Integer nbva,nbvw,nbvr,nbvg;
  nbva=nbvr=nbvw=nbvg=0;
  for (j=1; j<=nbl; j++) {
    Standard_Integer v,nbvtx;
    const Handle(IntPatch_Line)& intersLinej = Line(j);
    Standard_Integer NbLines;
    LineConstructor.Perform(SequenceOfLine(),intersLinej,S1,D1,S2,D2,1e-7);
    NbLines = LineConstructor.NbLines();
    
    for(Standard_Integer k=1;k<=NbLines;k++) { 
      nbllc++;
      const Handle(IntPatch_Line)& LineK = LineConstructor.Line(k);
      if (LineK->ArcType() == IntPatch_Analytic) { 
        Handle(IntPatch_ALine) alin (Handle(IntPatch_ALine)::DownCast (LineK));
        nbvtx=alin->NbVertex();
        nbva+=nbvtx;        nba++;
        for(v=1;v<=nbvtx;v++) { 
          IntPatch_Intersection__MAJ_R(R1,R2,NR1,NR2,nbR1,nbR2,alin->Vertex(v));
        }
      }
      else if (LineK->ArcType() == IntPatch_Restriction) {
        Handle(IntPatch_RLine) rlin (Handle(IntPatch_RLine)::DownCast (LineK));
        nbvtx=rlin->NbVertex();
        nbvr+=nbvtx;        nbr++;
        for(v=1;v<=nbvtx;v++) { 
          IntPatch_Intersection__MAJ_R(R1,R2,NR1,NR2,nbR1,nbR2,rlin->Vertex(v));
        }
      }
      else if (LineK->ArcType() == IntPatch_Walking) {
        Handle(IntPatch_WLine) wlin (Handle(IntPatch_WLine)::DownCast (LineK));
        nbvtx=wlin->NbVertex();
        nbvw+=nbvtx;        nbw++;
        for(v=1;v<=nbvtx;v++) { 
          IntPatch_Intersection__MAJ_R(R1,R2,NR1,NR2,nbR1,nbR2,wlin->Vertex(v));
        }
      }
      else { 
        Handle(IntPatch_GLine) glin (Handle(IntPatch_GLine)::DownCast (LineK));
        nbvtx=glin->NbVertex();
        nbvg+=nbvtx;        nbg++;
        for(v=1;v<=nbvtx;v++) { 
          IntPatch_Intersection__MAJ_R(R1,R2,NR1,NR2,nbR1,nbR2,glin->Vertex(v));
        }
      }
    }
  }
  printf("\nDUMP_LC :Lines:%2d WLin:%2d Restr:%2d Ana:%2d Geom:%2d",
         nbllc,nbw,nbr,nba,nbg);
  printf("\nDUMP_LC :vtx          :%2d     r:%2d    :%2d     :%2d",
         nbvw,nbvr,nbva,nbvg);



   printf("\n");
}
