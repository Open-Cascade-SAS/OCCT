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

#ifndef _gp_Hypr_HeaderFile
#define _gp_Hypr_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax2.hxx>
#include <Standard_Real.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
class Standard_ConstructionError;
class Standard_DomainError;
class gp_Ax2;
class gp_Ax1;
class gp_Pnt;
class gp_Trsf;
class gp_Vec;


//! Describes a branch of a hyperbola in 3D space.
//! A hyperbola is defined by its major and minor radii and
//! positioned in space with a coordinate system (a gp_Ax2
//! object) of which:
//! -   the origin is the center of the hyperbola,
//! -   the "X Direction" defines the major axis of the
//! hyperbola, and
//! - the "Y Direction" defines the minor axis of the hyperbola.
//! The origin, "X Direction" and "Y Direction" of this
//! coordinate system together define the plane of the
//! hyperbola. This coordinate system is the "local
//! coordinate system" of the hyperbola. In this coordinate
//! system, the equation of the hyperbola is:
//! X*X/(MajorRadius**2)-Y*Y/(MinorRadius**2) = 1.0
//! The branch of the hyperbola described is the one located
//! on the positive side of the major axis.
//! The "main Direction" of the local coordinate system is a
//! normal vector to the plane of the hyperbola. This vector
//! gives an implicit orientation to the hyperbola. We refer to
//! the "main Axis" of the local coordinate system as the
//! "Axis" of the hyperbola.
//! The following schema shows the plane of the hyperbola,
//! and in it, the respective positions of the three branches of
//! hyperbolas constructed with the functions OtherBranch,
//! ConjugateBranch1, and ConjugateBranch2:
//!
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
//! |                  ^YAxis
//! Warning
//! The major radius can be less than the minor radius.
//! See Also
//! gce_MakeHypr which provides functions for more
//! complex hyperbola constructions
//! Geom_Hyperbola which provides additional functions for
//! constructing hyperbolas and works, in particular, with the
//! parametric equations of hyperbolas
class gp_Hypr 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates of an indefinite hyperbola.
    gp_Hypr();
  
  //! Creates a hyperbola with radii MajorRadius and
  //! MinorRadius, positioned in the space by the
  //! coordinate system A2 such that:
  //! -   the origin of A2 is the center of the hyperbola,
  //! -   the "X Direction" of A2 defines the major axis of
  //! the hyperbola, that is, the major radius
  //! MajorRadius is measured along this axis, and
  //! -   the "Y Direction" of A2 defines the minor axis of
  //! the hyperbola, that is, the minor radius
  //! MinorRadius is measured along this axis.
  //! Note: This class does not prevent the creation of a
  //! hyperbola where:
  //! -   MajorAxis is equal to MinorAxis, or
  //! -   MajorAxis is less than MinorAxis.
  //! Exceptions
  //! Standard_ConstructionError if MajorAxis or MinorAxis is negative.
  //! Raises ConstructionError if MajorRadius < 0.0 or MinorRadius < 0.0
  //! Raised if MajorRadius < 0.0 or MinorRadius < 0.0
    gp_Hypr(const gp_Ax2& A2, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Modifies this hyperbola, by redefining its local coordinate
  //! system so that:
  //! -   its origin and "main Direction" become those of the
  //! axis A1 (the "X Direction" and "Y Direction" are then
  //! recomputed in the same way as for any gp_Ax2).
  //! Raises ConstructionError if the direction of A1 is parallel to the direction of
  //! the "XAxis" of the hyperbola.
    void SetAxis (const gp_Ax1& A1);
  
  //! Modifies this hyperbola, by redefining its local coordinate
  //! system so that its origin becomes P.
    void SetLocation (const gp_Pnt& P);
  

  //! Modifies the major  radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if MajorRadius is negative.
    void SetMajorRadius (const Standard_Real MajorRadius);
  

  //! Modifies the minor  radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if MinorRadius is negative.
    void SetMinorRadius (const Standard_Real MinorRadius);
  
  //! Modifies this hyperbola, by redefining its local coordinate
  //! system so that it becomes A2.
    void SetPosition (const gp_Ax2& A2);
  

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = (B/A)*X
  //! where A is the major radius and B is the minor radius. Raises ConstructionError if MajorRadius = 0.0
    gp_Ax1 Asymptote1() const;
  

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = -(B/A)*X.
  //! where A is the major radius and B is the minor radius. Raises ConstructionError if MajorRadius = 0.0
    gp_Ax1 Asymptote2() const;
  
  //! Returns the axis passing through the center,
  //! and normal to the plane of this hyperbola.
    const gp_Ax1& Axis() const;
  

  //! Computes the branch of hyperbola which is on the positive side of the
  //! "YAxis" of <me>.
    gp_Hypr ConjugateBranch1() const;
  

  //! Computes the branch of hyperbola which is on the negative side of the
  //! "YAxis" of <me>.
    gp_Hypr ConjugateBranch2() const;
  

  //! This directrix is the line normal to the XAxis of the hyperbola
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the hyperbola, where e is the eccentricity of
  //! the hyperbola.
  //! This line is parallel to the "YAxis". The intersection point
  //! between the directrix1 and the "XAxis" is the "Location" point
  //! of the directrix1. This point is on the positive side of the
  //! "XAxis".
    gp_Ax1 Directrix1() const;
  

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the hyperbola.
    gp_Ax1 Directrix2() const;
  

  //! Returns the excentricity of the hyperbola (e > 1).
  //! If f is the distance between the location of the hyperbola
  //! and the Focus1 then the eccentricity e = f / MajorRadius. Raises DomainError if MajorRadius = 0.0
    Standard_Real Eccentricity() const;
  

  //! Computes the focal distance. It is the distance between the
  //! the two focus of the hyperbola.
    Standard_Real Focal() const;
  

  //! Returns the first focus of the hyperbola. This focus is on the
  //! positive side of the "XAxis" of the hyperbola.
    gp_Pnt Focus1() const;
  

  //! Returns the second focus of the hyperbola. This focus is on the
  //! negative side of the "XAxis" of the hyperbola.
    gp_Pnt Focus2() const;
  

  //! Returns  the location point of the hyperbola. It is the
  //! intersection point between the "XAxis" and the "YAxis".
    const gp_Pnt& Location() const;
  

  //! Returns the major radius of the hyperbola. It is the radius
  //! on the "XAxis" of the hyperbola.
    Standard_Real MajorRadius() const;
  

  //! Returns the minor radius of the hyperbola. It is the radius
  //! on the "YAxis" of the hyperbola.
    Standard_Real MinorRadius() const;
  

  //! Returns the branch of hyperbola obtained by doing the
  //! symmetrical transformation of <me> with respect to the
  //! "YAxis"  of <me>.
    gp_Hypr OtherBranch() const;
  

  //! Returns p = (e * e - 1) * MajorRadius where e is the
  //! eccentricity of the hyperbola.
  //! Raises DomainError if MajorRadius = 0.0
    Standard_Real Parameter() const;
  
  //! Returns the coordinate system of the hyperbola.
    const gp_Ax2& Position() const;
  
  //! Computes an axis, whose
  //! -   the origin is the center of this hyperbola, and
  //! -   the unit vector is the "X Direction"
  //! of the local coordinate system of this hyperbola.
  //! These axes are, the major axis (the "X
  //! Axis") and  of this hyperboReturns the "XAxis" of the hyperbola.
    gp_Ax1 XAxis() const;
  
  //! Computes an axis, whose
  //! -   the origin is the center of this hyperbola, and
  //! -   the unit vector is the "Y Direction"
  //! of the local coordinate system of this hyperbola.
  //! These axes are the minor axis (the "Y Axis") of this hyperbola
    gp_Ax1 YAxis() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect  to the point P which is the center of the symmetry.
  Standard_EXPORT gp_Hypr Mirrored (const gp_Pnt& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect to an axis placement which is the axis of the symmetry.
  Standard_EXPORT gp_Hypr Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect to a plane. The axis placement A2 locates the plane
  //! of the symmetry (Location, XDirection, YDirection).
  Standard_EXPORT gp_Hypr Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates an hyperbola. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Hypr Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Scales an hyperbola. S is the scaling value.
    gp_Hypr Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf& T);
  

  //! Transforms an hyperbola with the transformation T from
  //! class Trsf.
    gp_Hypr Transformed (const gp_Trsf& T) const;
  
    void Translate (const gp_Vec& V);
  

  //! Translates an hyperbola in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Hypr Translated (const gp_Vec& V) const;
  
    void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Translates an hyperbola from the point P1 to the point P2.
    gp_Hypr Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_Ax2 pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;


};


#include <gp_Hypr.lxx>





#endif // _gp_Hypr_HeaderFile
