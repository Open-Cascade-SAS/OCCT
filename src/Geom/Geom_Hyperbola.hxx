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

#ifndef _Geom_Hyperbola_HeaderFile
#define _Geom_Hyperbola_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Geom_Conic.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class gp_Hypr;
class gp_Ax2;
class gp_Ax1;
class gp_Pnt;
class gp_Vec;
class gp_Trsf;
class Geom_Geometry;


class Geom_Hyperbola;
DEFINE_STANDARD_HANDLE(Geom_Hyperbola, Geom_Conic)

//! Describes a branch of a hyperbola in 3D space.
//! A hyperbola is defined by its major and minor radii
//! and, as with any conic curve, is positioned in space
//! with a right-handed coordinate system (gp_Ax2 object) where:
//! - the origin is the center of the hyperbola,
//! - the "X Direction" defines the major axis, and
//! - the "Y Direction" defines the minor axis.
//! The origin, "X Direction" and "Y Direction" of this
//! coordinate system define the plane of the hyperbola.
//! The coordinate system is the local coordinate
//! system of the hyperbola.
//! The branch of the hyperbola described is the one
//! located on the positive side of the major axis.
//! The "main Direction" of the local coordinate system is
//! a vector normal to the plane of the hyperbola. The
//! axis, of which the origin and unit vector are
//! respectively the origin and "main Direction" of the
//! local coordinate system, is termed the "Axis" or "main
//! Axis" of the hyperbola.
//! The "main Direction" of the local coordinate system
//! gives an explicit orientation to the hyperbola,
//! determining the direction in which the parameter
//! increases along the hyperbola.
//! The Geom_Hyperbola hyperbola is parameterized as follows:
//! P(U) = O + MajRad*Cosh(U)*XDir + MinRad*Sinh(U)*YDir, where:
//! - P is the point of parameter U,
//! - O, XDir and YDir are respectively the origin, "X
//! Direction" and "Y Direction" of its local coordinate system,
//! - MajRad and MinRad are the major and minor radii of the hyperbola.
//! The "X Axis" of the local coordinate system therefore
//! defines the origin of the parameter of the hyperbola.
//! The parameter range is ] -infinite, +infinite [.
//! The following diagram illustrates the respective
//! positions, in the plane of the hyperbola, of the three
//! branches of hyperbolas constructed using the
//! functions OtherBranch, ConjugateBranch1 and
//! ConjugateBranch2: Defines the main branch of an hyperbola.
//! ^YAxis
//! |
//! FirstConjugateBranch
//! |
//! Other            |                Main
//! --------------------- C ------------------------------>XAxis
//! Branch           |                Branch
//! |
//! SecondConjugateBranch
//! |
//! Warning
//! The value of the major radius (on the major axis) can
//! be less than the value of the minor radius (on the minor axis).
class Geom_Hyperbola : public Geom_Conic
{

public:

  
  //! Constructs a hyperbola by conversion of the gp_Hypr hyperbola H.
  Standard_EXPORT Geom_Hyperbola(const gp_Hypr& H);
  
  //! Constructs a hyperbola defined by its major and
  //! minor radii, MajorRadius and MinorRadius, where A2 locates the
  //! hyperbola and defines its orientation in 3D space such that:
  //! - the center of the hyperbola is the origin of A2,
  //! - the "X Direction" of A2 defines the major axis
  //! of the hyperbola, i.e. the major radius
  //! MajorRadius is measured along this axis,
  //! - the "Y Direction" of A2 defines the minor axis
  //! of the hyperbola, i.e. the minor radius
  //! MinorRadius is measured along this axis,
  //! - A2 is the local coordinate system of the   hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is less than 0.0,
  //! - MinorRadius is less than 0.0.
  Standard_EXPORT Geom_Hyperbola(const gp_Ax2& A2, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Converts the gp_Hypr hyperbola H into this hyperbola.
  Standard_EXPORT void SetHypr (const gp_Hypr& H);
  
  //! Assigns a value to the major radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is less than 0.0, or
  //! - MinorRadius is less than 0.0.Raised if MajorRadius < 0.0
  Standard_EXPORT void SetMajorRadius (const Standard_Real MajorRadius);
  
  //! Assigns a value to the minor radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is less than 0.0, or
  //! - MinorRadius is less than 0.0.Raised if MajorRadius < 0.0
  Standard_EXPORT void SetMinorRadius (const Standard_Real MinorRadius);
  

  //! returns the non transient parabola from gp with the same
  //! geometric properties as <me>.
  Standard_EXPORT gp_Hypr Hypr() const;
  
  //! Computes the parameter on the reversed hyperbola,
  //! for the point of parameter U on this hyperbola.
  //! For a hyperbola, the returned value is: -U.
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Returns RealFirst from Standard.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! returns RealLast from Standard.
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  //! Returns False.
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  //! return False for an hyperbola.
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = (B/A)*X.
  //! Raises ConstructionError if MajorRadius = 0.0
  Standard_EXPORT gp_Ax1 Asymptote1() const;
  

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = -(B/A)*X.
  //! Raises ConstructionError if MajorRadius = 0.0
  Standard_EXPORT gp_Ax1 Asymptote2() const;
  

  //! This branch of hyperbola is on the positive side of the
  //! YAxis of <me>.
  Standard_EXPORT gp_Hypr ConjugateBranch1() const;
  

  //! This branch of hyperbola is on the negative side of the
  //! YAxis of <me>.
  //! Note: The diagram given under the class purpose
  //! indicates where these two branches of hyperbola are
  //! positioned in relation to this branch of hyperbola.
  Standard_EXPORT gp_Hypr ConjugateBranch2() const;
  

  //! This directrix is the line normal to the XAxis of the hyperbola
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the hyperbola, where e is the eccentricity of
  //! the hyperbola.
  //! This line is parallel to the YAxis. The intersection point between
  //! directrix1 and the XAxis is the location point of the directrix1.
  //! This point is on the positive side of the XAxis.
  Standard_EXPORT gp_Ax1 Directrix1() const;
  

  //! This line is obtained by the symmetrical transformation
  //! of "directrix1" with respect to the YAxis of the hyperbola.
  Standard_EXPORT gp_Ax1 Directrix2() const;
  

  //! Returns the eccentricity of the hyperbola (e > 1).
  //! If f is the distance between the location of the hyperbola
  //! and the Focus1 then the eccentricity e = f / MajorRadius.
  //! raised if MajorRadius = 0.0
  Standard_EXPORT Standard_Real Eccentricity() const Standard_OVERRIDE;
  

  //! Computes the focal distance. It is the distance between the
  //! two focus of the hyperbola.
  Standard_EXPORT Standard_Real Focal() const;
  

  //! Returns the first focus of the hyperbola. This focus is on the
  //! positive side of the XAxis of the hyperbola.
  Standard_EXPORT gp_Pnt Focus1() const;
  

  //! Returns the second focus of the hyperbola. This focus is on the
  //! negative side of the XAxis of the hyperbola.
  Standard_EXPORT gp_Pnt Focus2() const;
  
  //! Returns the major or minor radius of this hyperbola.
  //! The major radius is also the distance between the
  //! center of the hyperbola and the apex of the main
  //! branch (located on the "X Axis" of the hyperbola).
  Standard_EXPORT Standard_Real MajorRadius() const;
  
  //! Returns the major or minor radius of this hyperbola.
  //! The minor radius is also the distance between the
  //! center of the hyperbola and the apex of a conjugate
  //! branch (located on the "Y Axis" of the hyperbola).
  Standard_EXPORT Standard_Real MinorRadius() const;
  
  //! Computes the "other" branch of this hyperbola. This
  //! is the symmetrical branch with respect to the center of this hyperbola.
  //! Note: The diagram given under the class purpose
  //! indicates where the "other" branch is positioned in
  //! relation to this branch of the hyperbola.
  Standard_EXPORT gp_Hypr OtherBranch() const;
  

  //! Returns p = (e * e - 1) * MajorRadius where e is the
  //! eccentricity of the hyperbola.
  //! raised if MajorRadius = 0.0
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! Returns in P the point of parameter U.
  //! P = C + MajorRadius * Cosh (U) * XDir +
  //! MinorRadius * Sinh (U) * YDir
  //! where C is the center of the hyperbola , XDir the XDirection and
  //! YDir the YDirection of the hyperbola's local coordinate system.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U and the first derivative V1.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first second and
  //! third derivatives V1 V2 and V3.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const Standard_OVERRIDE;
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Applies the transformation T to this hyperbola.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this hyperbola.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom_Hyperbola,Geom_Conic)

protected:




private:


  Standard_Real majorRadius;
  Standard_Real minorRadius;


};







#endif // _Geom_Hyperbola_HeaderFile
