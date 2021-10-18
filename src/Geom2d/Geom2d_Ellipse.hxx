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

#ifndef _Geom2d_Ellipse_HeaderFile
#define _Geom2d_Ellipse_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Geom2d_Conic.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class gp_Elips2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;
class gp_Vec2d;
class gp_Trsf2d;
class Geom2d_Geometry;


class Geom2d_Ellipse;
DEFINE_STANDARD_HANDLE(Geom2d_Ellipse, Geom2d_Conic)

//! Describes an ellipse in the plane (2D space).
//! An ellipse is defined by its major and minor radii and,
//! as with any conic curve, is positioned in the plane
//! with a coordinate system (gp_Ax22d object) where:
//! - the origin is the center of the ellipse,
//! - the "X Direction" defines the major axis, and
//! - the "Y Direction" defines the minor axis.
//! This coordinate system is the local coordinate system of the ellipse.
//! The orientation (direct or indirect) of the local
//! coordinate system gives an explicit orientation to the
//! ellipse, determining the direction in which the
//! parameter increases along the ellipse.
//! The Geom2d_Ellipse ellipse is parameterized by an angle:
//! P(U) = O + MajorRad*Cos(U)*XDir + MinorRad*Sin(U)*YDir
//! where:
//! - P is the point of parameter U,
//! - O, XDir and YDir are respectively the origin, "X
//! Direction" and "Y Direction" of its local coordinate system,
//! - MajorRad and MinorRad are the major and
//! minor radii of the ellipse.
//! The "X Axis" of the local coordinate system therefore
//! defines the origin of the parameter of the ellipse.
//! An ellipse is a closed and periodic curve. The period
//! is 2.*Pi and the parameter range is [ 0,2.*Pi [.
//! See Also
//! GCE2d_MakeEllipse which provides functions for
//! more complex ellipse constructions
//! gp_Ax22d
//! gp_Elips2d for an equivalent, non-parameterized data structure
class Geom2d_Ellipse : public Geom2d_Conic
{

public:

  

  //! Creates an ellipse by conversion of the gp_Elips2d ellipse E.
  Standard_EXPORT Geom2d_Ellipse(const gp_Elips2d& E);
  
  //! Creates an ellipse defined by its major and minor radii,
  //! MajorRadius and MinorRadius, and positioned
  //! in the plane by its major axis MajorAxis; the
  //! center of the ellipse is the origin of MajorAxis
  //! and the unit vector of MajorAxis is the "X
  //! Direction" of the local coordinate system of the
  //! ellipse; this coordinate system is direct if Sense
  //! is true (default value) or indirect if Sense is false.
  //! Warnings :
  //! It is not forbidden to create an ellipse with MajorRadius =
  //! MinorRadius.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is less than MinorRadius, or
  //! - MinorRadius is less than 0.
  Standard_EXPORT Geom2d_Ellipse(const gp_Ax2d& MajorAxis, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Boolean Sense = Standard_True);
  
  //! Creates an ellipse defined by its major and minor radii,
  //! MajorRadius and MinorRadius, where the
  //! coordinate system Axis locates the ellipse and
  //! defines its orientation in the plane such that:
  //! - the center of the ellipse is the origin of Axis,
  //! - the "X Direction" of Axis defines the major
  //! axis of the ellipse,
  //! - the "Y Direction" of Axis defines the minor
  //! axis of the ellipse,
  //! - the orientation of Axis (direct or indirect)
  //! gives the orientation of the ellipse.
  //! Warnings :
  //! It is not forbidden to create an ellipse with MajorRadius =
  //! MinorRadius.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is less than MinorRadius, or
  //! - MinorRadius is less than 0.
  Standard_EXPORT Geom2d_Ellipse(const gp_Ax22d& Axis, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Converts the gp_Elips2d ellipse E into this ellipse.
  Standard_EXPORT void SetElips2d (const gp_Elips2d& E);
  
  //! Assigns a value to the major radius of this ellipse.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - the major radius of this ellipse becomes less than
  //! the minor radius, or
  //! - MinorRadius is less than 0.
  Standard_EXPORT void SetMajorRadius (const Standard_Real MajorRadius);
  
  //! Assigns a value to the minor radius of this ellipse.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - the major radius of this ellipse becomes less than
  //! the minor radius, or
  //! - MinorRadius is less than 0.
  Standard_EXPORT void SetMinorRadius (const Standard_Real MinorRadius);
  
  //! Converts this ellipse into a gp_Elips2d ellipse.
  Standard_EXPORT gp_Elips2d Elips2d() const;
  
  //! Computes the parameter on the reversed ellipse for
  //! the point of parameter U on this ellipse.
  //! For an ellipse, the returned value is: 2.*Pi - U.
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Computes the directrices of this ellipse.
  //! This directrix is the line normal to the XAxis of the ellipse
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the ellipse, where e is the eccentricity of
  //! the ellipse.
  //! This line is parallel to the "YAxis". The intersection point
  //! between directrix1 and the "XAxis" is the "Location" point
  //! of the directrix1. This point is on the positive side of
  //! the "XAxis".
  //! Raises ConstructionError if Eccentricity = 0.0. (The ellipse degenerates
  //! into a circle)
  Standard_EXPORT gp_Ax2d Directrix1() const;
  

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the ellipse.
  //! Raises ConstructionError if Eccentricity = 0.0. (The ellipse degenerates into a
  //! circle).
  Standard_EXPORT gp_Ax2d Directrix2() const;
  

  //! Returns the eccentricity of the ellipse  between 0.0 and 1.0
  //! If f is the distance between the center of the ellipse and
  //! the Focus1 then the eccentricity e = f / MajorRadius.
  //! Returns 0 if MajorRadius = 0
  Standard_EXPORT Standard_Real Eccentricity() const Standard_OVERRIDE;
  

  //! Computes the focal distance. The focal distance is the distance between the center
  //! and a focus of the ellipse.
  Standard_EXPORT Standard_Real Focal() const;
  

  //! Returns the first focus of the ellipse. This focus is on the
  //! positive side of the "XAxis" of the ellipse.
  Standard_EXPORT gp_Pnt2d Focus1() const;
  

  //! Returns the second focus of the ellipse. This focus is on
  //! the negative side of the "XAxis" of the ellipse.
  Standard_EXPORT gp_Pnt2d Focus2() const;
  
  //! Returns the major radius of this ellipse.
  Standard_EXPORT Standard_Real MajorRadius() const;
  
  //! Returns the minor radius of this ellipse.
  Standard_EXPORT Standard_Real MinorRadius() const;
  

  //! Computes the parameter of this ellipse. This value is
  //! given by the formula p = (1 - e * e) * MajorRadius where e is the eccentricity
  //! of the ellipse.
  //! Returns 0 if MajorRadius = 0
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! Returns the value of the first parameter of this
  //! ellipse. This is  0.0, which gives the start point of this ellipse.
  //! The start point and end point of an ellipse are coincident.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! Returns the value of the  last parameter of this
  //! ellipse. This is  2.*Pi, which gives the end point of this ellipse.
  //! The start point and end point of an ellipse are coincident.
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  //! return True.
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  //! return True.
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  //! Returns in P the point of parameter U.
  //! P = C + MajorRadius * Cos (U) * XDir + MinorRadius * Sin (U) * YDir
  //! where C is the center of the ellipse , XDir the direction of
  //! the "XAxis" and "YDir" the "YAxis" of the ellipse.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U. The vectors V1 and V2
  //! are the first and second derivatives at this point.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first second and
  //! third derivatives V1 V2 and V3.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const Standard_OVERRIDE;
  
  //! For the point of parameter U of this ellipse,
  //! computes the vector corresponding to the Nth derivative.
  //! Exceptions Standard_RangeError if N is less than 1.
  Standard_EXPORT gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Applies the transformation T to this ellipse.
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this ellipse.
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom2d_Ellipse,Geom2d_Conic)

protected:




private:


  Standard_Real majorRadius;
  Standard_Real minorRadius;


};







#endif // _Geom2d_Ellipse_HeaderFile
