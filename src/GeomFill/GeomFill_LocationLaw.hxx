// Created on: 1997-11-20
// Created by: Philippe MANGIN
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

#ifndef _GeomFill_LocationLaw_HeaderFile
#define _GeomFill_LocationLaw_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomFill_PipeError.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <TColStd_Array1OfReal.hxx>

class gp_Mat;
class gp_Vec;
class gp_Pnt;

class GeomFill_LocationLaw;
DEFINE_STANDARD_HANDLE(GeomFill_LocationLaw, Standard_Transient)

//! To define location  law in Sweeping location is --
//! defined   by an  Matrix  M and  an Vector  V,  and
//! transform an point P in MP+V.
class GeomFill_LocationLaw : public Standard_Transient
{

public:
  //! initialize curve of location law
  Standard_EXPORT virtual Standard_Boolean SetCurve (const Handle(Adaptor3d_Curve)& C) = 0;
  
  Standard_EXPORT virtual const Handle(Adaptor3d_Curve)& GetCurve() const = 0;
  
  //! Set a transformation Matrix like   the law M(t) become
  //! Mat * M(t)
  Standard_EXPORT virtual void SetTrsf (const gp_Mat& Transfo) = 0;
  
  Standard_EXPORT virtual Handle(GeomFill_LocationLaw) Copy() const = 0;
  
  //! compute Location
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, gp_Mat& M, gp_Vec& V) = 0;
  
  //! compute Location and 2d points
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, TColgp_Array1OfPnt2d& Poles2d) = 0;
  
  //! compute location 2d  points and  associated
  //! first derivatives.
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual Standard_Boolean D1 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, gp_Mat& DM, gp_Vec& DV, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d);
  
  //! compute location 2d  points and associated
  //! first and seconde  derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual Standard_Boolean D2 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, gp_Mat& DM, gp_Vec& DV, gp_Mat& D2M, gp_Vec& D2V, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColgp_Array1OfVec2d& D2Poles2d);
  
  //! get the number of  2d  curves (Restrictions  +  Traces)
  //! to approximate.
  Standard_EXPORT Standard_Integer Nb2dCurves() const;
  
  //! Say if the first restriction is defined in this class.
  //! If it  is true the  first element  of poles array   in
  //! D0,D1,D2... Correspond to this restriction.
  //! Returns Standard_False (default implementation)
  Standard_EXPORT virtual Standard_Boolean HasFirstRestriction() const;
  
  //! Say if the last restriction is defined in this class.
  //! If it is  true the  last element  of poles array in
  //! D0,D1,D2... Correspond to this restriction.
  //! Returns Standard_False (default implementation)
  Standard_EXPORT virtual Standard_Boolean HasLastRestriction() const;
  
  //! Give the number of trace (Curves 2d which are not restriction)
  //! Returns 0 (default implementation)
  Standard_EXPORT virtual Standard_Integer TraceNumber() const;
  
  //! Give a status to the Law
  //! Returns PipeOk (default implementation)
  Standard_EXPORT virtual GeomFill_PipeError ErrorStatus() const;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals (const GeomAbs_Shape S) const = 0;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const = 0;
  
  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval (const Standard_Real First, const Standard_Real Last) = 0;
  
  //! Gets the bounds of the parametric interval on
  //! the function
  Standard_EXPORT virtual void GetInterval (Standard_Real& First, Standard_Real& Last) const = 0;
  
  //! Gets the bounds of the function parametric domain.
  //! Warning: This domain it is  not modified by the
  //! SetValue method
  Standard_EXPORT virtual void GetDomain (Standard_Real& First, Standard_Real& Last) const = 0;
  
  //! Returns the resolutions in the  sub-space 2d <Index>
  //! This information is usfull to find an good tolerance in
  //! 2d approximation.
  Standard_EXPORT virtual void Resolution (const Standard_Integer Index, const Standard_Real Tol, Standard_Real& TolU, Standard_Real& TolV) const;
  
  //! Is useful, if (me) have to run numerical
  //! algorithm to perform D0, D1 or D2
  //! The default implementation make nothing.
  Standard_EXPORT virtual void SetTolerance (const Standard_Real Tol3d, const Standard_Real Tol2d);
  
  //! Get the maximum Norm  of the matrix-location part.  It
  //! is usful to find an good Tolerance to approx M(t).
  Standard_EXPORT virtual Standard_Real GetMaximalNorm() = 0;
  
  //! Get average value of M(t) and V(t) it is usfull to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT virtual void GetAverageLaw (gp_Mat& AM, gp_Vec& AV) = 0;
  
  //! Say if the Location  Law, is an translation of  Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual Standard_Boolean IsTranslation (Standard_Real& Error) const;
  
  //! Say if the Location  Law, is a rotation of Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual Standard_Boolean IsRotation (Standard_Real& Error) const;
  
  Standard_EXPORT virtual void Rotation (gp_Pnt& Center) const;




  DEFINE_STANDARD_RTTIEXT(GeomFill_LocationLaw,Standard_Transient)

protected:




private:




};







#endif // _GeomFill_LocationLaw_HeaderFile
