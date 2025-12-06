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

#include <gp_Ax2d.hxx>

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
  constexpr gp_Lin2d() noexcept {}

  //! Creates a line located with theA.
  constexpr gp_Lin2d(const gp_Ax2d& theA) noexcept
      : pos(theA)
  {
  }

  //! <theP> is the location point (origin) of the line and
  //! <theV> is the direction of the line.
  constexpr gp_Lin2d(const gp_Pnt2d& theP, const gp_Dir2d& theV) noexcept
      : pos(theP, theV)
  {
  }

  //! Creates the line from the equation theA*X + theB*Y + theC = 0.0 Raises ConstructionError if
  //! std::sqrt(theA*theA + theB*theB) <= Resolution from gp. Raised if std::sqrt(theA*theA +
  //! theB*theB) <= Resolution from gp.
  Standard_EXPORT gp_Lin2d(const Standard_Real theA,
                           const Standard_Real theB,
                           const Standard_Real theC);

  constexpr void Reverse() noexcept { pos.Reverse(); }

  //! Reverses the positioning axis of this line.
  //! Note:
  //! -   Reverse assigns the result to this line, while
  //! -   Reversed creates a new one.
  Standard_NODISCARD constexpr gp_Lin2d Reversed() const noexcept
  {
    gp_Lin2d aL = *this;
    aL.pos.Reverse();
    return aL;
  }

  //! Changes the direction of the line.
  constexpr void SetDirection(const gp_Dir2d& theV) noexcept { pos.SetDirection(theV); }

  //! Changes the origin of the line.
  constexpr void SetLocation(const gp_Pnt2d& theP) noexcept { pos.SetLocation(theP); }

  //! Complete redefinition of the line.
  //! The "Location" point of <theA> is the origin of the line.
  //! The "Direction" of <theA> is the direction of the line.
  constexpr void SetPosition(const gp_Ax2d& theA) noexcept { pos = theA; }

  //! Returns the normalized coefficients of the line :
  //! theA * X + theB * Y + theC = 0.
  constexpr void Coefficients(Standard_Real& theA,
                              Standard_Real& theB,
                              Standard_Real& theC) const noexcept
  {
    theA = pos.Direction().Y();
    theB = -pos.Direction().X();
    theC = -(theA * pos.Location().X() + theB * pos.Location().Y());
  }

  //! Returns the direction of the line.
  constexpr const gp_Dir2d& Direction() const noexcept { return pos.Direction(); }

  //! Returns the location point (origin) of the line.
  constexpr const gp_Pnt2d& Location() const noexcept { return pos.Location(); }

  //! Returns the axis placement one axis with the same
  //! location and direction as <me>.
  constexpr const gp_Ax2d& Position() const noexcept { return pos; }

  //! Computes the angle between two lines in radians.
  Standard_Real Angle(const gp_Lin2d& theOther) const
  {
    return pos.Direction().Angle(theOther.pos.Direction());
  }

  //! Returns true if this line contains the point theP, that is, if the
  //! distance between point theP and this line is less than or
  //! equal to theLinearTolerance.
  Standard_Boolean Contains(const gp_Pnt2d&     theP,
                            const Standard_Real theLinearTolerance) const noexcept
  {
    return Distance(theP) <= theLinearTolerance;
  }

  //! Computes the distance between <me> and the point <theP>.
  Standard_Real Distance(const gp_Pnt2d& theP) const noexcept;

  //! Computes the distance between two lines.
  Standard_Real Distance(const gp_Lin2d& theOther) const noexcept;

  //! Computes the square distance between <me> and the point
  //! <theP>.
  constexpr Standard_Real SquareDistance(const gp_Pnt2d& theP) const noexcept;

  //! Computes the square distance between two lines.
  Standard_Real SquareDistance(const gp_Lin2d& theOther) const noexcept;

  //! Computes the line normal to the direction of <me>,
  //! passing through the point <theP>.
  gp_Lin2d Normal(const gp_Pnt2d& theP) const noexcept
  {
    return gp_Lin2d(gp_Ax2d(theP, gp_Dir2d(-(pos.Direction().Y()), pos.Direction().X())));
  }

  Standard_EXPORT void Mirror(const gp_Pnt2d& theP) noexcept;

  //! Performs the symmetrical transformation of a line
  //! with respect to the point <theP> which is the center
  //! of the symmetry
  Standard_NODISCARD Standard_EXPORT gp_Lin2d Mirrored(const gp_Pnt2d& theP) const noexcept;

  Standard_EXPORT void Mirror(const gp_Ax2d& theA) noexcept;

  //! Performs the symmetrical transformation of a line
  //! with respect to an axis placement which is the axis
  //! of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Lin2d Mirrored(const gp_Ax2d& theA) const noexcept;

  void Rotate(const gp_Pnt2d& theP, const Standard_Real theAng) { pos.Rotate(theP, theAng); }

  //! Rotates a line. theP is the center of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Lin2d Rotated(const gp_Pnt2d& theP, const Standard_Real theAng) const
  {
    gp_Lin2d aL = *this;
    aL.pos.Rotate(theP, theAng);
    return aL;
  }

  void Scale(const gp_Pnt2d& theP, const Standard_Real theS) { pos.Scale(theP, theS); }

  //! Scales a line. theS is the scaling value. Only the
  //! origin of the line is modified.
  Standard_NODISCARD gp_Lin2d Scaled(const gp_Pnt2d& theP, const Standard_Real theS) const
  {
    gp_Lin2d aL = *this;
    aL.pos.Scale(theP, theS);
    return aL;
  }

  void Transform(const gp_Trsf2d& theT) noexcept { pos.Transform(theT); }

  //! Transforms a line with the transformation theT from class Trsf2d.
  Standard_NODISCARD gp_Lin2d Transformed(const gp_Trsf2d& theT) const noexcept
  {
    gp_Lin2d aL = *this;
    aL.pos.Transform(theT);
    return aL;
  }

  constexpr void Translate(const gp_Vec2d& theV) noexcept { pos.Translate(theV); }

  //! Translates a line in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Lin2d Translated(const gp_Vec2d& theV) const noexcept
  {
    gp_Lin2d aL = *this;
    aL.pos.Translate(theV);
    return aL;
  }

  constexpr void Translate(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) noexcept
  {
    pos.Translate(theP1, theP2);
  }

  //! Translates a line from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Lin2d Translated(const gp_Pnt2d& theP1,
                                         const gp_Pnt2d& theP2) const noexcept
  {
    gp_Lin2d aL = *this;
    aL.pos.Translate(gp_Vec2d(theP1, theP2));
    return aL;
  }

private:
  gp_Ax2d pos;
};

//=================================================================================================

inline Standard_Real gp_Lin2d::Distance(const gp_Pnt2d& theP) const noexcept
{
  gp_XY aCoord = theP.XY();
  aCoord.Subtract((pos.Location()).XY());
  return std::abs(aCoord.Crossed(pos.Direction().XY()));
}

//=================================================================================================

inline Standard_Real gp_Lin2d::Distance(const gp_Lin2d& theOther) const noexcept
{
  Standard_Real aD = 0.0;
  if (pos.IsParallel(theOther.pos, gp::Resolution()))
  {
    aD = theOther.Distance(pos.Location());
  }
  return aD;
}

//=================================================================================================

inline constexpr Standard_Real gp_Lin2d::SquareDistance(const gp_Pnt2d& theP) const noexcept
{
  gp_XY aCoord = theP.XY();
  aCoord.Subtract((pos.Location()).XY());
  Standard_Real aD = aCoord.Crossed(pos.Direction().XY());
  return aD * aD;
}

//=================================================================================================

inline Standard_Real gp_Lin2d::SquareDistance(const gp_Lin2d& theOther) const noexcept
{
  Standard_Real aD = 0.0;
  if (pos.IsParallel(theOther.pos, gp::Resolution()))
  {
    aD = theOther.SquareDistance(pos.Location());
  }
  return aD;
}

#endif // _gp_Lin2d_HeaderFile
