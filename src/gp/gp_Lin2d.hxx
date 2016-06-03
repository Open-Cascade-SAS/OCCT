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

#ifndef _gp_Lin2d_HeaderFile
#define _gp_Lin2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class gp_Ax2d;
class gp_Pnt2d;
class gp_Dir2d;
class gp_Trsf2d;
class gp_Vec2d;


//! Describes a line in 2D space.
//! A line is positioned in the plane with an axis (a gp_Ax2d
//! object) which gives the line its origin and unit vector. A
//! line and an axis are similar objects, thus, we can convert
//! one into the other.
//! A line provides direct access to the majority of the edit
//! and query functions available on its positioning axis. In
//! addition, however, a line has specific functions for
//! computing distances and positions.
//! See Also
//! GccAna and Geom2dGcc packages which provide
//! functions for constructing lines defined by geometric
//! constraints
//! gce_MakeLin2d which provides functions for more
//! complex line constructions
//! Geom2d_Line which provides additional functions for
//! constructing lines and works, in particular, with the
//! parametric equations of lines
class gp_Lin2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Line corresponding to X axis of the
  //! reference coordinate system.
    gp_Lin2d();
  
  //! Creates a line located with A.
    gp_Lin2d(const gp_Ax2d& A);
  

  //! <P> is the location point (origin) of the line and
  //! <V> is the direction of the line.
    gp_Lin2d(const gp_Pnt2d& P, const gp_Dir2d& V);
  

  //! Creates the line from the equation A*X + B*Y + C = 0.0 Raises ConstructionError if Sqrt(A*A + B*B) <= Resolution from gp.
  //! Raised if Sqrt(A*A + B*B) <= Resolution from gp.
  Standard_EXPORT gp_Lin2d(const Standard_Real A, const Standard_Real B, const Standard_Real C);
  
    void Reverse();
  

  //! Reverses the positioning axis of this line.
  //! Note:
  //! -   Reverse assigns the result to this line, while
  //! -   Reversed creates a new one.
    gp_Lin2d Reversed() const;
  
  //! Changes the direction of the line.
    void SetDirection (const gp_Dir2d& V);
  
  //! Changes the origin of the line.
    void SetLocation (const gp_Pnt2d& P);
  

  //! Complete redefinition of the line.
  //! The "Location" point of <A> is the origin of the line.
  //! The "Direction" of <A> is  the direction of the line.
    void SetPosition (const gp_Ax2d& A);
  

  //! Returns the normalized coefficients of the line :
  //! A * X + B * Y + C = 0.
    void Coefficients (Standard_Real& A, Standard_Real& B, Standard_Real& C) const;
  
  //! Returns the direction of the line.
    const gp_Dir2d& Direction() const;
  
  //! Returns the location point (origin) of the line.
    const gp_Pnt2d& Location() const;
  

  //! Returns the axis placement one axis whith the same
  //! location and direction as <me>.
    const gp_Ax2d& Position() const;
  
  //! Computes the angle between two lines in radians.
    Standard_Real Angle (const gp_Lin2d& Other) const;
  
  //! Returns true if this line contains the point P, that is, if the
  //! distance between point P and this line is less than or
  //! equal to LinearTolerance.
    Standard_Boolean Contains (const gp_Pnt2d& P, const Standard_Real LinearTolerance) const;
  

  //! Computes the distance between <me> and the point <P>.
    Standard_Real Distance (const gp_Pnt2d& P) const;
  
  //! Computes the distance between two lines.
    Standard_Real Distance (const gp_Lin2d& Other) const;
  

  //! Computes the square distance between <me> and the point
  //! <P>.
    Standard_Real SquareDistance (const gp_Pnt2d& P) const;
  
  //! Computes the square distance between two lines.
    Standard_Real SquareDistance (const gp_Lin2d& Other) const;
  

  //! Computes the line normal to the direction of <me>,
  //! passing through the point <P>.
    gp_Lin2d Normal (const gp_Pnt2d& P) const;
  
  Standard_EXPORT void Mirror (const gp_Pnt2d& P);
  

  //! Performs the symmetrical transformation of a line
  //! with respect to the point <P> which is the center
  //! of the symmetry
  Standard_EXPORT gp_Lin2d Mirrored (const gp_Pnt2d& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2d& A);
  

  //! Performs the symmetrical transformation of a line
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_EXPORT gp_Lin2d Mirrored (const gp_Ax2d& A) const;
  
    void Rotate (const gp_Pnt2d& P, const Standard_Real Ang);
  

  //! Rotates a line. P is the center of the rotation.
  //! Ang is the angular value of the rotation in radians.
    gp_Lin2d Rotated (const gp_Pnt2d& P, const Standard_Real Ang) const;
  
  void Scale (const gp_Pnt2d& P, const Standard_Real S);
  

  //! Scales a line. S is the scaling value. Only the
  //! origin of the line is modified.
    gp_Lin2d Scaled (const gp_Pnt2d& P, const Standard_Real S) const;
  
    void Transform (const gp_Trsf2d& T);
  

  //! Transforms a line with the transformation T from class Trsf2d.
    gp_Lin2d Transformed (const gp_Trsf2d& T) const;
  
    void Translate (const gp_Vec2d& V);
  

  //! Translates a line in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Lin2d Translated (const gp_Vec2d& V) const;
  
    void Translate (const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  

  //! Translates a line from the point P1 to the point P2.
    gp_Lin2d Translated (const gp_Pnt2d& P1, const gp_Pnt2d& P2) const;




protected:





private:



  gp_Ax2d pos;


};


#include <gp_Lin2d.lxx>





#endif // _gp_Lin2d_HeaderFile
