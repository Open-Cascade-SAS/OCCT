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

#ifndef _gp_Dir_HeaderFile
#define _gp_Dir_HeaderFile

#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

class gp_Vec;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;

//! Describes a unit vector in 3D space. This unit vector is also called "Direction".
//! See Also
//! gce_MakeDir which provides functions for more complex
//! unit vector constructions
//! Geom_Direction which provides additional functions for
//! constructing unit vectors and works, in particular, with the
//! parametric equations of unit vectors.
class gp_Dir 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a direction corresponding to X axis.
  gp_Dir()
  : coord (1., 0., 0.)
  {}

  //! Normalizes the vector theV and creates a direction. Raises ConstructionError if theV.Magnitude() <= Resolution.
  gp_Dir (const gp_Vec& theV);

  //! Creates a direction from a triplet of coordinates. Raises ConstructionError if theCoord.Modulus() <= Resolution from gp.
  gp_Dir (const gp_XYZ& theCoord);

  //! Creates a direction with its 3 cartesian coordinates. Raises ConstructionError if Sqrt(theXv*theXv + theYv*theYv + theZv*theZv) <= Resolution
  //! Modification of the direction's coordinates
  //! If Sqrt (theXv*theXv + theYv*theYv + theZv*theZv) <= Resolution from gp where
  //! theXv, theYv ,theZv are the new coordinates it is not possible to
  //! construct the direction and the method raises the
  //! exception ConstructionError.
  gp_Dir (const Standard_Real theXv, const Standard_Real theYv, const Standard_Real theZv);

  //! For this unit vector,  assigns the value Xi to:
  //! -   the X coordinate if theIndex is 1, or
  //! -   the Y coordinate if theIndex is 2, or
  //! -   the Z coordinate if theIndex is 3,
  //! and then normalizes it.
  //! Warning
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  //! Exceptions
  //! Standard_OutOfRange if theIndex is not 1, 2, or 3.
  //! Standard_ConstructionError if either of the following
  //! is less than or equal to gp::Resolution():
  //! -   Sqrt(Xv*Xv + Yv*Yv + Zv*Zv), or
  //! -   the modulus of the number triple formed by the new
  //! value theXi and the two other coordinates of this vector
  //! that were not directly modified.
  void SetCoord (const Standard_Integer theIndex, const Standard_Real theXi);

  //! For this unit vector,  assigns the values theXv, theYv and theZv to its three coordinates.
  //! Remember that all the coordinates of a unit vector are
  //! implicitly modified when any single one is changed directly.
  void SetCoord (const Standard_Real theXv, const Standard_Real theYv, const Standard_Real theZv);

  //! Assigns the given value to the X coordinate of this   unit vector.
  void SetX (const Standard_Real theX);

  //! Assigns the given value to the Y coordinate of this   unit vector.
  void SetY (const Standard_Real theY);

  //! Assigns the given value to the Z  coordinate of this   unit vector.
  void SetZ (const Standard_Real theZ);

  //! Assigns the three coordinates of theCoord to this unit vector.
  void SetXYZ (const gp_XYZ& theCoord);

  //! Returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! Ithendex = 2 => Y is returned
  //! theIndex = 3 => Z is returned
  //! Exceptions
  //! Standard_OutOfRange if theIndex is not 1, 2, or 3.
  Standard_Real Coord (const Standard_Integer theIndex) const  { return coord.Coord (theIndex); }

  //! Returns for the  unit vector  its three coordinates theXv, theYv, and theZv.
  void Coord (Standard_Real& theXv, Standard_Real& theYv, Standard_Real& theZv) const  { coord.Coord (theXv, theYv, theZv); }

  //! Returns the X coordinate for a  unit vector.
  Standard_Real X() const { return coord.X(); }

  //! Returns the Y coordinate for a  unit vector.
  Standard_Real Y() const { return coord.Y(); }

  //! Returns the Z coordinate for a  unit vector.
  Standard_Real Z() const { return coord.Z(); }

  //! for this unit vector, returns  its three coordinates as a number triplea.
  const gp_XYZ& XYZ() const { return coord; }

  //! Returns True if the angle between the two directions is
  //! lower or equal to theAngularTolerance.
  Standard_Boolean IsEqual (const gp_Dir& theOther, const Standard_Real theAngularTolerance) const
  {
    return Angle (theOther) <= theAngularTolerance;
  }

  //! Returns True if  the angle between this unit vector and the unit vector theOther is equal to Pi/2 (normal).
  Standard_Boolean IsNormal (const gp_Dir& theOther, const Standard_Real theAngularTolerance) const
  {
    Standard_Real anAng = M_PI / 2.0 - Angle (theOther);
    if (anAng < 0)
    {
      anAng = -anAng;
    }
    return anAng <= theAngularTolerance;
  }

  //! Returns True if  the angle between this unit vector and the unit vector theOther is equal to  Pi (opposite).
  Standard_Boolean IsOpposite (const gp_Dir& theOther, const Standard_Real theAngularTolerance) const
  {
    return M_PI - Angle (theOther) <= theAngularTolerance;
  }

  //! Returns true if the angle between this unit vector and the
  //! unit vector theOther is equal to 0 or to Pi.
  //! Note: the tolerance criterion is given by theAngularTolerance.
  Standard_Boolean IsParallel (const gp_Dir& theOther, const Standard_Real theAngularTolerance) const
  {
    Standard_Real anAng = Angle (theOther);
    return anAng <= theAngularTolerance || M_PI - anAng <= theAngularTolerance;
  }

  //! Computes the angular value in radians between <me> and
  //! <theOther>. This value is always positive in 3D space.
  //! Returns the angle in the range [0, PI]
  Standard_EXPORT Standard_Real Angle (const gp_Dir& theOther) const;

  //! Computes the angular value between <me> and <theOther>.
  //! <theVRef> is the direction of reference normal to <me> and <theOther>
  //! and its orientation gives the positive sense of rotation.
  //! If the cross product <me> ^ <theOther> has the same orientation
  //! as <theVRef> the angular value is positive else negative.
  //! Returns the angular value in the range -PI and PI (in radians). Raises  DomainError if <me> and <theOther> are not parallel this exception is raised
  //! when <theVRef> is in the same plane as <me> and <theOther>
  //! The tolerance criterion is Resolution from package gp.
  Standard_EXPORT Standard_Real AngleWithRef (const gp_Dir& theOther, const gp_Dir& theVRef) const;

  //! Computes the cross product between two directions
  //! Raises the exception ConstructionError if the two directions
  //! are parallel because the computed vector cannot be normalized
  //! to create a direction.
  void Cross (const gp_Dir& theRight);

  void operator ^= (const gp_Dir& theRight) { Cross (theRight); }

  //! Computes the triple vector product.
  //! <me> ^ (V1 ^ V2)
  //! Raises the exception ConstructionError if V1 and V2 are parallel
  //! or <me> and (V1^V2) are parallel because the computed vector
  //! can't be normalized to create a direction.
  Standard_NODISCARD gp_Dir Crossed (const gp_Dir& theRight) const;

  Standard_NODISCARD gp_Dir operator ^ (const gp_Dir& theRight) const { return Crossed (theRight); }

  void CrossCross (const gp_Dir& theV1, const gp_Dir& theV2);

  //! Computes the double vector product this ^ (theV1 ^ theV2).
  //! -   CrossCrossed creates a new unit vector.
  //! Exceptions
  //! Standard_ConstructionError if:
  //! -   theV1 and theV2 are parallel, or
  //! -   this unit vector and (theV1 ^ theV2) are parallel.
  //! This is because, in these conditions, the computed vector
  //! is null and cannot be normalized.
  Standard_NODISCARD gp_Dir CrossCrossed (const gp_Dir& theV1, const gp_Dir& theV2) const;

  //! Computes the scalar product
  Standard_Real Dot (const gp_Dir& theOther) const { return coord.Dot (theOther.coord); }

  Standard_Real operator * (const gp_Dir& theOther) const { return Dot (theOther); }

  //! Computes the triple scalar product <me> * (theV1 ^ theV2).
  //! Warnings :
  //! The computed vector theV1' = theV1 ^ theV2 is not normalized
  //! to create a unitary vector. So this method never
  //! raises an exception even if theV1 and theV2 are parallel.
  Standard_Real DotCross (const gp_Dir& theV1, const gp_Dir& theV2) const
  {
    return coord.Dot (theV1.coord.Crossed (theV2.coord));
  }

  void Reverse() { coord.Reverse(); }

  //! Reverses the orientation of a direction
  //! geometric transformations
  //! Performs the symmetrical transformation of a direction
  //! with respect to the direction V which is the center of
  //! the  symmetry.]
  Standard_NODISCARD gp_Dir Reversed() const
  {
    gp_Dir aV = *this;
    aV.coord.Reverse();
    return aV;
  }

  Standard_NODISCARD gp_Dir operator -() const { return Reversed(); }

  Standard_EXPORT void Mirror (const gp_Dir& theV);

  //! Performs the symmetrical transformation of a direction
  //! with respect to the direction theV which is the center of
  //! the  symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Dir Mirrored (const gp_Dir& theV) const;

  Standard_EXPORT void Mirror (const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of a direction
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Dir Mirrored (const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror (const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of a direction
  //! with respect to a plane. The axis placement theA2 locates
  //! the plane of the symmetry : (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Dir Mirrored (const gp_Ax2& theA2) const;

  void Rotate(const gp_Ax1& theA1, const Standard_Real theAng);

  //! Rotates a direction. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Dir Rotated (const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Dir aV = *this;
    aV.Rotate (theA1, theAng);
    return aV;
  }

  Standard_EXPORT void Transform (const gp_Trsf& theT);

  //! Transforms a direction with a "Trsf" from gp.
  //! Warnings :
  //! If the scale factor of the "Trsf" theT is negative then the
  //! direction <me> is reversed.
  Standard_NODISCARD gp_Dir Transformed (const gp_Trsf& theT) const
  {
    gp_Dir aV = *this;
    aV.Transform (theT);
    return aV;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  //! Inits the content of me from the stream
  Standard_EXPORT Standard_Boolean InitFromJson (const Standard_SStream& theSStream, Standard_Integer& theStreamPos);

private:

  gp_XYZ coord;

};

#include <gp_Trsf.hxx>

// =======================================================================
// function : gp_Dir
// purpose  :
// =======================================================================
inline gp_Dir::gp_Dir (const gp_Vec& theV)
{
  const gp_XYZ& aXYZ = theV.XYZ();
  Standard_Real aX = aXYZ.X();
  Standard_Real aY = aXYZ.Y();
  Standard_Real aZ = aXYZ.Z();
  Standard_Real aD = sqrt (aX * aX + aY * aY + aZ * aZ);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir() - input vector has zero norm");
  coord.SetX (aX / aD);
  coord.SetY (aY / aD);
  coord.SetZ (aZ / aD);
}

// =======================================================================
// function : gp_Dir
// purpose  :
// =======================================================================
inline gp_Dir::gp_Dir (const gp_XYZ& theXYZ)
{
  Standard_Real aX = theXYZ.X();
  Standard_Real aY = theXYZ.Y();
  Standard_Real aZ = theXYZ.Z();
  Standard_Real aD = sqrt (aX * aX + aY * aY + aZ * aZ);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir() - input vector has zero norm");
  coord.SetX (aX / aD);
  coord.SetY (aY / aD);
  coord.SetZ (aZ / aD);
}

// =======================================================================
// function : gp_Dir
// purpose  :
// =======================================================================
inline gp_Dir::gp_Dir (const Standard_Real theXv,
                       const Standard_Real theYv,
                       const Standard_Real theZv)
{
  Standard_Real aD = sqrt (theXv * theXv + theYv * theYv + theZv * theZv);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir() - input vector has zero norm");
  coord.SetX (theXv / aD);
  coord.SetY (theYv / aD);
  coord.SetZ (theZv / aD);
}

// =======================================================================
// function : SetCoord
// purpose  :
// =======================================================================
inline void gp_Dir::SetCoord (const Standard_Integer theIndex,
                              const Standard_Real theXi)
{
  Standard_Real aX = coord.X();
  Standard_Real aY = coord.Y();
  Standard_Real aZ = coord.Z();
  Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > 3, "gp_Dir::SetCoord() - index is out of range [1, 3]");
  if (theIndex == 1)
  {
    aX = theXi;
  }
  else if (theIndex == 2)
  {
    aY = theXi;
  }
  else
  {
    aZ = theXi;
  }
  Standard_Real aD = sqrt (aX * aX + aY * aY + aZ * aZ);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::SetCoord() - result vector has zero norm");
  coord.SetX (aX / aD);
  coord.SetY (aY / aD);
  coord.SetZ (aZ / aD);
}

// =======================================================================
// function : SetCoord
// purpose  :
// =======================================================================
inline void gp_Dir::SetCoord (const Standard_Real theXv,
                              const Standard_Real theYv,
                              const Standard_Real theZv) {
  Standard_Real aD = sqrt (theXv * theXv + theYv * theYv + theZv * theZv);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::SetCoord() - input vector has zero norm");
  coord.SetX (theXv / aD);
  coord.SetY (theYv / aD);
  coord.SetZ (theZv / aD);
}

// =======================================================================
// function : SetX
// purpose  :
// =======================================================================
inline void gp_Dir::SetX (const Standard_Real theX)
{
  Standard_Real anY = coord.Y();
  Standard_Real aZ = coord.Z();
  Standard_Real aD = sqrt (theX * theX + anY * anY + aZ * aZ);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::SetX() - result vector has zero norm");
  coord.SetX (theX / aD);
  coord.SetY (anY / aD);
  coord.SetZ (aZ / aD);
}

// =======================================================================
// function : SetY
// purpose  :
// =======================================================================
inline void gp_Dir::SetY (const Standard_Real theY)
{
  Standard_Real aZ = coord.Z();
  Standard_Real aX = coord.X();
  Standard_Real aD = sqrt (aX * aX + theY * theY + aZ * aZ);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::SetY() - result vector has zero norm");
  coord.SetX (aX / aD);
  coord.SetY (theY / aD);
  coord.SetZ (aZ / aD);
}

// =======================================================================
// function : SetZ
// purpose  :
// =======================================================================
inline void gp_Dir::SetZ (const Standard_Real theZ)
{
  Standard_Real aX = coord.X();
  Standard_Real anY = coord.Y();
  Standard_Real aD = sqrt (aX * aX + anY * anY + theZ * theZ);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::SetZ() - result vector has zero norm");
  coord.SetX (aX / aD);
  coord.SetY (anY / aD);
  coord.SetZ (theZ / aD);
}

// =======================================================================
// function : SetXYZ
// purpose  :
// =======================================================================
inline void gp_Dir::SetXYZ (const gp_XYZ& theXYZ)
{
  Standard_Real aX = theXYZ.X();
  Standard_Real anY = theXYZ.Y();
  Standard_Real aZ = theXYZ.Z();
  Standard_Real aD = sqrt(aX * aX + anY * anY + aZ * aZ);
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::SetX() - input vector has zero norm");
  coord.SetX (aX / aD);
  coord.SetY (anY / aD);
  coord.SetZ (aZ / aD);
}

// =======================================================================
// function : Cross
// purpose  :
// =======================================================================
inline void gp_Dir::Cross(const gp_Dir& theRight)
{
  coord.Cross (theRight.coord);
  Standard_Real aD = coord.Modulus();
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::Cross() - result vector has zero norm");
  coord.Divide (aD);
}

// =======================================================================
// function : Crossed
// purpose  :
// =======================================================================
inline gp_Dir gp_Dir::Crossed (const gp_Dir& theRight) const
{
  gp_Dir aV = *this;
  aV.coord.Cross (theRight.coord);
  Standard_Real aD = aV.coord.Modulus();
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::Crossed() - result vector has zero norm");
  aV.coord.Divide (aD);
  return aV;
}

// =======================================================================
// function : CrossCross
// purpose  :
// =======================================================================
inline void gp_Dir::CrossCross (const gp_Dir& theV1, const gp_Dir& theV2)
{
  coord.CrossCross (theV1.coord, theV2.coord);
  Standard_Real aD = coord.Modulus();
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::CrossCross() - result vector has zero norm");
  coord.Divide (aD);
}

// =======================================================================
// function : CrossCrossed
// purpose  :
// =======================================================================
inline gp_Dir gp_Dir::CrossCrossed (const gp_Dir& theV1, const gp_Dir& theV2) const
{
  gp_Dir aV = *this;
  (aV.coord).CrossCross (theV1.coord, theV2.coord);
  Standard_Real aD = aV.coord.Modulus();
  Standard_ConstructionError_Raise_if (aD <= gp::Resolution(), "gp_Dir::CrossCrossed() - result vector has zero norm");
  aV.coord.Divide (aD);
  return aV;
}

// =======================================================================
// function : Rotate
// purpose  :
// =======================================================================
inline void gp_Dir::Rotate(const gp_Ax1& theA1, const Standard_Real theAng)
{
  gp_Trsf aT;
  aT.SetRotation (theA1, theAng);
  coord.Multiply (aT.HVectorialPart());
}

#endif // _gp_Dir_HeaderFile
