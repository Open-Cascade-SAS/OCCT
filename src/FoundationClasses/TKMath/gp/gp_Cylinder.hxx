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

#ifndef _gp_Cylinder_HeaderFile
#define _gp_Cylinder_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>

//! Describes an infinite cylindrical surface.
//! A cylinder is defined by its radius and positioned in space
//! with a coordinate system (a gp_Ax3 object), the "main
//! Axis" of which is the axis of the cylinder. This coordinate
//! system is the "local coordinate system" of the cylinder.
//! Note: when a gp_Cylinder cylinder is converted into a
//! Geom_CylindricalSurface cylinder, some implicit
//! properties of its local coordinate system are used explicitly:
//! -   its origin, "X Direction", "Y Direction" and "main
//! Direction" are used directly to define the parametric
//! directions on the cylinder and the origin of the parameters,
//! -   its implicit orientation (right-handed or left-handed)
//! gives an orientation (direct or indirect) to the
//! Geom_CylindricalSurface cylinder.
//! See Also
//! gce_MakeCylinder which provides functions for more
//! complex cylinder constructions
//! Geom_CylindricalSurface which provides additional
//! functions for constructing cylinders and works, in
//! particular, with the parametric equations of cylinders gp_Ax3
class gp_Cylinder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a indefinite cylinder.
  gp_Cylinder() { radius = RealLast(); }

  //! Creates a cylinder of radius Radius, whose axis is the "main
  //! Axis" of theA3. theA3 is the local coordinate system of the cylinder.   Raises
  //! ConstructionErrord if theRadius < 0.0
  gp_Cylinder(const gp_Ax3& theA3, const Standard_Real theRadius)
      : pos(theA3),
        radius(theRadius)
  {
    Standard_ConstructionError_Raise_if(theRadius < 0.0,
                                        "gp_Cylinder() - radius should be positive number");
  }

  //! Changes the symmetry axis of the cylinder. Raises ConstructionError if the direction of theA1
  //! is parallel to the "XDirection" of the coordinate system of the cylinder.
  void SetAxis(const gp_Ax1& theA1) { pos.SetAxis(theA1); }

  //! Changes the location of the surface.
  void SetLocation(const gp_Pnt& theLoc) { pos.SetLocation(theLoc); }

  //! Change the local coordinate system of the surface.
  void SetPosition(const gp_Ax3& theA3) { pos = theA3; }

  //! Modifies the radius of this cylinder.
  //! Exceptions
  //! Standard_ConstructionError if theR is negative.
  void SetRadius(const Standard_Real theR)
  {
    Standard_ConstructionError_Raise_if(
      theR < 0.0,
      "gp_Cylinder::SetRadius() - radius should be positive number");
    radius = theR;
  }

  //! Reverses the   U   parametrization of   the cylinder
  //! reversing the YAxis.
  void UReverse() { pos.YReverse(); }

  //! Reverses the   V   parametrization of   the  plane
  //! reversing the Axis.
  void VReverse() { pos.ZReverse(); }

  //! Returns true if the local coordinate system of this cylinder is right-handed.
  Standard_Boolean Direct() const { return pos.Direct(); }

  //! Returns the symmetry axis of the cylinder.
  const gp_Ax1& Axis() const { return pos.Axis(); }

  //! Computes the coefficients of the implicit equation of the quadric
  //! in the absolute cartesian coordinate system :
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

  //! Returns the "Location" point of the cylinder.
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the local coordinate system of the cylinder.
  const gp_Ax3& Position() const { return pos; }

  //! Returns the radius of the cylinder.
  Standard_Real Radius() const { return radius; }

  //! Returns the axis X of the cylinder.
  gp_Ax1 XAxis() const { return gp_Ax1(pos.Location(), pos.XDirection()); }

  //! Returns the axis Y of the cylinder.
  gp_Ax1 YAxis() const { return gp_Ax1(pos.Location(), pos.YDirection()); }

  Standard_EXPORT void Mirror(const gp_Pnt& theP);

  //! Performs the symmetrical transformation of a cylinder
  //! with respect to the point theP which is the center of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Cylinder Mirrored(const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror(const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of a cylinder with
  //! respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Cylinder Mirrored(const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror(const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of a cylinder with respect
  //! to a plane. The axis placement theA2 locates the plane of the
  //! of the symmetry : (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Cylinder Mirrored(const gp_Ax2& theA2) const;

  void Rotate(const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate(theA1, theAng); }

  //! Rotates a cylinder. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Cylinder Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Cylinder aCyl = *this;
    aCyl.pos.Rotate(theA1, theAng);
    return aCyl;
  }

  void Scale(const gp_Pnt& theP, const Standard_Real theS);

  //! Scales a cylinder. theS is the scaling value.
  //! The absolute value of theS is used to scale the cylinder
  Standard_NODISCARD gp_Cylinder Scaled(const gp_Pnt& theP, const Standard_Real theS) const;

  void Transform(const gp_Trsf& theT);

  //! Transforms a cylinder with the transformation theT from class Trsf.
  Standard_NODISCARD gp_Cylinder Transformed(const gp_Trsf& theT) const;

  void Translate(const gp_Vec& theV) { pos.Translate(theV); }

  //! Translates a cylinder in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Cylinder Translated(const gp_Vec& theV) const
  {
    gp_Cylinder aCyl = *this;
    aCyl.pos.Translate(theV);
    return aCyl;
  }

  void Translate(const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate(theP1, theP2); }

  //! Translates a cylinder from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Cylinder Translated(const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Cylinder aCyl = *this;
    aCyl.pos.Translate(theP1, theP2);
    return aCyl;
  }

private:
  gp_Ax3        pos;
  Standard_Real radius;
};

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
inline void gp_Cylinder::Scale(const gp_Pnt& theP, const Standard_Real theS)
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
inline gp_Cylinder gp_Cylinder::Scaled(const gp_Pnt& theP, const Standard_Real theS) const
{
  gp_Cylinder aCyl = *this;
  aCyl.pos.Scale(theP, theS);
  aCyl.radius *= theS;
  if (aCyl.radius < 0)
  {
    aCyl.radius = -aCyl.radius;
  }
  return aCyl;
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
inline void gp_Cylinder::Transform(const gp_Trsf& theT)
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
inline gp_Cylinder gp_Cylinder::Transformed(const gp_Trsf& theT) const
{
  gp_Cylinder aCyl = *this;
  aCyl.pos.Transform(theT);
  aCyl.radius *= theT.ScaleFactor();
  if (aCyl.radius < 0)
  {
    aCyl.radius = -aCyl.radius;
  }
  return aCyl;
}

#endif // _gp_Cylinder_HeaderFile
