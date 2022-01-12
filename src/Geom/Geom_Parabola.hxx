// Created on: 1993-03-10
// Created by: JCV
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

#ifndef _Geom_Parabola_HeaderFile
#define _Geom_Parabola_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_Conic.hxx>
#include <Standard_Integer.hxx>
class gp_Parab;
class gp_Ax2;
class gp_Ax1;
class gp_Pnt;
class gp_Vec;
class gp_Trsf;
class Geom_Geometry;


class Geom_Parabola;
DEFINE_STANDARD_HANDLE(Geom_Parabola, Geom_Conic)

//! Describes a parabola in 3D space.
//! A parabola is defined by its focal length (i.e. the
//! distance between its focus and its apex) and is
//! positioned in space with a coordinate system
//! (gp_Ax2 object) where:
//! - the origin is the apex of the parabola,
//! - the "X Axis" defines the axis of symmetry; the
//! parabola is on the positive side of this axis,
//! - the origin, "X Direction" and "Y Direction" define the
//! plane of the parabola.
//! This coordinate system is the local coordinate
//! system of the parabola.
//! The "main Direction" of this coordinate system is a
//! vector normal to the plane of the parabola. The axis,
//! of which the origin and unit vector are respectively the
//! origin and "main Direction" of the local coordinate
//! system, is termed the "Axis" or "main Axis" of the parabola.
//! The "main Direction" of the local coordinate system
//! gives an explicit orientation to the parabola,
//! determining the direction in which the parameter
//! increases along the parabola.
//! The Geom_Parabola parabola is parameterized as follows:
//! P(U) = O + U*U/(4.*F)*XDir + U*YDir
//! where:
//! - P is the point of parameter U,
//! - O, XDir and YDir are respectively the origin, "X
//! Direction" and "Y Direction" of its local coordinate system,
//! - F is the focal length of the parabola.
//! The parameter of the parabola is therefore its Y
//! coordinate in the local coordinate system, with the "X
//! Axis" of the local coordinate system defining the origin
//! of the parameter.
//! The parameter range is ] -infinite, +infinite [.
class Geom_Parabola : public Geom_Conic
{

public:

  
  //! Creates a parabola from a non transient one.
  Standard_EXPORT Geom_Parabola(const gp_Parab& Prb);
  

  //! Creates a parabola with its local coordinate system "A2"
  //! and it's focal length "Focal".
  //! The XDirection of A2 defines the axis of symmetry of the
  //! parabola. The YDirection of A2 is parallel to the directrix
  //! of the parabola. The Location point of A2 is the vertex of
  //! the parabola
  //! Raised if Focal < 0.0
  Standard_EXPORT Geom_Parabola(const gp_Ax2& A2, const Standard_Real Focal);
  

  //! D is the directrix of the parabola and F the focus point.
  //! The symmetry axis (XAxis) of the parabola is normal to the
  //! directrix and pass through the focus point F, but its
  //! location point is the vertex of the parabola.
  //! The YAxis of the parabola is parallel to D and its location
  //! point is the vertex of the parabola. The normal to the plane
  //! of the parabola is the cross product between the XAxis and the
  //! YAxis.
  Standard_EXPORT Geom_Parabola(const gp_Ax1& D, const gp_Pnt& F);
  
  //! Assigns the value Focal to the focal distance of this parabola.
  //! Exceptions Standard_ConstructionError if Focal is negative.
  Standard_EXPORT void SetFocal (const Standard_Real Focal);
  
  //! Converts the gp_Parab parabola Prb into this parabola.
  Standard_EXPORT void SetParab (const gp_Parab& Prb);
  

  //! Returns the non transient parabola from gp with the same
  //! geometric properties as <me>.
  Standard_EXPORT gp_Parab Parab() const;
  
  //! Computes the parameter on the reversed parabola,
  //! for the point of parameter U on this parabola.
  //! For a parabola, the returned value is: -U.
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Returns the value of the first or last parameter of this
  //! parabola. This is, respectively:
  //! - Standard_Real::RealFirst(), or
  //! - Standard_Real::RealLast().
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! Returns the value of the first or last parameter of this
  //! parabola. This is, respectively:
  //! - Standard_Real::RealFirst(), or
  //! - Standard_Real::RealLast().
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  //! Returns False
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  //! Returns False
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  //! Computes the directrix of this parabola.
  //! This is a line normal to the axis of symmetry, in the
  //! plane of this parabola, located on the negative side
  //! of its axis of symmetry, at a distance from the apex
  //! equal to the focal length.
  //! The directrix is returned as an axis (gp_Ax1 object),
  //! where the origin is located on the "X Axis" of this parabola.
  Standard_EXPORT gp_Ax1 Directrix() const;
  
  //! Returns 1. (which is the eccentricity of any parabola).
  Standard_EXPORT Standard_Real Eccentricity() const Standard_OVERRIDE;
  
  //! Computes the focus of this parabola. The focus is on the
  //! positive side of the "X Axis" of the local coordinate
  //! system of the parabola.
  Standard_EXPORT gp_Pnt Focus() const;
  
  //! Computes the focal distance of this parabola
  //! The focal distance is the distance between the apex
  //! and the focus of the parabola.
  Standard_EXPORT Standard_Real Focal() const;
  
  //! Computes the parameter of this parabola which is the
  //! distance between its focus and its directrix. This
  //! distance is twice the focal length.
  //! If P is the parameter of the parabola, the equation of
  //! the parabola in its local coordinate system is: Y**2 = 2.*P*X.
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! Returns in P the point of parameter U.
  //! If U = 0 the returned point is the origin of the XAxis and
  //! the YAxis of the parabola and it is the vertex of the parabola.
  //! P = S + F * (U * U * XDir +  * U * YDir)
  //! where S is the vertex of the parabola, XDir the XDirection and
  //! YDir the YDirection of the parabola's local coordinate system.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U and the first derivative V1.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first second and third
  //! derivatives V1 V2 and V3.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const Standard_OVERRIDE;
  
  //! For the point of parameter U of this parabola,
  //! computes the vector corresponding to the Nth derivative.
  //! Exceptions Standard_RangeError if N is less than 1.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Applies the transformation T to this parabola.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  
  //! Returns the  parameter on the  transformed  curve for
  //! the transform of the point of parameter U on <me>.
  //!
  //! me->Transformed(T)->Value(me->TransformedParameter(U,T))
  //!
  //! is the same point as
  //!
  //! me->Value(U).Transformed(T)
  //!
  //! This methods returns <U> * T.ScaleFactor()
  Standard_EXPORT Standard_Real TransformedParameter (const Standard_Real U, const gp_Trsf& T) const Standard_OVERRIDE;
  
  //! Returns a  coefficient to compute the parameter on
  //! the transformed  curve  for  the transform  of the
  //! point on <me>.
  //!
  //! Transformed(T)->Value(U * ParametricTransformation(T))
  //!
  //! is the same point as
  //!
  //! Value(U).Transformed(T)
  //!
  //! This methods returns T.ScaleFactor()
  Standard_EXPORT Standard_Real ParametricTransformation (const gp_Trsf& T) const Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this parabola.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom_Parabola,Geom_Conic)

protected:




private:


  Standard_Real focalLength;


};







#endif // _Geom_Parabola_HeaderFile
