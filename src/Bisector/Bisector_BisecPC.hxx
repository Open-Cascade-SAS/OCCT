// Created on: 1994-01-10
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Bisector_BisecPC_HeaderFile
#define _Bisector_BisecPC_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Bisector_Curve.hxx>
#include <GeomAbs_Shape.hxx>
class Geom2d_Curve;
class Standard_DomainError;
class Standard_RangeError;
class gp_Pnt2d;
class Geom2d_Geometry;
class gp_Trsf2d;
class gp_Vec2d;


class Bisector_BisecPC;
DEFINE_STANDARD_HANDLE(Bisector_BisecPC, Bisector_Curve)

//! Provides the bisector between a point and a curve.
//! the curvature on the curve has to be monoton.
//! the point can't be on the curve exept at the extremitys.
class Bisector_BisecPC : public Bisector_Curve
{

public:

  
  Standard_EXPORT Bisector_BisecPC();
  
  //! Constructs the bisector between the point <P> and
  //! the curve <Cu>.
  //! <Side> = 1. if the bisector curve is on the Left of <Cu>
  //! else <Side> = -1.
  //! <DistMax> is used to trim the bisector.The distance
  //! between the points of the bisector and <Cu> is smaller
  //! than <DistMax>.
  Standard_EXPORT Bisector_BisecPC(const Handle(Geom2d_Curve)& Cu, const gp_Pnt2d& P, const Standard_Real Side, const Standard_Real DistMax = 500);
  
  //! Constructs the bisector between the point <P> and
  //! the curve <Cu> Trimmed by <UMin> and <UMax>
  //! <Side> = 1. if the bisector curve is on the Left of <Cu>
  //! else <Side> = -1.
  //! Warning: the bisector is supposed all over defined between
  //! <UMin> and <UMax>.
  Standard_EXPORT Bisector_BisecPC(const Handle(Geom2d_Curve)& Cu, const gp_Pnt2d& P, const Standard_Real Side, const Standard_Real UMin, const Standard_Real UMax);
  
  //! Construct the bisector between the point <P> and
  //! the curve <Cu>.
  //! <Side> = 1. if the bisector curve is on the Left of <Cu>
  //! else <Side> = -1.
  //! <DistMax> is used to trim the bisector.The distance
  //! between the points of the bisector and <Cu> is smaller
  //! than <DistMax>.
  Standard_EXPORT void Perform (const Handle(Geom2d_Curve)& Cu, const gp_Pnt2d& P, const Standard_Real Side, const Standard_Real DistMax = 500);
  
  //! Returns True if the bisector is extended at start.
  Standard_EXPORT Standard_Boolean IsExtendAtStart() const Standard_OVERRIDE;
  
  //! Returns True if the bisector is extended at end.
  Standard_EXPORT Standard_Boolean IsExtendAtEnd() const Standard_OVERRIDE;
  

  //! Changes the direction of parametrization of <me>.
  //! The orientation  of the curve is modified. If the curve
  //! is bounded the StartPoint of the initial curve becomes the
  //! EndPoint of the reversed curve  and the EndPoint of the initial
  //! curve becomes the StartPoint of the reversed curve.
  Standard_EXPORT void Reverse() Standard_OVERRIDE;
  
  //! Returns the  parameter on the  reversed  curve for
  //! the point of parameter U on <me>.
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;
  

  //! Transformation of a geometric object. This tansformation
  //! can be a translation, a rotation, a symmetry, a scaling
  //! or a complex transformation obtained by combination of
  //! the previous elementaries transformations.
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  
  //! Returns the order of continuity of the curve.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCN (const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Value of the first parameter.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! Value of the last parameter.
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! If necessary,  breaks the  curve in  intervals  of
  //! continuity  <C1>.    And  returns   the number   of
  //! intervals.
  Standard_EXPORT Standard_Integer NbIntervals() const Standard_OVERRIDE;
  
  //! Returns  the  first  parameter    of  the  current
  //! interval.
  Standard_EXPORT Standard_Real IntervalFirst (const Standard_Integer Index) const Standard_OVERRIDE;
  
  //! Returns  the  last  parameter    of  the  current
  //! interval.
  Standard_EXPORT Standard_Real IntervalLast (const Standard_Integer Index) const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape IntervalContinuity() const;
  
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  //! Returns   the   distance   between  the  point  of
  //! parameter U on <me> and my point or my curve.
  Standard_EXPORT Standard_Real Distance (const Standard_Real U) const;
  
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V) const Standard_OVERRIDE;
  
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  Standard_EXPORT void Dump (const Standard_Integer Deep = 0, const Standard_Integer Offset = 0) const;
  
  //! Returns the parameter on the curve1 of the projection
  //! of the point of parameter U on <me>.
  Standard_EXPORT Standard_Real LinkBisCurve (const Standard_Real U) const;
  
  //! Returns the reciproque of LinkBisCurve.
  Standard_EXPORT Standard_Real LinkCurveBis (const Standard_Real U) const;
  
  //! Returns the parameter on <me> corresponding to <P>.
  Standard_EXPORT Standard_Real Parameter (const gp_Pnt2d& P) const Standard_OVERRIDE;
  
  //! Returns <True> if the bisector is empty.
  Standard_EXPORT Standard_Boolean IsEmpty() const;




  DEFINE_STANDARD_RTTIEXT(Bisector_BisecPC,Bisector_Curve)

protected:




private:

  
  Standard_EXPORT void Values (const Standard_Real U, const Standard_Integer N, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const;
  
  Standard_EXPORT void Extension (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const;
  
  //! Computes the interval where the bisector is defined.
  Standard_EXPORT void ComputeIntervals();
  
  Standard_EXPORT void CuspFilter();
  
  Standard_EXPORT Standard_Real SearchBound (const Standard_Real U1, const Standard_Real U2) const;
  
  Standard_EXPORT void Init (const Handle(Geom2d_Curve)& Curve, const gp_Pnt2d& Point, const Standard_Real Sign, const TColStd_SequenceOfReal& StartIntervals, const TColStd_SequenceOfReal& EndIntervals, const Standard_Integer BisInterval, const Standard_Integer CurrentInterval, const Standard_Real ShiftParameter, const Standard_Real DistMax, const Standard_Boolean IsEmpty, const Standard_Boolean IsConvex, const Standard_Boolean ExtensionStart, const Standard_Boolean ExtensionEnd, const gp_Pnt2d& PointStartBis, const gp_Pnt2d& PointEndBis);

  Handle(Geom2d_Curve) curve;
  gp_Pnt2d point;
  Standard_Real sign;
  TColStd_SequenceOfReal startIntervals;
  TColStd_SequenceOfReal endIntervals;
  Standard_Integer bisInterval;
  Standard_Integer currentInterval;
  Standard_Real shiftParameter;
  Standard_Real distMax;
  Standard_Boolean isEmpty;
  Standard_Boolean isConvex;
  Standard_Boolean extensionStart;
  Standard_Boolean extensionEnd;
  gp_Pnt2d pointStartBis;
  gp_Pnt2d pointEndBis;


};







#endif // _Bisector_BisecPC_HeaderFile
