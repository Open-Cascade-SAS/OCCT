// Created on: 1997-07-11
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

#ifndef _GeomFill_CircularBlendFunc_HeaderFile
#define _GeomFill_CircularBlendFunc_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Integer.hxx>
#include <Convert_ParameterisationType.hxx>
#include <Approx_SweepFunction.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <GeomAbs_Shape.hxx>

//! Circular Blend Function to approximate by
//! SweepApproximation from Approx
class GeomFill_CircularBlendFunc : public Approx_SweepFunction
{

public:
  //! Create a Blend with a constant radius with 2
  //! guide-line. <FShape> sets the type of fillet
  //! surface. The default value is Convert_TgtThetaOver2
  //! (classical nurbs representation of circles).
  //! ChFi3d_QuasiAngular corresponds to a nurbs
  //! representation of circles which parameterisation
  //! matches the circle one. ChFi3d_Polynomial
  //! corresponds to a polynomial representation of
  //! circles.
  Standard_EXPORT GeomFill_CircularBlendFunc(const occ::handle<Adaptor3d_Curve>& Path,
                                             const occ::handle<Adaptor3d_Curve>& Curve1,
                                             const occ::handle<Adaptor3d_Curve>& Curve2,
                                             const double            Radius,
                                             const bool Polynomial = false);

  //! compute the section for v = param
  Standard_EXPORT virtual bool D0(const double   Param,
                                              const double   First,
                                              const double   Last,
                                              NCollection_Array1<gp_Pnt>&   Poles,
                                              NCollection_Array1<gp_Pnt2d>& Poles2d,
                                              NCollection_Array1<double>& Weigths) override;

  //! compute the first derivative in v direction of the
  //! section for v = param
  Standard_EXPORT virtual bool D1(const double   Param,
                                              const double   First,
                                              const double   Last,
                                              NCollection_Array1<gp_Pnt>&   Poles,
                                              NCollection_Array1<gp_Vec>&   DPoles,
                                              NCollection_Array1<gp_Pnt2d>& Poles2d,
                                              NCollection_Array1<gp_Vec2d>& DPoles2d,
                                              NCollection_Array1<double>& Weigths,
                                              NCollection_Array1<double>& DWeigths) override;

  //! compute the second derivative in v direction of the
  //! section for v = param
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
                                              NCollection_Array1<double>& D2Weigths) override;

  //! get the number of 2d curves to approximate.
  Standard_EXPORT virtual int Nb2dCurves() const override;

  //! get the format of an section
  Standard_EXPORT virtual void SectionShape(int& NbPoles,
                                            int& NbKnots,
                                            int& Degree) const override;

  //! get the Knots of the section
  Standard_EXPORT virtual void Knots(NCollection_Array1<double>& TKnots) const override;

  //! get the Multplicities of the section
  Standard_EXPORT virtual void Mults(NCollection_Array1<int>& TMults) const override;

  //! Returns if the section is rational or not
  Standard_EXPORT virtual bool IsRational() const override;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const
    override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T,
                                         const GeomAbs_Shape   S) const override;

  //! Sets the bounds of the parametric interval on
  //! the fonction
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval(const double First,
                                           const double Last) override;

  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary (in radian)
  //! SurfTol error inside the surface.
  Standard_EXPORT virtual void GetTolerance(const double   BoundTol,
                                            const double   SurfTol,
                                            const double   AngleTol,
                                            NCollection_Array1<double>& Tol3d) const override;

  //! Is useful, if (me) has to be run numerical
  //! algorithm to perform D0, D1 or D2
  Standard_EXPORT virtual void SetTolerance(const double Tol3d,
                                            const double Tol2d) override;

  //! Get the barycentre of Surface. A very poor
  //! estimation is sufficient. This information is useful
  //! to perform well conditioned rational approximation.
  Standard_EXPORT virtual gp_Pnt BarycentreOfSurf() const override;

  //! Returns the length of the maximum section. This
  //! information is useful to perform well conditioned rational
  //! approximation.
  Standard_EXPORT virtual double MaximalSection() const override;

  //! Compute the minimal value of weight for each poles
  //! of all sections. This information is useful to
  //! perform well conditioned rational approximation.
  Standard_EXPORT virtual void GetMinimalWeight(NCollection_Array1<double>& Weigths) const
    override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_CircularBlendFunc, Approx_SweepFunction)

private:
  Standard_EXPORT void Discret();

  gp_Pnt                       myBary;
  double                myRadius;
  double                maxang;
  double                minang;
  double                distmin;
  occ::handle<Adaptor3d_Curve>      myPath;
  occ::handle<Adaptor3d_Curve>      myCurve1;
  occ::handle<Adaptor3d_Curve>      myCurve2;
  occ::handle<Adaptor3d_Curve>      myTPath;
  occ::handle<Adaptor3d_Curve>      myTCurve1;
  occ::handle<Adaptor3d_Curve>      myTCurve2;
  int             myDegree;
  int             myNbKnots;
  int             myNbPoles;
  Convert_ParameterisationType myTConv;
  bool             myreverse;
};

#endif // _GeomFill_CircularBlendFunc_HeaderFile
