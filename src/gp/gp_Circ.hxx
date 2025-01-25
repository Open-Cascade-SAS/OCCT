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

#ifndef _gp_Circ_HeaderFile
#define _gp_Circ_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes a circle in 3D space.
//! A circle is defined by its radius and positioned in space
//! with a coordinate system (a gp_Ax2 object) as follows:
//! -   the origin of the coordinate system is the center of the circle, and
//! -   the origin, "X Direction" and "Y Direction" of the
//! coordinate system define the plane of the circle.
//! This positioning coordinate system is the "local
//! coordinate system" of the circle. Its "main Direction"
//! gives the normal vector to the plane of the circle. The
//! "main Axis" of the coordinate system is referred to as
//! the "Axis" of the circle.
//! Note: when a gp_Circ circle is converted into a
//! Geom_Circle circle, some implicit properties of the
//! circle are used explicitly:
//! -   the "main Direction" of the local coordinate system
//! gives an implicit orientation to the circle (and defines
//! its trigonometric sense),
//! -   this orientation corresponds to the direction in
//! which parameter values increase,
//! -   the starting point for parameterization is that of the
//! "X Axis" of the local coordinate system (i.e. the "X Axis" of the circle).
//! See Also
//! gce_MakeCirc which provides functions for more complex circle constructions
//! Geom_Circle which provides additional functions for
//! constructing circles and works, in particular, with the
//! parametric equations of circles
class gp_Circ
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an indefinite circle.
  gp_Circ()
      : radius(RealLast())
  {
  }

  //! A2 locates the circle and gives its orientation in 3D space.
  //! Warnings :
  //! It is not forbidden to create a circle with theRadius = 0.0  Raises ConstructionError if
  //! theRadius < 0.0
  gp_Circ(const gp_Ax2& theA2, const Standard_Real theRadius)
      : pos(theA2),
        radius(theRadius)
  {
    Standard_ConstructionError_Raise_if(theRadius < 0.0,
                                        "gp_Circ() - radius should be positive number");
  }

  //! Changes the main axis of the circle. It is the axis
  //! perpendicular to the plane of the circle.
  //! Raises ConstructionError if the direction of theA1
  //! is parallel to the "XAxis" of the circle.
  void SetAxis(const gp_Ax1& theA1) { pos.SetAxis(theA1); }

  //! Changes the "Location" point (center) of the circle.
  void SetLocation(const gp_Pnt& theP) { pos.SetLocation(theP); }

  //! Changes the position of the circle.
  void SetPosition(const gp_Ax2& theA2) { pos = theA2; }

  //! Modifies the radius of this circle.
  //! Warning. This class does not prevent the creation of a circle where theRadius is null.
  //! Exceptions
  //! Standard_ConstructionError if theRadius is negative.
  void SetRadius(const Standard_Real theRadius)
  {
    Standard_ConstructionError_Raise_if(theRadius < 0.0,
                                        "gp_Circ::SetRadius() - radius should be positive number");
    radius = theRadius;
  }

  //! Computes the area of the circle.
  Standard_Real Area() const { return M_PI * radius * radius; }

  //! Returns the main axis of the circle.
  //! It is the axis perpendicular to the plane of the circle,
  //! passing through the "Location" point (center) of the circle.
  const gp_Ax1& Axis() const { return pos.Axis(); }

  //! Computes the circumference of the circle.
  Standard_Real Length() const { return 2. * M_PI * radius; }

  //! Returns the center of the circle. It is the
  //! "Location" point of the local coordinate system
  //! of the circle
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the position of the circle.
  //! It is the local coordinate system of the circle.
  const gp_Ax2& Position() const { return pos; }

  //! Returns the radius of this circle.
  Standard_Real Radius() const { return radius; }

  //! Returns the "XAxis" of the circle.
  //! This axis is perpendicular to the axis of the conic.
  //! This axis and the "Yaxis" define the plane of the conic.
  gp_Ax1 XAxis() const { return gp_Ax1(pos.Location(), pos.XDirection()); }

  //! Returns the "YAxis" of the circle.
  //! This axis and the "Xaxis" define the plane of the conic.
  //! The "YAxis" is perpendicular to the "Xaxis".
  gp_Ax1 YAxis() const { return gp_Ax1(pos.Location(), pos.YDirection()); }

  //! Computes the minimum of distance between the point theP and
  //! any point on the circumference of the circle.
  Standard_Real Distance(const gp_Pnt& theP) const { return sqrt(SquareDistance(theP)); }

  //! Computes the square distance between <me> and the point theP.
  Standard_Real SquareDistance(const gp_Pnt& theP) const
  {
    gp_Vec        aV(Location(), theP);
    Standard_Real aX  = aV.Dot(pos.XDirection());
    Standard_Real anY = aV.Dot(pos.YDirection());
    Standard_Real aZ  = aV.Dot(pos.Direction());
    Standard_Real aT  = sqrt(aX * aX + anY * anY) - radius;
    return (aT * aT + aZ * aZ);
  }

  //! Returns True if the point theP is on the circumference.
  //! The distance between <me> and <theP> must be lower or
  //! equal to theLinearTolerance.
  Standard_Boolean Contains(const gp_Pnt& theP, const Standard_Real theLinearTolerance) const
  {
    return Distance(theP) <= theLinearTolerance;
  }

  Standard_EXPORT void Mirror(const gp_Pnt& theP);

  //! Performs the symmetrical transformation of a circle
  //! with respect to the point theP which is the center of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Circ Mirrored(const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror(const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of a circle with
  //! respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Circ Mirrored(const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror(const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of a circle with respect
  //! to a plane. The axis placement theA2 locates the plane of the
  //! of the symmetry : (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Circ Mirrored(const gp_Ax2& theA2) const;

  void Rotate(const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate(theA1, theAng); }

  //! Rotates a circle. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Circ Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Circ aC = *this;
    aC.pos.Rotate(theA1, theAng);
    return aC;
  }

  void Scale(const gp_Pnt& theP, const Standard_Real theS);

  //! Scales a circle. theS is the scaling value.
  //! Warnings :
  //! If theS is negative the radius stay positive but
  //! the "XAxis" and the "YAxis" are  reversed as for
  //! an ellipse.
  Standard_NODISCARD gp_Circ Scaled(const gp_Pnt& theP, const Standard_Real theS) const;

  void Transform(const gp_Trsf& theT);

  //! Transforms a circle with the transformation theT from class Trsf.
  Standard_NODISCARD gp_Circ Transformed(const gp_Trsf& theT) const;

  void Translate(const gp_Vec& theV) { pos.Translate(theV); }

  //! Translates a circle in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Circ Translated(const gp_Vec& theV) const
  {
    gp_Circ aC = *this;
    aC.pos.Translate(theV);
    return aC;
  }

  void Translate(const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate(theP1, theP2); }

  //! Translates a circle from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Circ Translated(const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Circ aC = *this;
    aC.pos.Translate(theP1, theP2);
    return aC;
  }

private:
  gp_Ax2        pos;
  Standard_Real radius;
};

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
inline void gp_Circ::Scale(const gp_Pnt& theP, const Standard_Real theS)
{
  radius *= theS;
  if (radius < 0)
  {
    radius = -radius;
  }
  pos.Scale(theP, theS);
}

// =======================================================================
// function : Scaled
// purpose  :
// =======================================================================
inline gp_Circ gp_Circ::Scaled(const gp_Pnt& theP, const Standard_Real theS) const
{
  gp_Circ aC = *this;
  aC.radius *= theS;
  if (aC.radius < 0)
  {
    aC.radius = -aC.radius;
  }
  aC.pos.Scale(theP, theS);
  return aC;
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
inline void gp_Circ::Transform(const gp_Trsf& theT)
{
  radius *= theT.ScaleFactor();
  if (radius < 0)
  {
    radius = -radius;
  }
  pos.Transform(theT);
}

// =======================================================================
// function : Transformed
// purpose  :
// =======================================================================
inline gp_Circ gp_Circ::Transformed(const gp_Trsf& theT) const
{
  gp_Circ aC = *this;
  aC.radius *= theT.ScaleFactor();
  if (aC.radius < 0)
  {
    aC.radius = -aC.radius;
  }
  aC.pos.Transform(theT);
  return aC;
}

#endif // _gp_Circ_HeaderFile
