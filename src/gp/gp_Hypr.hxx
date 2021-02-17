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

#ifndef _gp_Hypr_HeaderFile
#define _gp_Hypr_HeaderFile

#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes a branch of a hyperbola in 3D space.
//! A hyperbola is defined by its major and minor radii and
//! positioned in space with a coordinate system (a gp_Ax2
//! object) of which:
//! -   the origin is the center of the hyperbola,
//! -   the "X Direction" defines the major axis of the
//! hyperbola, and
//! - the "Y Direction" defines the minor axis of the hyperbola.
//! The origin, "X Direction" and "Y Direction" of this
//! coordinate system together define the plane of the
//! hyperbola. This coordinate system is the "local
//! coordinate system" of the hyperbola. In this coordinate
//! system, the equation of the hyperbola is:
//! X*X/(MajorRadius**2)-Y*Y/(MinorRadius**2) = 1.0
//! The branch of the hyperbola described is the one located
//! on the positive side of the major axis.
//! The "main Direction" of the local coordinate system is a
//! normal vector to the plane of the hyperbola. This vector
//! gives an implicit orientation to the hyperbola. We refer to
//! the "main Axis" of the local coordinate system as the
//! "Axis" of the hyperbola.
//! The following schema shows the plane of the hyperbola,
//! and in it, the respective positions of the three branches of
//! hyperbolas constructed with the functions OtherBranch,
//! ConjugateBranch1, and ConjugateBranch2:
//! @code
//! ^YAxis
//! |
//! FirstConjugateBranch
//! |
//! Other            |                Main
//! --------------------- C ------------------------------>XAxis
//! Branch           |                Branch
//! |
//! |
//! SecondConjugateBranch
//! |                  ^YAxis
//! @endcode
//! Warning
//! The major radius can be less than the minor radius.
//! See Also
//! gce_MakeHypr which provides functions for more
//! complex hyperbola constructions
//! Geom_Hyperbola which provides additional functions for
//! constructing hyperbolas and works, in particular, with the
//! parametric equations of hyperbolas
class gp_Hypr 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates of an indefinite hyperbola.
  gp_Hypr()
  : majorRadius (RealLast()),
    minorRadius (RealFirst())
  {}

  //! Creates a hyperbola with radius theMajorRadius and
  //! theMinorRadius, positioned in the space by the
  //! coordinate system theA2 such that:
  //! -   the origin of theA2 is the center of the hyperbola,
  //! -   the "X Direction" of theA2 defines the major axis of
  //! the hyperbola, that is, the major radius
  //! theMajorRadius is measured along this axis, and
  //! -   the "Y Direction" of theA2 defines the minor axis of
  //! the hyperbola, that is, the minor radius
  //! theMinorRadius is measured along this axis.
  //! Note: This class does not prevent the creation of a
  //! hyperbola where:
  //! -   theMajorAxis is equal to theMinorAxis, or
  //! -   theMajorAxis is less than theMinorAxis.
  //! Exceptions
  //! Standard_ConstructionError if theMajorAxis or theMinorAxis is negative.
  //! Raises ConstructionError if theMajorRadius < 0.0 or theMinorRadius < 0.0
  //! Raised if theMajorRadius < 0.0 or theMinorRadius < 0.0
  gp_Hypr (const gp_Ax2& theA2, const Standard_Real theMajorRadius, const Standard_Real theMinorRadius)
  : pos (theA2),
    majorRadius (theMajorRadius),
    minorRadius (theMinorRadius)
  {
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0 || theMajorRadius < 0.0,
      "gp_Hypr() - invalid construction parameters");
  }

  //! Modifies this hyperbola, by redefining its local coordinate
  //! system so that:
  //! -   its origin and "main Direction" become those of the
  //! axis theA1 (the "X Direction" and "Y Direction" are then
  //! recomputed in the same way as for any gp_Ax2).
  //! Raises ConstructionError if the direction of theA1 is parallel to the direction of
  //! the "XAxis" of the hyperbola.
  void SetAxis (const gp_Ax1& theA1) { pos.SetAxis (theA1); }

  //! Modifies this hyperbola, by redefining its local coordinate
  //! system so that its origin becomes theP.
  void SetLocation (const gp_Pnt& theP) { pos = gp_Ax2 (theP, pos.Direction(), pos.XDirection()); }

  //! Modifies the major  radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if theMajorRadius is negative.
  void SetMajorRadius (const Standard_Real theMajorRadius)
  {
    Standard_ConstructionError_Raise_if (theMajorRadius < 0.0,
      "gp_Hypr::SetMajorRadius() - major radius should be greater or equal zero");
    majorRadius = theMajorRadius;
  }

  //! Modifies the minor  radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if theMinorRadius is negative.
  void SetMinorRadius (const Standard_Real theMinorRadius)
  {
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0,
      "gp_Hypr::SetMinorRadius() - minor radius should be greater or equal zero");
    minorRadius = theMinorRadius;
  }

  //! Modifies this hyperbola, by redefining its local coordinate
  //! system so that it becomes A2.
  void SetPosition (const gp_Ax2& theA2) { pos = theA2; }

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = (B/A)*X
  //! where A is the major radius and B is the minor radius. Raises ConstructionError if MajorRadius = 0.0
  gp_Ax1 Asymptote1() const;

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = -(B/A)*X.
  //! where A is the major radius and B is the minor radius. Raises ConstructionError if MajorRadius = 0.0
  gp_Ax1 Asymptote2() const;

  //! Returns the axis passing through the center,
  //! and normal to the plane of this hyperbola.
  const gp_Ax1& Axis() const { return pos.Axis(); }

  //! Computes the branch of hyperbola which is on the positive side of the
  //! "YAxis" of <me>.
  gp_Hypr ConjugateBranch1() const
  {
    return gp_Hypr (gp_Ax2 (pos.Location(), pos.Direction(), pos.YDirection()), minorRadius, majorRadius);
  }

  //! Computes the branch of hyperbola which is on the negative side of the
  //! "YAxis" of <me>.
  gp_Hypr ConjugateBranch2() const
  {
    gp_Dir aD = pos.YDirection();
    aD.Reverse();
    return gp_Hypr (gp_Ax2(pos.Location(), pos.Direction(), aD), minorRadius, majorRadius);
  }

  //! This directrix is the line normal to the XAxis of the hyperbola
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the hyperbola, where e is the eccentricity of
  //! the hyperbola.
  //! This line is parallel to the "YAxis". The intersection point
  //! between the directrix1 and the "XAxis" is the "Location" point
  //! of the directrix1. This point is on the positive side of the
  //! "XAxis".
  gp_Ax1 Directrix1() const;

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the hyperbola.
  gp_Ax1 Directrix2() const;

  //! Returns the eccentricity of the hyperbola (e > 1).
  //! If f is the distance between the location of the hyperbola
  //! and the Focus1 then the eccentricity e = f / MajorRadius. Raises DomainError if MajorRadius = 0.0
  Standard_Real Eccentricity() const
  {
    Standard_DomainError_Raise_if (majorRadius <= gp::Resolution(),
      "gp_Hypr::Eccentricity() - major radius is zero");
    return sqrt (majorRadius * majorRadius + minorRadius * minorRadius) / majorRadius;
  }

  //! Computes the focal distance. It is the distance between the
  //! the two focus of the hyperbola.
  Standard_Real Focal() const
  {
    return 2.0 * sqrt (majorRadius * majorRadius + minorRadius * minorRadius);
  }

  //! Returns the first focus of the hyperbola. This focus is on the
  //! positive side of the "XAxis" of the hyperbola.
  gp_Pnt Focus1() const;

  //! Returns the second focus of the hyperbola. This focus is on the
  //! negative side of the "XAxis" of the hyperbola.
  gp_Pnt Focus2() const;

  //! Returns  the location point of the hyperbola. It is the
  //! intersection point between the "XAxis" and the "YAxis".
  const gp_Pnt& Location() const { return pos.Location(); }

  //! Returns the major radius of the hyperbola. It is the radius
  //! on the "XAxis" of the hyperbola.
  Standard_Real MajorRadius() const { return majorRadius; }

  //! Returns the minor radius of the hyperbola. It is the radius
  //! on the "YAxis" of the hyperbola.
  Standard_Real MinorRadius() const { return minorRadius; }

  //! Returns the branch of hyperbola obtained by doing the
  //! symmetrical transformation of <me> with respect to the
  //! "YAxis"  of <me>.
  gp_Hypr OtherBranch() const
  {
    gp_Dir aD = pos.XDirection();
    aD.Reverse();
    return gp_Hypr (gp_Ax2 (pos.Location(), pos.Direction(), aD), majorRadius, minorRadius);
  }

  //! Returns p = (e * e - 1) * MajorRadius where e is the
  //! eccentricity of the hyperbola.
  //! Raises DomainError if MajorRadius = 0.0
  Standard_Real Parameter() const
  {
    Standard_DomainError_Raise_if (majorRadius <= gp::Resolution(),
                                   "gp_Hypr::Parameter() - major radius is zero");
    return (minorRadius * minorRadius) / majorRadius;
  }

  //! Returns the coordinate system of the hyperbola.
  const gp_Ax2& Position() const { return pos; }

  //! Computes an axis, whose
  //! -   the origin is the center of this hyperbola, and
  //! -   the unit vector is the "X Direction"
  //! of the local coordinate system of this hyperbola.
  //! These axes are, the major axis (the "X
  //! Axis") and  of this hyperboReturns the "XAxis" of the hyperbola.
  gp_Ax1 XAxis() const { return gp_Ax1 (pos.Location(), pos.XDirection()); }

  //! Computes an axis, whose
  //! -   the origin is the center of this hyperbola, and
  //! -   the unit vector is the "Y Direction"
  //! of the local coordinate system of this hyperbola.
  //! These axes are the minor axis (the "Y Axis") of this hyperbola
  gp_Ax1 YAxis() const { return gp_Ax1 (pos.Location(), pos.YDirection()); }

  Standard_EXPORT void Mirror (const gp_Pnt& theP);

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect  to the point theP which is the center of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Hypr Mirrored (const gp_Pnt& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax1& theA1);

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect to an axis placement which is the axis of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Hypr Mirrored (const gp_Ax1& theA1) const;

  Standard_EXPORT void Mirror (const gp_Ax2& theA2);

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect to a plane. The axis placement theA2 locates the plane
  //! of the symmetry (Location, XDirection, YDirection).
  Standard_NODISCARD Standard_EXPORT gp_Hypr Mirrored (const gp_Ax2& theA2) const;

  void Rotate (const gp_Ax1& theA1, const Standard_Real theAng) { pos.Rotate (theA1, theAng); }

  //! Rotates an hyperbola. theA1 is the axis of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Hypr Rotated (const gp_Ax1& theA1, const Standard_Real theAng) const
  {
    gp_Hypr aH = *this;
    aH.pos.Rotate (theA1, theAng);
    return aH;
  }

  void Scale (const gp_Pnt& theP, const Standard_Real theS);

  //! Scales an hyperbola. theS is the scaling value.
  Standard_NODISCARD gp_Hypr Scaled (const gp_Pnt& theP, const Standard_Real theS) const;

  void Transform (const gp_Trsf& theT);

  //! Transforms an hyperbola with the transformation theT from
  //! class Trsf.
  Standard_NODISCARD gp_Hypr Transformed (const gp_Trsf& theT) const;

  void Translate (const gp_Vec& theV) { pos.Translate (theV); }

  //! Translates an hyperbola in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Hypr Translated (const gp_Vec& theV) const
  {
    gp_Hypr aH = *this;
    aH.pos.Translate (theV);
    return aH;
  }

  void Translate (const gp_Pnt& theP1, const gp_Pnt& theP2) { pos.Translate (theP1, theP2); }

  //! Translates an hyperbola from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Hypr Translated (const gp_Pnt& theP1, const gp_Pnt& theP2) const
  {
    gp_Hypr aH = *this;
    aH.pos.Translate (theP1, theP2);
    return aH;
  }

private:

  gp_Ax2 pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;

};

//=======================================================================
//function : Asymptote1
// purpose :
//=======================================================================
inline gp_Ax1 gp_Hypr::Asymptote1() const
{
  Standard_ConstructionError_Raise_if (majorRadius <= gp::Resolution(),
                                       "gp_Hypr::Asymptote1() - major radius is zero");
  gp_Vec aV1 = gp_Vec (pos.YDirection());
  aV1.Multiply (minorRadius / majorRadius);
  gp_Vec aV = gp_Vec (pos.XDirection());
  aV.Add (aV1);
  return  gp_Ax1 (pos.Location(), gp_Dir (aV));
}

//=======================================================================
//function : Asymptote2
// purpose :
//=======================================================================
inline gp_Ax1 gp_Hypr::Asymptote2() const
{
  Standard_ConstructionError_Raise_if (majorRadius <= gp::Resolution(),
                                       "gp_Hypr::Asymptote1() - major radius is zero");
  gp_Vec aV1 = gp_Vec (pos.YDirection());
  aV1.Multiply (-minorRadius / majorRadius);
  gp_Vec aV = gp_Vec (pos.XDirection());
  aV.Add (aV1);
  return  gp_Ax1 ( pos.Location(), gp_Dir (aV));
}

//=======================================================================
//function : Focus1
// purpose :
//=======================================================================
inline gp_Pnt gp_Hypr::Focus1() const
{
  Standard_Real aC = sqrt (majorRadius * majorRadius + minorRadius * minorRadius);
  const gp_Pnt& aPP = pos.Location  ();
  const gp_Dir& aDD = pos.XDirection();
  return gp_Pnt (aPP.X() + aC * aDD.X(),
                 aPP.Y() + aC * aDD.Y(),
                 aPP.Z() + aC * aDD.Z());
}

//=======================================================================
//function : Focus2
// purpose :
//=======================================================================
inline gp_Pnt gp_Hypr::Focus2 () const
{
  Standard_Real aC = sqrt (majorRadius * majorRadius + minorRadius * minorRadius);
  const gp_Pnt& aPP = pos.Location  ();
  const gp_Dir& aDD = pos.XDirection();
  return gp_Pnt (aPP.X() - aC * aDD.X(),
                 aPP.Y() - aC * aDD.Y(),
                 aPP.Z() - aC * aDD.Z());
}

//=======================================================================
//function : Scale
// purpose :
//=======================================================================
inline void gp_Hypr::Scale (const gp_Pnt& theP,
                            const Standard_Real theS)
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

//=======================================================================
//function : Scaled
// purpose :
//=======================================================================
inline gp_Hypr gp_Hypr::Scaled (const gp_Pnt& theP,
                                const Standard_Real theS) const
{
  gp_Hypr aH = *this;
  aH.majorRadius *= theS;
  if (aH.majorRadius < 0)
  {
    aH.majorRadius = -aH.majorRadius;
  }
  aH.minorRadius *= theS;
  if (aH.minorRadius < 0)
  {
    aH.minorRadius = -aH.minorRadius;
  }
  aH.pos.Scale (theP, theS);
  return aH; 
}

//=======================================================================
//function : Transform
// purpose :
//=======================================================================
inline void gp_Hypr::Transform (const gp_Trsf& theT)
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

//=======================================================================
//function : Transformed
// purpose :
//=======================================================================
inline gp_Hypr gp_Hypr::Transformed (const gp_Trsf& theT) const
{
  gp_Hypr aH = *this;
  aH.majorRadius *= theT.ScaleFactor();
  if (aH.majorRadius < 0)
  {
    aH.majorRadius = -aH.majorRadius;
  }
  aH.minorRadius *= theT.ScaleFactor();
  if (aH.minorRadius < 0)
  {
    aH.minorRadius = -aH.minorRadius;
  }
  aH.pos.Transform (theT);
  return aH; 
}

//=======================================================================
//function : Directrix1
// purpose :
//=======================================================================
inline gp_Ax1 gp_Hypr::Directrix1 () const
{
  Standard_Real anE = Eccentricity();
  gp_XYZ anOrig = pos.XDirection().XYZ();
  anOrig.Multiply (majorRadius / anE);
  anOrig.Add (pos.Location().XYZ());
  return gp_Ax1 (gp_Pnt (anOrig), pos.YDirection());
}

//=======================================================================
//function : Directrix2
// purpose :
//=======================================================================
inline gp_Ax1 gp_Hypr::Directrix2 () const
{
  Standard_Real anE = Eccentricity();
  gp_XYZ anOrig = pos.XDirection().XYZ();
  anOrig.Multiply (-majorRadius / anE);
  anOrig.Add (pos.Location().XYZ());
  return gp_Ax1 (gp_Pnt (anOrig), pos.YDirection());
}

#endif // _gp_Hypr_HeaderFile
