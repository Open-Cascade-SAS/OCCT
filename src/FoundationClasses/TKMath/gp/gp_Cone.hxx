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

#ifndef _gp_Cone_HeaderFile
#define _gp_Cone_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

//! Defines an infinite conical surface.
//! A cone is defined by its half-angle (can be negative) at the apex and
//! positioned in space with a coordinate system (a gp_Ax3
//! object) and a "reference radius" where:
//! -   the "main Axis" of the coordinate system is the axis of   revolution of the cone,
//! -   the plane defined by the origin, the "X Direction" and
//! the "Y Direction" of the coordinate system is the
//! reference plane of the cone; the intersection of the
//! cone with this reference plane is a circle of radius
//! equal to the reference radius,
//! if the half-angle is positive, the apex of the cone is on
//! the negative side of the "main Axis" of the coordinate
//! system. If the half-angle is negative, the apex is on the   positive side.
//! This coordinate system is the "local coordinate system" of the cone.
//! Note: when a gp_Cone cone is converted into a
//! Geom_ConicalSurface cone, some implicit properties of
//! its local coordinate system are used explicitly:
//! -   its origin, "X Direction", "Y Direction" and "main
//! Direction" are used directly to define the parametric
//! directions on the cone and the origin of the parameters,
//! -   its implicit orientation (right-handed or left-handed)
//! gives the orientation (direct or indirect) of the
//! Geom_ConicalSurface cone.
//! See Also
//! gce_MakeCone which provides functions for more
//! complex cone constructions
//! Geom_ConicalSurface which provides additional
//! functions for constructing cones and works, in particular,
//! with the parametric equations of cones gp_Ax3
class gp_Cone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an indefinite Cone.
  gp_Cone()
      : radius(RealLast()),
        semiAngle(M_PI * 0.25)
  {
  }

  //! Creates an infinite conical surface. theA3 locates the cone
  //! in the space and defines the reference plane of the surface.
  //! Ang is the conical surface semi-angle. Its absolute value is in range
  //! ]0, PI/2[.
  //! theRadius is the radius of the circle in the reference plane of
  //! the cone.
  //! theRaises ConstructionError
  //! * if theRadius is lower than 0.0
  //! * Abs(theAng) < Resolution from gp  or Abs(theAng) >= (PI/2) - Resolution.
  gp_Cone(const gp_Ax3& theA3, const Standard_Real theAng, const Standard_Real theRadius);

  //! Changes the symmetry axis of the cone.  Raises ConstructionError
  //! the direction of theA1 is parallel to the "XDirection"
  //! of the coordinate system of the cone.
  void SetAxis(const gp_Ax1& theA1) { pos.SetAxis(theA1); }

  //! Changes the location of the cone.
  void SetLocation(const gp_Pnt& theLoc) { pos.SetLocation(theLoc); }

  //! Changes the local coordinate system of the cone.
  //! This coordinate system defines the reference plane of the cone.
  void SetPosition(const gp_Ax3& theA3) { pos = theA3; }

  //! Changes the radius of the cone in the reference plane of
  //! the cone.
  //! Raised if theR < 0.0
  void SetRadius(const Standard_Real theR)
  {
    Standard_ConstructionError_Raise_if(theR < 0.0,
                                        "gp_Cone::SetRadius() - radius should be positive number");
    radius = theR;
  }

  //! Changes the semi-angle of the cone.
  //! Semi-angle can be negative. Its absolute value
  //! Abs(theAng) is in range ]0,PI/2[.
  //! Raises ConstructionError if Abs(theAng) < Resolution from gp or Abs(theAng) >= PI/2 -
  //! Resolution
  void SetSemiAngle(const Standard_Real theAng);

  //! Computes the cone's top. The Apex of the cone is on the
  //! negative side of the symmetry axis of the cone.
  gp_Pnt Apex() const
  {
    gp_XYZ aCoord = pos.Direction().XYZ();
    aCoord.Multiply(-radius / Tan(semiAngle));
    aCoord.Add(pos.Location().XYZ());
    return gp_Pnt(aCoord);
  }

  //! Reverses the   U   parametrization of   the  cone
  //! reversing the YAxis.
  void UReverse() { pos.YReverse(); }

  //! Reverses the   V   parametrization of   the  cone  reversing the ZAxis.
  void VReverse()
  {
    pos.ZReverse();
    semiAngle = -semiAngle;
  }

  //! Returns true if the local coordinate system of this cone is right-handed.
  Standard_Boolean Direct() const { return pos.Direct(); }

  //! returns the symmetry axis of the cone.
  const gp_Ax1& Axis() const { return pos.Axis(); }

  //! Computes the coefficients of the implicit equation of the quadric
  //! in the absolute cartesian coordinates system :
  //! theA1.X**2 + theA2.Y**2 + theA3.Z**2 + 2.(theB1.X.Y + theB2.X.Z + theB3.Y.Z) +
  //! 2.(theC1.X + theC2.Y + theC3.Z) + theD = 0.0
  Standard_EXPORT void Coefficients(Standard_Real& theA1,
                                    Standard_Real& theA2,
                                    Standard_Real& theA3,
                                    Standard_Real& theB1,
                                    Standard_Real& theB2,
                                    Standard_Real& theB3,
                                    Standard_Real& theC1,
                                    Standard_Real& theC2,
                                    Standard_Real& theC3,
                                    Standard_Real& theD) const;

  //! returns the "Location" point of the cone.
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the local coordinates system of the cone.
  const gp_Ax3& Position() const { return pos; }

  //! Returns the radius of the cone in the reference plane.
  Standard_Real RefRadius() const { return radius; }

  //! Returns the half-angle at the apex of this cone.
  //! Attention! Semi-angle can be negative.
  Standard_Real SemiAngle() const { return semiAngle; }

  //! Returns the XAxis of the reference plane.
  gp_Ax1 XAxis() const { return gp_Ax1(pos.Location(), pos.XDirection()); }

  //! Returns the YAxis of the reference plane.
  gp_Ax1 YAxis() const { return gp_Ax1(pos.Location(), pos.YDirection()); }

  Standard_EXPORT void Mirror(const gp_Pnt& theP);

  //! Performs the symmetrical transformation of a cone
  //! with respect to the point theP which is the center of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Cone Mirrored(const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror(const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of a cone with
  //! respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Cone Mirrored(const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror(const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of a cone with respect
  //! to a plane. The axis placement theA2 locates the plane of the
  //! of the symmetry : (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Cone Mirrored(const gp_Ax2& theA2) const;

  void Rotate(const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate(theA1, theAng); }

  //! Rotates a cone. theA1 is the axis of the rotation.
  //! Ang is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Cone Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Cone aCone = *this;
    aCone.pos.Rotate(theA1, theAng);
    return aCone;
  }

  void Scale(const gp_Pnt& theP, const Standard_Real theS);

  //! Scales a cone. theS is the scaling value.
  //! The absolute value of theS is used to scale the cone
  Standard_NODISCARD gp_Cone Scaled(const gp_Pnt& theP, const Standard_Real theS) const;

  void Transform(const gp_Trsf& theT);

  //! Transforms a cone with the transformation theT from class Trsf.
  Standard_NODISCARD gp_Cone Transformed(const gp_Trsf& theT) const;

  void Translate(const gp_Vec& theV) { pos.Translate(theV); }

  //! Translates a cone in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Cone Translated(const gp_Vec& theV) const
  {
    gp_Cone aCone = *this;
    aCone.pos.Translate(theV);
    return aCone;
  }

  void Translate(const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate(theP1, theP2); }

  //! Translates a cone from the point P1 to the point P2.
  Standard_NODISCARD gp_Cone Translated(const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Cone aCone = *this;
    aCone.pos.Translate(theP1, theP2);
    return aCone;
  }

private:
  gp_Ax3        pos;
  Standard_Real radius;
  Standard_Real semiAngle;
};

// =======================================================================
// function : gp_Cone
// purpose  :
// =======================================================================
inline gp_Cone::gp_Cone(const gp_Ax3&       theA3,
                        const Standard_Real theAng,
                        const Standard_Real theRadius)
    : pos(theA3),
      radius(theRadius),
      semiAngle(theAng)
{
  Standard_Real aVal = theAng;
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  Standard_ConstructionError_Raise_if(theRadius < 0. || aVal <= gp::Resolution()
                                        || M_PI * 0.5 - aVal <= gp::Resolution(),
                                      "gp_Cone() - invalid construction parameters");
}

// =======================================================================
// function : SetSemiAngle
// purpose  :
// =======================================================================
inline void gp_Cone::SetSemiAngle(const Standard_Real theAng)
{
  Standard_Real aVal = theAng;
  if (aVal < 0)
  {
    aVal = -aVal;
  }
  Standard_ConstructionError_Raise_if(aVal <= gp::Resolution()
                                        || M_PI * 0.5 - aVal <= gp::Resolution(),
                                      "gp_Cone::SetSemiAngle() - invalid angle range");
  semiAngle = theAng;
}

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
inline void gp_Cone::Scale(const gp_Pnt& theP, const Standard_Real theS)
{
  pos.Scale(theP, theS);
  radius *= theS;
  if (radius < 0)
  {
    radius = -radius;
  }
}

// =======================================================================
// function : Scaled
// purpose  :
// =======================================================================
inline gp_Cone gp_Cone::Scaled(const gp_Pnt& theP, const Standard_Real theS) const
{
  gp_Cone aC = *this;
  aC.pos.Scale(theP, theS);
  aC.radius *= theS;
  if (aC.radius < 0)
  {
    aC.radius = -aC.radius;
  }
  return aC;
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
inline void gp_Cone::Transform(const gp_Trsf& theT)
{
  pos.Transform(theT);
  radius *= theT.ScaleFactor();
  if (radius < 0)
  {
    radius = -radius;
  }
}

// =======================================================================
// function : Transformed
// purpose  :
// =======================================================================
inline gp_Cone gp_Cone::Transformed(const gp_Trsf& theT) const
{
  gp_Cone aC = *this;
  aC.pos.Transform(theT);
  aC.radius *= theT.ScaleFactor();
  if (aC.radius < 0)
  {
    aC.radius = -aC.radius;
  }
  return aC;
}

#endif // _gp_Cone_HeaderFile
