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

#ifndef _Geom2d_Hyperbola_HeaderFile
#define _Geom2d_Hyperbola_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Geom2d_Conic.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class gp_Hypr2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;
class gp_Vec2d;
class gp_Trsf2d;
class Geom2d_Geometry;


class Geom2d_Hyperbola;
DEFINE_STANDARD_HANDLE(Geom2d_Hyperbola, Geom2d_Conic)

//! Describes a branch of a hyperbola in the plane (2D space).
//! A hyperbola is defined by its major and minor radii
//! and, as with any conic curve, is positioned in the
//! plane with a coordinate system (gp_Ax22d object) where:
//! - the origin is the center of the hyperbola,
//! - the "X Direction" defines the major axis, and
//! - the "Y Direction" defines the minor axis.
//! This coordinate system is the local coordinate
//! system of the hyperbola.
//! The branch of the hyperbola described is the one
//! located on the positive side of the major axis.
//! The orientation (direct or indirect) of the local
//! coordinate system gives an explicit orientation to the
//! hyperbola, determining the direction in which the
//! parameter increases along the hyperbola.
//! The Geom2d_Hyperbola hyperbola is parameterized as follows:
//! P(U) = O + MajRad*Cosh(U)*XDir + MinRad*Sinh(U)*YDir
//! where:
//! - P is the point of parameter U,
//! - O, XDir and YDir are respectively the origin, "X
//! Direction" and "Y Direction" of its local coordinate system,
//! - MajRad and MinRad are the major and minor radii of the hyperbola.
//! The "X Axis" of the local coordinate system therefore
//! defines the origin of the parameter of the hyperbola.
//! The parameter range is ] -infinite,+infinite [.
//! The following diagram illustrates the respective
//! positions, in the plane of the hyperbola, of the three
//! branches of hyperbolas constructed using the
//! functions OtherBranch, ConjugateBranch1 and
//! ConjugateBranch2:
//! ^YAxis
//! |
//! FirstConjugateBranch
//! |
//! Other         |          Main
//! --------------------- C
//! --------------------->XAxis
//! Branch       |
//! Branch
//! |
//! SecondConjugateBranch
//! |
//! Warning
//! The value of the major radius (on the major axis) can
//! be less than the value of the minor radius (on the minor axis).
//! See Also
//! GCE2d_MakeHyperbola which provides functions for
//! more complex hyperbola constructions
//! gp_Ax22d
//! gp_Hypr2d for an equivalent, non-parameterized data structure
class Geom2d_Hyperbola : public Geom2d_Conic
{

public:

  
  //! Creates  an Hyperbola from a non persistent one from package gp
  Standard_EXPORT Geom2d_Hyperbola(const gp_Hypr2d& H);
  

  //! MajorAxis is the "XAxis" of the hyperbola.
  //! The YAxis is in the direct sense if "Sense" is True;
  //! The major radius of the hyperbola is on this "XAxis" and
  //! the minor radius is on the "YAxis" of the hyperbola.
  //! Raised if MajorRadius < 0.0 or if MinorRadius < 0.0
  Standard_EXPORT Geom2d_Hyperbola(const gp_Ax2d& MajorAxis, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Boolean Sense = Standard_True);
  

  //! The XDirection of "Axis" is the "XAxis" of the hyperbola and
  //! the YDirection of "Axis" is the "YAxis".
  //! The major radius of the hyperbola is on this "XAxis" and
  //! the minor radius is on the "YAxis" of the hyperbola.
  //! Raised if MajorRadius < 0.0 or if MinorRadius < 0.0
  Standard_EXPORT Geom2d_Hyperbola(const gp_Ax22d& Axis, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Converts the gp_Hypr2d hyperbola H into this hyperbola.
  Standard_EXPORT void SetHypr2d (const gp_Hypr2d& H);
  
  //! Assigns a value to the major or minor radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is less than 0.0,
  //! - MinorRadius is less than 0.0.
  Standard_EXPORT void SetMajorRadius (const Standard_Real MajorRadius);
  
  //! Assigns a value to the major or minor radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! - MajorRadius is less than 0.0,
  //! - MinorRadius is less than 0.0.
  Standard_EXPORT void SetMinorRadius (const Standard_Real MinorRadius);
  
  //! Converts this hyperbola into a gp_Hypr2d one.
  Standard_EXPORT gp_Hypr2d Hypr2d() const;
  
  //! Computes the parameter on the reversed hyperbola,
  //! for the point of parameter U on this hyperbola.
  //! For a hyperbola, the returned value is -U.
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Returns RealFirst from Standard.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! returns RealLast from Standard.
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  //! Returns False.
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  //! return False for an hyperbola.
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  

  //! In the local coordinate system of the hyperbola the
  //! equation of the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0
  //! and the equation of the first asymptote is Y = (B/A)*X
  //! where A is the major radius of the hyperbola and B is the
  //! minor radius of the hyperbola.
  //! Raised if MajorRadius = 0.0
  Standard_EXPORT gp_Ax2d Asymptote1() const;
  

  //! In the local coordinate system of the hyperbola the
  //! equation of the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0
  //! and the equation of the first asymptote is Y = -(B/A)*X.
  //! where A is the major radius of the hyperbola and B is the
  //! minor radius of the hyperbola.
  //! raised if MajorRadius = 0.0
  Standard_EXPORT gp_Ax2d Asymptote2() const;
  
  //! Computes the first conjugate branch relative to this hyperbola.
  //! Note: The diagram given under the class purpose
  //! indicates where these two branches of hyperbola are
  //! positioned in relation to this branch of hyperbola.
  Standard_EXPORT gp_Hypr2d ConjugateBranch1() const;
  
  //! Computes the second conjugate branch relative to this hyperbola.
  //! Note: The diagram given under the class purpose
  //! indicates where these two branches of hyperbola are
  //! positioned in relation to this branch of hyperbola.
  Standard_EXPORT gp_Hypr2d ConjugateBranch2() const;
  

  //! This directrix is the line normal to the XAxis of the hyperbola
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the hyperbola, where e is the eccentricity of
  //! the hyperbola.
  //! This line is parallel to the "YAxis". The intersection point
  //! between directrix1 and the "XAxis" is the location point of the
  //! directrix1. This point is on the positive side of the "XAxis".
  Standard_EXPORT gp_Ax2d Directrix1() const;
  

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the hyperbola.
  Standard_EXPORT gp_Ax2d Directrix2() const;
  

  //! Returns the eccentricity of the hyperbola (e > 1).
  //! If f is the distance between the location of the hyperbola
  //! and the Focus1 then the eccentricity e = f / MajorRadius.
  //! raised if MajorRadius = 0.0
  Standard_EXPORT Standard_Real Eccentricity() const Standard_OVERRIDE;
  

  //! Computes the focal distance. It is the distance between the
  //! two focus of the hyperbola.
  Standard_EXPORT Standard_Real Focal() const;
  

  //! Returns the first focus of the hyperbola. This focus is on the
  //! positive side of the "XAxis" of the hyperbola.
  Standard_EXPORT gp_Pnt2d Focus1() const;
  

  //! Returns the second focus of the hyperbola. This focus is on the
  //! negative side of the "XAxis" of the hyperbola.
  Standard_EXPORT gp_Pnt2d Focus2() const;
  
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
  //! is a symmetrical branch with respect to the center of this hyperbola.
  //! Note: The diagram given under the class purpose
  //! indicates where the "other" branch is positioned in
  //! relation to this branch of the hyperbola.
  //! ^ YAxis
  //! |
  //! FirstConjugateBranch
  //! |
  //! Other   | Main
  //! ---------------------------- C
  //! ------------------------------------------&gtXAxis
  //! Branch |  Branch
  //! |
  //! |
  //! SecondConjugateBranch
  //! |
  //! Warning
  //! The major radius can be less than the minor radius.
  Standard_EXPORT gp_Hypr2d OtherBranch() const;
  
  //! Computes the parameter of this hyperbola.
  //! The parameter is:
  //! p = (e*e - 1) * MajorRadius
  //! where e is the eccentricity of this hyperbola and
  //! MajorRadius its major radius.
  //! Exceptions
  //! Standard_DomainError if the major radius of this
  //! hyperbola is null.
  Standard_EXPORT Standard_Real Parameter() const;
  
  //! Returns in P the point of parameter U.
  //! P = C + MajorRadius * Cosh (U) * XDir +
  //! MinorRadius * Sinh (U) * YDir
  //! where C is the center of the hyperbola , XDir the XDirection and
  //! YDir the YDirection of the hyperbola's local coordinate system.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U and the first derivative V1.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first second and
  //! third derivatives V1 V2 and V3.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const Standard_OVERRIDE;
  
  //! For the point of parameter U of this hyperbola,
  //! computes the vector corresponding to the Nth derivative.
  //! Exceptions Standard_RangeError if N is less than 1.
  Standard_EXPORT gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Applies the transformation T to this hyperbola.
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this hyperbola.
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom2d_Hyperbola,Geom2d_Conic)

protected:




private:


  Standard_Real majorRadius;
  Standard_Real minorRadius;


};







#endif // _Geom2d_Hyperbola_HeaderFile
