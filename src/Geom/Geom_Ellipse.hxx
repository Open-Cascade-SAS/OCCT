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

#ifndef _Geom_Ellipse_HeaderFile
#define _Geom_Ellipse_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Geom_Conic.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class gp_Elips;
class gp_Ax2;
class gp_Ax1;
class gp_Pnt;
class gp_Vec;
class gp_Trsf;
class Geom_Geometry;


class Geom_Ellipse;
DEFINE_STANDARD_HANDLE(Geom_Ellipse, Geom_Conic)

//! Describes an ellipse in 3D space.
//! An ellipse is defined by its major and minor radii and,
//! as with any conic curve, is positioned in space with a
//! right-handed coordinate system (gp_Ax2 object) where:
//! - the origin is the center of the ellipse,
//! - the "X Direction" defines the major axis, and
//! - the "Y Direction" defines the minor axis.
//! The origin, "X Direction" and "Y Direction" of this
//! coordinate system define the plane of the ellipse. The
//! coordinate system is the local coordinate system of the ellipse.
//! The "main Direction" of this coordinate system is the
//! vector normal to the plane of the ellipse. The axis, of
//! which the origin and unit vector are respectively the
//! origin and "main Direction" of the local coordinate
//! system, is termed the "Axis" or "main Axis" of the ellipse.
//! The "main Direction" of the local coordinate system
//! gives an explicit orientation to the ellipse (definition of
//! the trigonometric sense), determining the direction in
//! which the parameter increases along the ellipse.
//! The Geom_Ellipse ellipse is parameterized by an angle:
//! P(U) = O + MajorRad*Cos(U)*XDir + MinorRad*Sin(U)*YDir
//! where:
//! - P is the point of parameter U,
//! - O, XDir and YDir are respectively the origin, "X
//! Direction" and "Y Direction" of its local coordinate system,
//! - MajorRad and MinorRad are the major and minor radii of the ellipse.
//! The "X Axis" of the local coordinate system therefore
//! defines the origin of the parameter of the ellipse.
//! An ellipse is a closed and periodic curve. The period
//! is 2.*Pi and the parameter range is [ 0, 2.*Pi [.
class Geom_Ellipse : public Geom_Conic
{

public:

  
  //! Constructs an ellipse by conversion of the gp_Elips ellipse E.
  Standard_EXPORT Geom_Ellipse(const gp_Elips& E);
  
  //! Constructs an ellipse
  //! defined by its major and minor radii, MajorRadius
  //! and MinorRadius, where A2 locates the ellipse
  //! and defines its orientation in 3D space such that:
  //! - the center of the ellipse is the origin of A2,
  //! - the "X Direction" of A2 defines the major axis
  //! of the ellipse, i.e. the major radius
  //! MajorRadius is measured along this axis,
  //! - the "Y Direction" of A2 defines the minor axis
  //! of the ellipse, i.e. the minor radius
  //! MinorRadius is measured along this axis,
  //! - A2 is the local coordinate system of the ellipse.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is less than MinorRadius, or
  //! - MinorRadius is less than 0.
  //! Warning The Geom package does not prevent the
  //! construction of an ellipse where MajorRadius and
  //! MinorRadius are equal.
  Standard_EXPORT Geom_Ellipse(const gp_Ax2& A2, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Converts the gp_Elips ellipse E into this ellipse.
  Standard_EXPORT void SetElips (const gp_Elips& E);
  
  //! Assigns a value to the major radius of this ellipse.
  //! ConstructionError raised if MajorRadius < MinorRadius.
  Standard_EXPORT void SetMajorRadius (const Standard_Real MajorRadius);
  
  //! Assigns a value to the minor radius of this ellipse.
  //! ConstructionError raised if MajorRadius < MinorRadius or if MinorRadius < 0.
  Standard_EXPORT void SetMinorRadius (const Standard_Real MinorRadius);
  

  //! returns the non transient ellipse from gp with the same
  Standard_EXPORT gp_Elips Elips() const;
  
  //! Computes the parameter on the reversed ellipse for
  //! the point of parameter U on this ellipse.
  //! For an ellipse, the returned value is: 2.*Pi - U.
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  

  //! This directrix is the line normal to the XAxis of the ellipse
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the ellipse, where e is the eccentricity of
  //! the ellipse.
  //! This line is parallel to the "YAxis". The intersection point
  //! between directrix1 and the "XAxis" is the "Location" point
  //! of the directrix1. This point is on the positive side of
  //! the "XAxis".
  //! Raised if Eccentricity = 0.0. (The ellipse degenerates
  //! into a circle)
  Standard_EXPORT gp_Ax1 Directrix1() const;
  

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the ellipse.
  //!
  //! Raised if Eccentricity = 0.0. (The ellipse degenerates into a
  //! circle).
  Standard_EXPORT gp_Ax1 Directrix2() const;
  

  //! Returns the eccentricity of the ellipse  between 0.0 and 1.0
  //! If f is the distance between the center of the ellipse and
  //! the Focus1 then the eccentricity e = f / MajorRadius.
  //! Returns 0 if MajorRadius = 0
  Standard_EXPORT Standard_Real Eccentricity() const Standard_OVERRIDE;
  

  //! Computes the focal distance. It is the distance between the
  //! the two focus of the ellipse.
  Standard_EXPORT Standard_Real Focal() const;
  

  //! Returns the first focus of the ellipse. This focus is on the
  //! positive side of the "XAxis" of the ellipse.
  Standard_EXPORT gp_Pnt Focus1() const;
  

  //! Returns the second focus of the ellipse. This focus is on
  //! the negative side of the "XAxis" of the ellipse.
  Standard_EXPORT gp_Pnt Focus2() const;
  
  //! Returns the major  radius of this ellipse.
  Standard_EXPORT Standard_Real MajorRadius() const;
  
  //! Returns the minor radius of this ellipse.
  Standard_EXPORT Standard_Real MinorRadius() const;
  

  //! Returns p = (1 - e * e) * MajorRadius where e is the eccentricity
  //! of the ellipse.
  //! Returns 0 if MajorRadius = 0
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! Returns the value of the first parameter of this
  //! ellipse. This is respectively:
  //! - 0.0, which gives the start point of this ellipse, or
  //! The start point and end point of an ellipse are coincident.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! Returns the value of the  last parameter of this
  //! ellipse. This is respectively:
  //! - 2.*Pi, which gives the end point of this ellipse.
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
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U. The vectors V1 and V2
  //! are the first and second derivatives at this point.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first second and
  //! third derivatives V1 V2 and V3.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const Standard_OVERRIDE;
  
  //! For the point of parameter U of this ellipse, computes
  //! the vector corresponding to the Nth derivative.
  //! Exceptions Standard_RangeError if N is less than 1.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Applies the transformation T to this ellipse.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this ellipse.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom_Ellipse,Geom_Conic)

protected:




private:


  Standard_Real majorRadius;
  Standard_Real minorRadius;


};







#endif // _Geom_Ellipse_HeaderFile
