// Created on: 1992-04-03
// Created by: Isabelle GRIGNON
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntWalk_PWalking_HeaderFile
#define _IntWalk_PWalking_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <gp_Dir.hxx>
#include <Standard_Real.hxx>
#include <IntImp_ConstIsoparametric.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <gp_Dir2d.hxx>
#include <IntWalk_TheInt2S.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <IntWalk_StatusDeflection.hxx>
class IntSurf_LineOn2S;
class Standard_OutOfRange;
class StdFail_NotDone;
class Adaptor3d_HSurface;
class IntSurf_PntOn2S;
class gp_Dir;
class gp_Pnt;


//! This class implements an algorithm to determine the
//! intersection between 2 parametrized surfaces, marching from
//! a starting point. The intersection line
//! starts and ends on the natural surface's  boundaries .
class IntWalk_PWalking 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructor used to set the data to compute intersection
  //! lines between Caro1 and Caro2.
  //! Deflection is the maximum deflection admitted between two
  //! consecutive points on the resulting polyline.
  //! TolTangency is the tolerance to find a tangent point.
  //! Func is the criterion which has to be evaluated at each
  //! solution point (each point of the line).
  //! It is necessary to call the Perform method to compute
  //! the intersection lines.
  //! The line found starts at a point on or in 2 natural domains
  //! of surfaces. It can be closed in the
  //! standard case if it is open it stops and begins at the
  //! border of one of the domains. If an open line
  //! stops at the middle of a domain, one stops at the tangent point.
  //! Epsilon is SquareTolerance of points confusion.
  Standard_EXPORT IntWalk_PWalking(const Handle(Adaptor3d_HSurface)& Caro1, const Handle(Adaptor3d_HSurface)& Caro2, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment);
  
  //! Returns the intersection line containing the exact
  //! point Poin. This line is a polygonal line.
  //! Deflection is the maximum deflection admitted between two
  //! consecutive points on the resulting polyline.
  //! TolTangency is the tolerance to find a tangent point.
  //! Func is the criterion which has to be evaluated at each
  //! solution point (each point of the line).
  //! The line found starts at a point on or in 2 natural domains
  //! of surfaces. It can be closed in the
  //! standard case if it is open it stops and begins at the
  //! border of one of the domains. If an open line
  //! stops at the middle of a domain, one stops at the tangent point.
  //! Epsilon is SquareTolerance of points confusion.
  Standard_EXPORT IntWalk_PWalking(const Handle(Adaptor3d_HSurface)& Caro1, const Handle(Adaptor3d_HSurface)& Caro2, const Standard_Real TolTangency, const Standard_Real Epsilon, const Standard_Real Deflection, const Standard_Real Increment, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  //! calculate the line of intersection
  Standard_EXPORT void Perform (const TColStd_Array1OfReal& ParDep);
  
  //! calculate the line of intersection. The regulation
  //! of steps is done using min and max values on u and
  //! v.  (if this data is not presented as in the
  //! previous method, the initial steps are calculated
  //! starting from min and max uv of faces).
  Standard_EXPORT void Perform (const TColStd_Array1OfReal& ParDep, const Standard_Real u1min, const Standard_Real v1min, const Standard_Real u2min, const Standard_Real v2min, const Standard_Real u1max, const Standard_Real v1max, const Standard_Real u2max, const Standard_Real v2max);
  
  //! calculate the first point of a line of intersection
  Standard_EXPORT Standard_Boolean PerformFirstPoint (const TColStd_Array1OfReal& ParDep, IntSurf_PntOn2S& FirstPoint);
  
  //! Returns true if the calculus was successful.
    Standard_Boolean IsDone() const;
  
  //! Returns the number of points of the resulting polyline.
  //! An exception is raised if IsDone returns False.
    Standard_Integer NbPoints() const;
  
  //! Returns the point of range Index on the polyline.
  //! An exception is raised if IsDone returns False.
  //! An exception is raised if Index<=0 or Index>NbPoints.
    const IntSurf_PntOn2S& Value (const Standard_Integer Index) const;
  
    const Handle(IntSurf_LineOn2S)& Line() const;
  
  //! Returns True if the surface are tangent at the first point
  //! of the line.
  //! An exception is raised if IsDone returns False.
    Standard_Boolean TangentAtFirst() const;
  
  //! Returns true if the surface are tangent at the last point
  //! of the line.
  //! An exception is raised if IsDone returns False.
    Standard_Boolean TangentAtLast() const;
  
  //! Returns True if the line is closed.
  //! An exception is raised if IsDone returns False.
    Standard_Boolean IsClosed() const;
  
    const gp_Dir& TangentAtLine (Standard_Integer& Index) const;
  
  Standard_EXPORT   IntWalk_StatusDeflection TestDeflection (const IntImp_ConstIsoparametric ChoixIso) ;
  
  Standard_EXPORT Standard_Boolean TestArret (const Standard_Boolean DejaReparti, TColStd_Array1OfReal& Param, IntImp_ConstIsoparametric& ChoixIso);
  
  Standard_EXPORT void RepartirOuDiviser (Standard_Boolean& DejaReparti, IntImp_ConstIsoparametric& ChoixIso, Standard_Boolean& Arrive);
  
  //! Inserts thePOn2S in the end of line
  void AddAPoint (const IntSurf_PntOn2S& thePOn2S);

  //! Removes point with index theIndex from line.
  //! If theIndex is greater than the number of points in line
  //! then the last point will be removed.
  //! theIndex must be started with 1.
  void RemoveAPoint(const Standard_Integer theIndex)
  {
    const Standard_Integer anIdx = Min(theIndex, line->NbPoints());
    
    if (anIdx < 1)
      return;

    if (anIdx <= myTangentIdx)
    {
      myTangentIdx--;

      if (myTangentIdx < 1)
        myTangentIdx = 1;
    }

    line->RemovePoint(anIdx);
  }
  
  Standard_EXPORT Standard_Boolean PutToBoundary (const Handle(Adaptor3d_HSurface)& theASurf1, const Handle(Adaptor3d_HSurface)& theASurf2);
  
  Standard_EXPORT Standard_Boolean SeekAdditionalPoints (const Handle(Adaptor3d_HSurface)& theASurf1, const Handle(Adaptor3d_HSurface)& theASurf2, const Standard_Integer theMinNbPoints);

  Standard_Real MaxStep(Standard_Integer theIndex)
  {
    Standard_OutOfRange_Raise_if ((theIndex < 0) || (theIndex > 3),
                                  "IntWalk_PWalking::MaxStep() - index is out of range");
    return pasInit[theIndex];
  }



protected:
  Standard_EXPORT void ComputePasInit(const Standard_Real theDeltaU1,
                                      const Standard_Real theDeltaV1,
                                      const Standard_Real theDeltaU2,
                                      const Standard_Real theDeltaV2);

  //! Uses Gradient method in order to find intersection point between the given surfaces
  //! Arrays theInit (initial point to be precise) and theStep0 (steps-array) must contain
  //! four items and must be filled strictly in following order:
  //! {U-parameter on S1, V-parameter on S1, U-parameter on S2, V-parameter on S2}
  Standard_EXPORT Standard_Boolean DistanceMinimizeByGradient(const Handle(Adaptor3d_HSurface)& theASurf1,
                                                              const Handle(Adaptor3d_HSurface)& theASurf2,
                                                              TColStd_Array1OfReal& theInit,
                                                              const Standard_Real* theStep0 = 0);
  
  //! Finds the point on theASurf which is the nearest point to theP0.
  //! theU0 and theV0 must be initialized (before calling the method) by initial
  //! parameters on theASurf. Their values are changed while algorithm being launched.
  //! Array theStep0 (steps-array) must contain two items and must be filled strictly in following order:
  //! {U-parameter, V-parameter}
  Standard_EXPORT Standard_Boolean DistanceMinimizeByExtrema (const Handle(Adaptor3d_HSurface)& theASurf,
                                                              const gp_Pnt& theP0,                                                              
                                                              Standard_Real& theU0,
                                                              Standard_Real& theV0,
                                                              const Standard_Real* theStep0 = 0);
  
  //! Searches an intersection point which lies on the some surface boundary.
  //! Found point (in case of successful result) is added in the line.
  //! theU1, theV1, theU2 and theV2 parameters are initial parameters in
  //! for used numeric algorithms. If isTheFirst == TRUE then
  //! a point on theASurf1 is searched. Otherwise, the point on theASurf2 is searched.
  //!
  //! ATTENTION!!!
  //!   This method can delete some points from the curve if it is necessary
  //!   (in order to obtain correct result after insertion).
  //!   Returns TRUE in case of success adding (i.e. can return FALSE even after
  //!   removing some points).
  Standard_EXPORT Standard_Boolean SeekPointOnBoundary (const Handle(Adaptor3d_HSurface)& theASurf1,
                                                        const Handle(Adaptor3d_HSurface)& theASurf2,
                                                        const Standard_Real theU1,
                                                        const Standard_Real theV1,
                                                        const Standard_Real theU2,
                                                        const Standard_Real theV2,
                                                        const Standard_Boolean isTheFirst);


  // Method to handle single singular point. Sub-method in SeekPointOnBoundary.
  Standard_EXPORT Standard_Boolean HandleSingleSingularPoint(const Handle(Adaptor3d_HSurface) &theASurf1,
                                                             const Handle(Adaptor3d_HSurface) &theASurf2,
                                                             const Standard_Real the3DTol,
                                                             TColStd_Array1OfReal &thePnt);
  
  Standard_EXPORT Standard_Boolean ExtendLineInCommonZone (const IntImp_ConstIsoparametric theChoixIso,
                                                           const Standard_Boolean theDirectionFlag);

private:
  Standard_Boolean done;
  Handle(IntSurf_LineOn2S) line;
  Standard_Boolean close;
  Standard_Boolean tgfirst;
  Standard_Boolean tglast;

  //! Index of point on the surface boundary.
  //! It is used for transition computation
  Standard_Integer myTangentIdx;

  //! Tangent to WLine in the point with index myTangentIdx
  gp_Dir tgdir;

  Standard_Real fleche;
  Standard_Real pasMax;
  Standard_Real tolconf;
  Standard_Real myTolTang;
  Standard_Real pasuv[4];
  Standard_Real myStepMin[4];
  Standard_Real pasSav[4];
  Standard_Real pasInit[4];
  Standard_Real Um1;
  Standard_Real UM1;
  Standard_Real Vm1;
  Standard_Real VM1;
  Standard_Real Um2;
  Standard_Real UM2;
  Standard_Real Vm2;
  Standard_Real VM2;
  Standard_Real ResoU1;
  Standard_Real ResoU2;
  Standard_Real ResoV1;
  Standard_Real ResoV2;
  Standard_Integer sensCheminement;
  IntImp_ConstIsoparametric choixIsoSav;
  IntSurf_PntOn2S previousPoint;
  Standard_Boolean previoustg;
  gp_Dir previousd;
  gp_Dir2d previousd1;
  gp_Dir2d previousd2;
  gp_Dir2d firstd1;
  gp_Dir2d firstd2;
  IntWalk_TheInt2S myIntersectionOn2S;
  Standard_Integer STATIC_BLOCAGE_SUR_PAS_TROP_GRAND;
  Standard_Integer STATIC_PRECEDENT_INFLEXION;


};


#include <IntWalk_PWalking.lxx>





#endif // _IntWalk_PWalking_HeaderFile
