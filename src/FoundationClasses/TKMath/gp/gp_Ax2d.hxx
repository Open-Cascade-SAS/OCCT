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

#ifndef _gp_Ax2d_HeaderFile
#define _gp_Ax2d_HeaderFile

#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>

class gp_Trsf2d;
class gp_Vec2d;

//! Describes an axis in the plane (2D space).
//! An axis is defined by:
//! -   its origin (also referred to as its "Location point"),   and
//! -   its unit vector (referred to as its "Direction").
//! An axis implicitly defines a direct, right-handed
//! coordinate system in 2D space by:
//! -   its origin,
//! - its "Direction" (giving the "X Direction" of the coordinate system), and
//! -   the unit vector normal to "Direction" (positive angle
//! measured in the trigonometric sense).
//! An axis is used:
//! -   to describe 2D geometric entities (for example, the
//! axis which defines angular coordinates on a circle).
//! It serves for the same purpose as the STEP function
//! "axis placement one axis", or
//! -   to define geometric transformations (axis of
//! symmetry, axis of rotation, and so on).
//! Note: to define a left-handed 2D coordinate system, use gp_Ax22d.
class gp_Ax2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an axis object representing X axis of the reference co-ordinate system.
  gp_Ax2d()
      : loc(0., 0.)
  // vdir(1.,0.) use default ctor of gp_Dir2d, as it creates the same dir (1,0)
  {
  }

  //! Creates an Ax2d.
  //! <theP> is the "Location" point of the axis placement
  //! and theV is the "Direction" of the axis placement.
  gp_Ax2d(const gp_Pnt2d& theP, const gp_Dir2d& theV)
      : loc(theP),
        vdir(theV)
  {
  }

  //! Changes the "Location" point (origin) of <me>.
  void SetLocation(const gp_Pnt2d& theP) { loc = theP; }

  //! Changes the direction of <me>.
  void SetDirection(const gp_Dir2d& theV) { vdir = theV; }

  //! Returns the origin of <me>.
  const gp_Pnt2d& Location() const { return loc; }

  //! Returns the direction of <me>.
  const gp_Dir2d& Direction() const { return vdir; }

  //! Returns True if  :
  //! . the angle between <me> and <Other> is lower or equal
  //! to <AngularTolerance> and
  //! . the distance between <me>.Location() and <Other> is lower
  //! or equal to <LinearTolerance> and
  //! . the distance between <Other>.Location() and <me> is lower
  //! or equal to LinearTolerance.
  Standard_EXPORT Standard_Boolean IsCoaxial(const gp_Ax2d&      Other,
                                             const Standard_Real AngularTolerance,
                                             const Standard_Real LinearTolerance) const;

  //! Returns true if this axis and the axis theOther are normal to each other.
  //! That is, if the angle between the two axes is equal to Pi/2 or -Pi/2.
  //! Note: the tolerance criterion is given by theAngularTolerance.
  Standard_Boolean IsNormal(const gp_Ax2d& theOther, const Standard_Real theAngularTolerance) const
  {
    return vdir.IsNormal(theOther.vdir, theAngularTolerance);
  }

  //! Returns true if this axis and the axis theOther are parallel, and have opposite orientations.
  //! That is, if the angle between the two axes is equal to Pi or -Pi.
  //! Note: the tolerance criterion is given by theAngularTolerance.
  Standard_Boolean IsOpposite(const gp_Ax2d&      theOther,
                              const Standard_Real theAngularTolerance) const
  {
    return vdir.IsOpposite(theOther.vdir, theAngularTolerance);
  }

  //! Returns true if this axis and the axis theOther are parallel,
  //! and have either the same or opposite orientations.
  //! That is, if the angle between the two axes is equal to 0, Pi or -Pi.
  //! Note: the tolerance criterion is given by theAngularTolerance.
  Standard_Boolean IsParallel(const gp_Ax2d&      theOther,
                              const Standard_Real theAngularTolerance) const
  {
    return vdir.IsParallel(theOther.vdir, theAngularTolerance);
  }

  //! Computes the angle, in radians, between this axis and the axis theOther.
  //! The value of the angle is between -Pi and Pi.
  Standard_Real Angle(const gp_Ax2d& theOther) const { return vdir.Angle(theOther.vdir); }

  //! Reverses the direction of <me> and assigns the result to this axis.
  void Reverse() { vdir.Reverse(); }

  //! Computes a new axis placement with a direction opposite to the direction of <me>.
  Standard_NODISCARD gp_Ax2d Reversed() const
  {
    gp_Ax2d aTemp = *this;
    aTemp.Reverse();
    return aTemp;
  }

  Standard_EXPORT void Mirror(const gp_Pnt2d& P);

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to the point P which is the
  //! center of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Ax2d Mirrored(const gp_Pnt2d& P) const;

  Standard_EXPORT void Mirror(const gp_Ax2d& A);

  //! Performs the symmetrical transformation of an axis
  //! placement with respect to an axis placement which
  //! is the axis of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Ax2d Mirrored(const gp_Ax2d& A) const;

  void Rotate(const gp_Pnt2d& theP, const Standard_Real theAng)
  {
    loc.Rotate(theP, theAng);
    vdir.Rotate(theAng);
  }

  //! Rotates an axis placement. <theP> is the center of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Ax2d Rotated(const gp_Pnt2d& theP, const Standard_Real theAng) const
  {
    gp_Ax2d anA = *this;
    anA.Rotate(theP, theAng);
    return anA;
  }

  Standard_EXPORT void Scale(const gp_Pnt2d& P, const Standard_Real S);

  //! Applies a scaling transformation on the axis placement.
  //! The "Location" point of the axisplacement is modified.
  //! The "Direction" is reversed if the scale is negative.
  Standard_NODISCARD gp_Ax2d Scaled(const gp_Pnt2d& theP, const Standard_Real theS) const
  {
    gp_Ax2d anA = *this;
    anA.Scale(theP, theS);
    return anA;
  }

  void Transform(const gp_Trsf2d& theT)
  {
    loc.Transform(theT);
    vdir.Transform(theT);
  }

  //! Transforms an axis placement with a Trsf.
  Standard_NODISCARD gp_Ax2d Transformed(const gp_Trsf2d& theT) const
  {
    gp_Ax2d anA = *this;
    anA.Transform(theT);
    return anA;
  }

  void Translate(const gp_Vec2d& theV) { loc.Translate(theV); }

  //! Translates an axis placement in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Ax2d Translated(const gp_Vec2d& theV) const
  {
    gp_Ax2d anA = *this;
    (anA.loc).Translate(theV);
    return anA;
  }

  void Translate(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) { loc.Translate(theP1, theP2); }

  //! Translates an axis placement from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Ax2d Translated(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) const
  {
    gp_Ax2d anA = *this;
    (anA.loc).Translate(gp_Vec2d(theP1, theP2));
    return anA;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  gp_Pnt2d loc;
  gp_Dir2d vdir;
};

#endif // _gp_Ax2d_HeaderFile
