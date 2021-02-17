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

#ifndef _gp_Elips_HeaderFile
#define _gp_Elips_HeaderFile

#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes an ellipse in 3D space.
//! An ellipse is defined by its major and minor radii and
//! positioned in space with a coordinate system (a gp_Ax2 object) as follows:
//! -   the origin of the coordinate system is the center of the ellipse,
//! -   its "X Direction" defines the major axis of the ellipse, and
//! - its "Y Direction" defines the minor axis of the ellipse.
//! Together, the origin, "X Direction" and "Y Direction" of
//! this coordinate system define the plane of the ellipse.
//! This coordinate system is the "local coordinate system"
//! of the ellipse. In this coordinate system, the equation of
//! the ellipse is:
//! @code
//! X*X / (MajorRadius**2) + Y*Y / (MinorRadius**2) = 1.0
//! @endcode
//! The "main Direction" of the local coordinate system gives
//! the normal vector to the plane of the ellipse. This vector
//! gives an implicit orientation to the ellipse (definition of the
//! trigonometric sense). We refer to the "main Axis" of the
//! local coordinate system as the "Axis" of the ellipse.
//! See Also
//! gce_MakeElips which provides functions for more
//! complex ellipse constructions
//! Geom_Ellipse which provides additional functions for
//! constructing ellipses and works, in particular, with the
//! parametric equations of ellipses
class gp_Elips 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates an indefinite ellipse.
  gp_Elips()
  : majorRadius (RealLast()),
    minorRadius (RealSmall())
  {}

  //! The major radius of the ellipse is on the "XAxis" and the
  //! minor radius is on the "YAxis" of the ellipse. The "XAxis"
  //! is defined with the "XDirection" of theA2 and the "YAxis" is
  //! defined with the "YDirection" of theA2.
  //! Warnings :
  //! It is not forbidden to create an ellipse with theMajorRadius =
  //! theMinorRadius.
  //! Raises ConstructionError if theMajorRadius < theMinorRadius or theMinorRadius < 0.
  gp_Elips (const gp_Ax2& theA2, const Standard_Real theMajorRadius, const Standard_Real theMinorRadius)
  : pos (theA2),
    majorRadius (theMajorRadius),
    minorRadius (theMinorRadius)
  {
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0 || theMajorRadius < theMinorRadius,
      "gp_Elips() - invalid construction parameters");
  }

  //! Changes the axis normal to the plane of the ellipse.
  //! It modifies the definition of this plane.
  //! The "XAxis" and the "YAxis" are recomputed.
  //! The local coordinate system is redefined so that:
  //! -   its origin and "main Direction" become those of the
  //! axis theA1 (the "X Direction" and "Y Direction" are then
  //! recomputed in the same way as for any gp_Ax2), or
  //! Raises ConstructionError if the direction of theA1
  //! is parallel to the direction of the "XAxis" of the ellipse.
  void SetAxis (const gp_Ax1& theA1) { pos.SetAxis (theA1); }

  //! Modifies this ellipse, by redefining its local coordinate
  //! so that its origin becomes theP.
  void SetLocation (const gp_Pnt& theP) { pos.SetLocation (theP); }

  //! The major radius of the ellipse is on the "XAxis" (major axis)
  //! of the ellipse.
  //! Raises ConstructionError if theMajorRadius < MinorRadius.
  void SetMajorRadius (const Standard_Real theMajorRadius)
  {
    Standard_ConstructionError_Raise_if (theMajorRadius < minorRadius,
       "gp_Elips::SetMajorRadius() - major radius should be greater or equal to minor radius");
    majorRadius = theMajorRadius;
  }

  //! The minor radius of the ellipse is on the "YAxis" (minor axis)
  //! of the ellipse.
  //! Raises ConstructionError if theMinorRadius > MajorRadius or MinorRadius < 0.
  void SetMinorRadius (const Standard_Real theMinorRadius)
  {
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0 || majorRadius < theMinorRadius,
      "gp_Elips::SetMinorRadius() - minor radius should be a positive number lesser or equal to major radius");
    minorRadius = theMinorRadius;
  }

  //! Modifies this ellipse, by redefining its local coordinate
  //! so that it becomes theA2.
  void SetPosition (const gp_Ax2& theA2) { pos = theA2; }

  //! Computes the area of the Ellipse.
  Standard_Real Area() const { return M_PI * majorRadius * minorRadius; }

  //! Computes the axis normal to the plane of the ellipse.
  const gp_Ax1& Axis() const { return pos.Axis(); }

  //! Computes the first or second directrix of this ellipse.
  //! These are the lines, in the plane of the ellipse, normal to
  //! the major axis, at a distance equal to
  //! MajorRadius/e from the center of the ellipse, where
  //! e is the eccentricity of the ellipse.
  //! The first directrix (Directrix1) is on the positive side of
  //! the major axis. The second directrix (Directrix2) is on
  //! the negative side.
  //! The directrix is returned as an axis (gp_Ax1 object), the
  //! origin of which is situated on the "X Axis" of the local
  //! coordinate system of this ellipse.
  //! Exceptions
  //! Standard_ConstructionError if the eccentricity is null
  //! (the ellipse has degenerated into a circle).
  gp_Ax1 Directrix1() const;

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the ellipse.
  //! Exceptions
  //! Standard_ConstructionError if the eccentricity is null
  //! (the ellipse has degenerated into a circle).
  gp_Ax1 Directrix2() const;

  //! Returns the eccentricity of the ellipse  between 0.0 and 1.0
  //! If f is the distance between the center of the ellipse and
  //! the Focus1 then the eccentricity e = f / MajorRadius.
  //! Raises ConstructionError if MajorRadius = 0.0
  Standard_Real Eccentricity() const;

  //! Computes the focal distance. It is the distance between the
  //! two focus focus1 and focus2 of the ellipse.
  Standard_Real Focal() const
  {
    return 2.0 * sqrt (majorRadius * majorRadius - minorRadius * minorRadius);
  }

  //! Returns the first focus of the ellipse. This focus is on the
  //! positive side of the "XAxis" of the ellipse.
  gp_Pnt Focus1() const;

  //! Returns the second focus of the ellipse. This focus is on the
  //! negative side of the "XAxis" of the ellipse.
  gp_Pnt Focus2() const;

  //! Returns the center of the ellipse. It is the "Location"
  //! point of the coordinate system of the ellipse.
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the major radius of the ellipse.
  Standard_Real MajorRadius() const { return majorRadius; }

  //! Returns the minor radius of the ellipse.
  Standard_Real MinorRadius() const { return minorRadius; }

  //! Returns p = (1 - e * e) * MajorRadius where e is the eccentricity
  //! of the ellipse.
  //! Returns 0 if MajorRadius = 0
  Standard_Real Parameter() const;

  //! Returns the coordinate system of the ellipse.
  const gp_Ax2& Position() const { return pos; }

  //! Returns the "XAxis" of the ellipse whose origin
  //! is the center of this ellipse. It is the major axis of the
  //! ellipse.
  gp_Ax1 XAxis() const { return gp_Ax1 (pos.Location(), pos.XDirection()); }

  //! Returns the "YAxis" of the ellipse whose unit vector is the "X Direction" or the "Y Direction"
  //! of the local coordinate system of this ellipse.
  //! This is the minor axis of the ellipse.
  gp_Ax1 YAxis() const { return gp_Ax1 (pos.Location(), pos.YDirection()); }

  Standard_EXPORT void Mirror (const gp_Pnt& theP);

  //! Performs the symmetrical transformation of an ellipse with
  //! respect to the point theP which is the center of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Elips Mirrored (const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of an ellipse with
  //! respect to an axis placement which is the axis of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Elips Mirrored (const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror (const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of an ellipse with
  //! respect to a plane. The axis placement theA2 locates the plane
  //! of the symmetry (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Elips Mirrored (const gp_Ax2& theA2) const;

  void Rotate (const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate (theA1, theAng); }

  //! Rotates an ellipse. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Elips Rotated (const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Elips anE = *this;
    anE.pos.Rotate (theA1, theAng);
    return anE;
  }

  void Scale (const gp_Pnt& theP, const Standard_Real theS);

  //! Scales an ellipse. theS is the scaling value.
  Standard_NODISCARD gp_Elips Scaled (const gp_Pnt& theP, const Standard_Real theS) const;

  void Transform (const gp_Trsf& theT);

  //! Transforms an ellipse with the transformation theT from class Trsf.
  Standard_NODISCARD gp_Elips Transformed (const gp_Trsf& theT) const;

  void Translate (const gp_Vec& theV) { pos.Translate (theV); }

  //! Translates an ellipse in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Elips Translated (const gp_Vec& theV) const
  {
    gp_Elips anE = *this;
    anE.pos.Translate (theV);
    return anE;
  }

  void Translate (const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate (theP1, theP2); }

  //! Translates an ellipse from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Elips Translated (const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Elips anE = *this;
    anE.pos.Translate (theP1, theP2);
    return anE;
  }

private:

  gp_Ax2 pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;

};

// =======================================================================
// function : Directrix1
// purpose  :
// =======================================================================
inline gp_Ax1 gp_Elips::Directrix1() const
{
  Standard_Real anE = Eccentricity();
  Standard_ConstructionError_Raise_if (anE <= gp::Resolution(), "gp_Elips::Directrix1() - zero eccentricity");
  gp_XYZ anOrig = pos.XDirection().XYZ();
  anOrig.Multiply (majorRadius / anE);
  anOrig.Add (pos.Location().XYZ());
  return gp_Ax1 (gp_Pnt (anOrig), pos.YDirection());
}

// =======================================================================
// function : Directrix2
// purpose  :
// =======================================================================
inline gp_Ax1 gp_Elips::Directrix2() const
{
  Standard_Real anE = Eccentricity();
  Standard_ConstructionError_Raise_if (anE <= gp::Resolution(), "gp_Elips::Directrix2() - zero eccentricity");
  gp_XYZ anOrig = pos.XDirection().XYZ();
  anOrig.Multiply (-majorRadius / anE);
  anOrig.Add (pos.Location().XYZ());
  return gp_Ax1 (gp_Pnt (anOrig), pos.YDirection());
}

// =======================================================================
// function : Eccentricity
// purpose  :
// =======================================================================
inline Standard_Real gp_Elips::Eccentricity() const
{
  if (majorRadius == 0.0)
  {
    return 0.0;
  }
  else
  {
    return sqrt (majorRadius * majorRadius - minorRadius * minorRadius) / majorRadius;
  }
}

// =======================================================================
// function : Focus1
// purpose  :
// =======================================================================
inline gp_Pnt gp_Elips::Focus1() const
{
  Standard_Real aC = sqrt (majorRadius * majorRadius - minorRadius * minorRadius);
  const gp_Pnt& aPP = pos.Location();
  const gp_Dir& aDD = pos.XDirection();
  return gp_Pnt (aPP.X() + aC * aDD.X(),
                 aPP.Y() + aC * aDD.Y(),
                 aPP.Z() + aC * aDD.Z());
}

// =======================================================================
// function : Focus2
// purpose  :
// =======================================================================
inline gp_Pnt gp_Elips::Focus2() const
{
  Standard_Real aC = sqrt (majorRadius * majorRadius - minorRadius * minorRadius);
  const gp_Pnt& aPP = pos.Location();
  const gp_Dir& aDD = pos.XDirection();
  return gp_Pnt (aPP.X() - aC * aDD.X(),
                 aPP.Y() - aC * aDD.Y(),
                 aPP.Z() - aC * aDD.Z());
}

// =======================================================================
// function : Parameter
// purpose  :
// =======================================================================
inline Standard_Real gp_Elips::Parameter() const
{
  if (majorRadius == 0.0)
  {
    return 0.0;
  }
  else
  {
    return (minorRadius * minorRadius) / majorRadius;
  }
}

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
inline void gp_Elips::Scale (const gp_Pnt& theP,
                             const Standard_Real theS)
  //  Modified by skv - Fri Apr  8 10:28:10 2005 OCC8559 Begin
  // { pos.Scale(P, S); }
{
  majorRadius *= theS;
  if (majorRadius < 0)
  {
    majorRadius = -majorRadius;
  }
  minorRadius *= theS;
  if (minorRadius < 0)
  {
    minorRadius = -minorRadius;
  }
  pos.Scale (theP, theS);
}
//  Modified by skv - Fri Apr  8 10:28:10 2005 OCC8559 End

// =======================================================================
// function : Scaled
// purpose  :
// =======================================================================
inline gp_Elips gp_Elips::Scaled (const gp_Pnt& theP,
                                  const Standard_Real theS) const
{
  gp_Elips anE = *this;
  anE.majorRadius *= theS;
  if (anE.majorRadius < 0)
  {
    anE.majorRadius = -anE.majorRadius;
  }
  anE.minorRadius *= theS;
  if (anE.minorRadius < 0)
  {
    anE.minorRadius = -anE.minorRadius;
  }
  anE.pos.Scale (theP, theS);
  return anE;
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
inline void gp_Elips::Transform (const gp_Trsf& theT)
{
  majorRadius *= theT.ScaleFactor();
  if (majorRadius < 0)
  {
    majorRadius = -majorRadius;
  }
  minorRadius *= theT.ScaleFactor();
  if (minorRadius < 0)
  {
    minorRadius = -minorRadius;
  }
  pos.Transform (theT);
}

// =======================================================================
// function : Transformed
// purpose  :
// =======================================================================
inline gp_Elips gp_Elips::Transformed (const gp_Trsf& theT) const
{
  gp_Elips anE = *this;
  anE.majorRadius *= theT.ScaleFactor();
  if (anE.majorRadius < 0)
  {
    anE.majorRadius = -anE.majorRadius;
  }
  anE.minorRadius *= theT.ScaleFactor();
  if (anE.minorRadius < 0)
  {
    anE.minorRadius = -anE.minorRadius;
  }
  anE.pos.Transform (theT);
  return anE;
}

#endif // _gp_Elips_HeaderFile
