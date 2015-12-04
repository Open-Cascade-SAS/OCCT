// Created on: 1996-11-21
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _GeomPlate_Surface_HeaderFile
#define _GeomPlate_Surface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Plate_Plate.hxx>
#include <Standard_Real.hxx>
#include <Geom_Surface.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_SequenceOfXY.hxx>
class Geom_Surface;
class Standard_RangeError;
class Standard_NoSuchObject;
class Geom_UndefinedDerivative;
class Geom_UndefinedValue;
class Plate_Plate;
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Curve;
class gp_Pnt;
class gp_Vec;
class Geom_Geometry;


class GeomPlate_Surface;
DEFINE_STANDARD_HANDLE(GeomPlate_Surface, Geom_Surface)


//! Describes the characteristics of plate surface objects
//! returned by BuildPlateSurface::Surface. These can be
//! used to verify the quality of the resulting surface before
//! approximating it to a Geom_BSpline surface generated
//! by MakeApprox. This proves necessary in cases where
//! you want to use the resulting surface as the support for
//! a shape. The algorithmically generated surface cannot
//! fill this function as is, and as a result must be converted first.
class GeomPlate_Surface : public Geom_Surface
{

public:

  
  Standard_EXPORT GeomPlate_Surface(const Handle(Geom_Surface)& Surfinit, const Plate_Plate& Surfinter);
  

  //! Reverses the U direction of parametrization of <me>.
  //! The bounds of the surface are not modified.
  Standard_EXPORT void UReverse() Standard_OVERRIDE;
  
  //! Return the  parameter on the  Ureversed surface for
  //! the point of parameter U on <me>.
  //!
  //! me->UReversed()->Value(me->UReversedParameter(U),V)
  //!
  //! is the same point as
  //!
  //! me->Value(U,V)
  Standard_EXPORT Standard_Real UReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  

  //! Reverses the V direction of parametrization of <me>.
  //! The bounds of the surface are not modified.
  Standard_EXPORT void VReverse() Standard_OVERRIDE;
  
  //! Return the  parameter on the  Vreversed surface for
  //! the point of parameter V on <me>.
  //!
  //! me->VReversed()->Value(U,me->VReversedParameter(V))
  //!
  //! is the same point as
  //!
  //! me->Value(U,V)
  Standard_EXPORT Standard_Real VReversedParameter (const Standard_Real V) const Standard_OVERRIDE;
  
  //! Computes the  parameters on the  transformed  surface for
  //! the transform of the point of parameters U,V on <me>.
  //!
  //! me->Transformed(T)->Value(U',V')
  //!
  //! is the same point as
  //!
  //! me->Value(U,V).Transformed(T)
  //!
  //! Where U',V' are the new values of U,V after calling
  //!
  //! me->TranformParameters(U,V,T)
  //!
  //! This methods does not change <U> and <V>
  //!
  //! It  can be redefined.  For  example on  the Plane,
  //! Cylinder, Cone, Revolved and Extruded surfaces.
  Standard_EXPORT virtual void TransformParameters (Standard_Real& U, Standard_Real& V, const gp_Trsf& T) const Standard_OVERRIDE;
  
  //! Returns a 2d transformation  used to find the  new
  //! parameters of a point on the transformed surface.
  //!
  //! me->Transformed(T)->Value(U',V')
  //!
  //! is the same point as
  //!
  //! me->Value(U,V).Transformed(T)
  //!
  //! Where U',V' are  obtained by transforming U,V with
  //! th 2d transformation returned by
  //!
  //! me->ParametricTransformation(T)
  //!
  //! This methods returns an identity transformation
  //!
  //! It  can be redefined.  For  example on  the Plane,
  //! Cylinder, Cone, Revolved and Extruded surfaces.
  Standard_EXPORT virtual gp_GTrsf2d ParametricTransformation (const gp_Trsf& T) const Standard_OVERRIDE;
  
  Standard_EXPORT void Bounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const Standard_OVERRIDE;
  

  //! Is the surface closed in the parametric direction U ?
  //! Returns True if for each parameter V  the distance
  //! between the point P (UFirst, V) and P (ULast, V) is
  //! lower or equal to Resolution from gp.  UFirst and ULast
  //! are the parametric bounds in the U direction.
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;
  

  //! Is the surface closed in the parametric direction V ?
  //! Returns True if for each parameter U  the distance
  //! between the point P (U, VFirst) and  P (U, VLast) is
  //! lower or equal to Resolution from gp.  VFirst and VLast
  //! are the parametric bounds in the V direction.
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;
  

  //! Is the parametrization of a surface periodic in the
  //! direction U ?
  //! It is possible only if the surface is closed in this
  //! parametric direction and if the following relation is
  //! satisfied :
  //! for each parameter V the distance between the point
  //! P (U, V)  and the point  P (U + T, V) is lower or equal
  //! to Resolution from package gp. T is the parametric period
  //! and must be a constant.
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;
  
  //! returns the Uperiod.
  //! raises if the surface is not uperiodic.
  Standard_EXPORT virtual Standard_Real UPeriod() const Standard_OVERRIDE;
  

  //! Is the parametrization of a surface periodic in the
  //! direction U ?
  //! It is possible only if the surface is closed in this
  //! parametric direction and if the following relation is
  //! satisfied :
  //! for each parameter V the distance between the point
  //! P (U, V)  and the point  P (U + T, V) is lower or equal
  //! to Resolution from package gp. T is the parametric period
  //! and must be a constant.
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;
  
  //! returns the Vperiod.
  //! raises if the surface is not vperiodic.
  Standard_EXPORT virtual Standard_Real VPeriod() const Standard_OVERRIDE;
  
  //! Computes the U isoparametric curve.
  Standard_EXPORT Handle(Geom_Curve) UIso (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Computes the V isoparametric curve.
  Standard_EXPORT Handle(Geom_Curve) VIso (const Standard_Real V) const Standard_OVERRIDE;
  

  //! Global Continuity of the surface in direction U and V :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the surface,
  //! C2 : continuity of the second derivative all along the surface,
  //! C3 : continuity of the third derivative all along the surface,
  //! G1 : tangency continuity all along the surface,
  //! G2 : curvature continuity all along the surface,
  //! CN : the order of continuity is infinite.
  //! Example :
  //! If the surface is C1 in the V parametric direction and C2
  //! in the U parametric direction Shape = C1.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  

  //! Returns the order of continuity of the surface in the
  //! U parametric direction.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCNu (const Standard_Integer N) const Standard_OVERRIDE;
  

  //! Returns the order of continuity of the surface in the
  //! V parametric direction.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCNv (const Standard_Integer N) const Standard_OVERRIDE;
  

  //! Computes the point of parameter U,V on the surface.
  //!
  //! Raised only for an "OffsetSurface" if it is not possible to
  //! compute the current point.
  Standard_EXPORT void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const Standard_OVERRIDE;
  

  //! Computes the point P and the first derivatives in the
  //! directions U and V at this point.
  //! Raised if the continuity of the surface is not C1.
  Standard_EXPORT void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const Standard_OVERRIDE;
  

  //! Computes the point P, the first and the second derivatives in
  //! the directions U and V at this point.
  //! Raised if the continuity of the surface is not C2.
  Standard_EXPORT void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const Standard_OVERRIDE;
  

  //! Computes the point P, the first,the second and the third
  //! derivatives in the directions U and V at this point.
  //! Raised if the continuity of the surface is not C2.
  Standard_EXPORT void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const Standard_OVERRIDE;
  
  //! ---Purpose ;
  //! Computes the derivative of order Nu in the direction U and Nv
  //! in the direction V at the point P(U, V).
  //!
  //! Raised if the continuity of the surface is not CNu in the U
  //! direction or not CNv in the V direction.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;
  

  //! Transformation of a geometric object. This tansformation
  //! can be a translation, a rotation, a symmetry, a scaling
  //! or a complex transformation obtained by combination of
  //! the previous elementaries transformations.
  //! (see class Transformation of the package Geom).
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom_Surface) CallSurfinit() const;
  
  Standard_EXPORT void SetBounds (const Standard_Real Umin, const Standard_Real Umax, const Standard_Real Vmin, const Standard_Real Vmax);
  
  Standard_EXPORT void RealBounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const;
  
  Standard_EXPORT void Constraints (TColgp_SequenceOfXY& Seq) const;




  DEFINE_STANDARD_RTTIEXT(GeomPlate_Surface,Geom_Surface)

protected:




private:


  Plate_Plate mySurfinter;
  Handle(Geom_Surface) mySurfinit;
  Standard_Real myUmin;
  Standard_Real myUmax;
  Standard_Real myVmin;
  Standard_Real myVmax;


};







#endif // _GeomPlate_Surface_HeaderFile
