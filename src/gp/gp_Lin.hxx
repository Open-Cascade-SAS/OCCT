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

#ifndef _gp_Lin_HeaderFile
#define _gp_Lin_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes a line in 3D space.
//! A line is positioned in space with an axis (a gp_Ax1
//! object) which gives it an origin and a unit vector.
//! A line and an axis are similar objects, thus, we can
//! convert one into the other. A line provides direct access
//! to the majority of the edit and query functions available
//! on its positioning axis. In addition, however, a line has
//! specific functions for computing distances and positions.
//! See Also
//! gce_MakeLin which provides functions for more complex
//! line constructions
//! Geom_Line which provides additional functions for
//! constructing lines and works, in particular, with the
//! parametric equations of lines
class gp_Lin 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a Line corresponding to Z axis of the
  //! reference coordinate system.
  gp_Lin() {}

  //! Creates a line defined by axis theA1.
  gp_Lin (const gp_Ax1& theA1)
  : pos (theA1)
  {}

  //! Creates a line passing through point theP and parallel to
  //! vector theV (theP and theV are, respectively, the origin and
  //! the unit vector of the positioning axis of the line).
  gp_Lin (const gp_Pnt& theP, const gp_Dir& theV)
  : pos (theP, theV)
  {}

  void Reverse()
  {
    pos.Reverse();
  }

  //! Reverses the direction of the line.
  //! Note:
  //! -   Reverse assigns the result to this line, while
  //! -   Reversed creates a new one.
  Standard_NODISCARD gp_Lin Reversed() const
  {
    gp_Lin aL = *this;
    aL.pos.Reverse();
    return aL;
  }

  //! Changes the direction of the line.
  void SetDirection (const gp_Dir& theV) { pos.SetDirection (theV); }

  //! Changes the location point (origin) of the line.
  void SetLocation (const gp_Pnt& theP) { pos.SetLocation (theP); }

  //! Complete redefinition of the line.
  //! The "Location" point of <theA1> is the origin of the line.
  //! The "Direction" of <theA1> is  the direction of the line.
  void SetPosition (const gp_Ax1& theA1) { pos = theA1; }

  //! Returns the direction of the line.
  const gp_Dir& Direction() const { return pos.Direction(); }

  //! Returns the location point (origin) of the line.
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the axis placement one axis with the same
  //! location and direction as <me>.
  const gp_Ax1& Position() const { return pos; }

  //! Computes the angle between two lines in radians.
  Standard_Real Angle (const gp_Lin& theOther) const
  {
    return pos.Direction().Angle (theOther.pos.Direction());
  }

  //! Returns true if this line contains the point theP, that is, if the
  //! distance between point theP and this line is less than or
  //! equal to theLinearTolerance..
  Standard_Boolean Contains (const gp_Pnt& theP, const Standard_Real theLinearTolerance) const
  {
    return Distance (theP) <= theLinearTolerance;
  }

  //! Computes the distance between <me> and the point theP.
  Standard_Real Distance (const gp_Pnt& theP) const;

  //! Computes the distance between two lines.
  Standard_EXPORT Standard_Real Distance (const gp_Lin& theOther) const;

  //! Computes the square distance between <me> and the point theP.
  Standard_Real SquareDistance (const gp_Pnt& theP) const;

  //! Computes the square distance between two lines.
  Standard_Real SquareDistance (const gp_Lin& theOther) const
  {
    Standard_Real aD = Distance (theOther);
    return aD * aD;
  }

  //! Computes the line normal to the direction of <me>, passing
  //! through the point theP.  Raises ConstructionError
  //! if the distance between <me> and the point theP is lower
  //! or equal to Resolution from gp because there is an infinity of
  //! solutions in 3D space.
  gp_Lin Normal (const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror (const gp_Pnt& theP);

  //! Performs the symmetrical transformation of a line
  //! with respect to the point theP which is the center of
  //! the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Lin Mirrored (const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of a line
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Lin Mirrored (const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror (const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of a line
  //! with respect to a plane. The axis placement  <theA2>
  //! locates the plane of the symmetry :
  //! (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Lin Mirrored (const gp_Ax2& theA2) const;

  void Rotate (const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate (theA1, theAng); }

  //! Rotates a line. A1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Lin Rotated (const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Lin aL = *this;
    aL.pos.Rotate (theA1, theAng);
    return aL;
  }

  void Scale (const gp_Pnt& theP, const Standard_Real theS) { pos.Scale (theP, theS); }

  //! Scales a line. theS is the scaling value.
  //! The "Location" point (origin) of the line is modified.
  //! The "Direction" is reversed if the scale is negative.
  Standard_NODISCARD gp_Lin Scaled (const gp_Pnt& theP, const Standard_Real theS) const
  {
    gp_Lin aL = *this;
    aL.pos.Scale (theP, theS);
    return aL;
  }

  void Transform (const gp_Trsf& theT) { pos.Transform (theT); }

  //! Transforms a line with the transformation theT from class Trsf.
  Standard_NODISCARD gp_Lin Transformed (const gp_Trsf& theT) const
  {
    gp_Lin aL = *this;
    aL.pos.Transform (theT);
    return aL;
  }

  void Translate (const gp_Vec& theV) { pos.Translate (theV); }

  //! Translates a line in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Lin Translated (const gp_Vec& theV) const
  {
    gp_Lin aL = *this;
    aL.pos.Translate (theV);
    return aL;
  }

  void Translate (const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate (theP1, theP2); }

  //! Translates a line from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Lin Translated (const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Lin aL = *this;
    aL.pos.Translate (gp_Vec(theP1, theP2));
    return aL;
  }

private:

  gp_Ax1 pos;

};

//=======================================================================
//function : Distance
// purpose :
//=======================================================================
inline Standard_Real gp_Lin::Distance (const gp_Pnt& theP) const
{
  gp_XYZ aCoord = theP.XYZ();
  aCoord.Subtract ((pos.Location()).XYZ());
  aCoord.Cross ((pos.Direction()).XYZ());
  return aCoord.Modulus();
}

//=======================================================================
//function : SquareDistance
// purpose :
//=======================================================================
inline Standard_Real gp_Lin::SquareDistance(const gp_Pnt& theP) const
{
  const gp_Pnt& aLoc = pos.Location();
  gp_Vec aV (theP.X() - aLoc.X(),
             theP.Y() - aLoc.Y(),
             theP.Z() - aLoc.Z());
  aV.Cross (pos.Direction());
  return aV.SquareMagnitude();
}

//=======================================================================
//function : Normal
// purpose :
//=======================================================================
inline gp_Lin gp_Lin::Normal(const gp_Pnt& theP) const
{
  const gp_Pnt& aLoc = pos.Location();
  gp_Dir aV (theP.X() - aLoc.X(),
             theP.Y() - aLoc.Y(),
             theP.Z() - aLoc.Z());
  aV = pos.Direction().CrossCrossed (aV, pos.Direction());
  return gp_Lin(theP, aV);
}

#endif // _gp_Lin_HeaderFile
