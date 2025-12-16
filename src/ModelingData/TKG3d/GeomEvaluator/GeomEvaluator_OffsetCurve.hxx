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

#ifndef _GeomEvaluator_OffsetCurve_HeaderFile
#define _GeomEvaluator_OffsetCurve_HeaderFile

#include <GeomAdaptor_Curve.hxx>
#include <GeomEvaluator_Curve.hxx>
#include <gp_Dir.hxx>

//! Allows to calculate values and derivatives for offset curves in 3D
class GeomEvaluator_OffsetCurve : public GeomEvaluator_Curve
{
public:
  //! Initialize evaluator by curve
  Standard_EXPORT GeomEvaluator_OffsetCurve(const Handle(Geom_Curve)& theBase,
                                            const Standard_Real       theOffset,
                                            const gp_Dir&             theDirection);
  //! Initialize evaluator by curve adaptor
  Standard_EXPORT GeomEvaluator_OffsetCurve(const Handle(GeomAdaptor_Curve)& theBase,
                                            const Standard_Real              theOffset,
                                            const gp_Dir&                    theDirection);

  //! Change the offset value
  void SetOffsetValue(Standard_Real theOffset) { myOffset = theOffset; }

  void SetOffsetDirection(const gp_Dir& theDirection) { myOffsetDir = theDirection; }

  //! Value of curve
  Standard_EXPORT void D0(const Standard_Real theU, gp_Pnt& theValue) const Standard_OVERRIDE;
  //! Value and first derivatives of curve
  Standard_EXPORT void D1(const Standard_Real theU,
                          gp_Pnt&             theValue,
                          gp_Vec&             theD1) const Standard_OVERRIDE;
  //! Value, first and second derivatives of curve
  Standard_EXPORT void D2(const Standard_Real theU,
                          gp_Pnt&             theValue,
                          gp_Vec&             theD1,
                          gp_Vec&             theD2) const Standard_OVERRIDE;
  //! Value, first, second and third derivatives of curve
  Standard_EXPORT void D3(const Standard_Real theU,
                          gp_Pnt&             theValue,
                          gp_Vec&             theD1,
                          gp_Vec&             theD2,
                          gp_Vec&             theD3) const Standard_OVERRIDE;
  //! Calculates N-th derivatives of curve, where N = theDeriv. Raises if N < 1
  Standard_EXPORT gp_Vec DN(const Standard_Real    theU,
                            const Standard_Integer theDeriv) const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(GeomEvaluator_Curve) ShallowCopy() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(GeomEvaluator_OffsetCurve, GeomEvaluator_Curve)

private:
  Handle(Geom_Curve)        myBaseCurve;
  Handle(GeomAdaptor_Curve) myBaseAdaptor;

  Standard_Real myOffset;    //!< offset value
  gp_Dir        myOffsetDir; //!< offset direction
};

DEFINE_STANDARD_HANDLE(GeomEvaluator_OffsetCurve, GeomEvaluator_Curve)

#endif // _GeomEvaluator_OffsetCurve_HeaderFile
