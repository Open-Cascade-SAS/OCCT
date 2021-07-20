// Created on: 2015-09-21
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Geom2dEvaluator_OffsetCurve_HeaderFile
#define _Geom2dEvaluator_OffsetCurve_HeaderFile

#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dEvaluator_Curve.hxx>

//! Allows to calculate values and derivatives for offset curves in 2D
class Geom2dEvaluator_OffsetCurve : public Geom2dEvaluator_Curve
{
public:
  //! Initialize evaluator by curve
  Standard_EXPORT Geom2dEvaluator_OffsetCurve(
      const Handle(Geom2d_Curve)& theBase,
      const Standard_Real theOffset);
  //! Initialize evaluator by curve adaptor
  Standard_EXPORT Geom2dEvaluator_OffsetCurve(
      const Handle(Geom2dAdaptor_Curve)& theBase,
      const Standard_Real theOffset);

  //! Change the offset value
  void SetOffsetValue(Standard_Real theOffset)
  { myOffset = theOffset; }

  //! Value of curve
  Standard_EXPORT void D0(const Standard_Real theU,
                          gp_Pnt2d& theValue) const Standard_OVERRIDE;
  //! Value and first derivatives of curve
  Standard_EXPORT void D1(const Standard_Real theU,
                          gp_Pnt2d& theValue, gp_Vec2d& theD1) const Standard_OVERRIDE;
  //! Value, first and second derivatives of curve
  Standard_EXPORT void D2(const Standard_Real theU,
                          gp_Pnt2d& theValue, gp_Vec2d& theD1, gp_Vec2d& theD2) const Standard_OVERRIDE;
  //! Value, first, second and third derivatives of curve
  Standard_EXPORT void D3(const Standard_Real theU,
                          gp_Pnt2d& theValue, gp_Vec2d& theD1,
                          gp_Vec2d& theD2, gp_Vec2d& theD3) const Standard_OVERRIDE;
  //! Calculates N-th derivatives of curve, where N = theDeriv. Raises if N < 1
  Standard_EXPORT gp_Vec2d DN(const Standard_Real theU,
                              const Standard_Integer theDeriv) const Standard_OVERRIDE;

  Standard_EXPORT Handle(Geom2dEvaluator_Curve) ShallowCopy() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom2dEvaluator_OffsetCurve,Geom2dEvaluator_Curve)

private:
  //! Calculate value of base curve/adaptor
  void BaseD0(const Standard_Real theU, gp_Pnt2d& theValue) const;
  //! Calculate value and first derivatives of base curve/adaptor
  void BaseD1(const Standard_Real theU,
              gp_Pnt2d& theValue, gp_Vec2d& theD1) const;
  //! Calculate value, first and second derivatives of base curve/adaptor
  void BaseD2(const Standard_Real theU,
              gp_Pnt2d& theValue, gp_Vec2d& theD1, gp_Vec2d& theD2) const;
  //! Calculate value, first, second and third derivatives of base curve/adaptor
  void BaseD3(const Standard_Real theU,
              gp_Pnt2d& theValue, gp_Vec2d& theD1, gp_Vec2d& theD2, gp_Vec2d& theD3) const;
  //! Calculate value and derivatives till 4th of base curve/adaptor
  void BaseD4(const Standard_Real theU,
              gp_Pnt2d& theValue, gp_Vec2d& theD1, gp_Vec2d& theD2, gp_Vec2d& theD3, gp_Vec2d& theD4) const;
  //! Calculate N-th derivative of base curve/adaptor
  gp_Vec2d BaseDN(const Standard_Real theU, const Standard_Integer theDeriv) const;

  // Recalculate derivatives in the singular point
  // Returns true if the direction of derivatives is changed
  Standard_Boolean AdjustDerivative(const Standard_Integer theMaxDerivative,
                                    const Standard_Real theU,
                                          gp_Vec2d& theD1,
                                          gp_Vec2d& theD2,
                                          gp_Vec2d& theD3,
                                          gp_Vec2d& theD4) const;

private:
  Handle(Geom2d_Curve)         myBaseCurve;
  Handle(Geom2dAdaptor_Curve) myBaseAdaptor;

  Standard_Real myOffset;    ///< offset value
};

DEFINE_STANDARD_HANDLE(Geom2dEvaluator_OffsetCurve, Geom2dEvaluator_Curve)


#endif // _Geom2dEvaluator_OffsetCurve_HeaderFile
