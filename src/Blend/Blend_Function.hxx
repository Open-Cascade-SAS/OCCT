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
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
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
class Blend_Function  : public Blend_AppFunction
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns 4.
  Standard_EXPORT Standard_Integer NbVariables() const Standard_OVERRIDE;
  
  //! Returns the point on the first support.
  Standard_EXPORT const gp_Pnt& Pnt1() const Standard_OVERRIDE;
  
  //! Returns the point on the seconde support.
  Standard_EXPORT const gp_Pnt& Pnt2() const Standard_OVERRIDE;
  
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
  Standard_EXPORT virtual Standard_Boolean IsTangencyPoint() const = 0;
  
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
  Standard_EXPORT virtual void Tangent (const Standard_Real U1,
                                        const Standard_Real V1,
                                        const Standard_Real U2,
                                        const Standard_Real V2,
                                        gp_Vec& TgFirst,
                                        gp_Vec& TgLast,
                                        gp_Vec& NormFirst,
                                        gp_Vec& NormLast) const = 0;
  
  Standard_EXPORT virtual Standard_Boolean TwistOnS1() const;
  
  Standard_EXPORT virtual Standard_Boolean TwistOnS2() const;
  
  Standard_EXPORT virtual void Section (const Blend_Point& P,
                                        TColgp_Array1OfPnt& Poles,
                                        TColgp_Array1OfPnt2d& Poles2d,
                                        TColStd_Array1OfReal& Weigths) Standard_OVERRIDE = 0;
  
  //! Used for the first and last section
  //! The method returns Standard_True if the derivatives
  //! are computed, otherwise it returns Standard_False
  Standard_EXPORT virtual Standard_Boolean Section (const Blend_Point& P,
                                                    TColgp_Array1OfPnt& Poles,
                                                    TColgp_Array1OfVec& DPoles,
                                                    TColgp_Array1OfVec& D2Poles,
                                                    TColgp_Array1OfPnt2d& Poles2d,
                                                    TColgp_Array1OfVec2d& DPoles2d,
                                                    TColgp_Array1OfVec2d& D2Poles2d,
                                                    TColStd_Array1OfReal& Weigths,
                                                    TColStd_Array1OfReal& DWeigths,
                                                    TColStd_Array1OfReal& D2Weigths) Standard_OVERRIDE;




protected:





private:





};







#endif // _Blend_Function_HeaderFile
