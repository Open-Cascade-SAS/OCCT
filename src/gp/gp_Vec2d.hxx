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

#ifndef _gp_Vec2d_HeaderFile
#define _gp_Vec2d_HeaderFile

#include <gp_VectorWithNullMagnitude.hxx>
#include <gp_XY.hxx>

class gp_Dir2d;
class gp_Pnt2d;
class gp_Ax2d;
class gp_Trsf2d;

//! Defines a non-persistent vector in 2D space.
class gp_Vec2d 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a zero vector.
  gp_Vec2d() {}

  //! Creates a unitary vector from a direction theV.
  gp_Vec2d (const gp_Dir2d& theV);

  //! Creates a vector with a doublet of coordinates.
  gp_Vec2d (const gp_XY& theCoord)
    : coord(theCoord)
  {}

  //! Creates a point with its two Cartesian coordinates.
  gp_Vec2d (const Standard_Real theXv, const Standard_Real theYv)
  : coord (theXv, theYv)
  {}

  //! Creates a vector from two points. The length of the vector
  //! is the distance between theP1 and theP2
  gp_Vec2d (const gp_Pnt2d& theP1, const gp_Pnt2d& theP2);

  //! Changes the coordinate of range theIndex
  //! theIndex = 1 => X is modified
  //! theIndex = 2 => Y is modified
  //! Raises OutOfRange if theIndex != {1, 2}.
  void SetCoord (const Standard_Integer theIndex, const Standard_Real theXi) { coord.SetCoord (theIndex, theXi); }

  //! For this vector, assigns
  //! the values theXv and theYv to its two coordinates
  void SetCoord (const Standard_Real theXv, const Standard_Real theYv) { coord.SetCoord (theXv, theYv); }

  //! Assigns the given value to the X coordinate of this vector.
  void SetX (const Standard_Real theX) { coord.SetX (theX); }

  //! Assigns the given value to the Y coordinate of this vector.
  void SetY (const Standard_Real theY) { coord.SetY (theY); }

  //! Assigns the two coordinates of theCoord to this vector.
  void SetXY (const gp_XY& theCoord) { coord = theCoord; }

  //! Returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! Raised if theIndex != {1, 2}.
  Standard_Real Coord (const Standard_Integer theIndex) const { return coord.Coord (theIndex); }

  //! For this vector, returns  its two coordinates theXv and theYv
  void Coord (Standard_Real& theXv, Standard_Real& theYv) const { coord.Coord (theXv, theYv); }

  //! For this vector, returns its X  coordinate.
  Standard_Real X() const { return coord.X(); }

  //! For this vector, returns its Y  coordinate.
  Standard_Real Y() const { return coord.Y(); }

  //! For this vector, returns its two coordinates as a number pair
  const gp_XY& XY() const { return coord; }

  //! Returns True if the two vectors have the same magnitude value
  //! and the same direction. The precision values are theLinearTolerance
  //! for the magnitude and theAngularTolerance for the direction.
  Standard_EXPORT Standard_Boolean IsEqual (const gp_Vec2d& theOther, const Standard_Real theLinearTolerance, const Standard_Real theAngularTolerance) const;

  //! Returns True if abs(Abs(<me>.Angle(theOther)) - PI/2.)
  //! <= theAngularTolerance
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! theOther.Magnitude() <= Resolution from gp.
  Standard_Boolean IsNormal (const gp_Vec2d& theOther, const Standard_Real theAngularTolerance) const
  {
    const Standard_Real anAng = Abs (M_PI_2 - Abs (Angle (theOther)));
    return !(anAng > theAngularTolerance);
  }

  //! Returns True if PI - Abs(<me>.Angle(theOther)) <= theAngularTolerance
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! theOther.Magnitude() <= Resolution from gp.
  Standard_Boolean IsOpposite (const gp_Vec2d& theOther, const Standard_Real theAngularTolerance) const;

  //! Returns true if Abs(Angle(<me>, theOther)) <= theAngularTolerance or
  //! PI - Abs(Angle(<me>, theOther)) <= theAngularTolerance
  //! Two vectors with opposite directions are considered as parallel.
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
  //! theOther.Magnitude() <= Resolution from gp
  Standard_Boolean IsParallel (const gp_Vec2d& theOther, const Standard_Real theAngularTolerance) const;

  //! Computes the angular value between <me> and <theOther>
  //! returns the angle value between -PI and PI in radian.
  //! The orientation is from <me> to theOther. The positive sense is the
  //! trigonometric sense.
  //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution from gp or
  //! theOther.Magnitude() <= Resolution because the angular value is
  //! indefinite if one of the vectors has a null magnitude.
  Standard_EXPORT Standard_Real Angle (const gp_Vec2d& theOther) const;

  //! Computes the magnitude of this vector.
  Standard_Real Magnitude() const { return coord.Modulus(); }

  //! Computes the square magnitude of this vector.
  Standard_Real SquareMagnitude() const { return coord.SquareModulus(); }

  void Add (const gp_Vec2d& theOther) { coord.Add (theOther.coord); }

  void operator += (const gp_Vec2d& theOther) { Add (theOther); }

  //! Adds two vectors
  Standard_NODISCARD gp_Vec2d Added (const gp_Vec2d& theOther) const
  {
    gp_Vec2d aV = *this;
    aV.coord.Add (theOther.coord);
    return aV;
  }

  Standard_NODISCARD gp_Vec2d operator + (const gp_Vec2d& theOther) const { return Added (theOther); }

  //! Computes the crossing product between two vectors
  Standard_NODISCARD Standard_Real Crossed (const gp_Vec2d& theRight) const
  {
    return coord.Crossed (theRight.coord);
  }

  Standard_NODISCARD Standard_Real operator ^ (const gp_Vec2d& theRight) const { return Crossed (theRight); }

  //! Computes the magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||
  Standard_Real CrossMagnitude (const gp_Vec2d& theRight) const
  {
    return coord.CrossMagnitude (theRight.coord);
  }

  //! Computes the square magnitude of the cross product between <me> and
  //! theRight. Returns || <me> ^ theRight ||**2
  Standard_Real CrossSquareMagnitude (const gp_Vec2d& theRight) const
  {
    return coord.CrossSquareMagnitude (theRight.coord);
  }

  void Divide (const Standard_Real theScalar) { coord.Divide (theScalar); }

  void operator /= (const Standard_Real theScalar) { Divide (theScalar); }

  //! divides a vector by a scalar
  Standard_NODISCARD gp_Vec2d Divided (const Standard_Real theScalar) const
  {
    gp_Vec2d aV = *this;
    aV.coord.Divide (theScalar);
    return aV;
  }

  Standard_NODISCARD gp_Vec2d operator / (const Standard_Real theScalar) const { return Divided (theScalar); }

  //! Computes the scalar product
  Standard_Real Dot (const gp_Vec2d& theOther) const { return coord.Dot (theOther.coord); }

  Standard_Real operator * (const gp_Vec2d& theOther) const { return Dot (theOther); }

  gp_Vec2d GetNormal() const { return gp_Vec2d (this->Y(), (-1)*this->X()); }

  void Multiply (const Standard_Real theScalar) { coord.Multiply (theScalar); }

  void operator *= (const Standard_Real theScalar) { Multiply (theScalar); }

  //! Normalizes a vector
  //! Raises an exception if the magnitude of the vector is
  //! lower or equal to Resolution from package gp.
  Standard_NODISCARD gp_Vec2d Multiplied (const Standard_Real theScalar) const
  {
    gp_Vec2d aV = *this;
    aV.coord.Multiply (theScalar);
    return aV;
  }

  Standard_NODISCARD gp_Vec2d operator * (const Standard_Real theScalar) const { return Multiplied (theScalar); }

  void Normalize()
  {
    Standard_Real aD = coord.Modulus();
    Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Vec2d::Normalize() - vector has zero norm");
    coord.Divide (aD);
  }

  //! Normalizes a vector
  //! Raises an exception if the magnitude of the vector is
  //! lower or equal to Resolution from package gp.
  //! Reverses the direction of a vector
  Standard_NODISCARD gp_Vec2d Normalized() const;

  void Reverse() { coord.Reverse(); }

  //! Reverses the direction of a vector
  Standard_NODISCARD gp_Vec2d Reversed() const
  {
    gp_Vec2d aV = *this;
    aV.coord.Reverse();
    return aV;
  }

  Standard_NODISCARD gp_Vec2d operator -() const { return Reversed(); }

  //! Subtracts two vectors
  void Subtract (const gp_Vec2d& theRight)
  {
    coord.Subtract (theRight.coord);
  }

  void operator -= (const gp_Vec2d& theRight) { Subtract (theRight); }

  //! Subtracts two vectors
  Standard_NODISCARD gp_Vec2d Subtracted (const gp_Vec2d& theRight) const
  {
    gp_Vec2d aV = *this;
    aV.coord.Subtract (theRight.coord);
    return aV;
  }

  Standard_NODISCARD gp_Vec2d operator - (const gp_Vec2d& theRight) const { return Subtracted (theRight); }

  //! <me> is set to the following linear form :
  //! theA1 * theV1 + theA2 * theV2 + theV3
  void SetLinearForm (const Standard_Real theA1, const gp_Vec2d& theV1,
                      const Standard_Real theA2, const gp_Vec2d& theV2, const gp_Vec2d& theV3)
  {
    coord.SetLinearForm (theA1, theV1.coord, theA2, theV2.coord, theV3.coord);
  }

  //! <me> is set to the following linear form : theA1 * theV1 + theA2 * theV2
  void SetLinearForm (const Standard_Real theA1, const gp_Vec2d& theV1,
                      const Standard_Real theA2, const gp_Vec2d& theV2)
  {
    coord.SetLinearForm (theA1, theV1.coord, theA2, theV2.coord);
  }

  //! <me> is set to the following linear form : theA1 * theV1 + theV2
  void SetLinearForm (const Standard_Real theA1, const gp_Vec2d& theV1, const gp_Vec2d& theV2)
  {
    coord.SetLinearForm (theA1, theV1.coord, theV2.coord);
  }

  //! <me> is set to the following linear form : theV1 + theV2
  void SetLinearForm (const gp_Vec2d& theV1, const gp_Vec2d& theV2)
  {
    coord.SetLinearForm (theV1.coord, theV2.coord);
  }

  //! Performs the symmetrical transformation of a vector
  //! with respect to the vector theV which is the center of
  //! the  symmetry.
  Standard_EXPORT void Mirror (const gp_Vec2d& theV);
 
  //! Performs the symmetrical transformation of a vector
  //! with respect to the vector theV which is the center of
  //! the  symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Vec2d Mirrored (const gp_Vec2d& theV) const;

  //! Performs the symmetrical transformation of a vector
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_EXPORT void Mirror (const gp_Ax2d& theA1);

  //! Performs the symmetrical transformation of a vector
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Vec2d Mirrored (const gp_Ax2d& theA1) const;

  void Rotate (const Standard_Real theAng);

  //! Rotates a vector. theAng is the angular value of the
  //! rotation in radians.
  Standard_NODISCARD gp_Vec2d Rotated (const Standard_Real theAng) const
  {
    gp_Vec2d aV = *this;
    aV.Rotate (theAng);
    return aV;
  }

  void Scale (const Standard_Real theS) { coord.Multiply (theS); }

  //! Scales a vector. theS is the scaling value.
  Standard_NODISCARD gp_Vec2d Scaled (const Standard_Real theS) const
  {
    gp_Vec2d aV = *this;
    aV.coord.Multiply (theS);
    return aV;
  }

  Standard_EXPORT void Transform (const gp_Trsf2d& theT);

  //! Transforms a vector with a Trsf from gp.
  Standard_NODISCARD gp_Vec2d Transformed (const gp_Trsf2d& theT) const
  {
    gp_Vec2d aV = *this;
    aV.Transform (theT);
    return aV;
  }

private:

  gp_XY coord;

};

#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Pnt2d.hxx>

//=======================================================================
//function :  gp_Vec2d
// purpose :
//=======================================================================
inline gp_Vec2d::gp_Vec2d (const gp_Dir2d& theV)
{
  coord = theV.XY();
}

//=======================================================================
//function :  gp_Vec2d
// purpose :
//=======================================================================
inline gp_Vec2d::gp_Vec2d (const gp_Pnt2d& theP1, const gp_Pnt2d& theP2)
{
  coord = theP2.XY().Subtracted (theP1.XY());
}

//=======================================================================
//function :  IsOpposite
// purpose :
//=======================================================================
inline Standard_Boolean gp_Vec2d::IsOpposite (const gp_Vec2d& theOther, const Standard_Real theAngularTolerance) const
{
  Standard_Real anAng = Angle (theOther);
  if (anAng < 0)
  {
    anAng = -anAng;
  }
  return M_PI - anAng <= theAngularTolerance;
}

//=======================================================================
//function :  IsParallel
// purpose :
//=======================================================================
inline Standard_Boolean gp_Vec2d::IsParallel (const gp_Vec2d& theOther, const Standard_Real theAngularTolerance) const
{
  Standard_Real anAng = Angle (theOther);
  if (anAng < 0)
  {
    anAng = -anAng;
  }
  return anAng <= theAngularTolerance || M_PI - anAng <= theAngularTolerance;
}

//=======================================================================
//function :  Normalized
// purpose :
//=======================================================================
inline gp_Vec2d gp_Vec2d::Normalized() const
{
  Standard_Real aD = coord.Modulus();
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Vec2d::Normalized() - vector has zero norm");
  gp_Vec2d aV = *this;
  aV.coord.Divide (aD);
  return aV;
}

//=======================================================================
//function :  Rotate
// purpose :
//=======================================================================
inline void gp_Vec2d::Rotate (const Standard_Real theAng)
{
  gp_Trsf2d aT;
  aT.SetRotation (gp_Pnt2d(0.0, 0.0), theAng);
  coord.Multiply (aT.VectorialPart());
}

//=======================================================================
//function :  operator*
// purpose :
//=======================================================================
inline gp_Vec2d operator* (const Standard_Real theScalar,
                           const gp_Vec2d& theV)
{
  return theV.Multiplied (theScalar);
}

#endif // _gp_Vec2d_HeaderFile
