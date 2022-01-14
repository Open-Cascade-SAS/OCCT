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

#ifndef _gp_Torus_HeaderFile
#define _gp_Torus_HeaderFile

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColStd_Array1OfReal.hxx>

//! Describes a torus.
//! A torus is defined by its major and minor radii and
//! positioned in space with a coordinate system (a gp_Ax3
//! object) as follows:
//! -   The origin of the coordinate system is the center of the torus;
//! -   The surface is obtained by rotating a circle of radius
//! equal to the minor radius of the torus about the "main
//! Direction" of the coordinate system. This circle is
//! located in the plane defined by the origin, the "X
//! Direction" and the "main Direction" of the coordinate
//! system. It is centered on the "X Axis" of this coordinate
//! system, and located at a distance, from the origin of
//! this coordinate system, equal to the major radius of the   torus;
//! -   The "X Direction" and "Y Direction" define the
//! reference plane of the torus.
//! The coordinate system described above is the "local
//! coordinate system" of the torus.
//! Note: when a gp_Torus torus is converted into a
//! Geom_ToroidalSurface torus, some implicit properties
//! of its local coordinate system are used explicitly:
//! -   its origin, "X Direction", "Y Direction" and "main
//! Direction" are used directly to define the parametric
//! directions on the torus and the origin of the parameters,
//! -   its implicit orientation (right-handed or left-handed)
//! gives the orientation (direct, indirect) to the
//! Geom_ToroidalSurface torus.
//! See Also
//! gce_MakeTorus which provides functions for more
//! complex torus constructions
//! Geom_ToroidalSurface which provides additional
//! functions for constructing tori and works, in particular,
//! with the parametric equations of tori.
class gp_Torus 
{
public:

  DEFINE_STANDARD_ALLOC

  //! creates an indefinite Torus.
  gp_Torus()
  : majorRadius (RealLast()),
    minorRadius (RealSmall())
  {}

  //! a torus centered on the origin of coordinate system
  //! theA3, with major radius theMajorRadius and minor radius
  //! theMinorRadius, and with the reference plane defined
  //! by the origin, the "X Direction" and the "Y Direction" of theA3.
  //! Warnings :
  //! It is not forbidden to create a torus with
  //! theMajorRadius = theMinorRadius = 0.0
  //! Raises ConstructionError if theMinorRadius < 0.0 or if theMajorRadius < 0.0
  gp_Torus (const gp_Ax3& theA3, const Standard_Real theMajorRadius, const Standard_Real theMinorRadius)
  : pos (theA3),
    majorRadius (theMajorRadius),
    minorRadius (theMinorRadius)
  {
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0 || theMajorRadius < 0.0,
      "gp_Torus() - invalid construction parameters");
  }

  //! Modifies this torus, by redefining its local coordinate
  //! system so that:
  //! -   its origin and "main Direction" become those of the
  //! axis theA1 (the "X Direction" and "Y Direction" are then recomputed).
  //! Raises ConstructionError if the direction of theA1 is parallel to the "XDirection"
  //! of the coordinate system of the toroidal surface.
  void SetAxis (const gp_Ax1& theA1) { pos.SetAxis (theA1); }

  //! Changes the location of the torus.
  void SetLocation (const gp_Pnt& theLoc) { pos.SetLocation (theLoc); }

  //! Assigns value to the major radius  of this torus.
  //! Raises ConstructionError if theMajorRadius - MinorRadius <= Resolution()
  void SetMajorRadius (const Standard_Real theMajorRadius)
  {
    Standard_ConstructionError_Raise_if (theMajorRadius - minorRadius <= gp::Resolution(),
                                         "gp_Torus::SetMajorRadius() - invalid input parameters");
    majorRadius = theMajorRadius;
  }

  //! Assigns value to the  minor radius of this torus.
  //! Raises ConstructionError if theMinorRadius < 0.0 or if
  //! MajorRadius - theMinorRadius <= Resolution from gp.
  void SetMinorRadius (const Standard_Real theMinorRadius)
  {
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0 || majorRadius - theMinorRadius <= gp::Resolution(),
                                         "gp_Torus::SetMinorRadius() - invalid input parameters");
    minorRadius = theMinorRadius;
  }

  //! Changes the local coordinate system of the surface.
  void SetPosition (const gp_Ax3& theA3) { pos = theA3; }

  //! Computes the area of the torus.
  Standard_Real Area() const { return 4.0 * M_PI * M_PI * minorRadius * majorRadius; }

  //! Reverses the   U   parametrization of   the  torus
  //! reversing the YAxis.
  void UReverse() { pos.YReverse(); }

  //! Reverses the   V   parametrization of   the  torus
  //! reversing the ZAxis.
  void VReverse() { pos.ZReverse(); }

  //! returns true if the Ax3, the local coordinate system of this torus, is right handed.
  Standard_Boolean Direct() const { return pos.Direct(); }

  //! returns the symmetry axis of the torus.
  const gp_Ax1& Axis() const { return pos.Axis(); }

  //! Computes the coefficients of the implicit equation of the surface
  //! in the absolute Cartesian coordinate system:
  //! @code
  //!     Coef(1) * X^4 + Coef(2) * Y^4 + Coef(3) * Z^4 +
  //!     Coef(4) * X^3 * Y + Coef(5) * X^3 * Z + Coef(6) * Y^3 * X +
  //!     Coef(7) * Y^3 * Z + Coef(8) * Z^3 * X + Coef(9) * Z^3 * Y +
  //!     Coef(10) * X^2 * Y^2 + Coef(11) * X^2 * Z^2 +
  //!     Coef(12) * Y^2 * Z^2 + Coef(13) * X^2 * Y * Z +
  //!     Coef(14) * X * Y^2 * Z + Coef(15) * X * Y * Z^2 +
  //!     Coef(16) * X^3 + Coef(17) * Y^3 + Coef(18) * Z^3 + 
  //!     Coef(19) * X^2 * Y + Coef(20) * X^2 * Z + Coef(21) * Y^2 * X +
  //!     Coef(22) * Y^2 * Z + Coef(23) * Z^2 * X + Coef(24) * Z^2 * Y +
  //!     Coef(25) * X * Y * Z +
  //!     Coef(26) * X^2 + Coef(27) * Y^2 + Coef(28) * Z^2 +
  //!     Coef(29) * X * Y + Coef(30) * X * Z + Coef(31) * Y * Z +
  //!     Coef(32) * X + Coef(33) * Y + Coef(34) *  Z + 
  //!     Coef(35) = 0.0
  //! @endcode
  //! Raises DimensionError if the length of theCoef is lower than 35.
  Standard_EXPORT void Coefficients (TColStd_Array1OfReal& theCoef) const;

  //! Returns the Torus's location.
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the local coordinates system of the torus.
  const gp_Ax3& Position() const { return pos; }

  //! returns the major radius of the torus.
  Standard_Real MajorRadius() const { return majorRadius; }

  //! returns the minor radius of the torus.
  Standard_Real MinorRadius() const { return minorRadius; }

  //! Computes the volume of the torus.
  Standard_Real Volume() const
  {
    return (M_PI * minorRadius * minorRadius) * (2.0 * M_PI * majorRadius);
  }

  //! returns the axis X of the torus.
  gp_Ax1 XAxis() const
  {
    return gp_Ax1 (pos.Location(), pos.XDirection());
  }

  //! returns the axis Y of the torus.
  gp_Ax1 YAxis() const
  {
    return gp_Ax1 (pos.Location(), pos.YDirection());
  }

  Standard_EXPORT void Mirror (const gp_Pnt& theP);

  //! Performs the symmetrical transformation of a torus
  //! with respect to the point theP which is the center of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Torus Mirrored (const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of a torus with
  //! respect to an axis placement which is the axis of the
  //! symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Torus Mirrored (const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror (const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of a torus with respect
  //! to a plane. The axis placement theA2 locates the plane of the
  //! of the symmetry : (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Torus Mirrored (const gp_Ax2& theA2) const;

  void Rotate (const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate (theA1, theAng); }

  //! Rotates a torus. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Torus Rotated (const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Torus aC = *this;
    aC.pos.Rotate (theA1, theAng);
    return aC;
  }

  void Scale (const gp_Pnt& theP, const Standard_Real theS);

  //! Scales a torus. S is the scaling value.
  //! The absolute value of S is used to scale the torus
  Standard_NODISCARD gp_Torus Scaled (const gp_Pnt& theP, const Standard_Real theS) const;

  void Transform (const gp_Trsf& theT);

  //! Transforms a torus with the transformation theT from class Trsf.
  Standard_NODISCARD gp_Torus Transformed (const gp_Trsf& theT) const;

  void Translate (const gp_Vec& theV) { pos.Translate (theV); }

  //! Translates a torus in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Torus Translated (const gp_Vec& theV) const
  {
    gp_Torus aC = *this;
    aC.pos.Translate (theV);
    return aC;
  }

  void Translate (const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate (theP1, theP2); }

  //! Translates a torus from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Torus Translated (const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Torus aC = *this;
    aC.pos.Translate (theP1, theP2);
    return aC;
  }

private:

  gp_Ax3 pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;

};

//=======================================================================
//function : Scale
// purpose :
//=======================================================================
inline void gp_Torus::Scale (const gp_Pnt& theP,
                             const Standard_Real theS)
{
  pos.Scale (theP, theS);
  Standard_Real s = theS;
  if (s < 0)
  {
    s = -s;
  }
  majorRadius *= s;
  minorRadius *= s;
}

//=======================================================================
//function : Scaled
// purpose :
//=======================================================================
inline gp_Torus gp_Torus::Scaled (const gp_Pnt& theP,
                                  const Standard_Real theS) const
{
  gp_Torus aC = *this;
  aC.pos.Scale (theP, theS);
  aC.majorRadius *= theS;
  if (aC.majorRadius < 0)
  {
    aC.majorRadius = -aC.majorRadius;
  }
  aC.minorRadius *= theS;
  if (aC.minorRadius < 0)
  {
    aC.minorRadius = -aC.minorRadius;
  }
  return aC;
}

//=======================================================================
//function : Transform
// purpose :
//=======================================================================
inline void gp_Torus::Transform (const gp_Trsf& theT)
{
  pos.Transform (theT);
  Standard_Real aT = theT.ScaleFactor();
  if (aT < 0)
  {
    aT = -aT;
  }
  minorRadius *= aT;
  majorRadius *= aT;
}

//=======================================================================
//function : Transformed
// purpose :
//=======================================================================
inline gp_Torus gp_Torus::Transformed (const gp_Trsf& theT) const
{
  gp_Torus aC = *this;
  aC.pos.Transform (theT);
  aC.majorRadius *= theT.ScaleFactor();
  if (aC.majorRadius < 0)
  {
    aC.majorRadius = -aC.majorRadius;
  }
  aC.minorRadius *= theT.ScaleFactor();
  if (aC.minorRadius < 0)
  {
    aC.minorRadius = -aC.minorRadius;
  }
  return aC;
}

#endif // _gp_Torus_HeaderFile
