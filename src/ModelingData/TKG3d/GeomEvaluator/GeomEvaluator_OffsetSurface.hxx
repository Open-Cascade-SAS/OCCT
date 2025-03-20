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

#ifndef _GeomEvaluator_OffsetSurface_HeaderFile
#define _GeomEvaluator_OffsetSurface_HeaderFile

#include <GeomAdaptor_Surface.hxx>
#include <GeomEvaluator_Surface.hxx>
#include <Geom_OsculatingSurface.hxx>
#include <Geom_Surface.hxx>

//! Allows to calculate values and derivatives for offset surfaces
class GeomEvaluator_OffsetSurface : public GeomEvaluator_Surface
{
public:
  //! Initialize evaluator by surface
  Standard_EXPORT GeomEvaluator_OffsetSurface(
    const Handle(Geom_Surface)&           theBase,
    const Standard_Real                   theOffset,
    const Handle(Geom_OsculatingSurface)& theOscSurf = Handle(Geom_OsculatingSurface)());
  //! Initialize evaluator by surface adaptor
  Standard_EXPORT GeomEvaluator_OffsetSurface(
    const Handle(GeomAdaptor_Surface)&    theBase,
    const Standard_Real                   theOffset,
    const Handle(Geom_OsculatingSurface)& theOscSurf = Handle(Geom_OsculatingSurface)());

  //! Change the offset value
  void SetOffsetValue(Standard_Real theOffset) { myOffset = theOffset; }

  //! Value of surface
  Standard_EXPORT void D0(const Standard_Real theU,
                          const Standard_Real theV,
                          gp_Pnt&             theValue) const Standard_OVERRIDE;
  //! Value and first derivatives of surface
  Standard_EXPORT void D1(const Standard_Real theU,
                          const Standard_Real theV,
                          gp_Pnt&             theValue,
                          gp_Vec&             theD1U,
                          gp_Vec&             theD1V) const Standard_OVERRIDE;
  //! Value, first and second derivatives of surface
  Standard_EXPORT void D2(const Standard_Real theU,
                          const Standard_Real theV,
                          gp_Pnt&             theValue,
                          gp_Vec&             theD1U,
                          gp_Vec&             theD1V,
                          gp_Vec&             theD2U,
                          gp_Vec&             theD2V,
                          gp_Vec&             theD2UV) const Standard_OVERRIDE;
  //! Value, first, second and third derivatives of surface
  Standard_EXPORT void D3(const Standard_Real theU,
                          const Standard_Real theV,
                          gp_Pnt&             theValue,
                          gp_Vec&             theD1U,
                          gp_Vec&             theD1V,
                          gp_Vec&             theD2U,
                          gp_Vec&             theD2V,
                          gp_Vec&             theD2UV,
                          gp_Vec&             theD3U,
                          gp_Vec&             theD3V,
                          gp_Vec&             theD3UUV,
                          gp_Vec&             theD3UVV) const Standard_OVERRIDE;
  //! Calculates N-th derivatives of surface, where N = theDerU + theDerV.
  //!
  //! Raises if N < 1 or theDerU < 0 or theDerV < 0
  Standard_EXPORT gp_Vec DN(const Standard_Real    theU,
                            const Standard_Real    theV,
                            const Standard_Integer theDerU,
                            const Standard_Integer theDerV) const Standard_OVERRIDE;

  Standard_EXPORT Handle(GeomEvaluator_Surface) ShallowCopy() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(GeomEvaluator_OffsetSurface, GeomEvaluator_Surface)

private:
  //! Returns bounds of a base surface
  void Bounds(Standard_Real& theUMin,
              Standard_Real& theUMax,
              Standard_Real& theVMin,
              Standard_Real& theVMax) const;

  //! Recalculate D1 values of base surface into D0 value of offset surface
  void CalculateD0(const Standard_Real theU,
                   const Standard_Real theV,
                   gp_Pnt&             theValue,
                   const gp_Vec&       theD1U,
                   const gp_Vec&       theD1V) const;
  //! Recalculate D2 values of base surface into D1 values of offset surface
  void CalculateD1(const Standard_Real theU,
                   const Standard_Real theV,
                   gp_Pnt&             theValue,
                   gp_Vec&             theD1U,
                   gp_Vec&             theD1V,
                   const gp_Vec&       theD2U,
                   const gp_Vec&       theD2V,
                   const gp_Vec&       theD2UV) const;
  //! Recalculate D3 values of base surface into D2 values of offset surface
  void CalculateD2(const Standard_Real theU,
                   const Standard_Real theV,
                   gp_Pnt&             theValue,
                   gp_Vec&             theD1U,
                   gp_Vec&             theD1V,
                   gp_Vec&             theD2U,
                   gp_Vec&             theD2V,
                   gp_Vec&             theD2UV,
                   const gp_Vec&       theD3U,
                   const gp_Vec&       theD3V,
                   const gp_Vec&       theD3UUV,
                   const gp_Vec&       theD3UVV) const;
  //! Recalculate D3 values of base surface into D3 values of offset surface
  void CalculateD3(const Standard_Real theU,
                   const Standard_Real theV,
                   gp_Pnt&             theValue,
                   gp_Vec&             theD1U,
                   gp_Vec&             theD1V,
                   gp_Vec&             theD2U,
                   gp_Vec&             theD2V,
                   gp_Vec&             theD2UV,
                   gp_Vec&             theD3U,
                   gp_Vec&             theD3V,
                   gp_Vec&             theD3UUV,
                   gp_Vec&             theD3UVV) const;
  //! Calculate DN of offset surface based on derivatives of base surface
  gp_Vec CalculateDN(const Standard_Real    theU,
                     const Standard_Real    theV,
                     const Standard_Integer theNu,
                     const Standard_Integer theNv,
                     const gp_Vec&          theD1U,
                     const gp_Vec&          theD1V) const;

  //! Calculate value of base surface/adaptor
  void BaseD0(const Standard_Real theU, const Standard_Real theV, gp_Pnt& theValue) const;
  //! Calculate value and first derivatives of base surface/adaptor
  void BaseD1(const Standard_Real theU,
              const Standard_Real theV,
              gp_Pnt&             theValue,
              gp_Vec&             theD1U,
              gp_Vec&             theD1V) const;
  //! Calculate value, first and second derivatives of base surface/adaptor
  void BaseD2(const Standard_Real theU,
              const Standard_Real theV,
              gp_Pnt&             theValue,
              gp_Vec&             theD1U,
              gp_Vec&             theD1V,
              gp_Vec&             theD2U,
              gp_Vec&             theD2V,
              gp_Vec&             theD2UV) const;
  //! Calculate value, first, second and third derivatives of base surface/adaptor
  void BaseD3(const Standard_Real theU,
              const Standard_Real theV,
              gp_Pnt&             theValue,
              gp_Vec&             theD1U,
              gp_Vec&             theD1V,
              gp_Vec&             theD2U,
              gp_Vec&             theD2V,
              gp_Vec&             theD2UV,
              gp_Vec&             theD3U,
              gp_Vec&             theD3V,
              gp_Vec&             theD3UUV,
              gp_Vec&             theD3UVV) const;

  //! Replace zero derivative by the corresponding derivative in a near point.
  //! Return true, if the derivative was replaced.
  Standard_Boolean ReplaceDerivative(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Vec&             theDU,
                                     gp_Vec&             theDV,
                                     const Standard_Real theSquareTol) const;

private:
  Handle(Geom_Surface)        myBaseSurf;
  Handle(GeomAdaptor_Surface) myBaseAdaptor;

  Standard_Real                  myOffset;  ///< offset value
  Handle(Geom_OsculatingSurface) myOscSurf; ///< auxiliary osculating surface
};

DEFINE_STANDARD_HANDLE(GeomEvaluator_OffsetSurface, GeomEvaluator_Surface)

#endif // _GeomEvaluator_OffsetSurface_HeaderFile
