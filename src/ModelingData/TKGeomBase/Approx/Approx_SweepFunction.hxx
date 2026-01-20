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
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
class gp_Pnt;

//! defined the function used by SweepApproximation to
//! perform sweeping application.
class Approx_SweepFunction : public Standard_Transient
{

public:
  //! compute the section for v = param
  Standard_EXPORT virtual bool D0(const double   Param,
                                              const double   First,
                                              const double   Last,
                                              NCollection_Array1<gp_Pnt>&   Poles,
                                              NCollection_Array1<gp_Pnt2d>& Poles2d,
                                              NCollection_Array1<double>& Weigths) = 0;

  //! compute the first derivative in v direction of the
  //! section for v = param
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual bool D1(const double   Param,
                                              const double   First,
                                              const double   Last,
                                              NCollection_Array1<gp_Pnt>&   Poles,
                                              NCollection_Array1<gp_Vec>&   DPoles,
                                              NCollection_Array1<gp_Pnt2d>& Poles2d,
                                              NCollection_Array1<gp_Vec2d>& DPoles2d,
                                              NCollection_Array1<double>& Weigths,
                                              NCollection_Array1<double>& DWeigths);

  //! compute the second derivative in v direction of the
  //! section for v = param
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual bool D2(const double   Param,
                                              const double   First,
                                              const double   Last,
                                              NCollection_Array1<gp_Pnt>&   Poles,
                                              NCollection_Array1<gp_Vec>&   DPoles,
                                              NCollection_Array1<gp_Vec>&   D2Poles,
                                              NCollection_Array1<gp_Pnt2d>& Poles2d,
                                              NCollection_Array1<gp_Vec2d>& DPoles2d,
                                              NCollection_Array1<gp_Vec2d>& D2Poles2d,
                                              NCollection_Array1<double>& Weigths,
                                              NCollection_Array1<double>& DWeigths,
                                              NCollection_Array1<double>& D2Weigths);

  //! get the number of 2d curves to approximate.
  Standard_EXPORT virtual int Nb2dCurves() const = 0;

  //! get the format of an section
  Standard_EXPORT virtual void SectionShape(int& NbPoles,
                                            int& NbKnots,
                                            int& Degree) const = 0;

  //! get the Knots of the section
  Standard_EXPORT virtual void Knots(NCollection_Array1<double>& TKnots) const = 0;

  //! get the Multplicities of the section
  Standard_EXPORT virtual void Mults(NCollection_Array1<int>& TMults) const = 0;

  //! Returns if the sections are rational or not
  Standard_EXPORT virtual bool IsRational() const = 0;

  //! Returns the number of intervals for continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const = 0;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const = 0;

  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval(const double First, const double Last) = 0;

  //! Returns the resolutions in the sub-space 2d <Index>
  //! This information is useful to find a good tolerance in
  //! 2d approximation.
  Standard_EXPORT virtual void Resolution(const int Index,
                                          const double    Tol,
                                          double&         TolU,
                                          double&         TolV) const;

  //! Returns the tolerance to reach in approximation
  //! to satisfy.
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary (in radian)
  //! SurfTol error inside the surface.
  Standard_EXPORT virtual void GetTolerance(const double   BoundTol,
                                            const double   SurfTol,
                                            const double   AngleTol,
                                            NCollection_Array1<double>& Tol3d) const = 0;

  //! Is useful, if (me) have to run numerical algorithm to perform D0, D1 or D2
  Standard_EXPORT virtual void SetTolerance(const double Tol3d,
                                            const double Tol2d) = 0;

  //! Get the barycentre of Surface.
  //! An very poor estimation is sufficient.
  //! This information is useful to perform well conditioned rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual gp_Pnt BarycentreOfSurf() const;

  //! Returns the length of the greater section.
  //! This information is useful to G1's control.
  //! Warning: With an little value, approximation can be slower.
  Standard_EXPORT virtual double MaximalSection() const;

  //! Compute the minimal value of weight for each poles in all sections.
  //! This information is useful to control error in rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual void GetMinimalWeight(NCollection_Array1<double>& Weigths) const;

  DEFINE_STANDARD_RTTIEXT(Approx_SweepFunction, Standard_Transient)

};

#endif // _Approx_SweepFunction_HeaderFile
