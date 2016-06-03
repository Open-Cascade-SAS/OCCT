// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _gp_Hypr2d_HeaderFile
#define _gp_Hypr2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax22d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
class Standard_ConstructionError;
class Standard_DomainError;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;
class gp_Trsf2d;
class gp_Vec2d;



//! Describes a branch of a hyperbola in the plane (2D space).
//! A hyperbola is defined by its major and minor radii, and
//! positioned in the plane with a coordinate system (a
//! gp_Ax22d object) of which:
//! -   the origin is the center of the hyperbola,
//! -   the "X Direction" defines the major axis of the hyperbola, and
//! -   the "Y Direction" defines the minor axis of the hyperbola.
//! This coordinate system is the "local coordinate system"
//! of the hyperbola. The orientation of this coordinate
//! system (direct or indirect) gives an implicit orientation to
//! the hyperbola. In this coordinate system, the equation of
//! the hyperbola is:
//! X*X/(MajorRadius**2)-Y*Y/(MinorRadius**2) = 1.0
//! The branch of the hyperbola described is the one located
//! on the positive side of the major axis.
//! The following schema shows the plane of the hyperbola,
//! and in it, the respective positions of the three branches of
//! hyperbolas constructed with the functions OtherBranch,
//! ConjugateBranch1, and ConjugateBranch2:
//! ^YAxis
//! |
//! FirstConjugateBranch
//! |
//! Other            |                Main
//! --------------------- C ------------------------------>XAxis
//! Branch           |                Branch
//! |
//! |
//! SecondConjugateBranch
//! |
//!
//! Warning
//! The major radius can be less than the minor radius.
//! See Also
//! gce_MakeHypr2d which provides functions for more
//! complex hyperbola constructions
//! Geom2d_Hyperbola which provides additional functions
//! for constructing hyperbolas and works, in particular, with
//! the parametric equations of hyperbolas
class gp_Hypr2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates of an indefinite hyperbola.
    gp_Hypr2d();
  

  //! Creates a hyperbola with radii MajorRadius and
  //! MinorRadius, centered on the origin of MajorAxis
  //! and where the unit vector of MajorAxis is the "X
  //! Direction" of the local coordinate system of the
  //! hyperbola. This coordinate system is direct if Sense
  //! is true (the default value), and indirect if Sense is false.
  //! Warnings :
  //! It is yet  possible to create an Hyperbola with
  //! MajorRadius <= MinorRadius.
  //! Raises ConstructionError if MajorRadius < 0.0 or MinorRadius < 0.0
  gp_Hypr2d(const gp_Ax2d& MajorAxis, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Boolean Sense = Standard_True);
  

  //! a hyperbola with radii MajorRadius and
  //! MinorRadius, positioned in the plane by coordinate system A where:
  //! -   the origin of A is the center of the hyperbola,
  //! -   the "X Direction" of A defines the major axis of
  //! the hyperbola, that is, the major radius
  //! MajorRadius is measured along this axis, and
  //! -   the "Y Direction" of A defines the minor axis of
  //! the hyperbola, that is, the minor radius
  //! MinorRadius is measured along this axis, and
  //! -   the orientation (direct or indirect sense) of A
  //! gives the implicit orientation of the hyperbola.
  //! Warnings :
  //! It is yet  possible to create an Hyperbola with
  //! MajorRadius <= MinorRadius.
  //! Raises ConstructionError if MajorRadius < 0.0 or MinorRadius < 0.0
    gp_Hypr2d(const gp_Ax22d& A, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Modifies this hyperbola, by redefining its local
  //! coordinate system so that its origin becomes P.
    void SetLocation (const gp_Pnt2d& P);
  
  //! Modifies the major or minor radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if MajorRadius or
  //! MinorRadius is negative.
    void SetMajorRadius (const Standard_Real MajorRadius);
  
  //! Modifies the major or minor radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if MajorRadius or
  //! MinorRadius is negative.
    void SetMinorRadius (const Standard_Real MinorRadius);
  
  //! Modifies this hyperbola, by redefining its local
  //! coordinate system so that it becomes A.
    void SetAxis (const gp_Ax22d& A);
  

  //! Changes the major axis of the hyperbola. The minor axis is
  //! recomputed and the location of the hyperbola too.
    void SetXAxis (const gp_Ax2d& A);
  

  //! Changes the minor axis of the hyperbola.The minor axis is
  //! recomputed and the location of the hyperbola too.
    void SetYAxis (const gp_Ax2d& A);
  

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = (B/A)*X
  //! where A is the major radius of the hyperbola and B the minor
  //! radius of the hyperbola.
  //! Raises ConstructionError if MajorRadius = 0.0
    gp_Ax2d Asymptote1() const;
  

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = -(B/A)*X
  //! where A is the major radius of the hyperbola and B the minor
  //! radius of the hyperbola.
  //! Raises ConstructionError if MajorRadius = 0.0
    gp_Ax2d Asymptote2() const;
  

  //! Computes the coefficients of the implicit equation of
  //! the hyperbola :
  //! A * (X**2) + B * (Y**2) + 2*C*(X*Y) + 2*D*X + 2*E*Y + F = 0.
  Standard_EXPORT void Coefficients (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D, Standard_Real& E, Standard_Real& F) const;
  

  //! Computes the branch of hyperbola which is on the positive side of the
  //! "YAxis" of <me>.
    gp_Hypr2d ConjugateBranch1() const;
  

  //! Computes the branch of hyperbola which is on the negative side of the
  //! "YAxis" of <me>.
    gp_Hypr2d ConjugateBranch2() const;
  

  //! Computes the directrix which is the line normal to the XAxis of the hyperbola
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the hyperbola, where e is the eccentricity of
  //! the hyperbola.
  //! This line is parallel to the "YAxis". The intersection point
  //! between the "Directrix1" and the "XAxis" is the "Location" point
  //! of the "Directrix1".
  //! This point is on the positive side of the "XAxis".
    gp_Ax2d Directrix1() const;
  

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the hyperbola.
    gp_Ax2d Directrix2() const;
  

  //! Returns the excentricity of the hyperbola (e > 1).
  //! If f is the distance between the location of the hyperbola
  //! and the Focus1 then the eccentricity e = f / MajorRadius. Raises DomainError if MajorRadius = 0.0.
    Standard_Real Eccentricity() const;
  

  //! Computes the focal distance. It is the distance between the
  //! "Location" of the hyperbola and "Focus1" or "Focus2".
    Standard_Real Focal() const;
  

  //! Returns the first focus of the hyperbola. This focus is on the
  //! positive side of the "XAxis" of the hyperbola.
    gp_Pnt2d Focus1() const;
  

  //! Returns the second focus of the hyperbola. This focus is on the
  //! negative side of the "XAxis" of the hyperbola.
    gp_Pnt2d Focus2() const;
  

  //! Returns  the location point of the hyperbola.
  //! It is the intersection point between the "XAxis" and
  //! the "YAxis".
    const gp_Pnt2d& Location() const;
  

  //! Returns the major radius of the hyperbola (it is the radius
  //! corresponding to the "XAxis" of the hyperbola).
    Standard_Real MajorRadius() const;
  

  //! Returns the minor radius of the hyperbola (it is the radius
  //! corresponding to the "YAxis" of the hyperbola).
    Standard_Real MinorRadius() const;
  

  //! Returns the branch of hyperbola obtained by doing the
  //! symmetrical transformation of <me> with respect to the
  //! "YAxis" of <me>.
    gp_Hypr2d OtherBranch() const;
  

  //! Returns p = (e * e - 1) * MajorRadius where e is the
  //! eccentricity of the hyperbola.
  //! Raises DomainError if MajorRadius = 0.0
    Standard_Real Parameter() const;
  
  //! Returns the axisplacement of the hyperbola.
    const gp_Ax22d& Axis() const;
  
  //! Computes an axis whose
  //! -   the origin is the center of this hyperbola, and
  //! -   the unit vector is the "X Direction" or "Y Direction"
  //! respectively of the local coordinate system of this hyperbola
  //! Returns the major axis of the hyperbola.
  gp_Ax2d XAxis() const;
  
  //! Computes an axis whose
  //! -   the origin is the center of this hyperbola, and
  //! -   the unit vector is the "X Direction" or "Y Direction"
  //! respectively of the local coordinate system of this hyperbola
  //! Returns the minor axis of the hyperbola.
    gp_Ax2d YAxis() const;
  
    void Reverse();
  
  //! Reverses the orientation of the local coordinate system
  //! of this hyperbola (the "Y Axis" is reversed). Therefore,
  //! the implicit orientation of this hyperbola is reversed.
  //! Note:
  //! -   Reverse assigns the result to this hyperbola, while
  //! -   Reversed creates a new one.
    gp_Hypr2d Reversed() const;
  
  //! Returns true if the local coordinate system is direct
  //! and false in the other case.
    Standard_Boolean IsDirect() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt2d& P);
  

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect  to the point P which is the center of the symmetry.
  Standard_EXPORT gp_Hypr2d Mirrored (const gp_Pnt2d& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2d& A);
  

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect to an axis placement which is the axis of the symmetry.
  Standard_EXPORT gp_Hypr2d Mirrored (const gp_Ax2d& A) const;
  
    void Rotate (const gp_Pnt2d& P, const Standard_Real Ang);
  

  //! Rotates an hyperbola. P is the center of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Hypr2d Rotated (const gp_Pnt2d& P, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt2d& P, const Standard_Real S);
  

  //! Scales an hyperbola. <S> is the scaling value.
  //! If <S> is positive only the location point is
  //! modified. But if <S> is negative the "XAxis" is
  //! reversed and the "YAxis" too.
    gp_Hypr2d Scaled (const gp_Pnt2d& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf2d& T);
  

  //! Transforms an hyperbola with the transformation T from
  //! class Trsf2d.
    gp_Hypr2d Transformed (const gp_Trsf2d& T) const;
  
    void Translate (const gp_Vec2d& V);
  

  //! Translates an hyperbola in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Hypr2d Translated (const gp_Vec2d& V) const;
  
    void Translate (const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  

  //! Translates an hyperbola from the point P1 to the point P2.
    gp_Hypr2d Translated (const gp_Pnt2d& P1, const gp_Pnt2d& P2) const;




protected:





private:



  gp_Ax22d pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;


};


#include <gp_Hypr2d.lxx>





#endif // _gp_Hypr2d_HeaderFile
