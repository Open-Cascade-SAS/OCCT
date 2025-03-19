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

#ifndef _gp_Elips2d_HeaderFile
#define _gp_Elips2d_HeaderFile

#include <gp.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ConstructionError.hxx>

//! Describes an ellipse in the plane (2D space).
//! An ellipse is defined by its major and minor radii and
//! positioned in the plane with a coordinate system (a
//! gp_Ax22d object) as follows:
//! -   the origin of the coordinate system is the center of the ellipse,
//! -   its "X Direction" defines the major axis of the ellipse, and
//! -   its "Y Direction" defines the minor axis of the ellipse.
//! This coordinate system is the "local coordinate system"
//! of the ellipse. Its orientation (direct or indirect) gives an
//! implicit orientation to the ellipse. In this coordinate
//! system, the equation of the ellipse is:
//! @code
//! X*X / (MajorRadius**2) + Y*Y / (MinorRadius**2) = 1.0
//! @endcode
//! See Also
//! gce_MakeElips2d which provides functions for more
//! complex ellipse constructions
//! Geom2d_Ellipse which provides additional functions for
//! constructing ellipses and works, in particular, with the
//! parametric equations of ellipses
class gp_Elips2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an indefinite ellipse.
  gp_Elips2d()
      : majorRadius(RealLast()),
        minorRadius(RealSmall())
  {
  }

  //! Creates an ellipse with the major axis, the major and the
  //! minor radius. The location of the theMajorAxis is the center
  //! of the  ellipse.
  //! The sense of parametrization is given by theIsSense.
  //! Warnings :
  //! It is possible to create an ellipse with
  //! theMajorRadius = theMinorRadius.
  //! Raises ConstructionError if theMajorRadius < theMinorRadius or theMinorRadius < 0.0
  gp_Elips2d(const gp_Ax2d&         theMajorAxis,
             const Standard_Real    theMajorRadius,
             const Standard_Real    theMinorRadius,
             const Standard_Boolean theIsSense = Standard_True)
      : majorRadius(theMajorRadius),
        minorRadius(theMinorRadius)
  {
    pos = gp_Ax22d(theMajorAxis, theIsSense);
    Standard_ConstructionError_Raise_if(theMinorRadius < 0.0 || theMajorRadius < theMinorRadius,
                                        "gp_Elips2d() - invalid construction parameters");
  }

  //! Creates an ellipse with radii MajorRadius and
  //! MinorRadius, positioned in the plane by coordinate system theA where:
  //! -   the origin of theA is the center of the ellipse,
  //! -   the "X Direction" of theA defines the major axis of
  //! the ellipse, that is, the major radius MajorRadius
  //! is measured along this axis, and
  //! -   the "Y Direction" of theA defines the minor axis of
  //! the ellipse, that is, the minor radius theMinorRadius
  //! is measured along this axis, and
  //! -   the orientation (direct or indirect sense) of theA
  //! gives the orientation of the ellipse.
  //! Warnings :
  //! It is possible to create an ellipse with
  //! theMajorRadius = theMinorRadius.
  //! Raises ConstructionError if theMajorRadius < theMinorRadius or theMinorRadius < 0.0
  gp_Elips2d(const gp_Ax22d&     theA,
             const Standard_Real theMajorRadius,
             const Standard_Real theMinorRadius)
      : pos(theA),
        majorRadius(theMajorRadius),
        minorRadius(theMinorRadius)
  {
    Standard_ConstructionError_Raise_if(theMinorRadius < 0.0 || theMajorRadius < theMinorRadius,
                                        "gp_Elips2d() - invalid construction parameters");
  }

  //! Modifies this ellipse, by redefining its local coordinate system so that
  //! -   its origin becomes theP.
  void SetLocation(const gp_Pnt2d& theP) { pos.SetLocation(theP); }

  //! Changes the value of the major radius.
  //! Raises ConstructionError if theMajorRadius < MinorRadius.
  void SetMajorRadius(const Standard_Real theMajorRadius)
  {
    Standard_ConstructionError_Raise_if(
      theMajorRadius < minorRadius,
      "gp_Elips2d::SetMajorRadius() - major radius should be greater or equal to minor radius");
    majorRadius = theMajorRadius;
  }

  //! Changes the value of the minor radius.
  //! Raises ConstructionError if MajorRadius < theMinorRadius or MinorRadius < 0.0
  void SetMinorRadius(const Standard_Real theMinorRadius)
  {
    Standard_ConstructionError_Raise_if(theMinorRadius < 0.0 || majorRadius < theMinorRadius,
                                        "gp_Elips2d::SetMinorRadius() - minor radius should be a "
                                        "positive number lesser or equal to major radius");
    minorRadius = theMinorRadius;
  }

  //! Modifies this ellipse, by redefining its local coordinate system so that
  //! it becomes theA.
  void SetAxis(const gp_Ax22d& theA) { pos.SetAxis(theA); }

  //! Modifies this ellipse, by redefining its local coordinate system so that
  //! its origin and its "X Direction"  become those
  //! of the axis theA. The "Y  Direction"  is then
  //! recomputed. The orientation of the local coordinate
  //! system is not modified.
  void SetXAxis(const gp_Ax2d& theA) { pos.SetXAxis(theA); }

  //! Modifies this ellipse, by redefining its local coordinate system so that
  //! its origin and its "Y Direction"  become those
  //! of the axis theA. The "X  Direction"  is then
  //! recomputed. The orientation of the local coordinate
  //! system is not modified.
  void SetYAxis(const gp_Ax2d& theA) { pos.SetYAxis(theA); }

  //! Computes the area of the ellipse.
  Standard_Real Area() const { return M_PI * majorRadius * minorRadius; }

  //! Returns the coefficients of the implicit equation of the ellipse.
  //! theA * (X**2) + theB * (Y**2) + 2*theC*(X*Y) + 2*theD*X + 2*theE*Y + theF = 0.
  Standard_EXPORT void Coefficients(Standard_Real& theA,
                                    Standard_Real& theB,
                                    Standard_Real& theC,
                                    Standard_Real& theD,
                                    Standard_Real& theE,
                                    Standard_Real& theF) const;

  //! This directrix is the line normal to the XAxis of the ellipse
  //! in the local plane (Z = 0) at a distance d = MajorRadius / e
  //! from the center of the ellipse, where e is the eccentricity of
  //! the ellipse.
  //! This line is parallel to the "YAxis". The intersection point
  //! between directrix1 and the "XAxis" is the location point of the
  //! directrix1. This point is on the positive side of the "XAxis".
  //!
  //! Raised if Eccentricity = 0.0. (The ellipse degenerates into a
  //! circle)
  gp_Ax2d Directrix1() const;

  //! This line is obtained by the symmetrical transformation
  //! of "Directrix1" with respect to the minor axis of the ellipse.
  //!
  //! Raised if Eccentricity = 0.0. (The ellipse degenerates into a
  //! circle).
  gp_Ax2d Directrix2() const;

  //! Returns the eccentricity of the ellipse  between 0.0 and 1.0
  //! If f is the distance between the center of the ellipse and
  //! the Focus1 then the eccentricity e = f / MajorRadius.
  //! Returns 0 if MajorRadius = 0.
  Standard_Real Eccentricity() const;

  //! Returns the distance between the center of the ellipse
  //! and focus1 or focus2.
  Standard_Real Focal() const
  {
    return 2.0 * sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  }

  //! Returns the first focus of the ellipse. This focus is on the
  //! positive side of the major axis of the ellipse.
  gp_Pnt2d Focus1() const;

  //! Returns the second focus of the ellipse. This focus is on the
  //! negative side of the major axis of the ellipse.
  gp_Pnt2d Focus2() const;

  //! Returns the center of the ellipse.
  const gp_Pnt2d& Location() const { return pos.Location(); }

  //! Returns the major radius of the Ellipse.
  Standard_Real MajorRadius() const { return majorRadius; }

  //! Returns the minor radius of the Ellipse.
  Standard_Real MinorRadius() const { return minorRadius; }

  //! Returns p = (1 - e * e) * MajorRadius where e is the eccentricity
  //! of the ellipse.
  //! Returns 0 if MajorRadius = 0
  Standard_Real Parameter() const;

  //! Returns the major axis of the ellipse.
  const gp_Ax22d& Axis() const { return pos; }

  //! Returns the major axis of the ellipse.
  gp_Ax2d XAxis() const { return pos.XAxis(); }

  //! Returns the minor axis of the ellipse.
  //! Reverses the direction of the circle.
  gp_Ax2d YAxis() const { return pos.YAxis(); }

  void Reverse()
  {
    gp_Dir2d aTemp = pos.YDirection();
    aTemp.Reverse();
    pos.SetAxis(gp_Ax22d(pos.Location(), pos.XDirection(), aTemp));
  }

  Standard_NODISCARD gp_Elips2d Reversed() const;

  //! Returns true if the local coordinate system is direct
  //! and false in the other case.
  Standard_Boolean IsDirect() const { return (pos.XDirection().Crossed(pos.YDirection())) >= 0.0; }

  Standard_EXPORT void Mirror(const gp_Pnt2d& theP);

  //! Performs the symmetrical transformation of a ellipse with respect
  //! to the point theP which is the center of the symmetry
  Standard_NODISCARD Standard_EXPORT gp_Elips2d Mirrored(const gp_Pnt2d& theP) const;

  Standard_EXPORT void Mirror(const gp_Ax2d& theA);

  //! Performs the symmetrical transformation of a ellipse with respect
  //! to an axis placement which is the axis of the symmetry.
  Standard_NODISCARD Standard_EXPORT gp_Elips2d Mirrored(const gp_Ax2d& theA) const;

  void Rotate(const gp_Pnt2d& theP, const Standard_Real theAng) { pos.Rotate(theP, theAng); }

  Standard_NODISCARD gp_Elips2d Rotated(const gp_Pnt2d& theP, const Standard_Real theAng) const
  {
    gp_Elips2d anE = *this;
    anE.pos.Rotate(theP, theAng);
    return anE;
  }

  void Scale(const gp_Pnt2d& theP, const Standard_Real theS);

  //! Scales a ellipse. theS is the scaling value.
  Standard_NODISCARD gp_Elips2d Scaled(const gp_Pnt2d& theP, const Standard_Real theS) const;

  void Transform(const gp_Trsf2d& theT);

  //! Transforms an ellipse with the transformation theT from class Trsf2d.
  Standard_NODISCARD gp_Elips2d Transformed(const gp_Trsf2d& theT) const;

  void Translate(const gp_Vec2d& theV) { pos.Translate(theV); }

  //! Translates a ellipse in the direction of the vector theV.
  //! The magnitude of the translation is the vector's magnitude.
  Standard_NODISCARD gp_Elips2d Translated(const gp_Vec2d& theV) const
  {
    gp_Elips2d anE = *this;
    anE.pos.Translate(theV);
    return anE;
  }

  void Translate(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) { pos.Translate(theP1, theP2); }

  //! Translates a ellipse from the point theP1 to the point theP2.
  Standard_NODISCARD gp_Elips2d Translated(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2) const
  {
    gp_Elips2d anE = *this;
    anE.pos.Translate(theP1, theP2);
    return anE;
  }

private:
  gp_Ax22d      pos;
  Standard_Real majorRadius;
  Standard_Real minorRadius;
};

// =======================================================================
// function : Directrix1
// purpose  :
// =======================================================================
inline gp_Ax2d gp_Elips2d::Directrix1() const
{
  Standard_Real anE = Eccentricity();
  Standard_ConstructionError_Raise_if(anE <= gp::Resolution(),
                                      "gp_Elips2d::Directrix1() - zero eccentricity");
  gp_XY anOrig = pos.XDirection().XY();
  anOrig.Multiply(majorRadius / anE);
  anOrig.Add(pos.Location().XY());
  return gp_Ax2d(gp_Pnt2d(anOrig), gp_Dir2d(pos.YDirection()));
}

// =======================================================================
// function : Directrix2
// purpose  :
// =======================================================================
inline gp_Ax2d gp_Elips2d::Directrix2() const
{
  Standard_Real anE = Eccentricity();
  Standard_ConstructionError_Raise_if(anE <= gp::Resolution(),
                                      "gp_Elips2d::Directrix2() - zero eccentricity");
  gp_XY anOrig = pos.XDirection().XY();
  anOrig.Multiply(-majorRadius / anE);
  anOrig.Add(pos.Location().XY());
  return gp_Ax2d(gp_Pnt2d(anOrig), gp_Dir2d(pos.YDirection()));
}

// =======================================================================
// function : Eccentricity
// purpose  :
// =======================================================================
inline Standard_Real gp_Elips2d::Eccentricity() const
{
  if (majorRadius == 0.0)
  {
    return 0.0;
  }
  else
  {
    return sqrt(majorRadius * majorRadius - minorRadius * minorRadius) / majorRadius;
  }
}

// =======================================================================
// function : Focus1
// purpose  :
// =======================================================================
inline gp_Pnt2d gp_Elips2d::Focus1() const
{
  Standard_Real   aC  = sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  const gp_Pnt2d& aPP = pos.Location();
  const gp_Dir2d& aDD = pos.XDirection();
  return gp_Pnt2d(aPP.X() + aC * aDD.X(), aPP.Y() + aC * aDD.Y());
}

// =======================================================================
// function : Focus2
// purpose  :
// =======================================================================
inline gp_Pnt2d gp_Elips2d::Focus2() const
{
  Standard_Real   aC  = sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  const gp_Pnt2d& aPP = pos.Location();
  const gp_Dir2d& aDD = pos.XDirection();
  return gp_Pnt2d(aPP.X() - aC * aDD.X(), aPP.Y() - aC * aDD.Y());
}

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
inline void gp_Elips2d::Scale(const gp_Pnt2d& theP, const Standard_Real theS)
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
  pos.Scale(theP, theS);
}

// =======================================================================
// function : Scaled
// purpose  :
// =======================================================================
inline gp_Elips2d gp_Elips2d::Scaled(const gp_Pnt2d& theP, const Standard_Real theS) const
{
  gp_Elips2d anE = *this;
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
  anE.pos.Scale(theP, theS);
  return anE;
}

// =======================================================================
// function : Parameter
// purpose  :
// =======================================================================
inline Standard_Real gp_Elips2d::Parameter() const
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
// function : Reversed
// purpose  :
// =======================================================================
inline gp_Elips2d gp_Elips2d::Reversed() const
{
  gp_Elips2d anE   = *this;
  gp_Dir2d   aTemp = pos.YDirection();
  aTemp.Reverse();
  anE.pos.SetAxis(gp_Ax22d(pos.Location(), pos.XDirection(), aTemp));
  return anE;
}

// =======================================================================
// function : Transform
// purpose  :
// =======================================================================
inline void gp_Elips2d::Transform(const gp_Trsf2d& theT)
{
  Standard_Real aTSca = theT.ScaleFactor();
  if (aTSca < 0.0)
  {
    aTSca = -aTSca;
  }
  majorRadius *= aTSca;
  minorRadius *= aTSca;
  pos.Transform(theT);
}

// =======================================================================
// function : Transformed
// purpose  :
// =======================================================================
inline gp_Elips2d gp_Elips2d::Transformed(const gp_Trsf2d& theT) const
{
  gp_Elips2d anE = *this;
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
  anE.pos.Transform(theT);
  return anE;
}

#endif // _gp_Elips2d_HeaderFile
