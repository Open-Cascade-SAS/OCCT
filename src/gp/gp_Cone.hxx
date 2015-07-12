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

#ifndef _gp_Cone_HeaderFile
#define _gp_Cone_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax3.hxx>
#include <Standard_Real.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Ax1.hxx>
class Standard_ConstructionError;
class gp_Ax3;
class gp_Ax1;
class gp_Pnt;
class gp_Ax2;
class gp_Trsf;
class gp_Vec;



//! Defines an infinite conical surface.
//! A cone is defined by its half-angle at the apex and
//! positioned in space with a coordinate system (a gp_Ax3
//! object) and a "reference radius" where:
//! -   the "main Axis" of the coordinate system is the axis of   revolution of the cone,
//! -   the plane defined by the origin, the "X Direction" and
//! the "Y Direction" of the coordinate system is the
//! reference plane of the cone; the intersection of the
//! cone with this reference plane is a circle of radius
//! equal to the reference radius,
//! if the half-angle is positive, the apex of the cone is on
//! the negative side of the "main Axis" of the coordinate
//! system. If the half-angle is negative, the apex is on the   positive side.
//! This coordinate system is the "local coordinate system" of the cone.
//! Note: when a gp_Cone cone is converted into a
//! Geom_ConicalSurface cone, some implicit properties of
//! its local coordinate system are used explicitly:
//! -   its origin, "X Direction", "Y Direction" and "main
//! Direction" are used directly to define the parametric
//! directions on the cone and the origin of the parameters,
//! -   its implicit orientation (right-handed or left-handed)
//! gives the orientation (direct or indirect) of the
//! Geom_ConicalSurface cone.
//! See Also
//! gce_MakeCone which provides functions for more
//! complex cone constructions
//! Geom_ConicalSurface which provides additional
//! functions for constructing cones and works, in particular,
//! with the parametric equations of cones gp_Ax3
class gp_Cone 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an indefinite Cone.
    gp_Cone();
  

  //! Creates an infinite conical surface. A3 locates the cone
  //! in the space and defines the reference plane of the surface.
  //! Ang is the conical surface semi-angle between 0 and PI/2 radians.
  //! Radius is the radius of the circle in the reference plane of
  //! the cone.
  //! Raises ConstructionError
  //! . if Radius is lower than 0.0
  //! . Ang < Resolution from gp  or Ang >= (PI/2) - Resolution.
    gp_Cone(const gp_Ax3& A3, const Standard_Real Ang, const Standard_Real Radius);
  
  //! Changes the symmetry axis of the cone.  Raises ConstructionError
  //! the direction of A1 is parallel to the "XDirection"
  //! of the coordinate system of the cone.
    void SetAxis (const gp_Ax1& A1);
  
  //! Changes the location of the cone.
    void SetLocation (const gp_Pnt& Loc);
  

  //! Changes the local coordinate system of the cone.
  //! This coordinate system defines the reference plane of the cone.
    void SetPosition (const gp_Ax3& A3);
  

  //! Changes the radius of the cone in the reference plane of
  //! the cone.
  //! Raised if R < 0.0
    void SetRadius (const Standard_Real R);
  

  //! Changes the semi-angle of the cone.
  //! Ang is the conical surface semi-angle  ]0,PI/2[.
  //! Raises ConstructionError if Ang < Resolution from gp or Ang >= PI/2 - Resolution
    void SetSemiAngle (const Standard_Real Ang);
  

  //! Computes the cone's top. The Apex of the cone is on the
  //! negative side of the symmetry axis of the cone.
    gp_Pnt Apex() const;
  
  //! Reverses the   U   parametrization of   the  cone
  //! reversing the YAxis.
    void UReverse();
  
  //! Reverses the   V   parametrization of   the  cone  reversing the ZAxis.
    void VReverse();
  
  //! Returns true if the local coordinate system of this cone is right-handed.
    Standard_Boolean Direct() const;
  
  //! returns the symmetry axis of the cone.
    const gp_Ax1& Axis() const;
  

  //! Computes the coefficients of the implicit equation of the quadric
  //! in the absolute cartesian coordinates system :
  //! A1.X**2 + A2.Y**2 + A3.Z**2 + 2.(B1.X.Y + B2.X.Z + B3.Y.Z) +
  //! 2.(C1.X + C2.Y + C3.Z) + D = 0.0
  Standard_EXPORT void Coefficients (Standard_Real& A1, Standard_Real& A2, Standard_Real& A3, Standard_Real& B1, Standard_Real& B2, Standard_Real& B3, Standard_Real& C1, Standard_Real& C2, Standard_Real& C3, Standard_Real& D) const;
  
  //! returns the "Location" point of the cone.
    const gp_Pnt& Location() const;
  

  //! Returns the local coordinates system of the cone.
    const gp_Ax3& Position() const;
  

  //! Returns the radius of the cone in the reference plane.
    Standard_Real RefRadius() const;
  
  //! Returns the half-angle at the apex of this cone.
    Standard_Real SemiAngle() const;
  
  //! Returns the XAxis of the reference plane.
    gp_Ax1 XAxis() const;
  
  //! Returns the YAxis of the reference plane.
    gp_Ax1 YAxis() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of a cone
  //! with respect to the point P which is the center of the
  //! symmetry.
  Standard_EXPORT gp_Cone Mirrored (const gp_Pnt& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of a cone with
  //! respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_EXPORT gp_Cone Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of a cone with respect
  //! to a plane. The axis placement A2 locates the plane of the
  //! of the symmetry : (Location, XDirection, YDirection).
  Standard_EXPORT gp_Cone Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates a cone. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Cone Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Scales a cone. S is the scaling value.
  //! The absolute value of S is used to scale the cone
    gp_Cone Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf& T);
  

  //! Transforms a cone with the transformation T from class Trsf.
    gp_Cone Transformed (const gp_Trsf& T) const;
  
    void Translate (const gp_Vec& V);
  

  //! Translates a cone in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Cone Translated (const gp_Vec& V) const;
  
    void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Translates a cone from the point P1 to the point P2.
    gp_Cone Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_Ax3 pos;
  Standard_Real radius;
  Standard_Real semiAngle;


};


#include <gp_Cone.lxx>





#endif // _gp_Cone_HeaderFile
