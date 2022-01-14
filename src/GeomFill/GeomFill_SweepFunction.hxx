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

#ifndef _GeomFill_SweepFunction_HeaderFile
#define _GeomFill_SweepFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Mat.hxx>
#include <gp_Vec.hxx>
#include <Approx_SweepFunction.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GeomAbs_Shape.hxx>
class GeomFill_LocationLaw;
class GeomFill_SectionLaw;
class gp_Pnt;


class GeomFill_SweepFunction;
DEFINE_STANDARD_HANDLE(GeomFill_SweepFunction, Approx_SweepFunction)

//! Function to approximate by SweepApproximation from
//! Approx. To build general sweep Surface.
class GeomFill_SweepFunction : public Approx_SweepFunction
{

public:


  Standard_EXPORT GeomFill_SweepFunction(const Handle(GeomFill_SectionLaw)& Section, const Handle(GeomFill_LocationLaw)& Location, const Standard_Real FirstParameter, const Standard_Real FirstParameterOnS, const Standard_Real RatioParameterOnS);
  
  //! compute the section for v = param
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, const Standard_Real First, const Standard_Real Last, TColgp_Array1OfPnt& Poles, TColgp_Array1OfPnt2d& Poles2d, TColStd_Array1OfReal& Weigths) Standard_OVERRIDE;

  //! compute the first derivative in v direction of the
  //! section for v = param
  Standard_EXPORT virtual Standard_Boolean D1 (const Standard_Real Param, const Standard_Real First, const Standard_Real Last, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths) Standard_OVERRIDE;

  //! compute the second derivative  in v direction of the
  //! section for v = param
  Standard_EXPORT virtual Standard_Boolean D2 (const Standard_Real Param, const Standard_Real First, const Standard_Real Last, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfVec& D2Poles, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColgp_Array1OfVec2d& D2Poles2d, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths, TColStd_Array1OfReal& D2Weigths) Standard_OVERRIDE;
  
  //! get the number of 2d curves to approximate.
  Standard_EXPORT virtual Standard_Integer Nb2dCurves() const Standard_OVERRIDE;
  
  //! get the format of a section
  Standard_EXPORT virtual void SectionShape (Standard_Integer& NbPoles, Standard_Integer& NbKnots, Standard_Integer& Degree) const Standard_OVERRIDE;

  //! get the Knots of the section
  Standard_EXPORT virtual void Knots (TColStd_Array1OfReal& TKnots) const Standard_OVERRIDE;

  //! get the Multplicities of the section
  Standard_EXPORT virtual void Mults (TColStd_Array1OfInteger& TMults) const Standard_OVERRIDE;

  //! Returns if the section is rational or not
  Standard_EXPORT virtual Standard_Boolean IsRational() const Standard_OVERRIDE;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;

  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval (const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE;

  //! Returns the resolutions in the sub-space 2d <Index>
  //! This information is usfull to find an good tolerance in
  //! 2d approximation.
  //! Warning: Used only if Nb2dCurve > 0
  Standard_EXPORT virtual void Resolution (const Standard_Integer Index, const Standard_Real Tol, Standard_Real& TolU, Standard_Real& TolV) const Standard_OVERRIDE;

  //! Returns the tolerance to reach in approximation
  //! to respecte
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary (in radian)
  //! SurfTol error inside the surface.
  Standard_EXPORT virtual void GetTolerance (const Standard_Real BoundTol, const Standard_Real SurfTol, const Standard_Real AngleTol, TColStd_Array1OfReal& Tol3d) const Standard_OVERRIDE;

  //! Is usfull, if (me) have to  be run numerical
  //! algorithme to perform D0, D1 or D2
  Standard_EXPORT virtual void SetTolerance (const Standard_Real Tol3d, const Standard_Real Tol2d) Standard_OVERRIDE;
  
  //! Get    the   barycentre of   Surface.   An   very  poor
  //! estimation is sufficient. This information is useful
  //! to perform well conditioned rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual gp_Pnt BarycentreOfSurf() const Standard_OVERRIDE;

  //! Returns the   length of the maximum section. This
  //! information is useful to perform well conditioned rational
  //! approximation.
  Standard_EXPORT virtual Standard_Real MaximalSection() const Standard_OVERRIDE;
  
  //! Compute the minimal value of weight for each poles
  //! of all  sections.  This information is  useful to
  //! perform well conditioned rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual void GetMinimalWeight (TColStd_Array1OfReal& Weigths) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_SweepFunction,Approx_SweepFunction)

protected:




private:


  Handle(GeomFill_LocationLaw) myLoc;
  Handle(GeomFill_SectionLaw) mySec;
  Standard_Real myf;
  Standard_Real myfOnS;
  Standard_Real myRatio;
  gp_Mat M;
  gp_Mat DM;
  gp_Mat D2M;
  gp_Vec V;
  gp_Vec DV;
  gp_Vec D2V;


};







#endif // _GeomFill_SweepFunction_HeaderFile
