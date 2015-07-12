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

#ifndef _gp_Elips_HeaderFile
#define _gp_Elips_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax2.hxx>
#include <Standard_Real.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
class Standard_ConstructionError;
class gp_Ax2;
class gp_Ax1;
class gp_Pnt;
class gp_Trsf;
class gp_Vec;



//! Describes an ellipse in 3D space.
//! An ellipse is defined by its major and minor radii and
//! positioned in space with a coordinate system (a gp_Ax2 object) as follows:
//! -   the origin of the coordinate system is the center of the ellipse,
//! -   its "X Direction" defines the major axis of the ellipse, and
//! - its "Y Direction" defines the minor axis of the ellipse.
//! Together, the origin, "X Direction" and "Y Direction" of
//! this coordinate system define the plane of the ellipse.
//! This coordinate system is the "local coordinate system"
//! of the ellipse. In this coordinate system, the equation of
//! the ellipse is:
//! X*X / (MajorRadius**2) + Y*Y / (MinorRadius**2) = 1.0
//! The "main Direction" of the local coordinate system gives
//! the normal vector to the plane of the ellipse. This vector
//! gives an implicit orientation to the ellipse (definition of the
//! trigonometric sense). We refer to the "main Axis" of the
//! local coordinate system as the "Axis" of the ellipse.
//! See Also
//! gce_MakeElips which provides functions for more
//! complex ellipse constructions
//! Geom_Ellipse which provides additional functions for
//! constructing ellipses and works, in particular, with the
//! parametric equations of ellipses
class gp_Elips 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an indefinite ellipse.
    gp_Elips();
  

  //! The major radius of the ellipse is on the "XAxis" and the
  //! minor radius is on the "YAxis" of the ellipse. The "XAxis"
  //! is defined with the "XDirection" of A2 and the "YAxis" is
  //! defined with the "YDirection" of A2.
  //! Warnings :
  //! It is not forbidden to create an ellipse with MajorRadius =
  //! MinorRadius.
  //! Raises ConstructionError if MajorRadius < MinorRadius or MinorRadius < 0.
    gp_Elips(const gp_Ax2& A2, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  

  //! Changes the axis normal to the plane of the ellipse.
  //! It modifies the definition of this plane.
  //! The "XAxis" and the "YAxis" are recomputed.
  //! The local coordinate system is redefined so that:
  //! -   its origin and "main Direction" become those of the
  //! axis A1 (the "X Direction" and "Y Direction" are then
  //! recomputed in the same way as for any gp_Ax2), or
  //! Raises ConstructionError if the direction of A1
  //! is parallel to the direction of the "XAxis" of the ellipse.
    void SetAxis (const gp_Ax1& A1);
  
  //! Modifies this ellipse, by redefining its local coordinate
  //! so that its origin becomes P.
    void SetLocation (const gp_Pnt& P);
  

  //! The major radius of the ellipse is on the "XAxis" (major axis)
  //! of the ellipse.
  //! Raises ConstructionError if MajorRadius < MinorRadius.
    void SetMajorRadius (const Standard_Real MajorRadius);
  

  //! The minor radius of the ellipse is on the "YAxis" (minor axis)
  //! of the ellipse.
  //! Raises ConstructionError if MinorRadius > MajorRadius or MinorRadius < 0.
    void SetMinorRadius (const Standard_Real MinorRadius);
  
  //! Modifies this ellipse, by redefining its local coordinate
  //! so that it becomes A2e.
    void SetPosition (const gp_Ax2& A2);
  
  //! Computes the area of the Ellipse.
    Standard_Real Area() const;
  

  //! Computes the axis normal to the plane of the ellipse.
    const gp_Ax1& Axis() const;
  
  //! Computes the first or second directrix of this ellipse.
  //! These are the lines, in the plane of the ellipse, normal to
  //! the major axis, at a distance equal to
  //! MajorRadius/e from the center of the ellipse, where
  //! e is the eccentricity of the ellipse.
  //! The first directrix (Directrix1) is on the positive side of
  //! the major axis. The second directrix (Directrix2) is on
  //! the negative side.
  //! The directrix is returned as an axis (gp_Ax1 object), the
  //! origin of which is situated on the "X Axis" of the local
  //! coordinate system of this ellipse.
  //! Exceptions
  //! Standard_ConstructionError if the eccentricity is null
  //! (the ellipse has degenerated into a circle).
    gp_Ax1 Directrix1() const;
  

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the ellipse.
  //! Exceptions
  //! Standard_ConstructionError if the eccentricity is null
  //! (the ellipse has degenerated into a circle).
    gp_Ax1 Directrix2() const;
  

  //! Returns the eccentricity of the ellipse  between 0.0 and 1.0
  //! If f is the distance between the center of the ellipse and
  //! the Focus1 then the eccentricity e = f / MajorRadius.
  //! Raises ConstructionError if MajorRadius = 0.0
    Standard_Real Eccentricity() const;
  

  //! Computes the focal distance. It is the distance between the
  //! two focus focus1 and focus2 of the ellipse.
    Standard_Real Focal() const;
  

  //! Returns the first focus of the ellipse. This focus is on the
  //! positive side of the "XAxis" of the ellipse.
    gp_Pnt Focus1() const;
  

  //! Returns the second focus of the ellipse. This focus is on the
  //! negative side of the "XAxis" of the ellipse.
    gp_Pnt Focus2() const;
  

  //! Returns the center of the ellipse. It is the "Location"
  //! point of the coordinate system of the ellipse.
    const gp_Pnt& Location() const;
  
  //! Returns the major radius of the ellipse.
    Standard_Real MajorRadius() const;
  
  //! Returns the minor radius of the ellipse.
    Standard_Real MinorRadius() const;
  

  //! Returns p = (1 - e * e) * MajorRadius where e is the eccentricity
  //! of the ellipse.
  //! Returns 0 if MajorRadius = 0
    Standard_Real Parameter() const;
  
  //! Returns the coordinate system of the ellipse.
    const gp_Ax2& Position() const;
  

  //! Returns the "XAxis" of the ellipse whose origin
  //! is the center of this ellipse. It is the major axis of the
  //! ellipse.
    gp_Ax1 XAxis() const;
  

  //! Returns the "YAxis" of the ellipse whose unit vector is the "X Direction" or the "Y Direction"
  //! of the local coordinate system of this ellipse.
  //! This is the minor axis of the ellipse.
    gp_Ax1 YAxis() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of an ellipse with
  //! respect to the point P which is the center of the symmetry.
  Standard_EXPORT gp_Elips Mirrored (const gp_Pnt& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of an ellipse with
  //! respect to an axis placement which is the axis of the symmetry.
  Standard_EXPORT gp_Elips Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Performs the symmetrical transformation of an ellipse with
  //! respect to a plane. The axis placement A2 locates the plane
  //! of the symmetry (Location, XDirection, YDirection).
  Standard_EXPORT gp_Elips Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  

  //! Rotates an ellipse. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Elips Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt& P, const Standard_Real S);
  

  //! Scales an ellipse. S is the scaling value.
    gp_Elips Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf& T);
  

  //! Transforms an ellipse with the transformation T from class Trsf.
    gp_Elips Transformed (const gp_Trsf& T) const;
  
    void Translate (const gp_Vec& V);
  

  //! Translates an ellipse in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Elips Translated (const gp_Vec& V) const;
  
    void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  

  //! Translates an ellipse from the point P1 to the point P2.
    gp_Elips Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_Ax2 pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;


};


#include <gp_Elips.lxx>





#endif // _gp_Elips_HeaderFile
