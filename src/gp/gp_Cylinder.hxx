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

#ifndef _gp_Cylinder_HeaderFile
#define _gp_Cylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax3.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Ax1.hxx>
class Standard_ConstructionError;
class gp_Ax3;
class gp_Ax1;
class gp_Pnt;
class gp_Ax2;
class gp_Trsf;
class gp_Vec;



//! Describes an infinite cylindrical surface.
//! A cylinder is defined by its radius and positioned in space
//! with a coordinate system (a gp_Ax3 object), the "main
//! Axis" of which is the axis of the cylinder. This coordinate
//! system is the "local coordinate system" of the cylinder.
//! Note: when a gp_Cylinder cylinder is converted into a
//! Geom_CylindricalSurface cylinder, some implicit
//! properties of its local coordinate system are used explicitly:
//! -   its origin, "X Direction", "Y Direction" and "main
//! Direction" are used directly to define the parametric
//! directions on the cylinder and the origin of the parameters,
//! -   its implicit orientation (right-handed or left-handed)
//! gives an orientation (direct or indirect) to the
//! Geom_CylindricalSurface cylinder.
//! See Also
//! gce_MakeCylinder which provides functions for more
//! complex cylinder constructions
//! Geom_CylindricalSurface which provides additional
//! functions for constructing cylinders and works, in
//! particular, with the parametric equations of cylinders gp_Ax3
class gp_Cylinder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a indefinite cylinder.
    gp_Cylinder();
  
  //! Creates a cylinder of radius Radius, whose axis is the "main
  //! Axis" of A3. A3 is the local coordinate system of the cylinder.   Raises ConstructionErrord if R < 0.0
    gp_Cylinder(const gp_Ax3& A3, const Standard_Real Radius);
  
  //! Changes the symmetry axis of the cylinder. Raises ConstructionError if the direction of A1 is parallel to the "XDirection"
  //! of the coordinate system of the cylinder.
    void SetAxis (const gp_Ax1& A1);
  
  //! Changes the location of the surface.
    void SetLocation (const gp_Pnt& Loc);
  
  //! Change the local coordinate system of the surface.
    void SetPosition (const gp_Ax3& A3);
  
  //! Modifies the radius of this cylinder.
  //! Exceptions
  //! Standard_ConstructionError if R is negative.
    void SetRadius (const Standard_Real R);
  
  //! Reverses the   U   parametrization of   the cylinder
  //! reversing the YAxis.
    void UReverse();
  
  //! Reverses the   V   parametrization of   the  plane
  //! reversing the Axis.
    void VReverse();
  
  //! Returns true if the local coordinate system of this cylinder is right-handed.
    Standard_Boolean Direct() const;
  
  //! Returns the symmetry axis of the cylinder.
    const gp_Ax1& Axis() const;
  

  //! Computes the coefficients of the implicit equation of the quadric
  //! in the absolute cartesian coordinate system :
  //! A1.X**2 + A2.Y**2 + A3.Z**2 + 2.(B1.X.Y + B2.X.Z + B3.Y.Z) +
  //! 2.(C1.X + C2.Y + C3.Z) + D = 0.0
  Standard_EXPORT void Coefficients (Standard_Real& A1, Standard_Real& A2, Standard_Real& A3, Standard_Real& B1, Standard_Real& B2, Standard_Real& B3, Standard_Real& C1, Standard_Real& C2, Standard_Real& C3, Standard_Real& D) const;
  
  //! Returns the "Location" point of the cylinder.
    const gp_Pnt& Location() const;
  

  //! Returns the local coordinate system of the cylinder.
    const gp_Ax3& Position() const;
  
  //! Returns the radius of the cylinder.
    Standard_Real Radius() const;
  
  //! Returns the axis X of the cylinder.
    gp_Ax1 XAxis() const;
  
  //! Returns the axis Y of the cylinder.
    gp_Ax1 YAxis() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of a cylinder
  //! with respect to the point P which is the center of the
  //! symmetry.
  Standard_EXPORT gp_Cylinder Mirrored (const gp_Pnt& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of a cylinder with
  //! respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_EXPORT gp_Cylinder Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of a cylinder with respect
  //! to a plane. The axis placement A2 locates the plane of the
  //! of the symmetry : (Location, XDirection, YDirection).
  Standard_EXPORT gp_Cylinder Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates a cylinder. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Cylinder Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Scales a cylinder. S is the scaling value.
  //! The absolute value of S is used to scale the cylinder
    gp_Cylinder Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf& T);
  

  //! Transforms a cylinder with the transformation T from class Trsf.
    gp_Cylinder Transformed (const gp_Trsf& T) const;
  
    void Translate (const gp_Vec& V);
  

  //! Translates a cylinder in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Cylinder Translated (const gp_Vec& V) const;
  
    void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Translates a cylinder from the point P1 to the point P2.
    gp_Cylinder Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_Ax3 pos;
  Standard_Real radius;


};


#include <gp_Cylinder.lxx>





#endif // _gp_Cylinder_HeaderFile
