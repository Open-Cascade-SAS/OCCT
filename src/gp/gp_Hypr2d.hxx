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

#ifndef _gp_Hypr2d_HeaderFile
#define _gp_Hypr2d_HeaderFile

#include <gp.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes a branch of a hyperbola in the plane (2D space).
//! A hyperbola is defined by its major and minor radii, and
//! positioned in the plane with a coordinate system (a
//! gp_Ax22d object) of which:
//! -   the origin is the center of the hyperbola,
//! -   the "X Direction" defines the major axis of the hyperbola, and
//! -   the "Y Direction" defines the minor axis of the hyperbola.
//! This coordinate system is the "local coordinate system"
//! of the hyperbola. The orientation of this coordinate
//! system (direct or indirect) gives an implicit orientation to
//! the hyperbola. In this coordinate system, the equation of
//! the hyperbola is:
//! X*X/(MajorRadius**2)-Y*Y/(MinorRadius**2) = 1.0
//! The branch of the hyperbola described is the one located
//! on the positive side of the major axis.
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
//! |
//! @endcode
//! Warning
//! The major radius can be less than the minor radius.
//! See Also
//! gce_MakeHypr2d which provides functions for more
//! complex hyperbola constructions
//! Geom2d_Hyperbola which provides additional functions
//! for constructing hyperbolas and works, in particular, with
//! the parametric equations of hyperbolas
class gp_Hypr2d 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates of an indefinite hyperbola.
  gp_Hypr2d()
  : majorRadius (RealLast()),
    minorRadius (RealLast())
  {}

  //! Creates a hyperbola with radii theMajorRadius and
  //! theMinorRadius, centered on the origin of theMajorAxis
  //! and where the unit vector of theMajorAxis is the "X
  //! Direction" of the local coordinate system of the
  //! hyperbola. This coordinate system is direct if theIsSense
  //! is true (the default value), and indirect if theIsSense is false.
  //! Warnings :
  //! It is yet  possible to create an Hyperbola with
  //! theMajorRadius <= theMinorRadius.
  //! Raises ConstructionError if theMajorRadius < 0.0 or theMinorRadius < 0.0
  gp_Hypr2d (const gp_Ax2d& theMajorAxis, const Standard_Real theMajorRadius,
            const Standard_Real theMinorRadius, const Standard_Boolean theIsSense = Standard_True)
  : majorRadius (theMajorRadius),
    minorRadius (theMinorRadius)
  {
    pos = gp_Ax22d (theMajorAxis, theIsSense);
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0 || theMajorRadius < 0.0,
      "gp_Hypr2d() - invalid construction parameters");
  }

  //! a hyperbola with radii theMajorRadius and
  //! theMinorRadius, positioned in the plane by coordinate system theA where:
  //! -   the origin of theA is the center of the hyperbola,
  //! -   the "X Direction" of theA defines the major axis of
  //! the hyperbola, that is, the major radius
  //! theMajorRadius is measured along this axis, and
  //! -   the "Y Direction" of theA defines the minor axis of
  //! the hyperbola, that is, the minor radius
  //! theMinorRadius is measured along this axis, and
  //! -   the orientation (direct or indirect sense) of theA
  //! gives the implicit orientation of the hyperbola.
  //! Warnings :
  //! It is yet  possible to create an Hyperbola with
  //! theMajorRadius <= theMinorRadius.
  //! Raises ConstructionError if theMajorRadius < 0.0 or theMinorRadius < 0.0
  gp_Hypr2d (const gp_Ax22d& theA, const Standard_Real theMajorRadius, const Standard_Real theMinorRadius)
  : pos (theA),
    majorRadius (theMajorRadius),
    minorRadius (theMinorRadius)
  {
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0 || theMajorRadius < 0.0,
                                         "gp_Hypr2d() - invalid construction parameters");
  }

  //! Modifies this hyperbola, by redefining its local
  //! coordinate system so that its origin becomes theP.
  void SetLocation (const gp_Pnt2d& theP) { pos.SetLocation (theP); }

  //! Modifies the major or minor radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if theMajorRadius or
  //! MinorRadius is negative.
  void SetMajorRadius (const Standard_Real theMajorRadius)
  {
    Standard_ConstructionError_Raise_if (theMajorRadius < 0.0,
                                         "gp_Hypr2d::SetMajorRadius() - major radius should be greater or equal zero");
    majorRadius = theMajorRadius;
  }

  //! Modifies the major or minor radius of this hyperbola.
  //! Exceptions
  //! Standard_ConstructionError if MajorRadius or
  //! theMinorRadius is negative.
  void SetMinorRadius (const Standard_Real theMinorRadius)
  {
    Standard_ConstructionError_Raise_if (theMinorRadius < 0.0,
                                         "gp_Hypr2d::SetMinorRadius() - minor radius should be greater or equal zero");
    minorRadius = theMinorRadius;
  }

  //! Modifies this hyperbola, by redefining its local
  //! coordinate system so that it becomes theA.
  void SetAxis (const gp_Ax22d& theA) { pos.SetAxis (theA); }

  //! Changes the major axis of the hyperbola. The minor axis is
  //! recomputed and the location of the hyperbola too.
  void SetXAxis (const gp_Ax2d& theA) { pos.SetXAxis (theA); }

  //! Changes the minor axis of the hyperbola.The minor axis is
  //! recomputed and the location of the hyperbola too.
  void SetYAxis (const gp_Ax2d& theA) { pos.SetYAxis (theA); }

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = (B/A)*X
  //! where A is the major radius of the hyperbola and B the minor
  //! radius of the hyperbola.
  //! Raises ConstructionError if MajorRadius = 0.0
  gp_Ax2d Asymptote1() const;

  //! In the local coordinate system of the hyperbola the equation of
  //! the hyperbola is (X*X)/(A*A) - (Y*Y)/(B*B) = 1.0 and the
  //! equation of the first asymptote is Y = -(B/A)*X
  //! where A is the major radius of the hyperbola and B the minor
  //! radius of the hyperbola.
  //! Raises ConstructionError if MajorRadius = 0.0
  gp_Ax2d Asymptote2() const;

  //! Computes the coefficients of the implicit equation of
  //! the hyperbola :
  //! theA * (X**2) + theB * (Y**2) + 2*theC*(X*Y) + 2*theD*X + 2*theE*Y + theF = 0.
  Standard_EXPORT void Coefficients (Standard_Real& theA, Standard_Real& theB, Standard_Real& theC,
                                     Standard_Real& theD, Standard_Real& theE, Standard_Real& theF) const;

  //! Computes the branch of hyperbola which is on the positive side of the
  //! "YAxis" of <me>.
  gp_Hypr2d ConjugateBranch1() const
  {
    gp_Dir2d aV (pos.YDirection());
    Standard_Boolean isSign = (pos.XDirection().Crossed (pos.YDirection())) >= 0.0;
    return gp_Hypr2d (gp_Ax2d (pos.Location(), aV), minorRadius, majorRadius, isSign);
  }

  //! Computes the branch of hyperbola which is on the negative side of the
  //! "YAxis" of <me>.
  gp_Hypr2d ConjugateBranch2() const
  {
    gp_Dir2d aV (pos.YDirection().Reversed());
    Standard_Boolean isSign = (pos.XDirection().Crossed (pos.YDirection())) >= 0.0;
    return gp_Hypr2d (gp_Ax2d (pos.Location(), aV), minorRadius, majorRadius, isSign);
  }

  //! Computes the directrix which is the line normal to the XAxis of the hyperbola
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the hyperbola, where e is the eccentricity of
  //! the hyperbola.
  //! This line is parallel to the "YAxis". The intersection point
  //! between the "Directrix1" and the "XAxis" is the "Location" point
  //! of the "Directrix1".
  //! This point is on the positive side of the "XAxis".
  gp_Ax2d Directrix1() const;

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the "YAxis" of the hyperbola.
  gp_Ax2d Directrix2() const;

  //! Returns the eccentricity of the hyperbola (e > 1).
  //! If f is the distance between the location of the hyperbola
  //! and the Focus1 then the eccentricity e = f / MajorRadius. Raises DomainError if MajorRadius = 0.0.
  Standard_Real Eccentricity() const
  {
    Standard_DomainError_Raise_if (majorRadius <= gp::Resolution(),
                                   "gp_Hypr2d::Eccentricity() - major radius is zero");
    return sqrt (majorRadius * majorRadius + minorRadius * minorRadius) / majorRadius;
  }

  //! Computes the focal distance. It is the distance between the
  //! "Location" of the hyperbola and "Focus1" or "Focus2".
  Standard_Real Focal() const
  {
    return 2.0 * sqrt (majorRadius * majorRadius + minorRadius * minorRadius);
  }

  //! Returns the first focus of the hyperbola. This focus is on the
  //! positive side of the "XAxis" of the hyperbola.
  gp_Pnt2d Focus1() const
  {
    Standard_Real aC = sqrt (majorRadius * majorRadius + minorRadius * minorRadius);
    return gp_Pnt2d (pos.Location().X() + aC * pos.XDirection().X(), pos.Location().Y() + aC * pos.XDirection().Y());
  }

  //! Returns the second focus of the hyperbola. This focus is on the
  //! negative side of the "XAxis" of the hyperbola.
  gp_Pnt2d Focus2() const
  {
    Standard_Real aC = sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
    return gp_Pnt2d(pos.Location().X() - aC * pos.XDirection().X(), pos.Location().Y() - aC * pos.XDirection().Y());
  }

  //! Returns  the location point of the hyperbola.
  //! It is the intersection point between the "XAxis" and
  //! the "YAxis".
  const gp_Pnt2d& Location() const { return pos.Location(); }

  //! Returns the major radius of the hyperbola (it is the radius
  //! corresponding to the "XAxis" of the hyperbola).
  Standard_Real MajorRadius() const { return majorRadius; }

  //! Returns the minor radius of the hyperbola (it is the radius
  //! corresponding to the "YAxis" of the hyperbola).
  Standard_Real MinorRadius() const { return minorRadius; }

  //! Returns the branch of hyperbola obtained by doing the
  //! symmetrical transformation of <me> with respect to the
  //! "YAxis" of <me>.
  gp_Hypr2d OtherBranch() const
  {
    Standard_Boolean isSign = (pos.XDirection().Crossed (pos.YDirection())) >= 0.0;
    return gp_Hypr2d (gp_Ax2d (pos.Location(), pos.XDirection().Reversed()), majorRadius, minorRadius, isSign);
  }

  //! Returns p = (e * e - 1) * MajorRadius where e is the
  //! eccentricity of the hyperbola.
  //! Raises DomainError if MajorRadius = 0.0
  Standard_Real Parameter() const
  {
    Standard_DomainError_Raise_if (majorRadius <= gp::Resolution(),
                                   "gp_Hypr2d::Parameter() - major radius is zero");
    return (minorRadius * minorRadius) / majorRadius;
  }

  //! Returns the axisplacement of the hyperbola.
  const gp_Ax22d& Axis() const { return pos; }

  //! Computes an axis whose
  //! -   the origin is the center of this hyperbola, and
  //! -   the unit vector is the "X Direction" or "Y Direction"
  //! respectively of the local coordinate system of this hyperbola
  //! Returns the major axis of the hyperbola.
  gp_Ax2d XAxis() const { return pos.XAxis(); }

  //! Computes an axis whose
  //! -   the origin is the center of this hyperbola, and
  //! -   the unit vector is the "X Direction" or "Y Direction"
  //! respectively of the local coordinate system of this hyperbola
  //! Returns the minor axis of the hyperbola.
  gp_Ax2d YAxis() const { return pos.YAxis(); }

  void Reverse()
  {
    gp_Dir2d aTemp = pos.YDirection();
    aTemp.Reverse();
    pos.SetAxis (gp_Ax22d(pos.Location(), pos.XDirection(), aTemp));
  }

  //! Reverses the orientation of the local coordinate system
  //! of this hyperbola (the "Y Axis" is reversed). Therefore,
  //! the implicit orientation of this hyperbola is reversed.
  //! Note:
  //! -   Reverse assigns the result to this hyperbola, while
  //! -   Reversed creates a new one.
  Standard_NODISCARD gp_Hypr2d Reversed() const;

  //! Returns true if the local coordinate system is direct
  //! and false in the other case.
  Standard_Boolean IsDirect() const
  {
    return (pos.XDirection().Crossed (pos.YDirection())) >= 0.0;
  }

  Standard_EXPORT void Mirror (const gp_Pnt2d& theP);

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect  to the point theP which is the center of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Hypr2d Mirrored (const gp_Pnt2d& theP) const;

  Standard_EXPORT void Mirror (const gp_Ax2d& theA);

  //! Performs the symmetrical transformation of an hyperbola with
  //! respect to an axis placement which is the axis of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Hypr2d Mirrored (const gp_Ax2d& theA) const;

  void Rotate (const gp_Pnt2d& theP, const Standard_Real theAng) { pos.Rotate (theP, theAng); }

  //! Rotates an hyperbola. theP is the center of the rotation.
  //! theAng is the angular value of the rotation in radians.
  Standard_NODISCARD gp_Hypr2d Rotated (const gp_Pnt2d& theP, const Standard_Real theAng) const
  {
    gp_Hypr2d aH = *this;
    aH.pos.Rotate (theP, theAng);
    return aH;
  }

  void Scale (const gp_Pnt2d& theP, const Standard_Real theS);

  //! Scales an hyperbola. <theS> is the scaling value.
  //! If <theS> is positive only the location point is
  //! modified. But if <theS> is negative the "XAxis" is
  //! reversed and the "YAxis" too.
  Standard_NODISCARD gp_Hypr2d Scaled (const gp_Pnt2d& theP, const Standard_Real theS) const;

  void Transform (const gp_Trsf2d& theT);

  //! Transforms an hyperbola with the transformation theT from
  //! class Trsf2d.
  Standard_NODISCARD gp_Hypr2d Transformed (const gp_Trsf2d& theT) const;

  void Translate (const gp_Vec2d& theV) { pos.Translate (theV); }

  //! Translates an hyperbola in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Hypr2d Translated (const gp_Vec2d& theV) const
  {
    gp_Hypr2d aH = *this;
    aH.pos.Translate (theV);
    return aH;
  }

  void Translate(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) { pos.Translate (theP1, theP2); }

  //! Translates an hyperbola from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Hypr2d Translated (const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) const
  {
    gp_Hypr2d aH = *this;
    aH.pos.Translate (theP1, theP2);
    return aH;
  }

private:

  gp_Ax22d pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;

};

//=======================================================================
//function : Asymptote1
// purpose :
//=======================================================================
inline gp_Ax2d gp_Hypr2d::Asymptote1() const 
{
  Standard_ConstructionError_Raise_if (majorRadius <= gp::Resolution(),
                                       "gp_Hypr2d::Asymptote1() - major radius is zero");
  gp_Dir2d aVdir = pos.XDirection();
  gp_XY aCoord1 (pos.YDirection().XY());
  gp_XY aCoord2 = aCoord1.Multiplied (minorRadius / majorRadius);
  aCoord1.Add (aCoord2);
  aVdir.SetXY (aCoord1);
  return gp_Ax2d (pos.Location(), aVdir);
}

//=======================================================================
//function : Asymptote2
// purpose :
//=======================================================================
inline gp_Ax2d gp_Hypr2d::Asymptote2() const
{
  Standard_ConstructionError_Raise_if (majorRadius <= gp::Resolution(),
                                       "gp_Hypr2d::Asymptote2() - major radius is zero");
  gp_Vec2d aVdir = pos.XDirection();
  gp_XY  aCoord1 (pos.YDirection().XY());
  gp_XY  aCoord2 = aCoord1.Multiplied (-minorRadius / majorRadius);
  aCoord1.Add (aCoord2);
  aVdir.SetXY (aCoord1);
  return gp_Ax2d (pos.Location(), aVdir);
}

//=======================================================================
//function : Directrix1
// purpose :
//=======================================================================
inline gp_Ax2d gp_Hypr2d::Directrix1() const 
{
  Standard_Real anE = Eccentricity();
  gp_XY anOrig = pos.XDirection().XY();
  anOrig.Multiply (majorRadius / anE);
  anOrig.Add (pos.Location().XY());
  return gp_Ax2d (gp_Pnt2d (anOrig), gp_Dir2d (pos.YDirection()));
}

//=======================================================================
//function : Directrix2
// purpose :
//=======================================================================
inline gp_Ax2d gp_Hypr2d::Directrix2() const
{
  Standard_Real anE = Eccentricity();
  gp_XY anOrig = pos.XDirection().XY();
  anOrig.Multiply (Parameter() / anE);
  anOrig.Add (Focus1().XY());
  return gp_Ax2d (gp_Pnt2d (anOrig), gp_Dir2d (pos.YDirection()));
}

//=======================================================================
//function : Reversed
// purpose :
//=======================================================================
inline gp_Hypr2d gp_Hypr2d::Reversed() const
{
  gp_Hypr2d aH = *this;
  gp_Dir2d aTemp = pos.YDirection();
  aTemp.Reverse ();
  aH.pos.SetAxis (gp_Ax22d (pos.Location(),pos.XDirection(), aTemp));
  return aH;
}

//=======================================================================
//function : Scale
// purpose :
//=======================================================================
inline void gp_Hypr2d::Scale (const gp_Pnt2d& theP, const Standard_Real theS)
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
inline gp_Hypr2d gp_Hypr2d::Scaled (const gp_Pnt2d& theP, const Standard_Real theS) const
{
  gp_Hypr2d aH = *this;
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
inline void gp_Hypr2d::Transform (const gp_Trsf2d& theT)
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
inline gp_Hypr2d gp_Hypr2d::Transformed (const gp_Trsf2d& theT) const
{
  gp_Hypr2d aH = *this;
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

#endif // _gp_Hypr2d_HeaderFile
