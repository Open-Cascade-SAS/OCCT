// Created on: 1998-05-12
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepBlend_AppFuncRoot_HeaderFile
#define _BRepBlend_AppFuncRoot_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <math_Vector.hxx>
#include <Blend_Point.hxx>
#include <gp_Pnt.hxx>
#include <Approx_SweepFunction.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>

#include <memory>

class BRepBlend_Line;
class Blend_AppFunction;
class math_FunctionSetRoot;

//! Function to approximate by AppSurface
class BRepBlend_AppFuncRoot : public Approx_SweepFunction
{

public:
  //! compute the section for v = param
  Standard_EXPORT bool D0(const double                  Param,
                          const double                  First,
                          const double                  Last,
                          NCollection_Array1<gp_Pnt>&   Poles,
                          NCollection_Array1<gp_Pnt2d>& Poles2d,
                          NCollection_Array1<double>&   Weigths) override;

  //! compute the first derivative in v direction of the
  //! section for v = param
  Standard_EXPORT bool D1(const double                  Param,
                          const double                  First,
                          const double                  Last,
                          NCollection_Array1<gp_Pnt>&   Poles,
                          NCollection_Array1<gp_Vec>&   DPoles,
                          NCollection_Array1<gp_Pnt2d>& Poles2d,
                          NCollection_Array1<gp_Vec2d>& DPoles2d,
                          NCollection_Array1<double>&   Weigths,
                          NCollection_Array1<double>&   DWeigths) override;

  //! compute the second derivative in v direction of the
  //! section for v = param
  Standard_EXPORT bool D2(const double                  Param,
                          const double                  First,
                          const double                  Last,
                          NCollection_Array1<gp_Pnt>&   Poles,
                          NCollection_Array1<gp_Vec>&   DPoles,
                          NCollection_Array1<gp_Vec>&   D2Poles,
                          NCollection_Array1<gp_Pnt2d>& Poles2d,
                          NCollection_Array1<gp_Vec2d>& DPoles2d,
                          NCollection_Array1<gp_Vec2d>& D2Poles2d,
                          NCollection_Array1<double>&   Weigths,
                          NCollection_Array1<double>&   DWeigths,
                          NCollection_Array1<double>&   D2Weigths) override;

  //! get the number of 2d curves to approximate.
  Standard_EXPORT int Nb2dCurves() const override;

  //! get the format of an section
  Standard_EXPORT void SectionShape(int& NbPoles, int& NbKnots, int& Degree) const override;

  //! get the Knots of the section
  Standard_EXPORT void Knots(NCollection_Array1<double>& TKnots) const override;

  //! get the Multplicities of the section
  Standard_EXPORT void Mults(NCollection_Array1<int>& TMults) const override;

  //! Returns if the section is rational or not
  Standard_EXPORT bool IsRational() const override;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  //! Sets the bounds of the parametric interval on
  //! the fonction
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT void SetInterval(const double First, const double Last) override;

  //! Returns the resolutions in the sub-space 2d <Index> --
  //! This information is useful to find a good tolerance in
  //! 2d approximation
  Standard_EXPORT void Resolution(const int    Index,
                                  const double Tol,
                                  double&      TolU,
                                  double&      TolV) const override;

  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary (in radian)
  //! SurfTol error inside the surface.
  Standard_EXPORT void GetTolerance(const double                BoundTol,
                                    const double                SurfTol,
                                    const double                AngleTol,
                                    NCollection_Array1<double>& Tol3d) const override;

  //! Is useful, if (me) has to be run numerical
  //! algorithm to perform D0, D1 or D2
  Standard_EXPORT void SetTolerance(const double Tol3d, const double Tol2d) override;

  //! Get the barycentre of Surface. An very poor
  //! estimation is sufficient. This information is useful
  //! to perform well conditioned rational approximation.
  Standard_EXPORT gp_Pnt BarycentreOfSurf() const override;

  //! Returns the length of the maximum section. This
  //! information is useful to perform well conditioned rational
  //! approximation.
  Standard_EXPORT double MaximalSection() const override;

  //! Compute the minimal value of weight for each poles
  //! of all sections. This information is useful to
  //! perform well conditioned rational approximation.
  Standard_EXPORT void GetMinimalWeight(NCollection_Array1<double>& Weigths) const override;

  Standard_EXPORT virtual void Point(const Blend_AppFunction& Func,
                                     const double             Param,
                                     const math_Vector&       Sol,
                                     Blend_Point&             Pnt) const = 0;

  Standard_EXPORT virtual void Vec(math_Vector& Sol, const Blend_Point& Pnt) const = 0;

  DEFINE_STANDARD_RTTIEXT(BRepBlend_AppFuncRoot, Approx_SweepFunction)

  Standard_EXPORT ~BRepBlend_AppFuncRoot();

protected:
  Standard_EXPORT BRepBlend_AppFuncRoot(occ::handle<BRepBlend_Line>& Line,
                                        Blend_AppFunction&           Func,
                                        const double                 Tol3d,
                                        const double                 Tol2d);

private:
  Standard_EXPORT bool SearchPoint(Blend_AppFunction& Func, const double Param, Blend_Point& Pnt);

  Standard_EXPORT bool SearchLocation(const double Param,
                                      const int    FirstIndex,
                                      const int    LastIndex,
                                      int&         ParamIndex) const;

  occ::handle<BRepBlend_Line>           myLine;
  void*                                 myFunc;
  math_Vector                           myTolerance;
  Blend_Point                           myPnt;
  gp_Pnt                                myBary;
  math_Vector                           X1;
  math_Vector                           X2;
  math_Vector                           XInit;
  math_Vector                           Sol;
  std::unique_ptr<math_FunctionSetRoot> mySolver;
};

#endif // _BRepBlend_AppFuncRoot_HeaderFile
