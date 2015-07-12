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

#ifndef _gp_Pnt_HeaderFile
#define _gp_Pnt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_OutOfRange;
class gp_XYZ;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;
class gp_Vec;


//! Defines a 3D cartesian point.
class gp_Pnt 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a point with zero coordinates.
    gp_Pnt();
  
  //! Creates a point from a XYZ object.
    gp_Pnt(const gp_XYZ& Coord);
  

  //! Creates a  point with its 3 cartesian's coordinates : Xp, Yp, Zp.
    gp_Pnt(const Standard_Real Xp, const Standard_Real Yp, const Standard_Real Zp);
  

  //! Changes the coordinate of range Index :
  //! Index = 1 => X is modified
  //! Index = 2 => Y is modified
  //! Index = 3 => Z is modified
  //! Raised if Index != {1, 2, 3}.
    void SetCoord (const Standard_Integer Index, const Standard_Real Xi);
  
  //! For this point, assigns  the values Xp, Yp and Zp to its three coordinates.
    void SetCoord (const Standard_Real Xp, const Standard_Real Yp, const Standard_Real Zp);
  
  //! Assigns the given value to the X coordinate of this point.
    void SetX (const Standard_Real X);
  
  //! Assigns the given value to the Y coordinate of this point.
    void SetY (const Standard_Real Y);
  
  //! Assigns the given value to the Z coordinate of this point.
    void SetZ (const Standard_Real Z);
  
  //! Assigns the three coordinates of Coord to this point.
    void SetXYZ (const gp_XYZ& Coord);
  

  //! Returns the coordinate of corresponding to the value of  Index :
  //! Index = 1 => X is returned
  //! Index = 2 => Y is returned
  //! Index = 3 => Z is returned
  //! Raises OutOfRange if Index != {1, 2, 3}.
  //! Raised if Index != {1, 2, 3}.
    Standard_Real Coord (const Standard_Integer Index) const;
  
  //! For this point gives its three coordinates Xp, Yp and Zp.
    void Coord (Standard_Real& Xp, Standard_Real& Yp, Standard_Real& Zp) const;
  
  //! For this point, returns its X coordinate.
    Standard_Real X() const;
  
  //! For this point, returns its Y coordinate.
    Standard_Real Y() const;
  
  //! For this point, returns its Z coordinate.
    Standard_Real Z() const;
  
  //! For this point, returns its three coordinates as a XYZ object.
    const gp_XYZ& XYZ() const;
  
  //! For this point, returns its three coordinates as a XYZ object.
    const gp_XYZ& Coord() const;
  

  //! Returns the coordinates of this point.
  //! Note: This syntax allows direct modification of the returned value.
    gp_XYZ& ChangeCoord();
  
  //! Assigns the result of the following expression to this point
  //! (Alpha*this + Beta*P) / (Alpha + Beta)
    void BaryCenter (const Standard_Real Alpha, const gp_Pnt& P, const Standard_Real Beta);
  
  //! Comparison
  //! Returns True if the distance between the two points is
  //! lower or equal to LinearTolerance.
    Standard_Boolean IsEqual (const gp_Pnt& Other, const Standard_Real LinearTolerance) const;
  
  //! Computes the distance between two points.
    Standard_Real Distance (const gp_Pnt& Other) const;
  
  //! Computes the square distance between two points.
    Standard_Real SquareDistance (const gp_Pnt& Other) const;
  

  //! Performs the symmetrical transformation of a point
  //! with respect to the point P which is the center of
  //! the  symmetry.
  Standard_EXPORT void Mirror (const gp_Pnt& P);
  

  //! Performs the symmetrical transformation of a point
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_EXPORT gp_Pnt Mirrored (const gp_Pnt& P) const;
  
  Standard_EXPORT void Mirror (const gp_Ax1& A1);
  

  //! Performs the symmetrical transformation of a point
  //! with respect to a plane. The axis placement A2 locates
  //! the plane of the symmetry : (Location, XDirection, YDirection).
  Standard_EXPORT gp_Pnt Mirrored (const gp_Ax1& A1) const;
  
  Standard_EXPORT void Mirror (const gp_Ax2& A2);
  

  //! Rotates a point. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
  Standard_EXPORT gp_Pnt Mirrored (const gp_Ax2& A2) const;
  
    void Rotate (const gp_Ax1& A1, const Standard_Real Ang);
  
  //! Scales a point. S is the scaling value.
    gp_Pnt Rotated (const gp_Ax1& A1, const Standard_Real Ang) const;
  
    void Scale (const gp_Pnt& P, const Standard_Real S);
  
  //! Transforms a point with the transformation T.
    gp_Pnt Scaled (const gp_Pnt& P, const Standard_Real S) const;
  
  Standard_EXPORT void Transform (const gp_Trsf& T);
  

  //! Translates a point in the direction of the vector V.
  //! The magnitude of the translation is the vector's magnitude.
    gp_Pnt Transformed (const gp_Trsf& T) const;
  
    void Translate (const gp_Vec& V);
  

  //! Translates a point from the point P1 to the point P2.
    gp_Pnt Translated (const gp_Vec& V) const;
  
    void Translate (const gp_Pnt& P1, const gp_Pnt& P2);
  
    gp_Pnt Translated (const gp_Pnt& P1, const gp_Pnt& P2) const;




protected:





private:



  gp_XYZ coord;


};


#include <gp_Pnt.lxx>





#endif // _gp_Pnt_HeaderFile
