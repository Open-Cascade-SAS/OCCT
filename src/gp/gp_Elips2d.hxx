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

#ifndef _gp_Elips2d_HeaderFile
#define _gp_Elips2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax22d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
class Standard_ConstructionError;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;
class gp_Trsf2d;
class gp_Vec2d;



//! Describes an ellipse in the plane (2D space).
//! An ellipse is defined by its major and minor radii and
//! positioned in the plane with a coordinate system (a
//! gp_Ax22d object) as follows:
//! -   the origin of the coordinate system is the center of the ellipse,
//! -   its "X Direction" defines the major axis of the ellipse, and
//! -   its "Y Direction" defines the minor axis of the ellipse.
//! This coordinate system is the "local coordinate system"
//! of the ellipse. Its orientation (direct or indirect) gives an
//! implicit orientation to the ellipse. In this coordinate
//! system, the equation of the ellipse is:
//! X*X / (MajorRadius**2) + Y*Y / (MinorRadius**2) = 1.0
//! See Also
//! gce_MakeElips2d which provides functions for more
//! complex ellipse constructions
//! Geom2d_Ellipse which provides additional functions for
//! constructing ellipses and works, in particular, with the
//! parametric equations of ellipses
class gp_Elips2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an indefinite ellipse.
    gp_Elips2d();
  

  //! Creates an ellipse with the major axis, the major and the
  //! minor radius. The location of the MajorAxis is the center
  //! of the  ellipse.
  //! The sense of parametrization is given by Sense.
  //! Warnings :
  //! It is possible to create an ellipse with
  //! MajorRadius = MinorRadius.
  //! Raises ConstructionError if MajorRadius < MinorRadius or MinorRadius < 0.0
    gp_Elips2d(const gp_Ax2d& MajorAxis, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Boolean Sense = Standard_True);
  
  //! Creates an ellipse with radii MajorRadius and
  //! MinorRadius, positioned in the plane by coordinate system A where:
  //! -   the origin of A is the center of the ellipse,
  //! -   the "X Direction" of A defines the major axis of
  //! the ellipse, that is, the major radius MajorRadius
  //! is measured along this axis, and
  //! -   the "Y Direction" of A defines the minor axis of
  //! the ellipse, that is, the minor radius MinorRadius
  //! is measured along this axis, and
  //! -   the orientation (direct or indirect sense) of A
  //! gives the orientation of the ellipse.
  //! Warnings :
  //! It is possible to create an ellipse with
  //! MajorRadius = MinorRadius.
  //! Raises ConstructionError if MajorRadius < MinorRadius or MinorRadius < 0.0
  gp_Elips2d(const gp_Ax22d& A, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Modifies this ellipse, by redefining its local coordinate system so that
  //! -   its origin becomes P.
  void SetLocation (const gp_Pnt2d& P);
  
  //! Changes the value of the major radius.
  //! Raises ConstructionError if MajorRadius < MinorRadius.
  void SetMajorRadius (const Standard_Real MajorRadius);
  
  //! Changes the value of the minor radius.
  //! Raises ConstructionError if MajorRadius < MinorRadius or MinorRadius < 0.0
  void SetMinorRadius (const Standard_Real MinorRadius);
  
  //! Modifies this ellipse, by redefining its local coordinate system so that
  //! it becomes A.
  void SetAxis (const gp_Ax22d& A);
  
  //! Modifies this ellipse, by redefining its local coordinate system so that
  //! its origin and its "X Direction"  become those
  //! of the axis A. The "Y  Direction"  is then
  //! recomputed. The orientation of the local coordinate
  //! system is not modified.
  void SetXAxis (const gp_Ax2d& A);
  
  //! Modifies this ellipse, by redefining its local coordinate system so that
  //! its origin and its "Y Direction"  become those
  //! of the axis A. The "X  Direction"  is then
  //! recomputed. The orientation of the local coordinate
  //! system is not modified.
  void SetYAxis (const gp_Ax2d& A);
  
  //! Computes the area of the ellipse.
    Standard_Real Area() const;
  

  //! Returns the coefficients of the implicit equation of the ellipse.
  //! A * (X**2) + B * (Y**2) + 2*C*(X*Y) + 2*D*X + 2*E*Y + F = 0.
  Standard_EXPORT void Coefficients (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D, Standard_Real& E, Standard_Real& F) const;
  

  //! This directrix is the line normal to the XAxis of the ellipse
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the ellipse, where e is the eccentricity of
  //! the ellipse.
  //! This line is parallel to the "YAxis". The intersection point
  //! between directrix1 and the "XAxis" is the location point of the
  //! directrix1. This point is on the positive side of the "XAxis".
  //!
  //! Raised if Eccentricity = 0.0. (The ellipse degenerates into a
  //! circle)
    gp_Ax2d Directrix1() const;
  

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the minor axis of the ellipse.
  //!
  //! Raised if Eccentricity = 0.0. (The ellipse degenerates into a
  //! circle).
    gp_Ax2d Directrix2() const;
  

  //! Returns the eccentricity of the ellipse  between 0.0 and 1.0
  //! If f is the distance between the center of the ellipse and
  //! the Focus1 then the eccentricity e = f / MajorRadius.
  //! Returns 0 if MajorRadius = 0.
    Standard_Real Eccentricity() const;
  

  //! Returns the distance between the center of the ellipse
  //! and focus1 or focus2.
    Standard_Real Focal() const;
  

  //! Returns the first focus of the ellipse. This focus is on the
  //! positive side of the major axis of the ellipse.
    gp_Pnt2d Focus1() const;
  

  //! Returns the second focus of the ellipse. This focus is on the
  //! negative side of the major axis of the ellipse.
    gp_Pnt2d Focus2() const;
  
  //! Returns the center of the ellipse.
    const gp_Pnt2d& Location() const;
  
  //! Returns the major radius of the Ellipse.
    Standard_Real MajorRadius() const;
  
  //! Returns the minor radius of the Ellipse.
    Standard_Real MinorRadius() const;
  

  //! Returns p = (1 - e * e) * MajorRadius where e is the eccentricity
  //! of the ellipse.
  //! Returns 0 if MajorRadius = 0
    Standard_Real Parameter() const;
  
  //! Returns the major axis of the ellipse.
    const gp_Ax22d& Axis() const;
  
  //! Returns the major axis of the ellipse.
    gp_Ax2d XAxis() const;
  
  //! Returns the minor axis of the ellipse.
  //! Reverses the direction of the circle.
    gp_Ax2d YAxis() const;
  
    void Reverse();
  
    gp_Elips2d Reversed() const;
  
  //! Returns true if the local coordinate system is direct
  //! and false in the other case.
    Standard_Boolean IsDirect() const;
  
  Standard_EXPORT void Mirror (const gp_Pnt2d& P);
  

  //! Performs the symmetrical transformation of a ellipse with respect
  //! to the point P which is the center of the symmetry
  Standard_EXPORT gp_Elips2d Mirrored (const gp_Pnt2d& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2d& A);
  

  //! Performs the symmetrical transformation of a ellipse with respect
  //! to an axis placement which is the axis of the symmetry.
  Standard_EXPORT gp_Elips2d Mirrored (const gp_Ax2d& A) const;
  
  void Rotate (const gp_Pnt2d& P, const Standard_Real Ang);
  
  gp_Elips2d Rotated (const gp_Pnt2d& P, const Standard_Real Ang) const;
  
  void Scale (const gp_Pnt2d& P, const Standard_Real S);
  

  //! Scales a ellipse. S is the scaling value.
  gp_Elips2d Scaled (const gp_Pnt2d& P, const Standard_Real S) const;
  
  void Transform (const gp_Trsf2d& T);
  

  //! Transforms an ellipse with the transformation T from class Trsf2d.
  gp_Elips2d Transformed (const gp_Trsf2d& T) const;
  
  void Translate (const gp_Vec2d& V);
  

  //! Translates a ellipse in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
  gp_Elips2d Translated (const gp_Vec2d& V) const;
  
  void Translate (const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  

  //! Translates a ellipse from the point P1 to the point P2.
  gp_Elips2d Translated (const gp_Pnt2d& P1, const gp_Pnt2d& P2) const;




protected:





private:



  gp_Ax22d pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;


};


#include <gp_Elips2d.lxx>





#endif // _gp_Elips2d_HeaderFile
