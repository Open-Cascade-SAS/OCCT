// Created on: 1994-05-19
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

#ifndef _Bisector_BisecAna_HeaderFile
#define _Bisector_BisecAna_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Bisector_Curve.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_JoinType.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
class Geom2d_TrimmedCurve;
class Geom2d_Curve;
class gp_Pnt2d;
class gp_Vec2d;
class Geom2d_Point;
class GccInt_Bisec;
class Geom2d_Geometry;
class gp_Trsf2d;


class Bisector_BisecAna;
DEFINE_STANDARD_HANDLE(Bisector_BisecAna, Bisector_Curve)

//! This class provides the bisecting line between two
//! geometric elements.The elements are Circles,Lines or
//! Points.
class Bisector_BisecAna : public Bisector_Curve
{

public:

  
  Standard_EXPORT Bisector_BisecAna();
  
  //! Performs  the bisecting line  between the  curves
  //! <Cu1> and <Cu2>.
  //! <oncurve> is True if the point <P> is common to <Cu1>
  //! and <Cu2>.
  Standard_EXPORT void Perform (const Handle(Geom2d_Curve)& Cu1, const Handle(Geom2d_Curve)& Cu2, const gp_Pnt2d& P, const gp_Vec2d& V1, const gp_Vec2d& V2, const Standard_Real Sense, const GeomAbs_JoinType jointype, const Standard_Real Tolerance, const Standard_Boolean oncurve = Standard_True);
  
  //! Performs  the bisecting line  between the  curve
  //! <Cu1> and the point <Pnt>.
  //! <oncurve> is True if the point <P> is the point <Pnt>.
  Standard_EXPORT void Perform (const Handle(Geom2d_Curve)& Cu, const Handle(Geom2d_Point)& Pnt, const gp_Pnt2d& P, const gp_Vec2d& V1, const gp_Vec2d& V2, const Standard_Real Sense, const Standard_Real Tolerance, const Standard_Boolean oncurve = Standard_True);
  
  //! Performs  the bisecting line  between the  curve
  //! <Cu> and the point <Pnt>.
  //! <oncurve> is True if the point <P> is the point <Pnt>.
  Standard_EXPORT void Perform (const Handle(Geom2d_Point)& Pnt, const Handle(Geom2d_Curve)& Cu, const gp_Pnt2d& P, const gp_Vec2d& V1, const gp_Vec2d& V2, const Standard_Real Sense, const Standard_Real Tolerance, const Standard_Boolean oncurve = Standard_True);
  
  //! Performs  the bisecting line  between the two points
  //! <Pnt1>  and <Pnt2>.
  Standard_EXPORT void Perform (const Handle(Geom2d_Point)& Pnt1, const Handle(Geom2d_Point)& Pnt2, const gp_Pnt2d& P, const gp_Vec2d& V1, const gp_Vec2d& V2, const Standard_Real Sense, const Standard_Real Tolerance = 0.0, const Standard_Boolean oncurve = Standard_True);
  
  Standard_EXPORT void Init (const Handle(Geom2d_TrimmedCurve)& bisector);
  
  Standard_EXPORT Standard_Boolean IsExtendAtStart() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsExtendAtEnd() const Standard_OVERRIDE;
  
  //! Trim <me> by a domain defined by the curve <Cu>.
  //! This domain is the set of the points which are
  //! nearest from <Cu> than the extremitis of <Cu>.
  Standard_EXPORT void SetTrim (const Handle(Geom2d_Curve)& Cu);
  
  //! Trim <me> by a domain defined by uf  and  ul
  Standard_EXPORT void SetTrim (const Standard_Real uf, const Standard_Real ul);
  
  Standard_EXPORT void Reverse() Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Returns the order of continuity of the curve.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCN (const Standard_Integer N) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;
  
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1) const Standard_OVERRIDE;
  
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom2d_Curve) Geom2dCurve() const;
  
  Standard_EXPORT Standard_Real Parameter (const gp_Pnt2d& P) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real ParameterOfStartPoint() const;
  
  Standard_EXPORT Standard_Real ParameterOfEndPoint() const;
  
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
  
  Standard_EXPORT void Dump (const Standard_Integer Deep = 0, const Standard_Integer Offset = 0) const;




  DEFINE_STANDARD_RTTIEXT(Bisector_BisecAna,Bisector_Curve)

protected:




private:

  
  //! Returns the distance between the point <P> and
  //! the bisecting <Bis>.
  Standard_EXPORT Standard_Real Distance (const gp_Pnt2d& P, const Handle(GccInt_Bisec)& Bis, const gp_Vec2d& V1, const gp_Vec2d& V2, const gp_Vec2d& VecRef, const Standard_Real Sense, Standard_Real& U, Standard_Boolean& sense, Standard_Boolean& ok, const Standard_Boolean IsBisecOfTwoLines = Standard_False);

  Handle(Geom2d_TrimmedCurve) thebisector;


};







#endif // _Bisector_BisecAna_HeaderFile
