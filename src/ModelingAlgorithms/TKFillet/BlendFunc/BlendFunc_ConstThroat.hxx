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
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>

class math_Matrix;

//! Class for a function used to compute a symmetric chamfer
//! with constant throat that is the height of isosceles triangle in section
class BlendFunc_ConstThroat : public BlendFunc_GenChamfer
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BlendFunc_ConstThroat(const occ::handle<Adaptor3d_Surface>& S1,
                                        const occ::handle<Adaptor3d_Surface>& S2,
                                        const occ::handle<Adaptor3d_Curve>&   C);

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Derivatives(const math_Vector& X,
                                               math_Matrix&       D) override;

  Standard_EXPORT void Set(const double Param) override;

  Standard_EXPORT bool IsSolution(const math_Vector&  Sol,
                                              const double Tol) override;

  Standard_EXPORT const gp_Pnt& PointOnS1() const override;

  Standard_EXPORT const gp_Pnt& PointOnS2() const override;

  Standard_EXPORT bool IsTangencyPoint() const override;

  Standard_EXPORT const gp_Vec& TangentOnS1() const override;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnS1() const override;

  Standard_EXPORT const gp_Vec& TangentOnS2() const override;

  Standard_EXPORT const gp_Vec2d& Tangent2dOnS2() const override;

  //! Returns the tangent vector at the section,
  //! at the beginning and the end of the section, and
  //! returns the normal (of the surfaces) at
  //! these points.
  Standard_EXPORT void Tangent(const double U1,
                               const double V1,
                               const double U2,
                               const double V2,
                               gp_Vec&             TgFirst,
                               gp_Vec&             TgLast,
                               gp_Vec&             NormFirst,
                               gp_Vec&             NormLast) const override;

  //! Sets the throat and the "quadrant".
  Standard_EXPORT void Set(const double aThroat,
                           const double,
                           const int Choix) override;

  //! Returns the length of the maximum section
  Standard_EXPORT double GetSectionSize() const override;

protected:
  gp_Pnt           pts1;
  gp_Pnt           pts2;
  gp_Vec           d1u1;
  gp_Vec           d1v1;
  gp_Vec           d1u2;
  gp_Vec           d1v2;
  bool istangent;
  gp_Vec           tg1;
  gp_Vec2d         tg12d;
  gp_Vec           tg2;
  gp_Vec2d         tg22d;
  double    param;
  double    Throat;

  gp_Pnt        ptgui;
  gp_Vec        nplan;
  double normtg;
  double theD;
  gp_Vec        d1gui;
  gp_Vec        d2gui;

};

#endif // _BlendFunc_ConstThroat_HeaderFile
