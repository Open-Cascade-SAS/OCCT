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

#ifndef _gp_Sphere_HeaderFile
#define _gp_Sphere_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax3.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Ax1.hxx>
class Standard_ConstructionError;
class gp_Ax3;
class gp_Pnt;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;
class gp_Vec;



//! Describes a sphere.
//! A sphere is defined by its radius and positioned in space
//! with a coordinate system (a gp_Ax3 object). The origin of
//! the coordinate system is the center of the sphere. This
//! coordinate system is the "local coordinate system" of the sphere.
//! Note: when a gp_Sphere sphere is converted into a
//! Geom_SphericalSurface sphere, some implicit
//! properties of its local coordinate system are used explicitly:
//! -   its origin, "X Direction", "Y Direction" and "main
//! Direction" are used directly to define the parametric
//! directions on the sphere and the origin of the parameters,
//! -   its implicit orientation (right-handed or left-handed)
//! gives the orientation (direct, indirect) to the
//! Geom_SphericalSurface sphere.
//! See Also
//! gce_MakeSphere which provides functions for more
//! complex sphere constructions
//! Geom_SphericalSurface which provides additional
//! functions for constructing spheres and works, in
//! particular, with the parametric equations of spheres.
class gp_Sphere 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an indefinite sphere.
    gp_Sphere();
  

  //! Constructs a sphere with radius Radius, centered on the origin
  //! of A3.  A3 is the local coordinate system of the sphere.
  //! Warnings :
  //! It is not forbidden to create a sphere with null radius.
  //! Raises ConstructionError if Radius < 0.0
    gp_Sphere(const gp_Ax3& A3, const Standard_Real Radius);
  
  //! Changes the center of the sphere.
    void SetLocation (const gp_Pnt& Loc);
  
  //! Changes the local coordinate system of the sphere.
    void SetPosition (const gp_Ax3& A3);
  
  //! Assigns R the radius of the Sphere.
  //! Warnings :
  //! It is not forbidden to create a sphere with null radius.
  //! Raises ConstructionError if R < 0.0
    void SetRadius (const Standard_Real R);
  

  //! Computes the aera of the sphere.
    Standard_Real Area() const;
  

  //! Computes the coefficients of the implicit equation of the quadric
  //! in the absolute cartesian coordinates system :
  //! A1.X**2 + A2.Y**2 + A3.Z**2 + 2.(B1.X.Y + B2.X.Z + B3.Y.Z) +
  //! 2.(C1.X + C2.Y + C3.Z) + D = 0.0
  Standard_EXPORT void Coefficients (Standard_Real& A1, Standard_Real& A2, Standard_Real& A3, Standard_Real& B1, Standard_Real& B2, Standard_Real& B3, Standard_Real& C1, Standard_Real& C2, Standard_Real& C3, Standard_Real& D) const;
  
  //! Reverses the   U   parametrization of   the sphere
  //! reversing the YAxis.
    void UReverse();
  
  //! Reverses the   V   parametrization of   the  sphere
  //! reversing the ZAxis.
    void VReverse();
  
  //! Returns true if the local coordinate system of this sphere
  //! is right-handed.
    Standard_Boolean Direct() const;
  
  //! --- Purpose ;
  //! Returns the center of the sphere.
    const gp_Pnt& Location() const;
  

  //! Returns the local coordinates system of the sphere.
    const gp_Ax3& Position() const;
  
  //! Returns the radius of the sphere.
    Standard_Real Radius() const;
  
  //! Computes the volume of the sphere
    Standard_Real Volume() const;
  
  //! Returns the axis X of the sphere.
    gp_Ax1 XAxis() const;
  
  //! Returns the axis Y of the sphere.
    gp_Ax1 YAxis() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of a sphere
  //! with respect to the point P which is the center of the
  //! symmetry.
  Standard_EXPORT gp_Sphere Mirrored (const gp_Pnt& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of a sphere with
  //! respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_EXPORT gp_Sphere Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of a sphere with respect
  //! to a plane. The axis placement A2 locates the plane of the
  //! of the symmetry : (Location, XDirection, YDirection).
  Standard_EXPORT gp_Sphere Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates a sphere. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Sphere Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Scales a sphere. S is the scaling value.
  //! The absolute value of S is used to scale the sphere
    gp_Sphere Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf& T);
  

  //! Transforms a sphere with the transformation T from class Trsf.
    gp_Sphere Transformed (const gp_Trsf& T) const;
  
    void Translate (const gp_Vec& V);
  

  //! Translates a sphere in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Sphere Translated (const gp_Vec& V) const;
  
    void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Translates a sphere from the point P1 to the point P2.
    gp_Sphere Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_Ax3 pos;
  Standard_Real radius;


};


#include <gp_Sphere.lxx>





#endif // _gp_Sphere_HeaderFile
