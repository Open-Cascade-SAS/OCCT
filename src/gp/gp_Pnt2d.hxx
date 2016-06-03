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

#ifndef _gp_Pnt2d_HeaderFile
#define _gp_Pnt2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XY.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_OutOfRange;
class gp_XY;
class gp_Ax2d;
class gp_Trsf2d;
class gp_Vec2d;


//! Defines  a non-persistent 2D cartesian point.
class gp_Pnt2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a point with zero coordinates.
    gp_Pnt2d();
  
  //! Creates a point with a doublet of coordinates.
    gp_Pnt2d(const gp_XY& Coord);
  

  //! Creates a  point with its 2 cartesian's coordinates : Xp, Yp.
    gp_Pnt2d(const Standard_Real Xp, const Standard_Real Yp);
  

  //! Assigns the value Xi to the coordinate that corresponds to Index:
  //! Index = 1 => X is modified
  //! Index = 2 => Y is modified
  //! Raises OutOfRange if Index != {1, 2}.
    void SetCoord (const Standard_Integer Index, const Standard_Real Xi);
  
  //! For this point, assigns the values Xp and Yp to its two coordinates
    void SetCoord (const Standard_Real Xp, const Standard_Real Yp);
  
  //! Assigns the given value to the X  coordinate of this point.
    void SetX (const Standard_Real X);
  
  //! Assigns the given value to the Y  coordinate of this point.
    void SetY (const Standard_Real Y);
  
  //! Assigns the two coordinates of Coord to this point.
    void SetXY (const gp_XY& Coord);
  

  //! Returns the coordinate of range Index :
  //! Index = 1 => X is returned
  //! Index = 2 => Y is returned
  //! Raises OutOfRange if Index != {1, 2}.
  Standard_Real Coord (const Standard_Integer Index) const;
  
  //! For this point returns its two coordinates as a number pair.
    void Coord (Standard_Real& Xp, Standard_Real& Yp) const;
  
  //! For this point, returns its X  coordinate.
    Standard_Real X() const;
  
  //! For this point, returns its Y coordinate.
    Standard_Real Y() const;
  
  //! For this point, returns its two coordinates as a number pair.
    const gp_XY& XY() const;
  
  //! For this point, returns its two coordinates as a number pair.
    const gp_XY& Coord() const;
  

  //! Returns the coordinates of this point.
  //! Note: This syntax allows direct modification of the returned value.
    gp_XY& ChangeCoord();
  
  //! Comparison
  //! Returns True if the distance between the two
  //! points is lower or equal to LinearTolerance.
    Standard_Boolean IsEqual (const gp_Pnt2d& Other, const Standard_Real LinearTolerance) const;
  
  //! Computes the distance between two points.
    Standard_Real Distance (const gp_Pnt2d& Other) const;
  
  //! Computes the square distance between two points.
    Standard_Real SquareDistance (const gp_Pnt2d& Other) const;
  

  //! Performs the symmetrical transformation of a point
  //! with respect to the point P which is the center of
  //! the  symmetry.
  Standard_EXPORT void Mirror (const gp_Pnt2d& P);
  

  //! Performs the symmetrical transformation of a point
  //! with respect to an axis placement which is the axis
  Standard_EXPORT gp_Pnt2d Mirrored (const gp_Pnt2d& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2d& A);
  

  //! Rotates a point. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
  Standard_EXPORT gp_Pnt2d Mirrored (const gp_Ax2d& A) const;
  
    void Rotate (const gp_Pnt2d& P, const Standard_Real Ang);
  
  //! Scales a point. S is the scaling value.
    gp_Pnt2d Rotated (const gp_Pnt2d& P, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt2d& P, const Standard_Real S);
  
  //! Transforms a point with the transformation T.
    gp_Pnt2d Scaled (const gp_Pnt2d& P, const Standard_Real S) const;
  
  Standard_EXPORT void Transform (const gp_Trsf2d& T);
  

  //! Translates a point in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Pnt2d Transformed (const gp_Trsf2d& T) const;
  
    void Translate (const gp_Vec2d& V);
  

  //! Translates a point from the point P1 to the point P2.
    gp_Pnt2d Translated (const gp_Vec2d& V) const;
  
    void Translate (const gp_Pnt2d& P1, const gp_Pnt2d& P2);
  
    gp_Pnt2d Translated (const gp_Pnt2d& P1, const gp_Pnt2d& P2) const;




protected:





private:



  gp_XY coord;


};


#include <gp_Pnt2d.lxx>





#endif // _gp_Pnt2d_HeaderFile
