// Created on: 1993-09-13
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Blend_Function_HeaderFile
#define _Blend_Function_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Blend_AppFunction.hxx>
#include <Standard_Boolean.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
class gp_Pnt;
class gp_Vec;
class gp_Vec2d;
class Blend_Point;

//! Deferred class for a function used to compute a blending
//! surface between two surfaces, using a guide line.
//! The vector <X> used in Value, Values and Derivatives methods
//! has to be the vector of the parametric coordinates U1,V1,
//! U2,V2, of the extremities of a section on the first and
//! second surface.
class Blend_Function : public Blend_AppFunction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns 4.
  Standard_EXPORT int NbVariables() const override;

  //! Returns the point on the first support.
  Standard_EXPORT const gp_Pnt& Pnt1() const override;

  //! Returns the point on the second support.
  Standard_EXPORT const gp_Pnt& Pnt2() const override;

  //! Returns the point on the first surface, at parameter
  //! Sol(1),Sol(2) (Sol is the vector used in the call of
  //! IsSolution.
  Standard_EXPORT virtual const gp_Pnt& PointOnS1() const = 0;

  //! Returns the point on the second surface, at parameter
  //! Sol(3),Sol(4) (Sol is the vector used in the call of
  //! IsSolution.
  Standard_EXPORT virtual const gp_Pnt& PointOnS2() const = 0;

  //! Returns True when it is not possible to compute
  //! the tangent vectors at PointOnS1 and/or PointOnS2.
  Standard_EXPORT virtual bool IsTangencyPoint() const = 0;

  //! Returns the tangent vector at PointOnS1, in 3d space.
  Standard_EXPORT virtual const gp_Vec& TangentOnS1() const = 0;

  //! Returns the tangent vector at PointOnS1, in the
  //! parametric space of the first surface.
  Standard_EXPORT virtual const gp_Vec2d& Tangent2dOnS1() const = 0;

  //! Returns the tangent vector at PointOnS2, in 3d space.
  Standard_EXPORT virtual const gp_Vec& TangentOnS2() const = 0;

  //! Returns the tangent vector at PointOnS2, in the
  //! parametric space of the second surface.
  Standard_EXPORT virtual const gp_Vec2d& Tangent2dOnS2() const = 0;

  //! Returns the tangent vector at the section,
  //! at the beginning and the end of the section, and
  //! returns the normal (of the surfaces) at
  //! these points.
  Standard_EXPORT virtual void Tangent(const double U1,
                                       const double V1,
                                       const double U2,
                                       const double V2,
                                       gp_Vec&      TgFirst,
                                       gp_Vec&      TgLast,
                                       gp_Vec&      NormFirst,
                                       gp_Vec&      NormLast) const = 0;

  Standard_EXPORT virtual bool TwistOnS1() const;

  Standard_EXPORT virtual bool TwistOnS2() const;

  Standard_EXPORT void Section(const Blend_Point&            P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<double>&   Weigths) override = 0;

  //! Used for the first and last section
  //! The method returns true if the derivatives
  //! are computed, otherwise it returns false
  Standard_EXPORT bool Section(const Blend_Point&            P,
                               NCollection_Array1<gp_Pnt>&   Poles,
                               NCollection_Array1<gp_Vec>&   DPoles,
                               NCollection_Array1<gp_Vec>&   D2Poles,
                               NCollection_Array1<gp_Pnt2d>& Poles2d,
                               NCollection_Array1<gp_Vec2d>& DPoles2d,
                               NCollection_Array1<gp_Vec2d>& D2Poles2d,
                               NCollection_Array1<double>&   Weigths,
                               NCollection_Array1<double>&   DWeigths,
                               NCollection_Array1<double>&   D2Weigths) override;
};

#endif // _Blend_Function_HeaderFile
