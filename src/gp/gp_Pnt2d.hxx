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
#include <Standard_Boolean.hxx>

class gp_Ax2d;
class gp_Trsf2d;
class gp_Vec2d;

//! Defines  a non-persistent 2D cartesian point.
class gp_Pnt2d 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a point with zero coordinates.
  gp_Pnt2d() {}

  //! Creates a point with a doublet of coordinates.
  gp_Pnt2d (const gp_XY& theCoord)
  : coord (theCoord)
  {}

  //! Creates a  point with its 2 cartesian's coordinates : theXp, theYp.
  gp_Pnt2d (const Standard_Real theXp, const Standard_Real theYp)
  : coord (theXp, theYp)
  {}

  //! Assigns the value Xi to the coordinate that corresponds to theIndex:
  //! theIndex = 1 => X is modified
  //! theIndex = 2 => Y is modified
  //! Raises OutOfRange if theIndex != {1, 2}.
  void SetCoord (const Standard_Integer theIndex, const Standard_Real theXi) { coord.SetCoord (theIndex, theXi); }

  //! For this point, assigns the values theXp and theYp to its two coordinates
  void SetCoord (const Standard_Real theXp, const Standard_Real theYp) { coord.SetCoord (theXp, theYp); }

  //! Assigns the given value to the X  coordinate of this point.
  void SetX (const Standard_Real theX) { coord.SetX (theX); }

  //! Assigns the given value to the Y  coordinate of this point.
  void SetY (const Standard_Real theY) { coord.SetY (theY); }

  //! Assigns the two coordinates of Coord to this point.
  void SetXY (const gp_XY& theCoord) { coord = theCoord; }

  //! Returns the coordinate of range theIndex :
  //! theIndex = 1 => X is returned
  //! theIndex = 2 => Y is returned
  //! Raises OutOfRange if theIndex != {1, 2}.
  Standard_Real Coord (const Standard_Integer theIndex) const { return coord.Coord (theIndex); }

  //! For this point returns its two coordinates as a number pair.
  void Coord (Standard_Real& theXp, Standard_Real& theYp) const { coord.Coord (theXp, theYp); }

  //! For this point, returns its X  coordinate.
  Standard_Real X() const { return coord.X(); }

  //! For this point, returns its Y coordinate.
  Standard_Real Y() const { return coord.Y(); }

  //! For this point, returns its two coordinates as a number pair.
  const gp_XY& XY() const { return coord; }

  //! For this point, returns its two coordinates as a number pair.
  const gp_XY& Coord() const { return coord; }

  //! Returns the coordinates of this point.
  //! Note: This syntax allows direct modification of the returned value.
  gp_XY& ChangeCoord() { return coord; }

  //! Comparison
  //! Returns True if the distance between the two
  //! points is lower or equal to theLinearTolerance.
  Standard_Boolean IsEqual (const gp_Pnt2d& theOther, const Standard_Real theLinearTolerance) const
  {
    return Distance (theOther) <= theLinearTolerance;
  }

  //! Computes the distance between two points.
  Standard_Real Distance (const gp_Pnt2d& theOther) const;

  //! Computes the square distance between two points.
  Standard_Real SquareDistance (const gp_Pnt2d& theOther) const;

  //! Performs the symmetrical transformation of a point
  //! with respect to the point theP which is the center of
  //! the  symmetry.
  Standard_EXPORT void Mirror (const gp_Pnt2d& theP);

  //! Performs the symmetrical transformation of a point
  //! with respect to an axis placement which is the axis
  Standard_NODISCARD Standard_EXPORT gp_Pnt2d Mirrored (const gp_Pnt2d& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax2d& theA);

  Standard_NODISCARD Standard_EXPORT gp_Pnt2d Mirrored (const gp_Ax2d& theA) const;

  //! Rotates a point. theA1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
  void Rotate (const gp_Pnt2d& theP, const Standard_Real theAng);

  Standard_NODISCARD gp_Pnt2d Rotated (const gp_Pnt2d& theP, const Standard_Real theAng) const
  {
    gp_Pnt2d aPres = *this;
    aPres.Rotate (theP, theAng);
    return aPres;
  }

  //! Scales a point. theS is the scaling value.
  void Scale (const gp_Pnt2d& theP, const Standard_Real theS);

  Standard_NODISCARD gp_Pnt2d Scaled (const gp_Pnt2d& theP, const Standard_Real theS) const
  {
    gp_Pnt2d aPres = *this;
    aPres.Scale (theP, theS);
    return aPres;
  }

  //! Transforms a point with the transformation theT.
  Standard_EXPORT void Transform (const gp_Trsf2d& theT);

  Standard_NODISCARD gp_Pnt2d Transformed (const gp_Trsf2d& theT) const
  {
    gp_Pnt2d aPres = *this;
    aPres.Transform (theT);
    return aPres;
  }

  //! Translates a point in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  void Translate (const gp_Vec2d& theV);

  Standard_NODISCARD gp_Pnt2d Translated (const gp_Vec2d& theV) const;

  //! Translates a point from the point theP1 to the point theP2.
  void Translate (const gp_Pnt2d& theP1, const gp_Pnt2d& theP2)
  {
    coord.Add (theP2.coord);
    coord.Subtract (theP1.coord);
  }

  Standard_NODISCARD gp_Pnt2d Translated (const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) const
  {
    gp_Pnt2d aP = *this;
    aP.Translate (theP1, theP2);
    return aP;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:

  gp_XY coord;

};

#include <gp_Vec2d.hxx>
#include <gp_Trsf2d.hxx>

//=======================================================================
//function : Distance
// purpose :
//=======================================================================
inline Standard_Real gp_Pnt2d::Distance (const gp_Pnt2d& theOther) const
{
  const gp_XY& aXY = theOther.coord;
  Standard_Real aX = coord.X() - aXY.X();
  Standard_Real aY = coord.Y() - aXY.Y();
  return sqrt (aX * aX + aY * aY);
}

//=======================================================================
//function : SquareDistance
// purpose :
//=======================================================================
inline Standard_Real gp_Pnt2d::SquareDistance (const gp_Pnt2d& theOther) const
{
  const gp_XY& aXY = theOther.coord;
  Standard_Real aX = coord.X() - aXY.X();
  Standard_Real aY = coord.Y() - aXY.Y();
  return (aX * aX + aY * aY);
}

//=======================================================================
//function : Rotate
// purpose :
//=======================================================================
inline void gp_Pnt2d::Rotate (const gp_Pnt2d& theP, const Standard_Real theAng)
{
  gp_Trsf2d aT;
  aT.SetRotation (theP, theAng);
  aT.Transforms (coord);
}

//=======================================================================
//function : Scale
// purpose :
//=======================================================================
inline void gp_Pnt2d::Scale (const gp_Pnt2d& theP, const Standard_Real theS)
{
  gp_XY aXY = theP.coord;
  aXY.Multiply (1.0 - theS);
  coord.Multiply (theS);
  coord.Add (aXY);
}

//=======================================================================
//function : Translate
// purpose :
//=======================================================================
inline void gp_Pnt2d::Translate(const gp_Vec2d& theV)
{
  coord.Add (theV.XY());
}

//=======================================================================
//function : Translated
// purpose :
//=======================================================================
inline gp_Pnt2d gp_Pnt2d::Translated (const gp_Vec2d& theV) const
{
  gp_Pnt2d aP = *this;
  aP.coord.Add (theV.XY());
  return aP;
}

#endif // _gp_Pnt2d_HeaderFile
