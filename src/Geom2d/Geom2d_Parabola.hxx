// Created on: 1993-03-24
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

#ifndef _Geom2d_Parabola_HeaderFile
#define _Geom2d_Parabola_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Conic.hxx>
#include <Standard_Integer.hxx>
class gp_Parab2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;
class gp_Vec2d;
class gp_Trsf2d;
class Geom2d_Geometry;

class Geom2d_Parabola;
DEFINE_STANDARD_HANDLE(Geom2d_Parabola, Geom2d_Conic)

//! Describes a parabola in the plane (2D space).
//! A parabola is defined by its focal length (i.e. the
//! distance between its focus and its apex) and is
//! positioned in the plane with a coordinate system
//! (gp_Ax22d object) where:
//! - the origin is the apex of the parabola, and
//! - the "X Axis" defines the axis of symmetry; the
//! parabola is on the positive side of this axis.
//! This coordinate system is the local coordinate
//! system of the parabola.
//! The orientation (direct or indirect) of the local
//! coordinate system gives an explicit orientation to the
//! parabola, determining the direction in which the
//! parameter increases along the parabola.
//! The Geom_Parabola parabola is parameterized as follows:
//! P(U) = O + U*U/(4.*F)*XDir + U*YDir, where:
//! - P is the point of parameter U,
//! - O, XDir and YDir are respectively the origin, "X
//! Direction" and "Y Direction" of its local coordinate system,
//! - F is the focal length of the parabola.
//! The parameter of the parabola is therefore its Y
//! coordinate in the local coordinate system, with the "X
//! Axis" of the local coordinate system defining the
//! origin of the parameter.
//! The parameter range is ] -infinite,+infinite [.
class Geom2d_Parabola : public Geom2d_Conic
{

public:
  //! Creates a parabola from a non persistent one.
  Standard_EXPORT Geom2d_Parabola(const gp_Parab2d& Prb);

  //! Creates a parabola with its "MirrorAxis" and it's focal
  //! length "Focal".
  //! MirrorAxis is the axis of symmetry of the curve, it is the
  //! "XAxis". The "YAxis" is parallel to the directrix of the
  //! parabola and is in the direct sense if Sense is True.
  //! The "Location" point of "MirrorAxis" is the vertex of the parabola
  //! Raised if Focal < 0.0
  Standard_EXPORT Geom2d_Parabola(const gp_Ax2d&         MirrorAxis,
                                  const Standard_Real    Focal,
                                  const Standard_Boolean Sense = Standard_True);

  //! Creates a parabola with its Axis and it's focal
  //! length "Focal".
  //! The XDirection of Axis is the axis of symmetry of the curve,
  //! it is the "XAxis". The "YAxis" is parallel to the directrix of the
  //! parabola. The "Location" point of "Axis" is the vertex
  //! of the parabola.
  //! Raised if Focal < 0.0
  Standard_EXPORT Geom2d_Parabola(const gp_Ax22d& Axis, const Standard_Real Focal);

  //! D is the directrix of the parabola and F the focus point.
  //! The symmetry axis "XAxis" of the parabola is normal to the
  //! directrix and pass through the focus point F, but its
  //! "Location" point is the vertex of the parabola.
  //! The "YAxis" of the parabola is parallel to D and its "Location"
  //! point is the vertex of the parabola.
  Standard_EXPORT Geom2d_Parabola(const gp_Ax2d& D, const gp_Pnt2d& F);

  //! Assigns the value Focal to the focal length of this parabola.
  //! Exceptions Standard_ConstructionError if Focal is negative.
  Standard_EXPORT void SetFocal(const Standard_Real Focal);

  //! Converts the gp_Parab2d parabola Prb into this parabola.
  Standard_EXPORT void SetParab2d(const gp_Parab2d& Prb);

  //! Returns the non persistent parabola from gp with the same
  //! geometric properties as <me>.
  Standard_EXPORT gp_Parab2d Parab2d() const;

  //! Computes the parameter on the reversed parabola
  //! for the point of parameter U on this parabola.
  //! For a parabola, the returned value is -U.
  Standard_EXPORT Standard_Real ReversedParameter(const Standard_Real U) const Standard_OVERRIDE;

  //! Returns RealFirst from Standard.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;

  //! Returns  RealLast from Standard.
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;

  //! Returns False
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;

  //! Returns False
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;

  //! The directrix is parallel to the "YAxis" of the parabola.
  //! The "Location" point of the directrix is the intersection
  //! point between the directrix and the symmetry axis ("XAxis") of the parabola.
  Standard_EXPORT gp_Ax2d Directrix() const;

  //! Returns the eccentricity e = 1.0
  Standard_EXPORT Standard_Real Eccentricity() const Standard_OVERRIDE;

  //! Computes the focus of this parabola The focus is on the
  //! positive side of the "X Axis" of the local coordinate system of the parabola.
  Standard_EXPORT gp_Pnt2d Focus() const;

  //! Computes the focal length of this parabola.
  //! The focal length is the distance between the apex and the focus of the parabola.
  Standard_EXPORT Standard_Real Focal() const;

  //! Computes the parameter of this parabola, which is
  //! the distance between its focus and its directrix. This
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
  Standard_EXPORT void D0(const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;

  //! Returns the point P of parameter U and the first derivative V1.
  Standard_EXPORT void D1(const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1) const Standard_OVERRIDE;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  Standard_EXPORT void D2(const Standard_Real U,
                          gp_Pnt2d&           P,
                          gp_Vec2d&           V1,
                          gp_Vec2d&           V2) const Standard_OVERRIDE;

  //! Returns the point P of parameter U, the first second and third
  //! derivatives V1 V2 and V3.
  Standard_EXPORT void D3(const Standard_Real U,
                          gp_Pnt2d&           P,
                          gp_Vec2d&           V1,
                          gp_Vec2d&           V2,
                          gp_Vec2d&           V3) const Standard_OVERRIDE;

  //! For the point of parameter U of this parabola,
  //! computes the vector corresponding to the Nth derivative.
  //! Exceptions Standard_RangeError if N is less than 1.
  Standard_EXPORT gp_Vec2d DN(const Standard_Real    U,
                              const Standard_Integer N) const Standard_OVERRIDE;

  //! Applies the transformation T to this parabola.
  Standard_EXPORT void Transform(const gp_Trsf2d& T) Standard_OVERRIDE;

  //! Computes the parameter on the transformed
  //! parabola, for the point of parameter U on this parabola.
  //! For a parabola, the returned value is equal to U
  //! multiplied by the scale factor of transformation T.
  Standard_EXPORT Standard_Real TransformedParameter(const Standard_Real U,
                                                     const gp_Trsf2d&    T) const Standard_OVERRIDE;

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
  Standard_EXPORT Standard_Real
    ParametricTransformation(const gp_Trsf2d& T) const Standard_OVERRIDE;

  //! Creates a new object, which is a copy of this parabola.
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom2d_Parabola, Geom2d_Conic)

protected:
private:
  Standard_Real focalLength;
};

#endif // _Geom2d_Parabola_HeaderFile
