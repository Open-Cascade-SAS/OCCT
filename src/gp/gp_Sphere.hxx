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

#ifndef _gp_Sphere_HeaderFile
#define _gp_Sphere_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes a sphere.
//! A sphere is defined by its radius and positioned in space
//! with a coordinate system (a gp_Ax3 object). The origin of
//! the coordinate system is the center of the sphere. This
//! coordinate system is the "local coordinate system" of the sphere.
//! Note: when a gp_Sphere sphere is converted into a
//! Geom_SphericalSurface sphere, some implicit
//! properties of its local coordinate system are used explicitly:
//! -   its origin, "X Direction", "Y Direction" and "main
//! Direction" are used directly to define the parametric
//! directions on the sphere and the origin of the parameters,
//! -   its implicit orientation (right-handed or left-handed)
//! gives the orientation (direct, indirect) to the
//! Geom_SphericalSurface sphere.
//! See Also
//! gce_MakeSphere which provides functions for more
//! complex sphere constructions
//! Geom_SphericalSurface which provides additional
//! functions for constructing spheres and works, in
//! particular, with the parametric equations of spheres.
class gp_Sphere 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates an indefinite sphere.
  gp_Sphere()
  : radius (RealLast())
  {}

  //! Constructs a sphere with radius theRadius, centered on the origin
  //! of theA3.  theA3 is the local coordinate system of the sphere.
  //! Warnings :
  //! It is not forbidden to create a sphere with null radius.
  //! Raises ConstructionError if theRadius < 0.0
  gp_Sphere (const gp_Ax3& theA3, const Standard_Real theRadius)
  : pos (theA3),
    radius (theRadius)
  {
    Standard_ConstructionError_Raise_if (theRadius < 0.0, "gp_Sphere() - radius should be >= 0");
  }

  //! Changes the center of the sphere.
  void SetLocation (const gp_Pnt& theLoc) { pos.SetLocation (theLoc); }

  //! Changes the local coordinate system of the sphere.
  void SetPosition (const gp_Ax3& theA3) { pos = theA3; }

  //! Assigns theR the radius of the Sphere.
  //! Warnings :
  //! It is not forbidden to create a sphere with null radius.
  //! Raises ConstructionError if theR < 0.0
  void SetRadius (const Standard_Real theR)
  {
    Standard_ConstructionError_Raise_if (theR < 0.0, "gp_Sphere::SetRadius() - radius should be >= 0");
    radius = theR;
  }

  //! Computes the area of the sphere.
  Standard_Real Area() const
  {
    return 4.0 * M_PI * radius * radius;
  }

  //! Computes the coefficients of the implicit equation of the quadric
  //! in the absolute cartesian coordinates system :
  //! @code
  //! theA1.X**2 + theA2.Y**2 + theA3.Z**2 + 2.(theB1.X.Y + theB2.X.Z + theB3.Y.Z) +
  //! 2.(theC1.X + theC2.Y + theC3.Z) + theD = 0.0
  //! @endcode
  Standard_EXPORT void Coefficients (Standard_Real& theA1, Standard_Real& theA2, Standard_Real& theA3,
                                     Standard_Real& theB1, Standard_Real& theB2, Standard_Real& theB3,
                                     Standard_Real& theC1, Standard_Real& theC2, Standard_Real& theC3, Standard_Real& theD) const;

  //! Reverses the   U   parametrization of   the sphere
  //! reversing the YAxis.
  void UReverse() { pos.YReverse(); }

  //! Reverses the   V   parametrization of   the  sphere
  //! reversing the ZAxis.
  void VReverse() { pos.ZReverse(); }

  //! Returns true if the local coordinate system of this sphere
  //! is right-handed.
  Standard_Boolean Direct() const { return pos.Direct(); }

  //! --- Purpose ;
  //! Returns the center of the sphere.
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the local coordinates system of the sphere.
  const gp_Ax3& Position() const { return pos; }

  //! Returns the radius of the sphere.
  Standard_Real Radius() const { return radius; }

  //! Computes the volume of the sphere
  Standard_Real Volume() const
  {
    return (4.0 * M_PI * radius * radius * radius) / 3.0;
  }

  //! Returns the axis X of the sphere.
  gp_Ax1 XAxis() const
  {
    return gp_Ax1 (pos.Location(), pos.XDirection());
  }

  //! Returns the axis Y of the sphere.
  gp_Ax1 YAxis() const
  {
    return gp_Ax1 (pos.Location(), pos.YDirection());
  }

  Standard_EXPORT void Mirror (const gp_Pnt& theP);

  //! Performs the symmetrical transformation of a sphere
  //! with respect to the point theP which is the center of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Sphere Mirrored (const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of a sphere with
  //! respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Sphere Mirrored (const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror (const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of a sphere with respect
  //! to a plane. The axis placement theA2 locates the plane of the
  //! of the symmetry : (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Sphere Mirrored (const gp_Ax2& theA2) const;

  void Rotate (const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate (theA1, theAng); }

  //! Rotates a sphere. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Sphere Rotated (const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Sphere aC = *this;
    aC.pos.Rotate (theA1, theAng);
    return aC;
  }

  void Scale (const gp_Pnt& theP, const Standard_Real theS);

  //! Scales a sphere. theS is the scaling value.
  //! The absolute value of S is used to scale the sphere
  Standard_NODISCARD gp_Sphere Scaled (const gp_Pnt& theP, const Standard_Real theS) const;

  void Transform (const gp_Trsf& theT);

  //! Transforms a sphere with the transformation theT from class Trsf.
  Standard_NODISCARD gp_Sphere Transformed (const gp_Trsf& theT) const;

  void Translate (const gp_Vec& theV) { pos.Translate (theV); }

  //! Translates a sphere in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Sphere Translated (const gp_Vec& theV) const
  {
    gp_Sphere aC = *this;
    aC.pos.Translate (theV);
    return aC;
  }

  void Translate (const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate (theP1, theP2); }

  //! Translates a sphere from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Sphere Translated (const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Sphere aC = *this;
    aC.pos.Translate (theP1, theP2);
    return aC;
  }

private:

  gp_Ax3 pos;
  Standard_Real radius;

};

//=======================================================================
//function : Scale
// purpose :
//=======================================================================
inline void gp_Sphere::Scale (const gp_Pnt& theP, const Standard_Real theS)
{
  pos.Scale (theP, theS);
  radius *= theS;
  if (radius < 0)
  {
    radius = -radius;
  }
}

//=======================================================================
//function : Scaled
// purpose :
//=======================================================================
inline gp_Sphere gp_Sphere::Scaled (const gp_Pnt& theP, const Standard_Real theS) const
{
  gp_Sphere aC = *this;
  aC.pos.Scale (theP, theS);
  aC.radius *= theS;
  if (aC.radius < 0)
  {
    aC.radius = -aC.radius;
  }
  return aC;
}

//=======================================================================
//function : Transform
// purpose :
//=======================================================================
inline void gp_Sphere::Transform (const gp_Trsf& theT)
{
  pos.Transform(theT);
  radius *= theT.ScaleFactor();
  if (radius < 0)
  {
    radius = -radius;
  }
}

//=======================================================================
//function : Transformed
// purpose :
//=======================================================================
inline gp_Sphere gp_Sphere::Transformed (const gp_Trsf& theT) const
{
  gp_Sphere aC = *this;
  aC.pos.Transform (theT);
  aC.radius *= theT.ScaleFactor();
  if (aC.radius < 0)
  {
    aC.radius = -aC.radius;
  }
  return aC;
}

#endif // _gp_Sphere_HeaderFile
