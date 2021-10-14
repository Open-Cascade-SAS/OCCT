// Created on: 1997-09-22
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _ProjLib_CompProjectedCurve_HeaderFile
#define _ProjLib_CompProjectedCurve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <ProjLib_HSequenceOfHSequenceOfPnt.hxx>
#include <ProjLib_Projector.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_CurveType.hxx>

class gp_Pnt2d;
class gp_Vec2d;

class ProjLib_CompProjectedCurve  : public Adaptor2d_Curve2d
{
  DEFINE_STANDARD_RTTIEXT(ProjLib_CompProjectedCurve, Adaptor2d_Curve2d)
public:

  Standard_EXPORT ProjLib_CompProjectedCurve();
  
  //! try to find all solutions
  Standard_EXPORT ProjLib_CompProjectedCurve(const Handle(Adaptor3d_Surface)& S, const Handle(Adaptor3d_Curve)& C, const Standard_Real TolU, const Standard_Real TolV);
  
  //! this constructor tries to optimize the search using the
  //! assumption that maximum distance between surface and curve less or
  //! equal then MaxDist.
  //! if MaxDist < 0 then algorithm works as above.
  Standard_EXPORT ProjLib_CompProjectedCurve(const Handle(Adaptor3d_Surface)& S, const Handle(Adaptor3d_Curve)& C, const Standard_Real TolU, const Standard_Real TolV, const Standard_Real MaxDist);

  //! this constructor tries to optimize the search using the
  //! assumption that maximum distance between surface and curve less or
  //! equal then MaxDist.
  //! if MaxDist < 0 then algorithm try to find all solutions
  //! Tolerances of parameters are calculated automatically.
  Standard_EXPORT ProjLib_CompProjectedCurve(const Standard_Real Tol3d, const Handle(Adaptor3d_Surface)& S, const Handle(Adaptor3d_Curve)& C, const Standard_Real MaxDist = -1.0);
  
  //! Shallow copy of adaptor
  Standard_EXPORT virtual Handle(Adaptor2d_Curve2d) ShallowCopy() const Standard_OVERRIDE;

  //! computes a set of projected point and determine the
  //! continuous parts of the projected  curves. The  points
  //! corresponding to a projection on the bounds of the surface are
  //! included  in this set of points.
  Standard_EXPORT void Init();
  
  //! Performs projecting for given curve.
  //! If projecting uses approximation, 
  //! approximation parameters can be set before by corresponding methods 
  //! SetTol3d(...), SeContinuity(...), SetMaxDegree(...), SetMaxSeg(...)
  Standard_EXPORT void Perform();

  //! Set the parameter, which defines 3d tolerance of approximation.
  Standard_EXPORT void SetTol3d(const Standard_Real theTol3d);

  //! Set the parameter, which defines curve continuity.
  //! Default value is GeomAbs_C2;
  Standard_EXPORT void SetContinuity(const GeomAbs_Shape theContinuity);

  //! Set max possible degree of result BSpline curve2d, which is got by approximation.
  //! If MaxDegree < 0, algorithm uses values that are chosen depending of types curve 3d
  //! and surface.
  Standard_EXPORT void SetMaxDegree(const Standard_Integer theMaxDegree);

  //! Set the parameter, which defines maximal value of parametric intervals the projected
  //! curve can be cut for approximation. If MaxSeg < 0, algorithm uses default 
  //! value = 16.
  Standard_EXPORT void SetMaxSeg(const Standard_Integer theMaxSeg);

  //! Set the parameter, which defines necessity of 2d results.
  Standard_EXPORT void SetProj2d(const Standard_Boolean theProj2d);

  //! Set the parameter, which defines necessity of 3d results.
  Standard_EXPORT void SetProj3d(const Standard_Boolean theProj3d);

  //! Changes the surface.
  Standard_EXPORT void Load (const Handle(Adaptor3d_Surface)& S);
  
  //! Changes the  curve.
  Standard_EXPORT void Load (const Handle(Adaptor3d_Curve)& C);
  
  Standard_EXPORT const Handle(Adaptor3d_Surface)& GetSurface() const;
  
  Standard_EXPORT const Handle(Adaptor3d_Curve)& GetCurve() const;
  
  Standard_EXPORT void GetTolerance (Standard_Real& TolU, Standard_Real& TolV) const;
  
  //! returns the number of continuous part of the projected curve
  Standard_EXPORT Standard_Integer NbCurves() const;
  
  //! returns the bounds of the continuous part corresponding to Index
  Standard_EXPORT void Bounds (const Standard_Integer Index, Standard_Real& Udeb, Standard_Real& Ufin) const;
  
  //! returns  True  if  part  of  projection with  number  Index is  a  single  point  and  writes  its  coordinates in  P
  Standard_EXPORT Standard_Boolean IsSinglePnt (const Standard_Integer Index, gp_Pnt2d& P) const;
  
  //! returns  True  if  part  of  projection with  number  Index is  an  u-isoparametric curve  of  input  surface
  Standard_EXPORT Standard_Boolean IsUIso (const Standard_Integer Index, Standard_Real& U) const;
  
  //! returns  True  if  part  of  projection with  number  Index is  an  v-isoparametric curve  of  input  surface
  Standard_EXPORT Standard_Boolean IsVIso (const Standard_Integer Index, Standard_Real& V) const;
  
  //! Computes the point of parameter U on the curve.
  Standard_EXPORT gp_Pnt2d Value (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if N < 1.
  //! Raised if N > 2.
  Standard_EXPORT gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Returns  the  first  parameter of  the  curve  C
  //! which  has  a  projection  on  S.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! Returns  the  last  parameter of  the  curve  C
  //! which  has  a  projection  on  S.
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  //! Returns the Continuity used in the approximation.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;

  //! Returns  the number  of  intervals which  define
  //! an  S  continuous  part  of  the  projected  curve
  Standard_EXPORT Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Returns    a  curve equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 2d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor2d_Curve2d) Trim (const Standard_Real FirstParam, const Standard_Real LastParam, const Standard_Real Tol) const Standard_OVERRIDE;
  
  //! Returns  the  parameters  corresponding  to
  //! S  discontinuities.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! returns  the  maximum  distance  between
  //! curve  to  project  and  surface
  Standard_EXPORT Standard_Real MaxDistance (const Standard_Integer Index) const;
  
  Standard_EXPORT const Handle(ProjLib_HSequenceOfHSequenceOfPnt)& GetSequence() const;
  
  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT GeomAbs_CurveType GetType() const Standard_OVERRIDE;

  //! Returns true if result of projecting of the curve interval
  //! with number Index is point.
  Standard_EXPORT Standard_Boolean ResultIsPoint(const Standard_Integer theIndex) const;

  //! Returns the error of approximation of U parameter 2d-curve as a result
  //! projecting of the curve interval with number Index.
  Standard_EXPORT Standard_Real GetResult2dUApproxError(const Standard_Integer theIndex) const;

  //! Returns the error of approximation of V parameter 2d-curve as a result
  //! projecting of the curve interval with number Index.
  Standard_EXPORT Standard_Real GetResult2dVApproxError(const Standard_Integer theIndex) const;

  //! Returns the error of approximation of 3d-curve as a result
  //! projecting of the curve interval with number Index.
  Standard_EXPORT Standard_Real GetResult3dApproxError(const Standard_Integer theIndex) const;

  //! Returns the resulting 2d-curve of projecting
  //! of the curve interval with number Index.
  Standard_EXPORT Handle(Geom2d_Curve) GetResult2dC(const Standard_Integer theIndex) const;

  //! Returns the resulting 3d-curve of projecting
  //! of the curve interval with number Index.
  Standard_EXPORT Handle(Geom_Curve) GetResult3dC(const Standard_Integer theIndex) const;

  //! Returns the resulting 2d-point of projecting
  //! of the curve interval with number Index.
  Standard_EXPORT gp_Pnt2d GetResult2dP(const Standard_Integer theIndex) const;

  //! Returns the resulting 3d-point of projecting
  //! of the curve interval with number Index.
  Standard_EXPORT gp_Pnt GetResult3dP(const Standard_Integer theIndex) const;

  //! Returns the parameter, which defines necessity of only 2d results.
  Standard_Boolean GetProj2d() const { return myProj2d; }

  //! Returns the parameter, which defines necessity of only 3d results.
  Standard_Boolean GetProj3d() const { return myProj3d; }

private:

  //! This method performs check possibility of optimization traps and tries to go out from them.
  //@return thePoint - input / corrected point.
  Standard_EXPORT void UpdateTripleByTrapCriteria(gp_Pnt &thePoint) const;

  Standard_EXPORT void BuildIntervals (const GeomAbs_Shape S) const;

private:

  Handle(Adaptor3d_Surface) mySurface;
  Handle(Adaptor3d_Curve) myCurve;
  Standard_Integer myNbCurves;
  Handle(ProjLib_HSequenceOfHSequenceOfPnt) mySequence;
  Handle(TColStd_HArray1OfBoolean) myUIso;
  Handle(TColStd_HArray1OfBoolean) myVIso;
  Handle(TColStd_HArray1OfBoolean) mySnglPnts;
  Handle(TColStd_HArray1OfReal) myMaxDistance;
  Handle(TColStd_HArray1OfReal) myTabInt;
  Standard_Real    myTol3d;
  GeomAbs_Shape    myContinuity;
  Standard_Integer myMaxDegree;
  Standard_Integer myMaxSeg;
  Standard_Boolean myProj2d;
  Standard_Boolean myProj3d;
  Standard_Real myMaxDist;
  Standard_Real myTolU;
  Standard_Real myTolV;

  Handle(TColStd_HArray1OfBoolean)  myResultIsPoint;
  Handle(TColStd_HArray1OfReal)     myResult2dUApproxError;
  Handle(TColStd_HArray1OfReal)     myResult2dVApproxError;
  Handle(TColStd_HArray1OfReal)     myResult3dApproxError;
  Handle(TColgp_HArray1OfPnt)       myResult3dPoint;
  Handle(TColgp_HArray1OfPnt2d)     myResult2dPoint;
  Handle(TColGeom_HArray1OfCurve)   myResult3dCurve;
  Handle(TColGeom2d_HArray1OfCurve) myResult2dCurve;
};

DEFINE_STANDARD_HANDLE(ProjLib_CompProjectedCurve, Adaptor2d_Curve2d)

#endif // _ProjLib_CompProjectedCurve_HeaderFile
