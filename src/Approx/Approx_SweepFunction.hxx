// Created on: 1997-06-25
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

#ifndef _Approx_SweepFunction_HeaderFile
#define _Approx_SweepFunction_HeaderFile

#include <Standard.hxx>

#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
class gp_Pnt;

class Approx_SweepFunction;
DEFINE_STANDARD_HANDLE(Approx_SweepFunction, Standard_Transient)

//! defined the function used by SweepApproximation to
//! perform sweeping application.
class Approx_SweepFunction : public Standard_Transient
{

public:
  //! compute the section for v = param
  Standard_EXPORT virtual Standard_Boolean D0(const Standard_Real   Param,
                                              const Standard_Real   First,
                                              const Standard_Real   Last,
                                              TColgp_Array1OfPnt&   Poles,
                                              TColgp_Array1OfPnt2d& Poles2d,
                                              TColStd_Array1OfReal& Weigths) = 0;

  //! compute the first  derivative in v direction  of the
  //! section for v =  param
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual Standard_Boolean D1(const Standard_Real   Param,
                                              const Standard_Real   First,
                                              const Standard_Real   Last,
                                              TColgp_Array1OfPnt&   Poles,
                                              TColgp_Array1OfVec&   DPoles,
                                              TColgp_Array1OfPnt2d& Poles2d,
                                              TColgp_Array1OfVec2d& DPoles2d,
                                              TColStd_Array1OfReal& Weigths,
                                              TColStd_Array1OfReal& DWeigths);

  //! compute the second derivative  in v direction of the
  //! section  for v = param
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual Standard_Boolean D2(const Standard_Real   Param,
                                              const Standard_Real   First,
                                              const Standard_Real   Last,
                                              TColgp_Array1OfPnt&   Poles,
                                              TColgp_Array1OfVec&   DPoles,
                                              TColgp_Array1OfVec&   D2Poles,
                                              TColgp_Array1OfPnt2d& Poles2d,
                                              TColgp_Array1OfVec2d& DPoles2d,
                                              TColgp_Array1OfVec2d& D2Poles2d,
                                              TColStd_Array1OfReal& Weigths,
                                              TColStd_Array1OfReal& DWeigths,
                                              TColStd_Array1OfReal& D2Weigths);

  //! get the number of 2d curves to  approximate.
  Standard_EXPORT virtual Standard_Integer Nb2dCurves() const = 0;

  //! get the format of an  section
  Standard_EXPORT virtual void SectionShape(Standard_Integer& NbPoles,
                                            Standard_Integer& NbKnots,
                                            Standard_Integer& Degree) const = 0;

  //! get the Knots of the section
  Standard_EXPORT virtual void Knots(TColStd_Array1OfReal& TKnots) const = 0;

  //! get the Multplicities of the section
  Standard_EXPORT virtual void Mults(TColStd_Array1OfInteger& TMults) const = 0;

  //! Returns if the sections are rational or not
  Standard_EXPORT virtual Standard_Boolean IsRational() const = 0;

  //! Returns  the number  of  intervals for  continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals(const GeomAbs_Shape S) const = 0;

  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const = 0;

  //! Sets the bounds of the parametric interval on
  //! the fonction
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval(const Standard_Real First, const Standard_Real Last) = 0;

  //! Returns the resolutions in the  sub-space 2d <Index>
  //! This information is usfull to find an good tolerance in
  //! 2d approximation.
  Standard_EXPORT virtual void Resolution(const Standard_Integer Index,
                                          const Standard_Real    Tol,
                                          Standard_Real&         TolU,
                                          Standard_Real&         TolV) const;

  //! Returns the tolerance to reach in approximation
  //! to satisfy.
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary (in radian)
  //! SurfTol error inside the surface.
  Standard_EXPORT virtual void GetTolerance(const Standard_Real   BoundTol,
                                            const Standard_Real   SurfTol,
                                            const Standard_Real   AngleTol,
                                            TColStd_Array1OfReal& Tol3d) const = 0;

  //! Is useful, if (me) have to run numerical algorithm to perform D0, D1 or D2
  Standard_EXPORT virtual void SetTolerance(const Standard_Real Tol3d,
                                            const Standard_Real Tol2d) = 0;

  //! Get the barycentre of Surface.
  //! An very poor estimation is sufficient.
  //! This information is useful to perform well conditioned rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual gp_Pnt BarycentreOfSurf() const;

  //! Returns the length of the greater section.
  //!  Thisinformation is useful to G1's control.
  //! Warning: With an little value, approximation can be slower.
  Standard_EXPORT virtual Standard_Real MaximalSection() const;

  //! Compute the minimal value of weight for each poles in all  sections.
  //! This information is useful to control error in rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual void GetMinimalWeight(TColStd_Array1OfReal& Weigths) const;

  DEFINE_STANDARD_RTTIEXT(Approx_SweepFunction, Standard_Transient)

protected:
private:
};

#endif // _Approx_SweepFunction_HeaderFile
