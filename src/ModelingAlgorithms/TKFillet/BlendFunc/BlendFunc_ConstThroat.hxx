// Created by: Julia GERASIMOVA
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

#ifndef _BlendFunc_ConstThroat_HeaderFile
#define _BlendFunc_ConstThroat_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <BlendFunc_GenChamfer.hxx>
#include <math_Vector.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfVec2d.hxx>

class math_Matrix;

//! Class for a function used to compute a symmetric chamfer
//! with constant throat that is the height of isosceles triangle in section
class BlendFunc_ConstThroat : public BlendFunc_GenChamfer
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_ConstThroat(const Handle(Adaptor3d_Surface)& S1,
                                        const Handle(Adaptor3d_Surface)& S2,
                                        const Handle(Adaptor3d_Curve)&   C);

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, math_Vector& F) Standard_OVERRIDE;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives(const math_Vector& X,
                                               math_Matrix&       D) Standard_OVERRIDE;

  Standard_EXPORT void Set(const Standard_Real Param) Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsSolution(const math_Vector&  Sol,
                                              const Standard_Real Tol) Standard_OVERRIDE;

  Standard_EXPORT const gp_Pnt& PointOnS1() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Pnt& PointOnS2() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsTangencyPoint() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Vec& TangentOnS1() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnS1() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Vec& TangentOnS2() const Standard_OVERRIDE;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnS2() const Standard_OVERRIDE;

  //! Returns the tangent vector at the section,
  //! at the beginning and the end of the section, and
  //! returns the normal (of the surfaces) at
  //! these points.
  Standard_EXPORT void Tangent(const Standard_Real U1,
                               const Standard_Real V1,
                               const Standard_Real U2,
                               const Standard_Real V2,
                               gp_Vec&             TgFirst,
                               gp_Vec&             TgLast,
                               gp_Vec&             NormFirst,
                               gp_Vec&             NormLast) const Standard_OVERRIDE;

  //! Sets the throat and the "quadrant".
  Standard_EXPORT void Set(const Standard_Real aThroat,
                           const Standard_Real,
                           const Standard_Integer Choix) Standard_OVERRIDE;

  //! Returns the length of the maximum section
  Standard_EXPORT Standard_Real GetSectionSize() const Standard_OVERRIDE;

protected:
  gp_Pnt           pts1;
  gp_Pnt           pts2;
  gp_Vec           d1u1;
  gp_Vec           d1v1;
  gp_Vec           d1u2;
  gp_Vec           d1v2;
  Standard_Boolean istangent;
  gp_Vec           tg1;
  gp_Vec2d         tg12d;
  gp_Vec           tg2;
  gp_Vec2d         tg22d;
  Standard_Real    param;
  Standard_Real    Throat;

  gp_Pnt        ptgui;
  gp_Vec        nplan;
  Standard_Real normtg;
  Standard_Real theD;
  gp_Vec        d1gui;
  gp_Vec        d2gui;

private:
};

#endif // _BlendFunc_ConstThroat_HeaderFile
